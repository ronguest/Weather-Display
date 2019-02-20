#!/usr/bin/env python

import urllib2
import json
import os
import subprocess

token_f = open("/mnt/sd/ecobee/REFRESH_TOKEN")
refresh_token = token_f.read()
refresh_token = refresh_token.strip()
token_f.close()

# Use the code from above and the API key to get a token and refresh token
refresh_token_url = 'https://api.ecobee.com/token?grant_type=refresh_token&refresh_token=' + refresh_token +'&client_id=QfrlscKsFN8w2eW2J8dMNykYWECshGET'
print "Refresh token URL is ", refresh_token_url
p = subprocess.Popen(["curl", "-k", "-X", "POST", refresh_token_url], stdout=subprocess.PIPE)
(json_string, err) = p.communicate()
print "curl output is", json_string
parsed_json = json.loads(json_string)
token = parsed_json['access_token']
refresh_token = parsed_json['refresh_token']
print "Token is ", token
print "Refresh token is ", refresh_token

# Save the new token and refresh token for use next time
output = open("/mnt/sd/ecobee/REFRESH_TOKEN", "w")
print >> output, refresh_token
output.close()
output = open("/mnt/sd/ecobee/TOKEN", "w")
print >> output, token
output.close()
