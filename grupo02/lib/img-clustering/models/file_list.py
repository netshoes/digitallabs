'''
Created on 8 de set de 2015

@author: Alexandre Yukio Yamashita
'''
from glob import glob
from os.path import basename


class FileList:
    '''
    List of files.
    '''

    def __init__(self, path = None, extension = None, paths = None):
        if paths is None:
            self.path = path
            self.paths = glob(path + '*.' + extension)
        else:
            self.path = paths


    def get_file_names(self):
        '''
        Get relative file paths from directory path.
        '''

        file_names = [basename(path) for path in self.paths]

        return file_names
