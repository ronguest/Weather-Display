#!/usr/bin/env python

import urllib2
import json
#import keys
import credentials

URL='https://api.ambientweather.net/v1/devices/' + credentials.AW_DEVICE + '?applicationKey=' + credentials.AW_APP_KEY + '&apiKey=' + credentials.AW_API_KEY + '&limit=1'

f = urllib2.urlopen(URL)
json_string = f.read()
parsed_json = json.loads(json_string)

#
# Write the temperature
#
for key in parsed_json:
    if key["tempinf"]:
        tempin_f = key["tempinf"]
output = open("/mnt/sd/inside_temp.txt", "w")
#output = open("inside_temp.txt", "w")
print >> output, tempin_f
output.close()

#
# Write humidity
#
for key in parsed_json:
    if key["humidityin"]:
        humidityin = key["humidityin"]
output = open("/mnt/sd/inside_humidity.txt", "w")
#output = open("inside_humidity.txt", "w")
print >> output, humidityin
output.close()

f.close()
