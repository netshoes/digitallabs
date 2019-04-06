'''
Created on 19 de ago de 2018

@author: alexandre_yamashita
'''
import json
import urllib.request

if __name__ == "__main__":
    path = '/home/grupo2/CatalogoFull.json'

    with open(path, encoding='utf-8') as F:
        json_data = json.loads(F.read())
