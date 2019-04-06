'''
Created on 13/10/2015

@author: Alexandre Yukio Yamashita
'''
from matplotlib import pyplot as plt

from extract_features import load_pca_features
from extract_features import save_clustering
from k_means import kmeans
from normalize import z_norm_by_feature
import numpy as np


if __name__ == "__main__":
    print "Loading"

    features = load_pca_features("pca_features.bin")
    features, _, _ = z_norm_by_feature(features)
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

    save_clustering("features/pca_cluster.bin", c_label, c_center)
    plt.plot(rets)
    plt.show()
