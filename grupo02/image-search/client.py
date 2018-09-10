""" HTTP client to predict customer decision.
"""
import requests


if __name__ == '__main__':
    """ Predict customer decision using HTTP service.
    """
    url = 'http://40.121.107.139:8090/matcher'
    headers = {'Accept': 'application/json',
               'Content-Type': 'application/json'}

#     json_data = """
#         {
#             "url": "https://grupo2storage2.blob.core.windows.net/grupo2cont/test3.jpg"
#         }"""
    json_data = """
        {
            "url": "https://cdn.images.dailystar.co.uk/dynamic/58/photos/809000/620x/554fb4231fd1c_gerr2.jpg"
        }"""
#     json_data = """
#         {
#             "url": "https://grupo2storage2.blob.core.windows.net/grupo2cont/test.jpg"
#         }"""

    try:
        r = requests.post(url, data=json_data, headers=headers)
        print (r.text)
    except Exception as e:
        print ("Connection with HTTP service failed.")
