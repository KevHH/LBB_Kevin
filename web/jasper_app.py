'''
    References: 
    - flask-sockietio https://medium.com/hackervalleystudio/weekend-project-part-2-turning-flask-into-a-real-time-websocket-cfg.server-using-flask-cfg.socketio-ab6b45f1d896
    - streaming https://blog.miguelgrinberg.com/post/video-streaming-with-flask
'''
# web interface
from flask import Flask, render_template, jsonify, Response
from flask_socketio import SocketIO, join_room, emit, send

# hardware interactions
from lib.streaming.camera_pi import Camera
from serial import Serial
from lib.arduino.listener import Ardlistener
import lib.arduino.cfg as cfg

# system
import time
import sys
sys.path.append("..")

# software processing
from processing.stats import get_age


# ===========
#  set up 
# ============

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
    return Response(gen(Camera()),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

def gen(camera):
    while True:
        frame = camera.get_frame()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

# ====================
#  Listening to client
# ====================

# nudged
@cfg.socketio.on('nudge')
def on_nudge():
    cfg.ser.write( b'py&nudge\n' )
    time.sleep(2)

# whisper
@cfg.socketio.on('whisper')
def on_whisper(response):
    cfg.ser.write(b'py&whisper&' + response.encode('utf-8') + b'\n')
    time.sleep(2)


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
    cfg.socketio.run(app, debug=True, port=8888, host='0.0.0.0')