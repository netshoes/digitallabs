import json
import requests

from flask import Flask, abort, request, jsonify
from requests import HTTPError
from cropper import Croper
from map import MapFile

app = Flask(__name__)

vision_base_url = "https://westcentralus.api.cognitive.microsoft.com/vision/v2.0/"
analyze_url = vision_base_url + "analyze"

segmentation_base_url = "http://40.74.233.210:8090/"
segmentation_url = segmentation_base_url + "segmentation"

matcher_base_url = "http://40.121.107.139:8090/"
matcher_url = matcher_base_url + "matcher"

subscription_key = "d36e250b867a4360baafc18ac5efd849"
container_search = 'search'

HEADERS = {'Content-Type': 'application/json'}
DATA_PATH = "/home/grupo2/grupo2storage2/img-data/"
DIR_PATH = "/home/grupo2/grupo2storage2/search/"
TMP_PATH = "/home/grupo2/grupo2storage2/search/tmp/"

STORAGE_DATA_URI = 'https://grupo2storage2.blob.core.windows.net/grupo2cont/img-data'
STORAGE_URI = 'https://hackathonnetshoes.blob.core.windows.net/hackthonns/produtos'

# mapping = MapFile()

@app.route('/')
def root():
    return json.dumps({'version', 0.1})


@app.route('/search', methods=['POST'])
def search():
    if not request.json:
        abort(400)

    m_json = request.get_json()

    cropped = Croper(DIR_PATH, TMP_PATH).crop_image(m_json['url'])

    for crop in cropped:
        if crop['tag'] == 'T-shirt':
            m_json = {'url': crop['url']}
            break

    seg_json = request_segmented_pic(m_json)
    matched = request_product_matches(seg_json)

    data_sku = []

    for sku in matched['skus']:
        data = {}
        data['sku_code'] = sku
        data['image_url'] = STORAGE_DATA_URI + '/' + sku + ".jpg"
        data_sku.append(data)

    result = {}
    result['skus'] = data_sku
    result['crop'] = cropped
    result['context'] = fetch_context(m_json['url'])

    return jsonify(result)


@app.route('/search-context', methods=['POST'])
def search_context():
    if not request.json:
        abort(400)

    if not request.json['url']:
        abort(400)

    image_url = request.json['url']
    return json.dumps(fetch_context(image_url))




def fetch_context(url):
    headers = {'Ocp-Apim-Subscription-Key': subscription_key}
    params = {'visualFeatures': 'Categories,Description,Color'}
    data = {'url': url}

    response = requests.post(analyze_url, headers=headers, params=params, json=data)
    response.raise_for_status()

    analysis = response.json()
    return analysis


def request_segmented_pic(url_json):
    segmented = requests.post(segmentation_url, data=json.dumps(url_json), headers=HEADERS)

    try:
        segmented.raise_for_status()
    except HTTPError:
        abort(400)

    return segmented.json()


def request_product_matches(url_json):
    matched = requests.post(matcher_url, json.dumps(url_json), headers=HEADERS)

    try:
        matched.raise_for_status()
    except HTTPError:
        abort(400)

    return matched.json()


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8090)



  #