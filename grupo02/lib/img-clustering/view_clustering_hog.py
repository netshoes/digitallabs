'''
Created on 09/10/2015

@author: Alexandre Yukio Yamashita
'''
from matplotlib import pyplot as plt

from extract_features import load_clustering
from models.features import extract_hog
from models.file_list import FileList
from models.image import Image
import numpy as np


if __name__ == "__main__":
    '''
    Load features.
    '''

    image_list = FileList("resources/images/", "jpg")
    c_label, c_center = load_clustering("features/hog_cluster.bin")

#     hist, _ = np.histogram(c_label, normed = True, bins = np.max(c_label))
#     plt.plot(hist)
#     plt.show()

    image_list.paths = np.array(image_list.paths)
    image_list.paths = image_list.paths[0:20000]
    image_paths = image_list.paths[np.where(c_label == 135)]
#
#     for path in image_paths:
#         image = Image(path)
#         image.plot()


    features = []
    for path in image_paths:
        image = Image(path)
        # simage.plot()
        hog_features, hog_image = extract_hog(image)
        features.append(hog_image)

        image = Image(data = hog_image)
        image.plot()

    features = np.array(features)
    data = np.mean(features, axis = 0)
    image = Image(data = data)
    image.plot()

