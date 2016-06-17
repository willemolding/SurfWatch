const config_page_url = "http://localhost:8000/"

const msw_key = "qG507rwB78RM89nmo25rfgtvAZ1M3c4W";
const msw_forecast_url = "http://magicseaweed.com/api/"+msw_key+"/forecast/";
const msw_tide_url = "http://magicseaweed.com/api/"+msw_key+"/tide/";

const locationOptions = {
	enableHighAccuracy: true,
	maximumAge: 10000,
	timeout: 10000
};

var spot_id = null;

function send_pebble_message(msg) {
	console.log("Message being sent is:\n"+JSON.stringify(msg));
	Pebble.sendAppMessage(msg,
		function(e) {
		console.log('Send successful.');
		},
		function(e) {
		console.log('Send failed!');
		}
	);
}


function add_forecast_data_to_message(response, message){
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

	return message;
}

function add_tide_data_to_message(response, message) {
	message['TIDE_UNITS'] = response.unit;

	message['TIDE_1_TIME'] = response.tide[0].timestamp;
	message['TIDE_1_HEIGHT'] = Math.round(response.tide[0].shift * 100);
	message['TIDE_1_STATE'] = response.tide[0].state == "High" ? 1 : 0;

	message['TIDE_2_TIME'] = response.tide[1].timestamp;
	message['TIDE_2_HEIGHT'] = Math.round(response.tide[1].shift*100);
	message['TIDE_2_STATE'] =response.tide[1].state == "High" ? 1 : 0;

	return message;
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
	var message = {}


	if(spot_id){
		//make the forecase request
		make_request(msw_forecast_url+'?spot_id='+spot_id, 
			function(request) {
			var data = JSON.parse(request.responseText);
			console.log("data returned is: \n" + JSON.stringify(data[0]))
			message = add_forecast_data_to_message(data[0], message);

			//make the tide request
			make_request(msw_tide_url+'?spot_id='+spot_id, 
			function(request) {
			var data = JSON.parse(request.responseText);
			message = add_tide_data_to_message(data[0], message);

			send_pebble_message(message);

			});
		});



	}
	else{
		console.log("Could not make request. No Spot ID stored.");
	}
}


Pebble.addEventListener("ready", function(e) {

	//retrieve the stored spot_id
	spot_id = localStorage.getItem('spot_id');

	if(!spot_id) {
		Pebble.showSimpleNotificationOnPebble("Please Open Config", 
			"You have not yet selected a location. Please open the configuration page.");
		console.log("No spot ID stored. User must open config.");
	}
	else{
		console.log("Loaded stored spot ID: "+spot_id);
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

	console.log("retured spot ID was: "+returned_spot_id);

	// if a spot_id was returned then store it
	if(returned_spot_id){
		spot_id = returned_spot_id;
		localStorage.setItem("spot_id", spot_id);
		//trigger an update
		data_update_event();
	}
});


