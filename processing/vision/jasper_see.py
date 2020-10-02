'''
    Object dection
'''

from edgetpu.detection.engine import DetectionEngine
from edgetpu.utils import dataset_utils
from PIL import Image
import io
import zmq

context = zmq.Context()

shutdown_socket = context.socket(zmq.PAIR)
shutdown_socket.RCVTIMEO = 100
shutdown_socket.connect("tcp://127.0.0.1:6000")

eyes_socket = context.socket(zmq.PAIR)
eyes_socket.setsockopt(zmq.LINGER, 0) # do not queue message up when jasper_app is disconnected
eyes_socket.RCVTIMEO = 200
eyes_socket.bind("tcp://127.0.0.1:6011")

preds = None

while True:
  # receive
  try:
    frame = eyes_socket.recv()
  except zmq.ZMQError as e:
    if e.errno == zmq.EAGAIN:
        pass  # no message was ready
    else:
        raise  # real error
  else:
    img = Image.open(io.BytesIO(frame)).convert("RGB")

    # setup
    model_dir = "models/mobilenet_v2_coco/"
    label_file = "coco_labels.txt"
    model_file = "mobilenet_ssd_v2_coco_quant_postprocess_edgetpu.tflite"

    # Initialize engine.
    engine = DetectionEngine( model_dir + model_file )
    labels = dataset_utils.read_label_file( model_dir + label_file )

    # Run inference.
    objs = engine.detect_with_image(img,
                                    threshold=0.5,
                                    keep_aspect_ratio=True,
                                    relative_coord=False,
                                    top_k=10)
                                    
    preds = [(labels[obj.label_id], obj.score, obj.bounding_box.flatten().tolist()) 
            for obj in objs]
  
  # send
  try:
    eyes_socket.send_pyobj(preds, flags=zmq.NOBLOCK)
  except zmq.ZMQError as e:
    if e.errno == zmq.EAGAIN:
        pass  # no message was ready
    else:
        raise  # real error

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
eyes_socket.close()
context.destroy()