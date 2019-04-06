import os


class MapFile:
    def __init__(self):
        root = "/home/grupo2/netshoes/produtos/"
        self.data = dict()

        for path, subdirs, files in os.walk(root):
            for name in files:
                file_path = os.path.join(path, name)
                key = os.path.basename(file_path).split(".")[0]
                self.data[key] = file_path

    def get_map(self):
        return self.data
