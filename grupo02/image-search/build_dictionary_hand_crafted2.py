from keras.preprocessing.image import img_to_array
from keras.preprocessing.image import load_img
import numpy as np
from azure.storage.blob import (BlockBlobService)
import io
import os
import pickle
from path_generator import PathGenerator
import cv2
from subprocess import call
from skimage.feature import hog


def extract_feature2(image):
    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
    data = np.zeros(image.shape)
    image = image.astype(np.uint8)
    data[:, :, 0] = clahe.apply(image[:, :, 0])
    data[:, :, 1] = clahe.apply(image[:, :, 1])
    data[:, :, 2] = clahe.apply(image[:, :, 2])
    image = cv2.cvtColor(data.astype(np.uint8), cv2.COLOR_YCR_CB2RGB)
                
    chans = cv2.split(image)
    colors = ("b", "g", "r")
    features = []

    for (chan, color) in zip(chans, colors):
        # create a histogram for the current channel and
        # concatenate the resulting histograms for each
        # channel
        hist = cv2.calcHist([chan], [0], None, [256], [0, 256])
        features.extend(hist)

    hist, _ = np.histogram(np.array(features).flatten(), normed = True, bins = 758)

    return np.array(features).flatten()


def build_dictionary():
    block_blob_service = BlockBlobService(account_name='hackathonnetshoes', account_key='1syRJGXjNT8s3eZR1lCLTuo7OCDm6LfMDAMNh1ej8Krs8mpwy3EeheTq1bnXCehOTIo0Glffu5MKZhskULys6A==')
    generator = block_blob_service.list_blobs('hackthonns')
    path_generator = PathGenerator()
    idx = 0

    with open('../../grupo2storage2/features/hand_crafted/features2.pickle', 'wb') as features_handle:
        with open('../../grupo2storage2/features/hand_crafted/skus2.pickle', 'wb') as sku_handle:

            for blob in generator:
                if (not blob.name.endswith('.jpg')):
                    continue

                if ('produtos' not in blob.name):
                    continue

                print('[{0}] processing {1}'.format(idx, os.path.basename(blob.name)))
                blob = block_blob_service.get_blob_to_bytes('hackthonns', blob.name)

                image_file_in_mem = io.BytesIO(blob.content)
                inputShape = (300, 300)
                img = load_img(image_file_in_mem)
                image = img_to_array(img)
                feature = extract_feature2(image)

                sku = os.path.basename(blob.name).split(".")[0]
                pickle.dump(feature, features_handle, protocol=pickle.HIGHEST_PROTOCOL)
                pickle.dump(sku, sku_handle, protocol=pickle.HIGHEST_PROTOCOL)
                idx += 1


def main():
    build_dictionary()


if __name__ == "__main__":
    main()
