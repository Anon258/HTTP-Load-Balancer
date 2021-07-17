import os
import http.server as server
import socketserver
import argparse


class HTTPRequestHandler(server.SimpleHTTPRequestHandler):

    def do_PUT(self):
        filename = os.path.basename(self.path)

        if os.path.exists(filename):
            os.remove(filename)

        file_length = int(self.headers['Content-Length'])
        read = 0
        with open(filename, 'wb+') as output_file:
            while read < file_length:
                new_read = self.rfile.read(min(66556, file_length - read))
                read += len(new_read)
                output_file.write(new_read)
        self.send_response(201, 'Created')
        self.end_headers()
        reply_body = 'Saved "%s"\n' % filename
        self.wfile.write(reply_body.encode('utf-8'))


if __name__ == '__main__':
    
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Specify port of the webserver", type=int)
    args = parser.parse_args()
    port = args.port

    handler = HTTPRequestHandler
    with socketserver.TCPServer(("",port), handler) as httpd:
        print("Started serving at localhost:" + str(port))
        httpd.serve_forever()