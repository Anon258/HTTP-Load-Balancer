import http.server
import socketserver
import requests
from requests.exceptions import Timeout

PORT = 8000
inter_hostName = "localhost"
hostname = 'localhost:8080/hello'

timeout = 10
inreq = 0
req_resp = 0
time = 0
freq_map = {}
    
class ProxyHTTPRequestHandler(http.server.BaseHTTPRequestHandler):
	protocol_version = 'HTTP/1.1'
	
	def do_HEAD(self):
		self.do_GET(body=False)

	def do_GET(self, body=True):
		sent = False
		self.log_data()
		try:
			# Forward Request to server
			url = 'http://{}{}'.format(hostname, self.path)
			req_header = self.parse_headers()

			print("Header\n", req_header)
			req_header['Host'] = hostname
			resp = requests.get(url, headers=req_header, verify=False, timeout = timeout)
			sent = True
			self.send_response(resp.status_code)
			self.send_resp_headers(resp)
			if body:
				self.wfile.write(resp.content)
			return
		except Timeout: 
			if sent:
				global time
				time = time + 1
				self.send_error(408, 'request timeout')
		else:
			if not sent:
				self.send_error(404, 'error trying to proxy')
			else:	
				req_resp = req_resp + 1

	
	def log_data(self):
		global inreq, freq_map
		inreq = inreq + 1
		if self.headers['Host'] in freq_map.keys():
			freq_map[self.headers['Host']] = freq_map[self.headers['Host']] + 1
		else:
			freq_map[self.headers['Host']] = 1
	
	def parse_headers(self):
		req_header = {}
		print(self.headers)
		for line in self.headers:
		    req_header[line] = self.headers[line]
		return req_header

	def send_resp_headers(self, resp):
		respheaders = resp.headers
		print('Response Header')
		print(respheaders)
		for key in respheaders:
			self.send_header(key, respheaders[key])
		self.end_headers()


Handler = ProxyHTTPRequestHandler

with socketserver.TCPServer((inter_hostName, PORT), Handler) as httpd:
    print("serving at port", PORT)
    httpd.serve_forever()
