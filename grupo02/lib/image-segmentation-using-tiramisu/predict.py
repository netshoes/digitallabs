from keras.models import load_model
import sys
import cv2
import numpy as np

model = load_model('TiramisuSmallModel_20180225.h5')
print('model loaded')

img_file = sys.argv[1]

size = 224
original_img = cv2.imread(img_file)
original_height = original_img.shape[0]
original_width = original_img.shape[1]

img = cv2.resize(original_img, (size, size))
img = np.reshape(img, [1, size, size, 3])
preds = model.predict(img)
preds = preds.reshape((size, size))
# preds = cv2.resize(img, (original_width, original_height))


path = img_file.split(".")
save = path[0] + "_result." + path[1]
cv2.imwrite(save, preds)
