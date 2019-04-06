'''
Created on 12/10/2015

@author: Alexandre Yukio Yamashita
'''

'''
Created on 09/10/2015

@author: Alexandre Yukio Yamashita
'''
import numpy as np
from models.features import extract_hog, extract_gch, extract_bic, extract_lbp
from models.file_list import FileList
from models.image import Image


def save_pca_features(path, features):
    '''
    Save extracted features.
    '''

    f = file(path, "wb")
    np.save(f, features)
    f.close()



def load_pca_features(path):
    '''
    Load extracted features.
    '''

    f = file(path, "rb")
    features = np.load(f)
    f.close()

    return features

def save_pca(path, features_reduced, U):
    '''
    Save extracted features.
    '''

    f = file(path, "wb")
    np.save(f, features_reduced)
    np.save(f, U)
    f.close()


def load_pca(path):
    '''
    Save extracted features.
    '''

    f = file(path, "rb")
    features_reduced = np.load(f)
    U = np.load(f)
    f.close()

    return features_reduced, U


def save_clustering(path, c_label, c_center):
    '''
    Save extracted features.
    '''

    f = file(path, "wb")
    np.save(f, c_label)
    np.save(f, c_center)
    f.close()


def load_clustering(path):
    '''
    Load extracted features.
    '''

    f = file(path, "rb")
    c_label = np.load(f)
    c_center = np.load(f)
    f.close()

    return c_label, c_center


def save(path, hog_features, gch_features, bic_features, lbp_features):
    '''
    Save extracted features.
    '''

    f = file(path, "wb")
    np.save(f, hog_features)
    np.save(f, gch_features)
    np.save(f, bic_features)
    np.save(f, lbp_features)
    f.close()


def load(path):
    '''
    Load extracted features.
    '''

    f = file(path, "rb")
    hog_features = np.load(f)
    gch_features = np.load(f)
    bic_features = np.load(f)
    lbp_features = np.load(f)
    f.close()

    return hog_features, gch_features, bic_features, lbp_features


def load_hog(path):
    '''
    Load extracted features.
    '''

    f = file(path, "rb")
    hog_features = np.load(f)
    f.close()

    return hog_features



def load_gch(path):
    '''
    Load extracted features.
    '''

    f = file(path, "rb")
    hog_features = np.load(f)
    gch_features = np.load(f)
    f.close()

    del hog_features

    return gch_features




def load_bic(path):
    '''
    Load extracted features.
    '''

    f = file(path, "rb")
    hog_features = np.load(f)
    gch_features = np.load(f)
    bic_features = np.load(f)
    f.close()

    del hog_features
    del gch_features

    return bic_features



def load_lbp(path):
    '''
    Load extracted features.
    '''

    f = file(path, "rb")
    hog_features = np.load(f)
    gch_features = np.load(f)
    bic_features = np.load(f)
    lbp_features = np.load(f)
    f.close()

    del hog_features
    del gch_features
    del bic_features

    return lbp_features


if __name__ == "__main__":
    '''
    Extract features from images.
    '''

    image_list = FileList("resources/images/", "jpg")
    hog_features = []
    gch_features = []
    bic_features = []
    lbp_features = []

    i = 0
    for path in image_list.paths:
        image = Image(path)

        hog_features.append(extract_hog(image))
        gch_features.append(extract_gch(image))
        bic_features.append(extract_bic(image))
        lbp_features.append(extract_lbp(image))

        i += 1
        if i % 100 == 0:
            print i

    hog_features = np.array(hog_features)
    gch_features = np.array(gch_features)
    bic_features = np.array(bic_features)
    lbp_features = np.array(lbp_features)

    save("features/features.bin" , hog_features, gch_features, bic_features, lbp_features)
