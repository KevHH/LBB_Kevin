'''
    References: 
    - flask-sockietio https://medium.com/hackervalleystudio/weekend-project-part-2-turning-flask-into-a-real-time-websocket-server-using-flask-socketio-ab6b45f1d896
    - streaming https://blog.miguelgrinberg.com/post/video-streaming-with-flask
'''
# web interface
from flask import Flask, render_template, jsonify, Response
from flask_socketio import SocketIO, join_room, emit, send

# hardware interactions
from lib.streaming.camera_pi import Camera
from serial import Serial

# system
import time
import sys
sys.path.append("..")

# software processing
from processing.stats import get_age


# ===========
#  set up 
# ============

app = Flask(__name__)
socketio = SocketIO(app)

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
@socketio.on('nudge')
def on_nudge():
    ser.write( b'py&nudge\n' )
    time.sleep(0.2)

# whisper
@socketio.on('whisper')
def on_whisper(response):
    ser.write(b'py&whisper&' + response.encode('utf-8') + b'\n')
    time.sleep(0.2)

# ============
#  Main
# ============

if __name__ == '__main__':
    # open serial port
    ser = Serial('/dev/ttyUSB0', 19200)
    time.sleep(1.00)

    # run app
    socketio.run(app, debug=True, port=8888, host='0.0.0.0')