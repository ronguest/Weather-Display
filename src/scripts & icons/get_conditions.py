#!/usr/bin/env python

import urllib2
import json
#import keys
import credentials

URL='https://api.ambientweather.net/v1/devices/' + credentials.AW_DEVICE + '?applicationKey=' + credentials.AW_APP_KEY + '&apiKey=' + credentials.AW_API_KEY + '&limit=1'
#f = urllib2.urlopen('http://api.wunderground.com/api/$WU_KEY/conditions/q/pws:KTXALLEN99.json')
f = urllib2.urlopen(URL)
json_string = f.read()
parsed_json = json.loads(json_string)
#
# Write the temperature
#
temp_f = parsed_json[0]['tempf']
output = open("/mnt/sd/temperature.txt", "w")
print >> output, temp_f
output.close()
#
# Write humidity
#
humidity = parsed_json[0]['humidity']
output = open("/mnt/sd/humidity.txt", "w")
print >> output, humidity
output.close()
#
# Write conditions icon file name
#
# The new WU API does not provide icon info for current conditions
#icon = parsed_json['current_observation']['icon']
icon = 'unk'
output = open("/mnt/sd/current_icon.txt", "w")
print >> output, "/mnt/sd/iconset9/"+icon+".bmp"
output.close()

f.close()
