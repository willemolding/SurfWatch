var config_page_url = ""

var msw_key = "4fa1b1a1cb45e3ade0ee1fe7560ff2ee";
var msw_forecast_url = "http://magicseaweed.com/api/"+msw_key+"/forecast/";
var msw_tide_url = "http://magicseaweed.com/api/"+msw_key+"/tide/";


var locationOptions = {
  enableHighAccuracy: true,
  maximumAge: 10000,
  timeout: 10000
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

	var message = {'WIND_STRENGTH' : Math.round(response.wind.speed),
				  'WIND_DIRECTION' : Math.round(response.wind.direction),
				  'WIND_UNITS' : response.wind.unit,
				  'SWELL_STRENGTH' : Math.round(response.swell.components.combined.height),
				  'SWELL_DIRECTION': Math.round(response.swell.components.combined.direction),
				  'SWELL_UNITS' : response.swell.unit,
				  'SURF_RATING' : response.solidRating + response.fadedRating,
				  'MIN_SURF_HEIGHT' : response.swell.minBreakingHeight,
				  'MAX_SURF_HEIGHT' : response.swell.maxBreakingHeight,
				  'WIND_RATING_PENALTY' : response.fadedRating}

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

function get_new_data(){

  console.log('Loading surf data...');

  var request = new XMLHttpRequest();
  request.open('GET', msw_url+'?spot_id='+spot_id, true);

  request.onload = function() {
	  if (request.status >= 200 && request.status < 400) {
		// Success!
		console.log('Data recieved from server successfully.');
		var surf_data = JSON.parse(request.responseText);
		send_data_to_pebble(surf_data[0]);
	  }
	  else if(request.status == 503){
		console.log('Unknown server error');
		send_error_message_to_pebble("Server Unavailable");
	  }
	  else if(request.status == 500) {
		console.log('Unknown server error');
		send_error_message_to_pebble("Server Unavailable");
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
		get_surf_data_for_user();
		setInterval(get_surf_data_for_user, 30*60*1000) //update every 30 minutes
	}


);



Pebble.addEventListener('showConfiguration', function(e) {

	// set up the location callbacks
	function success(pos) {
  		var url = config_page_url+'?lat='+pos.coords.latitude+'&lon='+pos.coords.longitude;
  		Pebble.openURL(url);
	}

	function error(err) {
		var url = config_page_url+'?token='+token+'&timeline='+timeline;
		Pebble.openURL(url);
	}

	//request lat,lon from location service
	navigator.geolocation.getCurrentPosition(success, error, locationOptions);
		
});


Pebble.addEventListener('webviewclosed',
  function(e) {
	console.log('Configuration window returned: ' + e.response);
	get_tide_data_for_user();
  }
);


