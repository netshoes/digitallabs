from models.features import extract_hog, extract_lbp, extract_gch, extract_bic
from models.file_list import FileList
from models.image import Image


image_list = FileList("resources/images/", "jpg")

for path in image_list.paths:
    image = Image(path)
    print extract_bic(image).shape