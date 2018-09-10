""" HTTP client to predict customer decision.
"""
import requests


if __name__ == '__main__':
    """ Predict customer decision using HTTP service.
    """
    url = 'http://40.74.233.210:8090/segmentation'
    headers = {'Accept': 'application/json',
               'Content-Type': 'application/json'}
# 
#     json_data = """
#         {
#             "url": "https://www.lance.com.br/files/article_main/uploads/2018/03/14/5aa9649e70c58.jpeg"
#         }"""
    
    json_data = """
        {
            "url": "https://www.lance.com.br/files/article_main/uploads/2018/03/14/5aa9649e70c58.jpeg"
        }"""
        
    try:
        r = requests.post(url, data=json_data, headers=headers)
        print (r.text)
    except Exception as e:
        print ("Connection with HTTP service failed.")
