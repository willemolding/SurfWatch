"""
A python scraper for getting tide data from magicseaweed.

The regular API does not support tide data requests (for whatever reason).
This python function adds this functionality by hackily scraping the data from the website
"""

from bs4 import BeautifulSoup
import requests
from datetime import datetime

def get_tide_data(spot_id):
	"""
	Parameters:
		spot_id : String
			The magicseaweed ID number of the spot of interest.
	"""

	url = "http://magicseaweed.com/Surf-Report/%s/Tide/"%spot_id
	r = requests.get(url)
	soup = BeautifulSoup(r.text)	

	data = {
		"tide": {
			"unit" : "m",
			"events" : []
			}
		}

	for data_box in soup.find_all("div", class_="msw-tide-con"):
		date = data_box.find("h2").string
		for row in data_box.find("table", class_="table-tide").find_all("tr"):
			event = row.contents[1].string
			time = row.contents[3].string
			height = row.contents[5].string[:-1]

			localDatetime = datetime.strptime(date+time, " %a  %d %b %Y  %I:%M%p")
			localTimestamp = (localDatetime - datetime(1970, 1, 1)).total_seconds()

			data["tide"]["events"].append({
				"localTimestamp" : int(localTimestamp),
				"event" : event,
				"height" : height
				})


	return data