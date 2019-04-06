from keras.preprocessing.image import img_to_array
from keras.preprocessing.image import load_img
import numpy as np
from azure.storage.blob import (BlockBlobService)
import io
import os
import keras
import pickle


def build_dictionary():
    model = keras.applications.vgg16.VGG16(weights='imagenet', include_top=False, input_shape=(224, 224, 3), pooling='avg')

    block_blob_service = BlockBlobService(account_name='hackathonnetshoes',account_key='1syRJGXjNT8s3eZR1lCLTuo7OCDm6LfMDAMNh1ej8Krs8mpwy3EeheTq1bnXCehOTIo0Glffu5MKZhskULys6A==')
    generator = block_blob_service.list_blobs('hackthonns')

    idx = 0

    with open('../../../grupo2storage2/features/dnn/features.pickle', 'wb') as features_handle:
        with open('../../../grupo2storage2/features/dnn/skus.pickle', 'wb') as sku_handle:

            for blob in generator:

                if (not blob.name.endswith('.jpg')):
                    continue

                if ('produtos' not in blob.name):
                    continue

                print('[{0}] processing {1}'.format(idx, os.path.basename(blob.name)))
                blob = block_blob_service.get_blob_to_bytes('hackthonns', blob.name)

                inputShape = (224, 224)

                image_file_in_mem = io.BytesIO(blob.content)
                img = load_img(image_file_in_mem, target_size=inputShape)
                image = img_to_array(img)
                image = np.expand_dims(image, axis=0)
                image = keras.applications.vgg16.preprocess_input(image)
                feature = model.predict(image)

                sku = os.path.basename(blob.name).split[0]

                pickle.dump(feature, features_handle, protocol=pickle.HIGHEST_PROTOCOL)
                pickle.dump(sku, sku_handle, protocol=pickle.HIGHEST_PROTOCOL)

                idx+=1


def main():
    build_dictionary()

if __name__ == "__main__":
    main()