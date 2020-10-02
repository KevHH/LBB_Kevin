from serial import Serial
import time
from flask_socketio import SocketIO

def init_once():
    global ser
    global socketio
    global ser_init
    try:
        ser_init
    except NameError:
        ser = Serial('/dev/ttyUSB0', 19200)
        ser_init = True
        socketio = None
        time.sleep(2.00)
