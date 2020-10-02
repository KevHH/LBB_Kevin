'''
    References: 
    - flask-sockietio https://medium.com/hackervalleystudio/weekend-project-part-2-turning-flask-into-a-real-time-websocket-server-using-flask-socketio-ab6b45f1d896
    - streaming https://blog.miguelgrinberg.com/post/video-streaming-with-flask
'''

# systems
import time
import sys
sys.path.append("..")

# web interface
from flask import Flask, render_template, jsonify, Response
from flask_socketio import SocketIO, join_room, emit, send

# software processing
import zmq
import threading
from processing.stats import get_age
import processing.body.cfg as cfg

# ===========
#  set up 
# ============

context = zmq.Context()

shutdown_socket = context.socket(zmq.PAIR)
shutdown_socket.RCVTIMEO = 100
shutdown_socket.bind("tcp://127.0.0.1:6000")

eyes_socket = context.socket(zmq.PAIR)
eyes_socket.setsockopt(zmq.LINGER,0)
eyes_socket.connect("tcp://127.0.0.1:6010")

body_socket = context.socket(zmq.PAIR)
body_socket.RCVTIMEO = 200
body_socket.connect("tcp://127.0.0.1:6020")


cfg.init_once()
app = Flask(__name__)
cfg.socketio = SocketIO(app)

# ============
#  Routing 
# ============

@app.route('/')
def index():
    return render_template('index.html', age = get_age())

# streaming video
@app.route('/video_feed')
def video_feed():
    return Response(gen(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

def gen():
    while True:
        img_encoded = eyes_socket.recv()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + img_encoded + b'\r\n')

# ====================
#  Listening to client
# ====================

# nudged
@cfg.socketio.on('nudge')
def on_nudge():
    body_socket.send( b'py&nudge\n' )

# whisper
@cfg.socketio.on('whisper')
def on_whisper(response):
    body_socket.send(b'py&whisper&' + response.encode('utf-8') + b'\n')

# shutdown
@cfg.socketio.on('shutdown')
def on_shutdown():
    shutdown_socket.send_string("shutdown")


# test commands
@cfg.socketio.on('test')
def on_test(response):
    body_socket.send(b'py&' + response.encode('utf-8') + b'\n')

# ============
#  Main
# ============
if __name__ == '__main__':
    # run app
    cfg.socketio.run(app, debug=False, port=8888, host='0.0.0.0', use_reloader=False)
'''
    # termination
    shutdown_socket.close()
    eyes_socket.close()
    body_socket.close()
    context.destroy()
'''