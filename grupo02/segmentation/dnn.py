import cv2
import numpy as np
import sys
import pandas as pd
import keras
from keras.models import Sequential
from keras.layers import Conv2D

from keras.layers import MaxPooling2D
from keras.layers import Dense

from keras.layers import Flatten, Dropout
from keras.models import model_from_json
import tensorflow as tf


class SegmentationDNN:
    LIB_PATH = "/home/grupo2/eldshoes/lib/Fashion-AI-segmentation/"
    JSON_PATH = LIB_PATH + "/model/binaryfas10.json"
    MODEL_PATH = LIB_PATH + "/model/binaryfashion.h5"
    ANNOTATION_PATH = LIB_PATH + "annotation.csv"
    IMAGE_WIDTH = 300
    IMAGE_HEIGHT = 500

    def __init__(self):
        json_file = open(self.JSON_PATH)
        loaded_model_json = json_file.read()
        json_file.close()
        self.loaded_model = model_from_json(loaded_model_json)
        self.graph = tf.get_default_graph()
        self.loaded_model.load_weights(self.MODEL_PATH)
        self.reader = pd.read_csv(self.ANNOTATION_PATH)

    def predict(self, img):
        resize = cv2.resize(img, (64, 64))
        img_fin = np.reshape(resize, [1, 64, 64, 3])
        with self.graph.as_default():
            prediction = self.loaded_model.predict_classes(img_fin)
        prediction = np.squeeze(prediction, axis=1)
        predict = int(np.squeeze(prediction, axis=0))

    #    img = fit_width(img, image_width, image_height)
        image_width = img.shape[1]
        image_height = img.shape[0]
        img = cv2.resize(img, (self.IMAGE_WIDTH, self.IMAGE_HEIGHT))

        mask = np.zeros(img.shape[:2], np.uint8)
        bgdModel = np.zeros((1, 65), np.float64)
        fgdModel = np.zeros((1, 65), np.float64)
        rect = (self.reader.x1[predict], self.reader.y1[predict],
                self.reader.x2[predict], self.reader.y2[predict])
        cv2.grabCut(img, mask, rect, bgdModel, fgdModel,
                    self.reader.i[predict], cv2.GC_INIT_WITH_RECT)
        mask2 = np.where((mask == 2) | (mask == 0), 255, 0).astype('uint8')
        img_cut = np.maximum(img, mask2[:, :, np.newaxis])

        img_cut = cv2.resize(img_cut, (image_width, image_height))

        return img_cut

# def fit_width(image, width, height):
#     """
#     Fit image to specified size.
#     """
#
#     scale = width * 1.0 / image.shape[1]
#     new_height = int(image.shape[0] * scale)
#
#     if new_height < height:
#         resized_image = cv2.resize(image, (width, new_height), None, 0.0, 0.0,
#                                    interpolation=cv2.INTER_CUBIC)
#
#         diff = (height - new_height) / 2
#         empty_image = np.ones((diff, width, 3)) * 255
#         resized_image = np.vstack((empty_image, resized_image,
#                                   empty_image))
#     else:
#         scale = height * 1.0 / image.shape[0]
#         new_width = int(image.shape[1] * scale)
#         resized_image = cv2.resize(image, (new_width, height),
#                                    None, 0.0, 0.0,
#                                    interpolation=cv2.INTER_CUBIC)
#
#         diff = (width - new_width) / 2
#
#         if diff > 0:
#             empty_image = np.ones((height, diff, 3)) * 255
#             resized_image = np.hstack(
#                 (empty_image, resized_image, empty_image))
#
#     return resized_image.astype('uint8')


if __name__ == '__main__':
    dnn = SegmentationDNN()

    # Read image.
    img_file = sys.argv[1]
    img = cv2.imread(img_file)

    # Predict segmented image.
    img_cut = dnn.predict(img)

    # Save image segmented.
    path = img_file.split(".")
    save = path[0] + "_result." + path[1]
    cv2.imwrite(save, img_cut)
