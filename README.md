# HTTP-Load-Balancer
## Done :
Added lb (loadbalancer) with basic reverse proxy functions and server health checking. Tested on various types of requests from http_client.

### Config File format (temp):

1. First line: interval = '<interval in seconds>'
2. Empty Line
3. Each subsequent line is of the format 

&lt; server url &gt;, &lt; bool for dedicated health check &gt; , &lt; health check url (only if previous was true) &gt;

## TODO:
1. Add health check without dedicated url
2. Loadbalancer logging
3. Testing configuration file and perhaps changing format

