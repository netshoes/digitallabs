'''
Created on 11/10/2015

@author: Alexandre Yukio Yamashita
'''

from models.file_list import FileList
from models.image import Image


if __name__ == '__main__':
    source_directory = "resources/images/"
    destination_directory = "resources/images_ppm/"
    image_list = FileList(source_directory, "jpg")

    for file_name in image_list.get_file_names():
        image = Image(source_directory + file_name)
        ppm_path = destination_directory + file_name.replace("jpg", "ppm")
        image.equalize_clahe()
        image.save(ppm_path)
        print ppm_path
