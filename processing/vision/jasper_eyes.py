import io
import zmq
import picamera
from datetime import datetime, timedelta
import time
from PIL import Image
from detect_obj import detect_obj, draw_obj

context = zmq.Context()
socket = context.socket(zmq.PAIR)
socket.bind("tcp://127.0.0.1:6001")

preds = None
detect_interval = timedelta(seconds=2)
prev_time = datetime.now()

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
        img = Image.open(io.BytesIO(frame)).convert("RGB")
        '''
        if (datetime.now()-prev_time >= detect_interval):
            # refresh timer and perform detection
            prev_time = datetime.now()
            preds = detect_obj(img)
        '''
        # draw predictions on image
        img_labelled = draw_obj(img, preds)

        # send to app
        img_encoded = io.BytesIO()
        img_labelled.save(img_encoded, "JPEG")
        socket.send(img_encoded.getvalue())

        # reset stream for next frame
        stream.seek(0)
        stream.truncate()


