from keras.preprocessing.image import img_to_array
from keras.preprocessing.image import load_img
import numpy as np
from azure.storage.blob import (BlockBlobService)
import io
import os
import keras
import pickle
import mxnet as mx
from mxnet import gluon, nd
from mxnet.gluon.model_zoo import vision
from os.path import join


SIZE = (224,224)
inputShape = (224, 224)

MEAN_IMAGE= mx.nd.array([0.485, 0.456, 0.406])
STD_IMAGE = mx.nd.array([0.229, 0.224, 0.225])


def transform(image):
    resized = mx.image.resize_short(image, SIZE[0]).astype('float32')
    cropped, crop_info = mx.image.center_crop(resized, SIZE)
    cropped /= 255.
    normalized = mx.image.color_normalize(cropped,
                                      mean=MEAN_IMAGE,
                                      std=STD_IMAGE)
    transposed = nd.transpose(normalized, (2,0,1))
    return transposed

def build_dictionary():
    #model = keras.applications.vgg16.VGG16(weights='imagenet', include_top=False, input_shape=(224, 224, 3), pooling='avg')

    ctx = mx.gpu() if len(mx.test_utils.list_gpus()) else mx.cpu()
    net = vision.resnet18_v2(pretrained=True, ctx=ctx).features

    net.hybridize()
    net(mx.nd.ones((1, 3, 224, 224), ctx=ctx))
    net.export(join('mms', 'visualsearch'))

    block_blob_service = BlockBlobService(account_name='hackathonnetshoes',account_key='1syRJGXjNT8s3eZR1lCLTuo7OCDm6LfMDAMNh1ej8Krs8mpwy3EeheTq1bnXCehOTIo0Glffu5MKZhskULys6A==')
    generator = block_blob_service.list_blobs('hackthonns')

    idx = 0

    with open('/../../grupo2storage2/features/dnn/features_mx.pickle', 'wb') as features_handle:
        with open('/../../grupo2storage2/features/dnn/skus_mx.pickle', 'wb') as sku_handle:

            for blob in generator:

                if (not blob.name.endswith('.jpg')):
                    continue

                if ('produtos' not in blob.name):
                    continue

                print('[{0}] processing {1}'.format(idx, os.path.basename(blob.name)))
                blob = block_blob_service.get_blob_to_bytes('hackthonns', blob.name)


                image_file_in_mem = io.BytesIO(blob.content)
                img = load_img(image_file_in_mem)
                img = img_to_array(img)

                img = transform(nd.array(img))
                feature = img.expand_dims(axis=0).as_in_context(ctx)

                print(feature)

                sku = os.path.basename(blob.name).split('.')[0]

                pickle.dump(feature, features_handle, protocol=pickle.HIGHEST_PROTOCOL)
                pickle.dump(sku, sku_handle, protocol=pickle.HIGHEST_PROTOCOL)

                idx+=1


def main():
    build_dictionary()

if __name__ == "__main__":
    main()