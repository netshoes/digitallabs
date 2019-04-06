'''
Created on 09/10/2015

@author: Alexandre Yukio Yamashita
'''
from matplotlib import pyplot as plt

from extract_features import load_clustering
from models.features import extract_lbp
from models.file_list import FileList
from models.image import Image
import numpy as np
from normalize import z_norm, z_norm_by_feature


if __name__ == "__main__":
    '''
    Load features.
    '''

    image_list = FileList("resources/images/", "jpg")
    c_label, c_center = load_clustering("features/lbp_cluster.bin")

#     hist, _ = np.histogram(c_label, normed = True, bins = np.max(c_label))
#     plt.plot(hist)
#     plt.show()

    image_list.paths = np.array(image_list.paths)
    image_list.paths = image_list.paths[0:20000]
    image_paths = image_list.paths[np.where(c_label == 120)]

#     for path in image_paths:
#         image = Image(path)
#         image.plot()

    features = []
    for path in image_paths:
        image = Image(path)
        lbp_image = extract_lbp(image)
        # features.append(lbp_image)

    # features = np.array(features)
    # data = np.mean(features, axis = 0)
    # image = Image(data = data)
    # image.plot()

    hist, _, _ = z_norm_by_feature(c_center[120])
    plt.plot(hist)
    plt.show()
