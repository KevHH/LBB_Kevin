import zmq
import time

context = zmq.Context()

shutdown_socket = context.socket(zmq.PAIR)
shutdown_socket.RCVTIMEO = 100
shutdown_socket.connect("tcp://127.0.0.1:6000")

body_socket = context.socket(zmq.PAIR)
body_socket.RCVTIMEO = 200
body_socket.bind("tcp://127.0.0.1:6031")

eyes_socket = context.socket(zmq.PAIR)
eyes_socket.RCVTIMEO = 200
eyes_socket.bind("tcp://127.0.0.1:6030")

preds = None 

counter = 3
led_list = ["r", "g", "b", "y"]


while True:
  # check vision
  try:
    preds = eyes_socket.recv_pyobj()
  except zmq.ZMQError as e:
    if e.errno == zmq.EAGAIN:
        pass  # no message was ready
    else:
        raise  # real error
  else:
    # give instructions
    body_socket.send(b"py&LED&L" + led_list[counter].encode() + b"\n")
    counter = (counter + 1) % 4
    body_socket.send(b"py&LED&H" + led_list[counter].encode() + b"\n")


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
body_socket.close()
context.destroy()