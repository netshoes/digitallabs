'''
Created on 09/10/2015

@author: Alexandre Yukio Yamashita
'''

import cv2
import numpy
from skimage.feature import hog

from matplotlib import pyplot as plt
from skimage.feature.texture import local_binary_pattern

from image import Image
import numpy as np
from normalize import z_norm_by_feature


def extract_hog(image):
    '''
    Extract hog features from image.
    '''
    # Equalize image before extracting features.
    image = Image(data = image.data)
    image.convert_to_gray()
    image.equalize_clahe()

    # Extract hog features.
#     hog_features = hog(image.data, orientations = 9, pixels_per_cell = (16, 16), cells_per_block = (1, 1), visualise = False, normalise = False)

    # Extract hog features and plot result.
    hog_features, hog_image = hog(image.data, orientations = 9, pixels_per_cell = (16, 16), cells_per_block = (1, 1), visualise = True, normalise = False)
#     image = Image(data = hog_image)
#     image.plot()

    return hog_features, hog_image


def extract_gch(image):
    '''
    Extract global color histogram from image.
    '''

    # Equalize image before extracting features.
    image = Image(data = image.data)
    image.equalize_clahe()
    image.convert_to_hsv()

    chans = cv2.split(image.data)
    colors = ("b", "g", "r")
#     plt.figure()
#     plt.title("'Flattened' Color Histogram")
#     plt.xlabel("Bins")
#     plt.ylabel("# of Pixels")
    features = []

    # loop over the image channels
    for (chan, color) in zip(chans, colors):
        # create a histogram for the current channel and
        # concatenate the resulting histograms for each
        # channel
        hist = cv2.calcHist([chan], [0], None, [256], [0, 256])
        features.extend(hist)

        # plot the histogram
        # plt.plot(hist, color = color)
        # plt.xlim([0, 256])
#
#     print "flattened feature vector size: %d" % (np.array(features).flatten().shape)
#     plt.show()

    hist, _ = np.histogram(np.array(features).flatten(), normed = True, bins = 758)
    hist, _, _ = z_norm_by_feature(hist)
    plt.plot(hist)
    plt.show()


    return np.array(features).flatten()

def extract_bic(image):
    '''
    Extract bic features from image.
    '''

    path = image.path
    feature_path = path.replace("jpg", "ppm.txt")
    feature_path = feature_path.replace("resources/images/", "features/bic/")
    feature = np.loadtxt(feature_path)

    n_bins = np.max(feature)
    hist, _ = np.histogram(feature, normed = True, bins = n_bins, range = (0, n_bins))
    plt.plot(hist)
    plt.show()

    return feature


def extract_lbp(image, radius = 1.5):
    '''
    Extract LBP features.
    '''
    image = Image(data = image.data)
    image.convert_to_gray()
    image.equalize_clahe()

    n_points = 8 * radius
    lbp = local_binary_pattern(image.data, n_points, radius)

    n_bins = lbp.max() + 1
    hist, _ = np.histogram(lbp, normed = True, bins = n_bins, range = (0, n_bins))
    hist, _, _ = z_norm_by_feature(hist)
    plt.plot(hist)
    plt.show()

    return lbp
