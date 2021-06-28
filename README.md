# HTTP-Load-Balancer
## Done :
Added lb (loadbalancer) with basic reverse proxy functions and server health checking. Tested on various types of requests from http_client.

### Config File format (temp):

First line: interval = '<interval in seconds>'
Empty Line
Each subsequent line is of the format 
'<server url>','<bool for dedicated health check>','<health check url (only if previous was true)>'

## TODO:
1. Add health check without dedicated url
2. Loadbalancer logging
3. Testing configuration file and perhaps changing format

