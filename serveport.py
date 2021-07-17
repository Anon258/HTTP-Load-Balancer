import http.server as server
import socketserver
import argparse


class HTTPRequestHandler(server.SimpleHTTPRequestHandler):
    port = None
    def do_GET(self):
        self.send_response(200, 'OK')
        self.end_headers()
        reply_body = '<h1>HTTP 200</h1><p>Serving' + str(port) + '</p>'
        self.wfile.write(reply_body.encode('utf-8'))


if __name__ == '__main__':
    
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Specify port of the webserver", type=int)
    args = parser.parse_args()
    port = args.port

    handler = HTTPRequestHandler
    HTTPRequestHandler.port = port
    with socketserver.TCPServer(("",port), handler) as httpd:
        print("Started serving at localhost:" + str(port))
        httpd.serve_forever()