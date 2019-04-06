from path_generator import PathGenerator
from urllib import request
from PIL import Image
import requests
import json
import numpy as np

SUBSCRIPTION_KEY = 'dfcc754dec614553a36066c96ab7f0b3'
BASE_URI = 'https://api.cognitive.microsoft.com/bing/v7.0/images/visualsearch'
STORAGE_URI = 'https://grupo2storage2.blob.core.windows.net/grupo2cont/search'

path_generator = PathGenerator()

class Croper:
    def __init__(self, data_path, temp_path):
        self.data_path = data_path
        self.temp_path = temp_path

    def print_json(obj):
        """Print the object as json"""
        print(json.dumps(obj, sort_keys=True, indent=2, separators=(',', ': ')))

    def calculate_bounding_box_coordinates(self, float_top_left, float_bottom_right, shape):
        int_top_left = (int(shape[1] * float_top_left['x']), int(shape[0] * float_top_left['y']))
        int_bottom_right = (int(shape[1] * float_bottom_right['x']), int(shape[0] * float_bottom_right['y']))
        return (int_top_left, int_bottom_right)

    def crop_image_products(self, img, products_visual_search):
        products_visual_search['croppedProduct'] = []
        for idx, ((top_left_x, top_left_y), (bottom_right_x, bottom_right_y)) in enumerate(
                products_visual_search['boundingBox']):
            img_product = img[top_left_y:bottom_right_y, top_left_x:bottom_right_x, :].copy()
            products_visual_search['croppedProduct'].append(img_product)

        return products_visual_search

    def find_product_visual_search(self, img, visual_search_json):
        products_visual_search = []

        crop_id = 0

        for element in visual_search_json['tags']:
            for action in element['actions']:
                if 'boundingBox' in element:
                    data = {}
                    data['tag'] = element['displayName']
                    data['boundingBox'] = self.calculate_bounding_box_coordinates(
                        element['boundingBox']['queryRectangle']['topLeft'],
                        element['boundingBox']['queryRectangle']['bottomRight'], img.shape)
                    data['crop_id'] = crop_id
                    crop_id += 1

                    if data['tag'] == "##TextRecognition":
                        data['text'] = element['actions'][0]['displayName']

                    products_visual_search.append(data)

        return products_visual_search

    def crop_image(self, img_url):
        file_name = img_url.split('/')[-1]
        file_path = path_generator.generate_path(file_name, directory=self.temp_path)
        f = open(file_path, 'wb')
        f.write(request.urlopen(img_url).read())
        f.close()

        HEADERS = {'Ocp-Apim-Subscription-Key': SUBSCRIPTION_KEY}

        file = {'image': ('myfile', open(file_path, 'rb'))}
        response = requests.post(BASE_URI, headers=HEADERS, files=file)
        response.raise_for_status()
        img = load_image(file_path)

        products = self.find_product_visual_search(img, response.json())

        for element in range(len(products)):
            b_box = []
            b_box.append(products[element]['boundingBox'][0][0])
            b_box.append(products[element]['boundingBox'][0][1])
            b_box.append(products[element]['boundingBox'][1][0])
            b_box.append(products[element]['boundingBox'][1][1])

            img = Image.open(file_path)
            cropped_img = img.crop(b_box)
            filename = file_path.split('.')[0] + '_crop_{0}.jpg'.format(element)
            cropped_img.save(filename, "JPEG", quality=80)
            products[element]['url'] = STORAGE_URI + '/tmp/' + filename.split('/')[-1]

        return products


def load_image( infilename ) :
    img = Image.open( infilename )
    img.load()
    data = np.asarray( img, dtype="int32" )
    return data