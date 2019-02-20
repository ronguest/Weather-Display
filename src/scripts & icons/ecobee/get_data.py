#!/usr/bin/env python

############
#
# See Evernote for the authorization sequence necessary to get the reusable TOKEN
#
############

import urllib2
import json
import os
import subprocess

token_f = open("/mnt/sd/ecobee/TOKEN")
token = token_f.read()
token = token.strip()
#print token
token_f.close()

# Take the token and use it to request the data from the thermostats
therm_url='https://api.ecobee.com/1/thermostat?format=json&body=\{%22selection%22:\{%22selectionType%22:%22registered%22,%22selectionMatch%22:%22%22,%22includeRuntime%22:true\}\}'
auth_param = 'Authorization: Bearer ' + token 
p = subprocess.Popen(["curl", "-k", "-H", "Content-Type: text/json", "-H", auth_param, therm_url], stdout=subprocess.PIPE)
(json_string, err) = p.communicate()
print "curl output is", json_string
parsed_json = json.loads(json_string)

###### Need to add code to check return value and do a token refresh if called for
status = parsed_json['status']['code']
#print "Status ", status                                                                                                
if status <> 0:                                                                                                         
        p = subprocess.Popen("/mnt/sd/ecobee/get_refreshed_token.py",stdout=subprocess.PIPE)                                                                           
else:                                                                                                                   
	#
	# Write the temperature from thermostat 0 which is 'downstairs'
	#
	temp_f = parsed_json['thermostatList'][0]['runtime']['actualTemperature']
	output = open("/mnt/sd/inside_temp.txt", "w")
	print >> output, temp_f
	output.close()
	
	#
	# Write humidity
	#
	humidity = parsed_json['thermostatList'][0]['runtime']['actualHumidity']
	output = open("/mnt/sd/inside_humidity.txt", "w")
	print >> output, humidity
	output.close()

	#
	# Write the temperature from thermostat 1 which is 'upstairs'
	#
	temp_f = parsed_json['thermostatList'][1]['runtime']['actualTemperature']
	output = open("/mnt/sd/upstairs_temp.txt", "w")
	print >> output, temp_f
	output.close()
	
	#
	# Write humidity
	#
	humidity = parsed_json['thermostatList'][1]['runtime']['actualHumidity']
	output = open("/mnt/sd/upstairs_humidity.txt", "w")
	print >> output, humidity
	output.close()
