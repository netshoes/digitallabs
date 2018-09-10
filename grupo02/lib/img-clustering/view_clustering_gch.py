'''
Created on 09/10/2015

@author: Alexandre Yukio Yamashita
'''
from models.image import Image

from matplotlib import pyplot as plt

from extract_features import load_clustering
from models.file_list import FileList
import numpy as np
from models.features import extract_gch


if __name__ == "__main__":
    '''
    Load features.
    '''

    image_list = FileList("resources/images/", "jpg")
    c_label, c_center = load_clustering("features/gch_cluster.bin")

    hist, _ = np.histogram(c_center[100], normed = True, bins = 758)
    plt.plot(hist)
    plt.show()

    image_list.paths = np.array(image_list.paths)
    image_list.paths = image_list.paths[0:20000]
    image_paths = image_list.paths[np.where(c_label == 100)]

    for path in image_paths:
        image = Image(path)
        extract_gch(image)
        # image.plot()
