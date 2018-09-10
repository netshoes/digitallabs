'''
Created on 30/08/2015

@author: Alexandre Yukio Yamashita
'''
import numpy as np


def z_norm(X):
    '''
    Normalize data using z-norm.
    '''

    # Calculate mean and std, if needed.
    mean_value = np.repeat(np.mean(X, axis = 1).reshape(len(X), 1), len(X[0])).reshape(len(X), len(X[0]))
    std_value = np.repeat(np.std(X, axis = 1).reshape(len(X), 1), len(X[0])).reshape(len(X), len(X[0]))

    mean_value[:, 0] = 0
    std_value[:, 0] = 1

    # Normalize data.
    X_norm = np.divide(np.subtract(X, mean_value), np.array(std_value))

    return X_norm

def z_norm_by_feature(X, mean_value = None, std_value = None):
    '''
    Normalize data using z-norm.
    '''

    # Calculate mean and std, if needed.
    if mean_value is None and std_value is None:
        mean_value = np.mean(X, axis = 0)
        std_value = np.std(X, axis = 0)
        # std_value[np.where(std_value == 0)] = 1.0
        return_mean_std = True
    else:
        return_mean_std = False

    # Normalize data.
    X = np.divide(np.subtract(X, mean_value), np.array(std_value))

    if return_mean_std:
        return X, mean_value, std_value
    else:
        return X


if __name__ == '__main__':
    X = np.array([[1, 2, 3], [4, 5, 7]])
