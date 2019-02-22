#!/usr/bin/env python

import credentials

import urllib2
import json
#f = open("conditions.json", "r")
#
# Get the current conditions via a URL from weatherunderground
#

#print "WU key ="+credentials.WU_API_KEY+"-"
URL='https://api.ambientweather.net/v1/devices/08:D8:33:44:63:13?applicationKey='+credentials.AW_APP_KEY+'&apiKey='+credentials.AW_API_KEY+'&limit=1'
#print URL
#exit()

f = urllib2.urlopen(URL)
json_string = f.read()
parsed_json = json.loads(json_string)

#print json.dumps(parsed_json, indent=4)
#print parsed_json
for key in parsed_json:
    if key["tempinf"]:
        #print "Found it!"
        print "Indoor temperature is",key["tempinf"]

#temp_f = parsed_json["tempinf"]
#print temp_f
exit()
#
# Write the temperature
#
temp_f = parsed_json['AW_my_data_json'][0]['tempinf']
#output = open("/mnt/sd/inside_temp.txt", "w")
print >> output, temp_f
output.close()
#
# Write humidity
#
humidity = parsed_json['current_observation'][0]['humidityin']
#output = open("/mnt/sd/inside_humidity.txt", "w")
print >> output, humidity
output.close()

f.close()
