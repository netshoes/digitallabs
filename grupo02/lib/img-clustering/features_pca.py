'''
Created on 12/10/2015

@author: Alexandre Yukio Yamashita
'''
from scipy.linalg import svd

from extract_features import load, load_hog, load_lbp, load_gch
from extract_features import load_bic, save_pca
import matplotlib.pyplot as plt
from normalize import z_norm_by_feature
import numpy as np
from pca import pca, pca_back


if __name__ == "__main__":
    print "Loading"
    gch_features = load_gch("features/features.bin")
    bic_features = load_bic("features/features.bin")
    features = np.hstack((gch_features, bic_features))

    n = 20000
    features = features[0:n]


    print "Normalizing"
    features, _, _ = z_norm_by_feature(features)

    print "PCA"
    feautures_reduced, U, variance = pca(features, 500)
    print variance
    print features.shape

    print "Saving"
    save_pca("features/pca_gch_bic.bin", feautures_reduced, U)
