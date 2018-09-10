import pickle

import keras
from keras.preprocessing.image import img_to_array
from keras.preprocessing.image import load_img
import numpy as np
import pandas as pd
import scipy as sp
import mxnet as mx
from mxnet import gluon, nd
from mxnet.gluon.model_zoo import vision
from os.path import join


SIZE = (224, 224)
inputShape = (224, 224)

MEAN_IMAGE = mx.nd.array([0.485, 0.456, 0.406])
STD_IMAGE = mx.nd.array([0.229, 0.224, 0.225])
EMBEDDING_SIZE = 512


class Matcher:


    def transform(self, image):
        resized = mx.image.resize_short(image, SIZE[0]).astype('float32')
        cropped, crop_info = mx.image.center_crop(resized, SIZE)
        cropped /= 255.
        normalized = mx.image.color_normalize(cropped,
                                              mean=MEAN_IMAGE,
                                              std=STD_IMAGE)
        transposed = nd.transpose(normalized, (2, 0, 1))
        return transposed



    def __init__(self, features_file, skus_file):
        #self._model = keras.applications.vgg16.VGG16(weights='imagenet',
        #                                             include_top=False,
        #                                             input_shape=(224, 224, 3),
        #                                             pooling='avg')
        #self.graph = tf.get_default_graph()

        self.ctx = mx.gpu() if len(mx.test_utils.list_gpus()) else mx.cpu()
        self.net = vision.resnet18_v2(pretrained=True, ctx=self.ctx).features

        print('finishing initialization')
  #      self._preprocess = keras.applications.vgg16.preprocess_input
        self._load_files(features_file, skus_file)
        #self.path_generator = PathGenerator()



    def _load_files(self, features_file, skus_file):
        self._feature_dict = []
        self._sku_dict = []

        with (open(features_file, "rb")) as openfile:
            while True:
                try:
                    self._feature_dict.append(pickle.load(openfile))
                except EOFError:
                    break

        with (open(skus_file, "rb")) as openfile:
            while True:
                try:
                    self._sku_dict.append(pickle.load(openfile))
                except EOFError:
                    break

    def match(self, file):

        #inputShape = (256, 256)
        #image = load_img(file, target_size=inputShape)
        #image = img_to_array(image)
        #hand_feature = extract_feature(image, self.path_generator)
        #hand_feature2 = extract_feature2(image)

        #image = load_img(file, target_size=inputShape)
        #image = img_to_array(image)

        img = load_img(file)
        img = img_to_array(img)

        img = self.transform(nd.array(img))
        feature = img.expand_dims(axis=0).as_in_context(self.ctx)

        #bic_feature = hand_feature[:128]
        #hog_feature = hand_feature[128:]
        #image = np.expand_dims(image, axis=0)
        #image = self._preprocess(image)

        #with self.graph.as_default():
        #    feature = self._model.predict(image)

        feature_np = np.array(feature)

        matches = []
        for d_value in self._feature_dict:
            distance = sp.spatial.distance.euclidean(d_value, feature_np)
            matches.append(distance)

        dataframe = pd.DataFrame({'sku': self._sku_dict, 'matches': matches})
        dataframe = dataframe.nsmallest(10, 'matches')

        return dataframe['sku'].values.tolist()


def main():
    matcher = Matcher("/Users/pdrglv/Desktop/features_vgg.pickle", "/Users/pdrglv/Desktop/skus_vgg.pickle")
    print(matcher.match('/Users/pdrglv/Desktop/90.jpg'))


if __name__ == "__main__":
    main()

