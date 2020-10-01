import time
from imutils.video import VideoStream
import imagezmq

sender = imagezmq.ImageSender()

picam = VideoStream(usePiCamera=True).start()
time.sleep(2.0)

while True:
    img = picam.read()
    sender.send_image("JasperEyes", img)