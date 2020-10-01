'''
    Object dection via picamera
'''

from edgetpu.detection.engine import DetectionEngine
from edgetpu.utils import dataset_utils
from PIL import Image, ImageFont, ImageDraw


def detect_obj(img):
  # setup
  model_dir = "models/mobilenet_v2_coco/"
  label_file = "coco_labels.txt"
  model_file = "mobilenet_ssd_v2_coco_quant_postprocess_edgetpu.tflite"
  font = ImageFont.truetype("assets/fonts/BalooTammudu2-Regular.ttf", 16)

  # Initialize engine.
  engine = DetectionEngine( model_dir + model_file )
  labels = dataset_utils.read_label_file( model_dir + label_file )

  # Open image.
  img = Image.open(model_dir + "bird.bmp").convert('RGB')
  draw = ImageDraw.Draw(img)

  # Run inference.
  objs = engine.detect_with_image(img,
                                  threshold=0.5,
                                  keep_aspect_ratio=True,
                                  relative_coord=False,
                                  top_k=10)
                                  
  # Print and draw detected objects.
  for obj in objs:
    obj_label = labels[obj.label_id]
    print('-----------------------------------------')
    print(obj_label)
    print('score =', obj.score)
    box = obj.bounding_box.flatten().tolist()
    print('box =', box)
    draw.rectangle(box, outline='red', width=2)
    w, h = font.getsize(obj_label)
    draw.rectangle((box[0], box[1], box[0] + w, box[1] + h), fill='red')
    draw.text((box[0], box[1]), labels[obj.label_id], font=font)

  return 