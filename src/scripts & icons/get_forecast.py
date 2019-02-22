#!/usr/bin/env python

import urllib2
import json
#import keys
import credentials

URL='http://api.wunderground.com/api/'+credentials.WU_API_KEY+'/forecast/q/TX/Allen.json'
f = urllib2.urlopen(URL)
json_string = f.read()
parsed_json = json.loads(json_string)

# Get today's high & low
high = parsed_json['forecast']['simpleforecast']['forecastday'][0]['high']['fahrenheit']
#print "High is ", high
output = open("/mnt/sd/todays_high.txt", "w")
print >> output, high
output.close()

low = parsed_json['forecast']['simpleforecast']['forecastday'][0]['low']['fahrenheit']
output = open("/mnt/sd/todays_low.txt", "w")
print >> output, low
output.close()

# Today's probability of precipitation
pop = parsed_json['forecast']['simpleforecast']['forecastday'][0]['pop']
output = open("/mnt/sd/todays_pop.txt", "w")
print >> output, pop
output.close()

# Today's short conditions phrase
conditions = parsed_json['forecast']['simpleforecast']['forecastday'][0]['conditions']
output = open("/mnt/sd/todays_conditions.txt", "w")
print >> output, conditions
output.close()

# Get tomorrow's high & low
high = parsed_json['forecast']['simpleforecast']['forecastday'][1]['high']['fahrenheit']
output = open("/mnt/sd/tomorrows_high.txt","w")
print >> output, high
output.close()

low = parsed_json['forecast']['simpleforecast']['forecastday'][1]['low']['fahrenheit']
output = open("/mnt/sd/tomorrows_low.txt", "w")
print >> output, low
output.close()

# Tomorrow's probability of precipitation
pop = parsed_json['forecast']['simpleforecast']['forecastday'][1]['pop']
output = open("/mnt/sd/tomorrows_pop.txt", "w")
print >> output, pop
output.close()

# Tomorrow's short conditions phrase
conditions = parsed_json['forecast']['simpleforecast']['forecastday'][1]['conditions']
output = open("/mnt/sd/tomorrows_conditions.txt", "w")
print >> output, conditions
output.close()

# Tomorrow's short conditions icon
icon = parsed_json['forecast']['simpleforecast']['forecastday'][1]['icon']
output = open("/mnt/sd/tomorrow_icon.txt", "w")
print >> output, "/mnt/sd/iconset9/"+icon+".bmp"
output.close()
