import http.server
import socketserver
import random

PORT = 5656
    
class BaseServerHandler(http.server.BaseHTTPRequestHandler):
	protocol_version = 'HTTP/1.1'

	def respond(self):
		resp = random.random()
		print(resp)
		if resp < 0.4:
			self.send_error(404, "File not Found")
		else:
			self.send_response(200, "Ok")

	def do_GET(self):
		self.respond()

	def do_PUT(self):
		self.respond()

	def do_POST(self):
		self.respond()

	def do_HEAD(self):
		self.respond()

	def do_DELETE(self):
		self.respond()

Handler = BaseServerHandler

with socketserver.TCPServer(("localhost", PORT), Handler) as httpd:
    print("Serving at port", PORT)
    httpd.serve_forever()