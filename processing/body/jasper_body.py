from serial import Serial
import zmq
import cfg
import time

cfg.init_once()

context = zmq.Context()

shutdown_socket = context.socket(zmq.PAIR)
shutdown_socket.RCVTIMEO = 100
shutdown_socket.connect("tcp://127.0.0.1:6000")

app_socket = context.socket(zmq.PAIR)
app_socket.RCVTIMEO = 200
app_socket.bind("tcp://127.0.0.1:6020")

brain_socket = context.socket(zmq.PAIR)
brain_socket.RCVTIMEO = 200
brain_socket.setsockopt(zmq.LINGER, 0) # do not queue message up when jasper_app is disconnected
brain_socket.connect("tcp://127.0.0.1:6031")

while True:
  # receive from app and send to arduino
  try:
    msg = app_socket.recv()
  except zmq.ZMQError as e:
    if e.errno == zmq.EAGAIN:
        pass  # no message was ready
    else:
        raise  # real error
  else:
    cfg.ser.write(msg)  
    print("APP SENT: " + msg.decode('utf-8'))

  # receive from brain and send to arduino
  try:
    msg = brain_socket.recv()
  except zmq.ZMQError as e:
    if e.errno == zmq.EAGAIN:
        pass  # no message was ready
    else:
        raise  # real error
  else:
    cfg.ser.write(msg)  
    print("BRAIN SENT: " + msg.decode('utf-8'))

  # listen to arduino
  if (cfg.ser.inWaiting() > 0):
    input = cfg.ser.readline()
    #log
    print("RECEIVED: " + input.decode('utf-8'))

    args = input.decode('utf-8').split('&')
    if len(args) == 3 and args[0] == "ard":
        # valid serial input from arduino
        event_str = args[1]
        data = args[2]
        # send to brain
        try:
            brain_socket.send_pyobj((event_str, data), flags=zmq.NOBLOCK)
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
app_socket.close()
context.destroy()