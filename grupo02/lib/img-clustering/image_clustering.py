'''
Created on 09/10/2015

@author: Alexandre Yukio Yamashita
'''

from extract_features import load
from models.features import extract_hog, extract_gch, extract_bic, extract_lbp
from models.file_list import FileList
from models.image import Image
import numpy as np
from k_means import kmeans
from matplotlib import pyplot as plt


if __name__ == "__main__":
    '''
    Load features.
    '''
    hog_features, gch_features, bic_features, lbp_features = load("features/features.bin")
    features = np.hstack((hog_features, gch_features, bic_features, lbp_features))

    rets = []

    for n in range(10, 13):
        print "K-means for " + str(n) + " centroids."

        center = None
        label = None
        ret = 9999999

        for _ in range(5):
            centroids = features[np.random.choice(range(len(features)), n)]

            c_ret, c_label, c_center = kmeans(features, k = n, \
                                              centroids = centroids, steps = 1000)

            if c_ret < ret:
                ret = c_ret
                label = c_label
                center = c_center

            print ret

        rets.append(ret)

    plt.plot(rets)
    plt.show()
