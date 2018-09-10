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

def extract_feature(image, path_generator):
    save = path_generator.generate_path()

    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
    data = np.zeros(image.shape)
    image = image.astype(np.uint8)
    data[:, :, 0] = clahe.apply(image[:, :, 0])
    data[:, :, 1] = clahe.apply(image[:, :, 1])
    data[:, :, 2] = clahe.apply(image[:, :, 2])
    image = cv2.cvtColor(data.astype(np.uint8), cv2.COLOR_YCR_CB2RGB)
    image = cv2.resize(image, (256, 256))
    cv2.imwrite(save, image)

    # Predict segmented image.
    call(["./extract_bic", save])
    feature_path = save.replace("ppm", "ppm.txt")
    feature = np.loadtxt(feature_path)
    call(["rm", feature_path])
    call(["rm", save])

    image = cv2.cvtColor(data.astype(np.uint8), cv2.COLOR_RGB2GRAY)
    hog_features = hog(image, orientations=9,
                       pixels_per_cell=(16, 16),
                       cells_per_block=(1, 1),
                       visualise=False)

    return feature


def build_dictionary():
    block_blob_service = BlockBlobService(account_name='hackathonnetshoes', account_key='1syRJGXjNT8s3eZR1lCLTuo7OCDm6LfMDAMNh1ej8Krs8mpwy3EeheTq1bnXCehOTIo0Glffu5MKZhskULys6A==')
    generator = block_blob_service.list_blobs('hackthonns')
    path_generator = PathGenerator()
    idx = 0

    with open('../../grupo2storage2/features/hand_crafted/features.pickle', 'wb') as features_handle:
        with open('../../grupo2storage2/features/hand_crafted/skus.pickle', 'wb') as sku_handle:

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
                feature = extract_feature(image, path_generator)

                sku = os.path.basename(blob.name).split(".")[0]
                pickle.dump(feature, features_handle, protocol=pickle.HIGHEST_PROTOCOL)
                pickle.dump(sku, sku_handle, protocol=pickle.HIGHEST_PROTOCOL)
                idx += 1


def main():
    build_dictionary()


if __name__ == "__main__":
    main()
