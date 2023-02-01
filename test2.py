from flask import Flask
from gevent import pywsgi

app = Flask(__name__)

@app.route('/')
def index():
	return '<p>index</p>'
	
	
if __name__ == '__main__':
	server = pywsgi.WSGIServer(('0.0.0.0', 5000), app)
	server.serve_forever()
