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
        lbp = extract_lbp(image)
        image_lbp = Image(data = lbp)
        image_lbp.plot()

        _, hog = extract_hog(image)
        image_hog = Image(data = hog)
        image.equalize_clahe()
        image_hog.plot()
