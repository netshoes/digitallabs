import pandas as pd
import numpy as np
import os
import requests, json
import cv2
import argparse

from outliers import smirnov_grubbs as grubbs

from sklearn import neighbors
from sklearn.externals import joblib

from keras import backend as K

K.clear_session()

import inception_v4
import json
from flask import Flask, request
from flask_cors import CORS

app = Flask(__name__) 
app.config['DEBUG'] = False
app.config['THREADED'] = False
CORS(app)

intermediate_layer_model = None

def auxRectangles(top_left, bottom_right, shape):
    new_top_left = int(shape[1]*top_left['x']),int(shape[0]*top_left['y'])
    new_bottom_right = int(shape[1]*bottom_right['x']),int(shape[0]*bottom_right['y'])
    return(new_top_left, new_bottom_right)

def crop_outfit(img):
    #img = cv2.imread(file_path)

    BASE_URI = 'https://api.cognitive.microsoft.com/bing/v7.0/images/visualsearch'
    HEADERS = {'Ocp-Apim-Subscription-Key': 'f62f5db4efa6496995702b547caa8386'}

    cv2.imwrite('image_crop.jpg',img)

    file = {'image' : ('myfile', open('image_crop.jpg', 'rb'))}

    response = requests.post(BASE_URI, headers=HEADERS, files=file)
    response.raise_for_status()
    
    resultados={}
    for element in response.json()['tags']:
        if np.shape(element['actions'])[0] == 2:
            if element['actions'][0]['actionType'] == 'VisualSearch':
                top_left, bottom_right = auxRectangles(element['boundingBox']['queryRectangle']['topLeft'],\
                                               element['boundingBox']['queryRectangle']['bottomRight'],\
                                               img.shape)
                resultados[element['displayName']]={'topLeft':top_left,'bottomRight':bottom_right}
            
    img_crops = []
    for key in resultados.keys():
        tl = resultados[key]['topLeft']
        br = resultados[key]['bottomRight']
        x,y=tl
        w = br[0]-tl[0]
        h = br[1]-tl[1]
        img_crops.append(img[y:y+h, x:x+w])

    #for i, image in enumerate(img_crops):
    #    cv2.imwrite(str(i)+'.png',image)
    if len(img_crops) != 0:
        return img_crops
    else:
        return [img]

def get_processed_image(img_crop):
    # Load image and convert from BGR to RGB
    im = np.asarray(img_crop)[:,:,::-1]
    im = cv2.resize(im, (299, 299))
    im = inception_v4.preprocess_input(im)
    if K.image_data_format() == "channels_first":
        im = np.transpose(im, (2,0,1))
        im = im.reshape(-1,3,299,299)
    else:
        im = im.reshape(-1,299,299,3)
    return im


def extract_features(img_crops, intermediate_layer_model):

    features = pd.DataFrame(np.zeros((len(img_crops),1536)))

    for i, img_crop in enumerate(img_crops):
        img = get_processed_image(img_crop)
        intermediate_output = intermediate_layer_model.predict(img)
        features.iloc[i,:] = np.squeeze(intermediate_output)
    
    return features

@app.route('/')
def hello_wor():
    return 'Hello John!'

@app.route("/api/predict", methods=['POST']) 
def predict_api():

    try:
        #encoded = request.get_json(force=True)
        r = request
    except Exception as e:
        raise e

    if not r:
        return(bad_request())
    else:
        #get user browser_id for recommendation prediction
        #encoded = post_data['image']
        #print(encoded)
        
        #b64_string = encoded.decode()
        # convert string of image data to uint8
        nparr = np.fromstring(r.data, np.uint8)
        # decode image
        img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
        # reconstruct image as an numpy array
        #img = imread(io.BytesIO(base64.b64decode(encoded)))
        #img = base64.b64decode(encoded)
        #print(img)
        #nparr = np.fromstring(encoded, np.base64)
        #print(nparr)
        # decode image
        #img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
        #print(img)
        #use visual search to create a list of cropped images
        img_crops = crop_outfit(img)

        #extract features from crops and save in pandas df
        features = extract_features(img_crops, intermediate_layer_model)

        #load similarity model and SKUs
        SKUs = pd.read_csv('dataset_netshoes.csv', index_col=False)
    
        kd_tree = joblib.load('kd_tree.pkl')

        crops_inds = []
        for i in range(len(features)):
            dist, ind = kd_tree.query([features.iloc[i,:]], k=6)
            crops_inds.append(ind)

        dados_preco = pd.read_csv('dados_preco.csv',index_col=False)
        SKUs = SKUs.iloc[crops_inds[0][0]]
        dados_preco = dados_preco[dados_preco.COD.isin(np.asarray(SKUs['COD']))]
        dados_preco = dados_preco.reset_index(drop=True)
        dados_preco = dados_preco.drop(grubbs.max_test_indices(dados_preco.VALOR_UNIDADE, alpha=0.05),axis=0)

        SKUs = SKUs.merge(dados_preco, on='COD', how='left').drop(['Unnamed: 0','VALOR_UNIDADE_y'],axis=1)
        print(SKUs)
        SKUs.columns = ['COD','image_serialized','VALOR_UNIDADE']

        #output response with recommendations
        responses = SKUs.to_json()
        #responses = {}
        #for i in range(len(SKUs)):
        #    responses[SKUs.iloc[i,0]] = SKUs.iloc[i,1]
        #responses.status_code = 200

        return responses
 
if __name__ == '__main__':
    
    intermediate_layer_model = inception_v4.create_model(weights='vectorizer', include_top=True)
    
    app.run(host='0.0.0.0', port=5000)
    #parser = argparse.ArgumentParser(description="simple script to demonstrate argparse usage")
    #parser.add_argument('file_path', help="The string to be printed")
    #arguments = parser.parse_args()
    
    #img = cv2.imread(arguments.file_path)
    # encode image as jpeg
    #_, img_encoded = cv2.imencode('.jpg', img)

    #encoded = img_encoded.tostring()

    # Create cnn model and load pre-trained weights
    
    
    #SKUs.to_csv('output.csv',index=False)