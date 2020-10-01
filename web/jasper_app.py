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

# others
from datetime import datetime
import time
import threading
import sys

# ============
#  set up 
# ============

async_mode = None
app = Flask(__name__)
socketio = SocketIO(app, async_mode = async_mode)

thread = None
threadlock = threading.Lock()
bglisteners = {}

# ============
#  Routing 
# ============

@app.route('/')
def index():
    return render_template('index.html', async_mode = async_mode)

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
    time.sleep(1.00)

# whisper
@socketio.on('whisper')
def on_whisper(response):
    ser.write(b'py&whisper&' + response.encode('utf-8') + b'\n')
    time.sleep(1.00)


# ======================
#  Listening to Arduino
# ======================

@socketio.on('read_ard')
def on_read_ard():
    if (ser.inWaiting() > 0):
        input = ser.readline()
        args = input.decode('utf-8').split('&')
        print()
        print()
        print(args[2], file=sys.stdout)
        print()
        print()
        if len(args) == 3 and args[0] == "ard":
            # valid serial input from arduino
            event_str = args[1]
            data = args[2]
            if event_str in bglisteners:
                # event detected, execute all callbacks with data as the argument
                for f in bglisteners[event_str]:
                    f(data)



#--------- main ---------------

@socketio.on('connect', namespace='/background')
def run_bg_thread():
    print("TEST")
    global thread
    with threadlock:
        if thread is None:
            thread = socketio.start_background_task(target=bg_thread)
    
def bg_thread():
    socketio.sleep(2)
    emit("update_age", "10", namespace="/background", broadcast=True)    
    '''
    while True:
        print("TEST", file=sys.stdout)
        if (ser.inWaiting() > 0):
            input = ser.readline()
            args = input.decode('utf-8').split('&')
            print()
            print()
            print(args[0], file=sys.stdout)
            print()
            print()
            if len(args) == 3 and args[0] == "ard":
                # valid serial input from arduino
                event_str = args[1]
                data = args[2]
                if event_str in bglisteners:
                    # event detected, execute all callbacks with data as the argument
                    for f in bglisteners[event_str]:
                        f(data)
        time.sleep(0.100)
        '''

def register_bglisteners(event_str, f):
    if event_str in bglisteners:
        bglisteners[event_str].append(f)
    else:
        bglisteners[event_str] = [f]

#--------- listeners --------------- 

def on_update_age(age):
    emit('update_age', age, broadcast=True)

register_bglisteners('update_age', on_update_age)

# ============
#  Main
# ============

if __name__ == '__main__':
    # open serial port
    ser = Serial('/dev/ttyUSB0', 19200)
    time.sleep(1.00)

    # run app
    socketio.run(app, debug=True, port=8888, host='0.0.0.0')