#!/usr/bin/env python

import urllib2
import json
#import keys
import credentials

URL='https://api.weather.com/v3/wx/forecast/daily/5day?postalKey=75013:US&units=e&language=en-US&format=json&apiKey='+credentials.WU_API_KEY
f = urllib2.urlopen(URL)
json_string = f.read()
parsed_json = json.loads(json_string)

# Get today's high & low
high = parsed_json['temperatureMax'][0]
#print "High is ", high
output = open("/mnt/sd/todays_high.txt", "w")
print >> output, high
output.close()

low = parsed_json['temperatureMin'][0]
output = open("/mnt/sd/todays_low.txt", "w")
print >> output, low
output.close()

# Today's probability of precipitation
pop = parsed_json['daypart'][0]['precipChance'][0]
output = open("/mnt/sd/todays_pop.txt", "w")
print >> output, pop
output.close()

# Today's short conditions phrase
conditions = parsed_json['daypart'][0]['wxPhraseShort'][0]
output = open("/mnt/sd/todays_conditions.txt", "w")
print >> output, conditions
output.close()

# Get tomorrow's high & low
high = parsed_json['temperatureMax'][1]
output = open("/mnt/sd/tomorrows_high.txt","w")
print >> output, high
output.close()

low = parsed_json['temperatureMin'][1]
output = open("/mnt/sd/tomorrows_low.txt", "w")
print >> output, low
output.close()

# Tomorrow's probability of precipitation
pop = parsed_json['daypart'][0]['precipChance'][2]
output = open("/mnt/sd/tomorrows_pop.txt", "w")
print >> output, pop
output.close()

# Tomorrow's short conditions phrase
conditions = parsed_json['daypart'][0]['wxPhraseShort'][2]
output = open("/mnt/sd/tomorrows_conditions.txt", "w")
print >> output, conditions
output.close()

# Tomorrow's short conditions icon
# The WU Icon PNG/BMP folders contain the official icons corresponding to the iconCode
# NOTE the folder name here makes no sense - but also the icon info is not actually used in this project
# Need to address this if I want to actually display icons. But this CPU+Display is too slow for that.
icon = parsed_json['daypart'][0]['iconCode'][2]
output = open("/mnt/sd/tomorrow_icon.txt", "w")
print >> output, "/mnt/sd/iconset9/"+str(icon)+".bmp"
output.close()

f.close()
