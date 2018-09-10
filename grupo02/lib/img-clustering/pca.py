'''
Created on 13/10/2015

@author: Alexandre Yukio Yamashita
'''

import numpy as np
from scipy.linalg import svd

def pca(features, n):
    U, s, Vt = svd(np.dot(np.transpose(features), features), full_matrices = False)
    V = Vt.T

    # sort the PCs by descending order of the singular values (i.e. by the
    # proportion of total variance they explain)
    ind = np.argsort(s)[::-1]
    U = U[:, ind]
    s = s[ind]
    V = V[:, ind]
    S = np.diag(s)
    diagonal = np.diag(S)
    variance = sum(diagonal[0:n]) / sum(diagonal)
    U_reduced = U[0:n]
    feautures_reduced = np.dot(features, np.transpose(U_reduced))

    return feautures_reduced, U_reduced, variance


def pca_back(features, U_reduced):
    return np.dot(features, U_reduced)
