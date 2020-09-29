'''
    References: 
    - flask-sockietio https://medium.com/hackervalleystudio/weekend-project-part-2-turning-flask-into-a-real-time-websocket-server-using-flask-socketio-ab6b45f1d896
    - streaming https://blog.miguelgrinberg.com/post/video-streaming-with-flask
'''
from flask import Flask, render_template, jsonify, Response
from flask_socketio import SocketIO, join_room, emit, send
from datetime import datetime
from lib.streaming.camera_pi import Camera
app = Flask(__name__)
socketio = SocketIO(app)

@app.route('/')
def index():
    return render_template('index.html')

# streaming video
@app.route('/video_feed')
def video_feed():
    return Response(gen(Camera()),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

@socketio.on('get_time')
def send_time():
    emit('update_time', datetime.now().strftime("%H:%M:%S, %d %B %y"), broadcast=True)

def gen(camera):
    while True:
        frame = camera.get_frame()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

if __name__ == '__main__':
    socketio.run(app, debug=True, port=8888, host='0.0.0.0')