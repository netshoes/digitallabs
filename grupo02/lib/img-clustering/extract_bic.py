'''
Created on 11/10/2015

@author: Alexandre Yukio Yamashita
'''

from subprocess import call

from models.file_list import FileList


if __name__ == '__main__':
    source_directory = "resources/images_ppm/"
    image_list = FileList(source_directory, "ppm")

    i = 0
    for path in image_list.paths:
        if i % 1000 == 0:
            print i
        i += 1
        call(["./extract_bic", path])

