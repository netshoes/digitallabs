'''
Created on 13/10/2015

@author: Alexandre Yukio Yamashita
'''
from extract_features import save_pca, load_pca, save_pca_features, \
    load_pca_features
import numpy as np


if __name__ == "__main__":
    print "Loading"


    hog, U = load_pca("features/pca_hog.bin")
    del U
    lbp, U = load_pca("features/pca_lbp.bin")
    del U
    gch_bic, U = load_pca("features/pca_gch_bic.bin")
    del U

    features = np.hstack((hog, lbp, gch_bic))
    save_pca_features("pca_features.bin", features)

    features = load_pca_features("pca_features.bin")
    print features.shape