'''
    References: 
    - flask-sockietio https://medium.com/hackervalleystudio/weekend-project-part-2-turning-flask-into-a-real-time-websocket-cfg.server-using-flask-cfg.socketio-ab6b45f1d896
    - streaming https://blog.miguelgrinberg.com/post/video-streaming-with-flask
'''

# system
import time
import sys
sys.path.append("..")

# web interface
from flask import Flask, render_template, jsonify, Response
from flask_socketio import SocketIO, join_room, emit, send

# hardware interactions
from serial import Serial
from processing.arduino.listener import Ardlistener
import processing.arduino.cfg as cfg

# software processing
import zmq
from processing.stats import get_age

# ===========
#  set up 
# ============

context = zmq.Context()

socket = context.socket(zmq.PAIR)
socket.bind("tcp://127.0.0.1:6000")

eyes_socket = context.socket(zmq.PAIR)
eyes_socket.connect("tcp://127.0.0.1:6001")
cfg.init()

app = Flask(__name__)

cfg.socketio = SocketIO(app)
ardlistener = Ardlistener()

# open cfg.serial port
cfg.ser = Serial('/dev/ttyUSB0', 19200)
time.sleep(1.00)

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
    cfg.ser.write( b'py&nudge\n' )
    time.sleep(0.1)

# whisper
@cfg.socketio.on('whisper')
def on_whisper(response):
    cfg.ser.write(b'py&whisper&' + response.encode('utf-8') + b'\n')
    time.sleep(0.1)


# ====================s
#  Listening to arduino
# ====================
def on_update_age(age):
    cfg.socketio.emit("update_age", age, broadcast=True)
ardlistener.register("update_age", on_update_age)

# ============
#  Main
# ============
if __name__ == '__main__':


    # listen to arduino
    ardlistener.run()

    # run app
    cfg.socketio.run(app, debug=False, port=8888, host='0.0.0.0', use_reloader=False)

    # termination
    socket.close()
    eyes_socket.close()
    context.destroy()