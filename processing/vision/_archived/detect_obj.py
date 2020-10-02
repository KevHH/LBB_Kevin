'''
    Object dection
'''

from edgetpu.detection.engine import DetectionEngine
from edgetpu.utils import dataset_utils
from PIL import Image, ImageFont, ImageDraw

def detect_obj(img):
  '''
    input: PIL Image (RGB)
    output: predictions list [(label, score, box coords)]
  '''
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
  
  return preds

def draw_obj(img, preds):
  '''
    input: PIL Image (RGB), predictions
    output: PIL Image (RGB) with bounding boxes
  '''
  if preds is None:
    return img
  if (len(preds) == 0):
    return img
    
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

  return img