import pickle

import keras
from keras.preprocessing.image import img_to_array
from keras.preprocessing.image import load_img
import numpy as np
import pandas as pd
import scipy as sp
import tensorflow as tf
from path_generator import PathGenerator
from build_dictionary_hand_crafted import extract_feature
from build_dictionary_hand_crafted2 import extract_feature2
from sklearn import preprocessing
import pickle
import json


class Matcher:
    def __init__(self, features_file, hand_features_file, hand_features_file2, skus_file):
        self._model = keras.applications.vgg16.VGG16(weights='imagenet', 
                                                     include_top=False, 
                                                     input_shape=(224, 224, 3),
                                                     pooling='avg')
        self.graph = tf.get_default_graph()

        self._preprocess = keras.applications.vgg16.preprocess_input
        self._load_files(features_file, hand_features_file, hand_features_file2, skus_file)
        self.path_generator = PathGenerator()

    def _load_files(self, features_file, hand_features_file, hand_features_file2, skus_file):
        self._feature_dict=[]
        self._hand_feature_dict=[]
        self._hand_feature_dict2=[]
        self._sku_dict=[]
        
        with (open(hand_features_file, "rb")) as openfile:
            while True:
                try:
                    self._hand_feature_dict.append(pickle.load(openfile))
                except EOFError:
                    break

        with (open(hand_features_file2, "rb")) as openfile:
            while True:
                try:
                    self._hand_feature_dict2.append(pickle.load(openfile))
                except EOFError:
                    break

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
        inputShape = (256, 256)
        image = load_img(file, target_size=inputShape)
        image = img_to_array(image)
        hand_feature = extract_feature(image, self.path_generator)
        hand_feature2 = extract_feature2(image)

        inputShape = (224, 224)
        image = load_img(file, target_size=inputShape)
        image = img_to_array(image)

        bic_feature = hand_feature[:128]
        hog_feature = hand_feature[128:]
        image = np.expand_dims(image, axis=0)
        image = self._preprocess(image)

        with self.graph.as_default():
            feature = self._model.predict(image)

        feature_np = np.array(feature)

        matches = []
        for d_value, h_value, h_value2 in zip(self._feature_dict, self._hand_feature_dict, self._hand_feature_dict2):
            d_bic_feature = h_value[:128]
            d_hog_feature = h_value[128:]
            
            distance_bic = sp.spatial.distance.cityblock(d_bic_feature, bic_feature)
            distance_hog = sp.spatial.distance.cityblock(d_hog_feature, hog_feature)
            distance_dnn = sp.spatial.distance.cityblock(d_value, feature_np)
            distance_h2 = sp.spatial.distance.cityblock(h_value2, hand_feature2)
            
            
            b = 1.0
            h = 0.0
            h2 = 0.00
            d = 0.05
            distance = distance_bic * b + distance_hog * h + distance_dnn * d + distance_h2 * h2
            matches.append(distance)

        dataframe = pd.DataFrame({'sku':self._sku_dict, 'matches':matches})
        dataframe = dataframe.nsmallest(10, 'matches')

        return dataframe['sku'].values.tolist()

    
    def train(self):
        matches = []
        for d_value, h_value, h_value2 in \
                zip(self._feature_dict, self._hand_feature_dict, self._hand_feature_dict2):
            
            feature = np.concatenate([d_value.flatten(), h_value, h_value2])
            matches.append(feature)
            
        matches = np.array(matches)
        self.le = preprocessing.LabelEncoder()
        self.le.fit(self._sku_dict)
        self.classes = self.le.transform(self._sku_dict)
#         self.le.inverse_transform([0, 0, 1, 2])
#         self._sku_dict
    
#         from sklearn import svm
#         self.clf = svm.SVC()
#         self.clf.fit(matches, self.classes)
#         filename = '../../grupo2storage2/clf.sav'
#         pickle.dump(model, open(filename, 'wb'))
    
    def load(self):
        filename = '../../grupo2storage2/clf.sav'
        self.clf = pickle.load(open(filename, 'rb'))
    
    def predict(self, file):
        inputShape = (256, 256)
        image = load_img(file, target_size=inputShape)
        image = img_to_array(image)
        hand_feature = extract_feature(image, self.path_generator)
        hand_feature2 = extract_feature2(image)

        inputShape = (224, 224)
        image = load_img(file, target_size=inputShape)
        image = img_to_array(image)

        bic_feature = hand_feature[:128]
        hog_feature = hand_feature[128:]
        image = np.expand_dims(image, axis=0)
        image = self._preprocess(image)

        with self.graph.as_default():
            feature = self._model.predict(image)

        feature_np = np.array(feature)
        feature = np.concatenate([feature_np.flatten(), hand_feature, hand_feature2])
        print(feature).shape

def main():
    matcher = Matcher("../../grupo2storage2/features/dnn/features.pickle",
                      "../../grupo2storage2/features/hand_crafted/features.pickle",
                      "../../grupo2storage2/features/hand_crafted/features2.pickle",
                      "../../grupo2storage2/features/dnn/skus.pickle")
    matcher.train()
#     matcher.load()
    matcher.predict('/home/grupo2/grupo2storage2/test.jpg')
#     print(matcher.match('/home/grupo2/grupo2storage2/test.jpeg'))
    

if __name__ == "__main__":
    main()

