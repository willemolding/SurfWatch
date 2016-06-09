const config_page_url = "http://localhost:8000/"

const msw_key = "4fa1b1a1cb45e3ade0ee1fe7560ff2ee";
const msw_forecast_url = "http://magicseaweed.com/api/"+msw_key+"/forecast/";
const msw_tide_url = "http://magicseaweed.com/api/"+msw_key+"/tide/";

const locationOptions = {
	enableHighAccuracy: true,
	maximumAge: 10000,
	timeout: 10000
};

var spot_id = null;

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


var message = {}

function add_forecast_data_to_message(response){
	message['SOLID_RATING'] = response.solidRating;
	message['FADED_RATING'] = response.fadedRating;

	message['SWELL_DIRECTION'] = Math.round(response.swell.components.combined.direction);
	message['SWELL_HEIGHT'] = Math.round(response.swell.components.combined.height);
	message['SWELL_UNITS'] = response.swell.unit;

	message['WIND_STRENGTH'] = Math.round(response.wind.speed);
	message['WIND_DIRECTION'] = Math.round(response.wind.direction);
	message['WIND_UNITS'] = response.wind.unit;

	message['MIN_SURF_HEIGHT'] = response.swell.minBreakingHeight;
	message['MAX_SURF_HEIGHT'] = response.swell.maxBreakingHeight;
}

function add_tide_data_to_message(response) {
	message['TIDE_UNITS'] = response.unit

	message['TIDE_1_TIME'] = response.tide[0].timestamp
	message['TIDE_1_HEIGHT'] = response.tide[0].shift
	message['TIDE_1_STATE'] = response.tide[0].state

	message['TIDE_2_TIME'] = response.tide[1].timestamp
	message['TIDE_2_HEIGHT'] = response.tide[1].shift
	message['TIDE_2_STATE'] =response.tide[1].state
}


function make_request(url, success_callback) {
	var request = new XMLHttpRequest();
	request.open('GET', url, true);

	request.onload = function() {
		if (request.status >= 200 && request.status < 400) {
			// Success!
			console.log('Data recieved from server successfully.');
			success_callback(request);
		}
		else if(request.status == 503){
			console.log('Unknown server error');
		}
		else if(request.status == 500) {
			console.log('Unknown server error');
		}
		else {
			console.log('Unknown server error');
		}
	};
	request.onerror = function() {
		// There was a connection error of some sort
		console.log('Could not reach server.');
	};
	request.send();
}



function data_update_event(){

	console.log('Loading surf data...');
	if(spot_id){
		//make the forecase request
		make_request(msw_forecast_url+'?spot_id='+spot_id, 
			function(request) {
			var data = JSON.parse(request.responseText);
			add_forecast_data_to_message(data[0]);
		});

		//make the tide request
		make_request(msw_tide_url+'?spot_id='+spot_id, 
			function(request) {
			var data = JSON.parse(request.responseText);
			add_tide_data_to_message(data[0]);
		});

		//whatever we receive to the watch
		send_pebble_message(message);
	}
}


Pebble.addEventListener("ready", function(e) {

	//retrieve the stored spot_id
	spot_id = localStorage.getItem('spot_id');

	if(!spot_id) {
		Pebble.showSimpleNotificationOnPebble("Please Open Config", 
			"You have not yet selected a location. Please open the configuration page.");
	}

	data_update_event();
	setInterval(data_update_event, 30*60*1000); //update every 30 minutes
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


Pebble.addEventListener('webviewclosed', function(e) {

	var returned_spot_id = decodeURIComponent(e.response);

	console.log(returned_spot_id);

	// if a spot_id was returned then store it
	if(returned_spot_id){
		spot_id = returned_spot_id;
		localStorage.setItem("spot_id", spot_id);
		//trigger an update
		date_update_event();
	}
});


