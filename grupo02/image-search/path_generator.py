import datetime
import hashlib
import os


class PathGenerator:

    def __init__(self):
        self.id = 0

    def generate_path(self, directory=""):
        image_extension = "ppm"
        now = datetime.datetime.now()
        hash_object = hashlib.md5(str(now).encode('utf-8'))
        file_path = hash_object.hexdigest() + str(self.id) + "." + \
            image_extension
        self.id += 1

        file_path = str(os.path.join(directory, file_path))
        return file_path


if __name__ == '__main__':
    file_path = "asdasd/a.jpg"
    path_generator = PathGenerator()
    print (path_generator.generate_path())
    print (path_generator.generate_path())
    print (path_generator.generate_path())
