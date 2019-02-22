#!/usr/bin/env python

import urllib2
import json
#import keys
import credentials

URL='http://api.wunderground.com/api/'+credentials.WU_API_KEY+'/conditions/q/pws:KTXALLEN99.json'
#f = urllib2.urlopen('http://api.wunderground.com/api/$WU_KEY/conditions/q/pws:KTXALLEN99.json')
f = urllib2.urlopen(URL)
json_string = f.read()
parsed_json = json.loads(json_string)
#
# Write the temperature
#
temp_f = parsed_json['current_observation']['temp_f']
output = open("/mnt/sd/temperature.txt", "w")
print >> output, temp_f
output.close()
#
# Write humidity
#
humidity = parsed_json['current_observation']['relative_humidity']
output = open("/mnt/sd/humidity.txt", "w")
print >> output, humidity
output.close()
#
# Write conditions icon file name
#
icon = parsed_json['current_observation']['icon']
output = open("/mnt/sd/current_icon.txt", "w")
print >> output, "/mnt/sd/iconset9/"+icon+".bmp"
output.close()

f.close()
