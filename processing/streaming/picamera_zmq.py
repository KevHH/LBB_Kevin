import io
import zmq
import picamera
import time

context = zmq.Context()
flask_socket = context.socket(zmq.PUB)
flask_socket.bind("tcp://127.0.0.1:6667")

with picamera.PiCamera() as camera:
    # let camera warm up
    time.sleep(2)

    camera.rotation = 180

    stream = io.BytesIO()
    for _ in camera.capture_continuous(stream, 'jpeg',
                                            use_video_port=True):
        # return current frame
        stream.seek(0)
        frame = stream.read()
        flask_socket.send(frame)

        # reset stream for next frame
        stream.seek(0)
        stream.truncate()
