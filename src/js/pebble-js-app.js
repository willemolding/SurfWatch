const config_page_url = ""

const msw_key = "4fa1b1a1cb45e3ade0ee1fe7560ff2ee";
const msw_forecast_url = "http://magicseaweed.com/api/"+msw_key+"/forecast/";
const msw_tide_url = "http://magicseaweed.com/api/"+msw_key+"/tide/";

const locationOptions = {
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




function date_update_event(){

  console.log('Loading surf data...');

  //retrieve the stored spot_id
  var spot_id = localStorage.getItem('spot_id');

  if(!spot_id) {
  	Pebble.showSimpleNotificationOnPebble("Please Open Config", 
  		"You have not yet selected a location. Please open the configuration page");
  }
  else{
  	var request = new XMLHttpRequest();
	  request.open('GET', msw_url+'?spot_id='+spot_id, true);

	  request.onload = function() {
		  if (request.status >= 200 && request.status < 400) {
			// Success!
			console.log('Data recieved from server successfully.');
			var data = JSON.parse(request.responseText);
			send_data_to_pebble(data[0]);
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

}



Pebble.addEventListener("ready", function(e) {
	get_surf_data_for_user();
	setInterval(get_surf_data_for_user, 30*60*1000) //update every 30 minutes
});



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

	var configData = JSON.parse(decodeURIComponent(e.response));

	// if a spot_id was returned then store it
	if(configData.spot_id){
		localStorage.setItem("spot_id", configData.spot_id)
		//trigger an update
		date_update_event();
	}


  }
);


