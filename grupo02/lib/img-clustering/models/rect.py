'''
Created on 26/06/2015

@author: Alexandre Yukio Yamashita
         Flavio Nicastro
'''

from point import Point
import numpy as np


class Rect:
    '''
    Rectangle with subrectangles.
    '''
    subrects = []
    image = []
    fit = 1
    quadrilateral = None

    def __init__(self, array):
        self.x = int(array[0])
        self.y = int(array[1])
        self.w = int(array[2])
        self.h = int(array[3])
        self.haar_recognized = True
        self.a = self.w * self.h
        self.center = Point(self.x + self.w / 2.0, self.y + self.h / 2.0)

    def contains(self, p):
        '''
        Check if rectangle contains p.
        '''
        return self.x <= p.x <= self.x + self.w and \
            self.y <= p.y <= self.y + self.h

    def intersect(self, p):
        '''
        Check intersect with p.
        '''
        return (self.x < p.x + p.w and self.x + self.w > p.x and self.y < p.y + p.h and self.y + self.h > p.y)

    def inside(self, p):
        '''
        Check if rectangle is inside p.
        '''
        ratio = 2
        if (p.x <= self.x or abs(p.x - self.x) < ratio) and \
           (p.y <= self.y or abs(p.y - self.y) < ratio) and \
           (p.x + p.w >= self.x + self.w or abs(p.x + p.w - self.x - self.w) < ratio) and \
           (p.y + p.h >= self.y + self.h or abs(p.y + p.h - self.y - self.h) < ratio):
            return True
        else:
            return False

    def equal(self, p):
        '''
        Check if rectangle is equal to p.
        '''
        ratio = 2
        if abs(p.x - self.x) < ratio and \
           abs(p.y - self.y) < ratio and \
           abs(p.x + p.w - self.x - self.w) < ratio and \
           abs(p.y + p.h - self.y - self.h) < ratio:
            return True
        else:
            return False

    def vsplit(self):
        '''
        Splits rectangle vertically.
        '''
        lRect = Rect((self.x, self.y, self.w / 2.0, self.h))
        rRect = Rect((self.center.x, self.y, self.w / 2.0, self.h))
        return lRect, rRect

    def __str__(self):
        '''
        To string.
        '''
        return '({0}, {1}), ({2}, {3}), w = {4}, h = {5}, a = {6}'.format(
            self.x, self.y, self.x + self.w, self.y + self.h, self.w, self.h, self.a
        )

    def __repr(self):
        '''
        To string.
        '''
        return self.__str__()