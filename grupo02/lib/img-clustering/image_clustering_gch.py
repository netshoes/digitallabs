'''
Created on 09/10/2015

@author: Alexandre Yukio Yamashita
'''

import math

from matplotlib import pyplot as plt

from extract_features import load_hog, load_gch, load_bic, save_clustering
from k_means import kmeans
from models.features import extract_hog, extract_gch, extract_bic, extract_lbp
from models.file_list import FileList
from models.image import Image
from normalize import z_norm_by_feature
import numpy as np


if __name__ == "__main__":
    '''
    Load features.
    '''
    features = load_gch("features/features.bin")
    print features.shape
    # features, _, _ = z_norm_by_feature(features)
    # features, _, _ = z_norm_by_feature(features)
    features, _, _ = z_norm_by_feature(features[0:20000, :])
    print features.shape
    rets = []

    for index in range(500, 501):
        if index == 2:
            n = 2
        else:
            # n = int(math.pow(10, (index - 2)))
            n = 100 * (index - 2)

        n = index

        print "K-means for " + str(n) + " centroids."

        center = None
        label = None
        ret = 99999999999

        for _ in range(1):
            centroids = features[np.random.choice(range(len(features)), n, replace = False)]

            c_ret, c_label, c_center = kmeans(features, k = n, \
                                              centroids = centroids, steps = 100)

            if c_ret < ret:
                ret = c_ret
                label = c_label
                center = c_center

            print ret

        rets.append(ret)

    save_clustering("features/gch_cluster.bin", c_label, c_center)
    # plt.plot(rets)
    # plt.show()
