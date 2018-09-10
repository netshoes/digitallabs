# -*- coding: utf-8 -*-
'''
Created on Jul 23, 2015

@author: Alexandre Yukio Yamashita
'''
import cv2

from PIL import Image as PILImage

import matplotlib.pyplot as plt
from models.logger import Logger
import numpy as np
from rect import Rect
from point import Point


class Image:
    '''
    Read data in lmdb format.
    '''

    _logger = Logger()
    data = None
    path = None

    def __init__(self, path = None, data = None):
        self._set_image_parameters(path, data)

        if self.path != None:
            self.load(self.path)

    def _set_image_data(self, data):
        '''
        Set image data.
        '''

        # Check if image is in rgb or gray scale
        if len(data.shape) == 3:
            # Image is in rgb.
            self.height, self.width, self.channels = data.shape
        else:
            # Image is in gray scale.
            self.height, self.width = data.shape
            self.channels = 1

        self.data = data

    def _set_image_parameters(self, path = None, data = None):
        '''
        Configure image.
        '''

        if path != None:
            self.path = path

        if data != None:
            self._set_image_data(data)

    def load(self, path):
        '''
        Load image from path.
        '''

        self._set_image_parameters(path = path)
        self.data = np.asarray(PILImage.open(self.path))

        return self.data

    def _configure_plot(self):
        '''
        Configure plot to display image.
        '''

        # Remove warning for Source ID not found.
        # The warning is a issue from matplotlib.
        import warnings
        warnings.simplefilter("ignore")

        # Configure pyplot.
        frame = plt.gca()
        frame.axes.get_xaxis().set_ticklabels([])
        frame.axes.get_yaxis().set_ticklabels([])

        is_gray = len(self.data.shape) < 3

        if is_gray:
            plt.imshow(self.data, cmap = plt.get_cmap("gray"))
        else:
            plt.imshow(self.data)

    def plot(self, image = None):
        '''
        Plot image.
        '''

        self._set_image_parameters(data = image)
        self._configure_plot()
        plt.show()

    def save(self, path = None, image = None):
        '''
        Save image.
        '''

        self._set_image_parameters(data = image, path = path)
        pil_image = PILImage.fromarray(self.data)
        pil_image.save(self.path)

    def get_level_data(self):
        '''
        Get level data from image.
        '''

        is_gray = len(self.data.shape) < 3

        if is_gray:
            data = self.data.reshape(1, len(self.data), len(self.data[0]))
        else:
            data = self.data.transpose(2, 0, 1)

        return data


    def equalize(self, image = None):
        '''
        Equalize image.
        '''

        if image == None and self.data == None:
            self._logger.log("There is no data to equalize image.", Logger.ERROR)
        elif image != None:
            self._set_image_data(image)

        if self.data != None:
            # Convert to gray scale if it image is in rgb.
            if len(self.data.shape) == 3:
                self._logger.log("We need to convert image to gray scale before equalization.", Logger.DEBUG)
                self.convert_to_gray(self.data)

            # Equalize image.
            self._logger.log("Equalizing image.")
            self.data = cv2.equalizeHist(self.data)

        return self.data

    def binarize(self, image = None):
        '''
        Binarize image.
        '''

        if image == None and self.data == None:
            self._logger.log("There is no data to binarize image.", Logger.ERROR)
        elif image != None:
            self._set_image_data(image)

        if self.data != None:
            # Convert to gray scale if it image is in rgb.
            if len(self.data.shape) == 3:
                self._logger.log("We need to convert image to gray scale before binarizing image.", Logger.DEBUG)
                self.convert_to_gray(self.data)

            # Equalize image.
            self._logger.log("Binarizing image.")
            self.data = cv2.medianBlur(self.data, 5)
            self.data = cv2.adaptiveThreshold(self.data, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, \
            cv2.THRESH_BINARY, 11, 2)

        return self.data

    def convert_to_gray(self, image = None):
        '''
        Convert rgb to gray.
        '''

        if image == None and self.data == None:
            self._logger.log("There is no data to convert image.", Logger.ERROR)
        elif image != None:
            self._set_image_data(image)

        if self.data != None:
            # Convert image only if it is in rgb.
            if len(self.data.shape) == 3:
                self._logger.log("Converting image to gray scale.")
                self.data = cv2.cvtColor(self.data, cv2.COLOR_RGB2GRAY)
            else:
                self._logger.log("Image is already in gray scale.")

            self.channels = 1

        return self.data

    def resize(self, width, height = 0, image = None):
        '''
        Resize image.
        '''

        if image == None and self.data == None:
            self._logger.log("There is no data to resize image.", Logger.ERROR)
        elif image != None:
            self._set_image_data(image)

        if height == 0:
            r = width * 1.0 / self.data.shape[1]
            height = int(self.data.shape[0] * r)

        self._logger.log("Resizing image to: width = " + str(width) + " height = " + str(height))
        resized = cv2.resize(self.data, (width, height), interpolation = cv2.INTER_AREA)
        return Image(data = resized)

    def crop(self, origin, end, image = None):
        '''
        Crop image.
        '''

        if image == None and self.data == None:
            self._logger.log("There is no data to crop image.", Logger.ERROR)
        elif image != None:
            self._set_image_data(image)

        if self.data != None:
            # Correct parameters.
            if origin.x >= self.width:
                origin.x = self.width - 1
            elif origin.x < 0:
                origin.x = 0

            if end.x >= self.width:
                end.x = self.width - 1
            elif end.x < 0:
                end.x = 0

            if origin.y >= self.height:
                origin.y = self.height - 1
            elif origin.y < 0:
                origin.y = 0

            if end.y >= self.height:
                end.y = self.height - 1
            elif end.y < 0:
                end.y = 0

            if origin.x > end.x:
                change = end.x
                end.x = origin.x
                origin.x = change

            if origin.y > end.y:
                change = end.y
                end.y = origin.y
                origin.y = change

            self._logger.log("Cropping image. Origin: (%d, %d) End: (%d, %d)" \
                % (origin.x, origin.y, end.x, end.y))
            return Image(data = self.data[origin.y:end.y, origin.x:end.x])

    def convert_to_hsv(self, image = None):
        '''
        Convert rgb to hsv.
        '''

        if image is None and self.data is None:
            self._logger.log("There is no data to convert image.", Logger.ERROR)
        elif image is not None:
            self._set_image_data(image)

        if self.data is not None and len(self.data.shape) == 3:
            self.data = cv2.cvtColor(self.data, cv2.COLOR_RGB2HSV)

        return self.data

    def equalize_clahe(self, image = None):
        '''
        Equalize image.
        '''

        if image == None and self.data == None:
            self._logger.log("There is no data to equalize image.", Logger.ERROR)
        elif image != None:
            self._set_image_data(image)

        if self.data != None:
            # Equalize image.
            self._logger.log("Equalizing image.")
            clahe = cv2.createCLAHE(clipLimit = 2.0, tileGridSize = (8, 8))

            if len(self.data.shape) == 3:
                self.data = cv2.cvtColor(self.data, cv2.COLOR_RGB2YCR_CB)
                data = np.zeros(self.data.shape)

                data[:, :, 0] = clahe.apply(self.data[:, :, 0])
                data[:, :, 1] = clahe.apply(self.data[:, :, 1])
                data[:, :, 2] = clahe.apply(self.data[:, :, 2])
                self.data = cv2.cvtColor(data.astype(np.uint8), cv2.COLOR_YCR_CB2RGB)
            else:
                self.data = clahe.apply(self.data)

        return self.data
