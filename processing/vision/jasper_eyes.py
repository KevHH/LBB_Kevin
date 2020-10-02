import io
import picamera
import zmq
import time
from PIL import Image, ImageFont, ImageDraw

context = zmq.Context()

shutdown_socket = context.socket(zmq.PAIR)
shutdown_socket.RCVTIMEO = 100
shutdown_socket.connect("tcp://127.0.0.1:6000")

app_socket = context.socket(zmq.PAIR)
app_socket.setsockopt(zmq.LINGER, 0) # do not queue message up when jasper_app is disconnected
app_socket.bind("tcp://127.0.0.1:6010")

see_socket = context.socket(zmq.PAIR)
see_socket.RCVTIMEO = 200
see_socket.connect("tcp://127.0.0.1:6011")

brain_socket = context.socket(zmq.PAIR)
brain_socket.setsockopt(zmq.LINGER, 0) # do not queue message up when jasper_app is disconnected
brain_socket.connect("tcp://127.0.0.1:6030")

preds = None

def draw_obj(frame, preds):
  '''
    input: frame, predictions
    output: frame with bounding boxes
  '''
  if preds is None:
    return frame
  if (len(preds) == 0):
    return frame

  # convert to image
  img = Image.open(io.BytesIO(frame)).convert("RGB")

  # Set up draw
  font = ImageFont.truetype("fonts/BalooTammudu2-Regular.ttf", 30)
  draw = ImageDraw.Draw(img)

  # Print and draw detected objects.
  for l, s, box in preds:
    draw.rectangle(box, outline='red', width=2)
    text = l + ", " + "{0:.2f}".format(s)
    w, h = font.getsize(text)
    draw.rectangle((box[0], box[1], box[0] + w, box[1] + h), fill='red')
    draw.text((box[0], box[1]), text, font=font)

  # convert to byte frame
  img_encoded = io.BytesIO()
  img.save(img_encoded, "JPEG")
  return img_encoded.getvalue()

if __name__ == '__main__':
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
          
          # get last detection
          try: 
            preds = see_socket.recv_pyobj()
          except zmq.ZMQError as e:
            if e.errno == zmq.EAGAIN:
                pass  # no message was ready
            else:
                raise  # real error
          else:
            # send new predictions to brain
            try:
              brain_socket.send_pyobj(preds, flags=zmq.NOBLOCK)
            except zmq.ZMQError as e:
              if e.errno == zmq.EAGAIN:
                  pass  # no message was ready
              else:
                  raise  # real error

          # send new image in for detection
          try: 
            see_socket.send(frame, flags=zmq.NOBLOCK)
          except zmq.ZMQError as e:
            if e.errno == zmq.EAGAIN:
                pass  # no message was ready
            else:
                raise  # real error

          
          # draw predictions on frame
          frame_labelled = draw_obj(frame, preds)

          # send to app
          try:
            app_socket.send(frame_labelled, flags=zmq.NOBLOCK)
          except zmq.ZMQError as e:
            if e.errno == zmq.EAGAIN:
                pass  # no message was ready
            else:
                raise  # real error

          # reset stream for next frame
          stream.seek(0)
          stream.truncate()

          # detect shutdown
          try: 
            shutdown_msg = shutdown_socket.recv_string()
          except zmq.ZMQError as e:
            if e.errno == zmq.EAGAIN:
                pass  # no message was ready
            else:
                raise  # real error
          else:
            if shutdown_msg == "shutdown":
              break

  # termination
  shutdown_socket.close()
  see_socket.close()
  app_socket.close()
  context.destroy()


