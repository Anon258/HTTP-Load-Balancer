# HTTP-Load-Balancer

## Testing
To test, execute serveport.py with port numbers 6050, 6060 and execute serve.py in test_dir/ with 6070 and go to localhost:8080. You can try turning the python servers on and off to observe changes in real time.

## Config Format
(int) -> interval in seconds for which to pause  
(string)* -> server_url [(space) health_url]
