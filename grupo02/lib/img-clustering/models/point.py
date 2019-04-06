'''
Created on 06/04/2015

@author: Alexandre Yukio Yamashita
'''

class Point:
    '''
    Point 2d. 
    '''
    
    def __init__(self, x, y):
        self.x = x
        self.y = y
        
    def dist(self, p1):
        '''
        Returns the pythagorean distance from this point to p1.
        '''
        return pow(pow(self.x - p1.x, 2) + pow(self.y - p1.y, 2), .5)