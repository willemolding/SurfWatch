var site_url = "http://pebtides-time-dev.herokuapp.com/";

var lat = 0;
var lon = 0;
var token = 'not_set';
var timeline = 0;
var current_watch;

var locationOptions = {
  enableHighAccuracy: false, 
  maximumAge: 0, 
  timeout: 1000
};


function send_pebble_message(message) {
  Pebble.sendAppMessage(message,
      function(e) {
        console.log('Send successful.');
      },
      function(e) {
        console.log('Send failed!');
      }
    );
}

function getInt32Bytes( x ) {
    var bytes = [];
    for (var i = 0; i < 4; i++){
      bytes[i] = x & (255);
      x = x>>8;
    }
    return bytes;
}


function send_data_to_pebble(response){

    console.log('data from server:');
    console.log(JSON.stringify(response));

    var times = [];
    var heights = [];
    var events = [];
    var unit = '';
    for (var tide_event_index in response.tide_data){
      //process the tide data into data
      var tide_event = response.tide_data[tide_event_index];
      console.log(JSON.stringify(tide_event));

      //send the local time if the watch is running APLITE as currently this has no timzone correction
      if(current_watch.platform == 'aplite') {
            times = times.concat(getInt32Bytes(tide_event.local_time));
      }
      else {  //send the UTC time if running BASALT as this has the ability to adjust timezones
            times = times.concat(getInt32Bytes(tide_event.time));
      }


      heights = heights.concat(getInt32Bytes(tide_event.height * 100));

      unit = tide_event.unit;
      if(tide_event.event == 'High Tide'){
        events.push(1);
      }
      else{
        events.push(0);
      }
    }

    var message = { 'NAME' : response.name,
                    'UNIT' : unit,
                    'N_EVENTS' : response.tide_data.length,
                    'TIMES' : times,
                    'HEIGHTS' : heights,
                    'EVENTS' : events};

    console.log('pebble message is:');
    console.log(JSON.stringify(message));

    send_pebble_message(message);

}

function send_error_message_to_pebble(error_string){
    var message = { 'ERROR_MSG' : error_string};

    console.log('pebble message is:');
    console.log(JSON.stringify(message));

    send_pebble_message(message);
}

function get_data_for_user(){

    console.log('My token is ' + token);

    var request = new XMLHttpRequest();

    console.log('sending request to: ' + site_url+'users?token='+token);
    request.open('GET', site_url+'users?token='+token, true);

    request.onload = function() {
      if (request.status >= 200 && request.status < 400) {
        // Success!
        console.log('Data recieved from server successfully.');
        var tide_data = JSON.parse(request.responseText);
        send_data_to_pebble(tide_data);
      }
      else if(request.status == 503){
        // there are no dynos running to respond
        console.log('Server is not running');
        send_error_message_to_pebble("Server Unavailable");
      }
      else if(request.status == 500) {
        console.log('No user with this token');
        send_error_message_to_pebble("Please open app configuration");
      }
      else {
        console.log('Unknown server error');
        send_error_message_to_pebble("Server Unavailable");
      }
    };
    request.onerror = function() {
      // There was a connection error of some sort
      console.log('Could not reach server.');
      //send error message
      send_error_message_to_pebble("Server Unavailable")
    };
    request.send();
}

Pebble.addEventListener("ready",
  
    function(e) {
      
        //determine the current watch info. This is a hack until pebble fixes the bug.
        if(Pebble.getActiveWatchInfo) {
          try {
            current_watch = Pebble.getActiveWatchInfo();
          } catch(err) {
            current_watch = {
              platform: "basalt",
            };
          }
        } else {
          current_watch = {
            platform: "aplite",
          };
        }

        console.log("running on " + current_watch.platform);

        //request current position
        navigator.geolocation.getCurrentPosition(function (pos){
          lat = pos.coords.latitude;
          lon = pos.coords.longitude;
        }, function (err){
            console.log('location error (' + err.code + '): ' + err.message);
        }, locationOptions);


        if(Pebble.getTimelineToken){
            // if the timeline token is available e.g. using a Pebble Time watch
            Pebble.getTimelineToken(
                function (timeline_token) {
                    console.log('Timeline token obtained.');
                    token = timeline_token;
                    timeline = 1;
                    get_data_for_user();
                },
                function (error) { 
                    console.log('Error getting timeline token: ' + error);
                    token = Pebble.getAccountToken();
                    get_data_for_user();
                }
            );
        }
        else{
            console.log('Timeline token is not available for this watch');
            token = Pebble.getAccountToken();
            get_data_for_user();
        }
    }
);



Pebble.addEventListener('showConfiguration', function(e) {

        console.log('showing configuration page.');

        var url;

        if(lat !== 0 && lon !== 0) {
          url = site_url+'configure?token='+token+'&timeline='+timeline+'&lat='+lat+'&lon='+lon;
        }
        else {
          url = site_url+'configure?token='+token+'&timeline='+timeline;
        }

        console.log(url);
        Pebble.openURL(url);
        
});


Pebble.addEventListener('webviewclosed',
  function(e) {
    console.log('Configuration window returned: ' + e.response);
    get_data_for_user();
  }
);
