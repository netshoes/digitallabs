'''
Segmentation service.
'''

from flask import Flask, request, jsonify
import os
from setuptools.sandbox import save_path
import urllib.request

from dnn import SegmentationDNN
from path_generator import PathGenerator
import cv2

DIR_PATH = "/home/grupo2/grupo2storage2/segmentation/"
TMP_PATH = "/home/grupo2/eldshoes/segmentation/tmp/"
URL_BASE = "https://grupo2storage2.blob.core.windows.net/grupo2cont" \
    "/segmentation/"

# Create Flask application.
path_generator = PathGenerator()
dnn = SegmentationDNN()
app = Flask(__name__)


@app.route('/segmentation', methods=['POST'])
def segmentationHandler():
    """ Segment image from url.
    """

    # Load data from json.
    try:
        content = request.get_json()
        url = content["url"]
        file_name = url.split('/')[-1]
        new_filename = path_generator.generate_path(file_name)
        tmp_path = str(os.path.join(TMP_PATH, new_filename))
        urllib.request.urlretrieve(url, tmp_path)
        img = cv2.imread(tmp_path)

        # Predict segmented image.
        img_cut = dnn.predict(img)

        # Save image segmented.
        new_filename = new_filename.split(".")[0] + ".jpg"
        save_path = str(os.path.join(DIR_PATH, new_filename))
        cv2.imwrite(save_path, img_cut)

        # Set result.
        result_url = URL_BASE + new_filename
        json_data = {"url": result_url}

    except Exception as e:
        print(e)
        json_data = {
            "url": "",
            "error": "Invalid json data."}

    result = jsonify(json_data)
    return result


# Run service on port 8090.
app.run(host='0.0.0.0', port=8090)
