'''
Created on 09/10/2015

@author: Alexandre Yukio Yamashita
'''
from models.image import Image

from matplotlib import pyplot as plt

from extract_features import load_clustering
from models.file_list import FileList
import numpy as np


if __name__ == "__main__":
    '''
    Load features.
    '''

    image_list = FileList("resources/images/", "jpg")
    c_label, c_center = load_clustering("features/bic_cluster.bin")

#     hist, _ = np.histogram(c_label, normed = True, bins = np.max(c_label))
#     plt.plot(hist)
#     plt.show()

    image_list.paths = np.array(image_list.paths)
    image_paths = image_list.paths[np.where(c_label == 400)]

    for path in image_paths:
        image = Image(path)
        image.plot()
