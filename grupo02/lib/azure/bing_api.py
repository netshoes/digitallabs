import http.client, urllib.request, urllib.parse, urllib.error, base64

headers = {
    # Request headers
    'Ocp-Apim-Subscription-Key': '7233e69a3dd3479e910880905f3c8730',
}

params = urllib.parse.urlencode({
    # Request parameters
    'imgurl': 'https://farm5.staticflickr.com/4273/33958978733_52a96932eb_z_d.jpg',
    'mkt': 'pt-BR',
    'modules': 'similarimages'
})

try:
    conn = http.client.HTTPSConnection('api.cognitive.microsoft.com')
    conn.request("GET", "/bing/v7.0/images/details?%s" % params, "{body}", headers)
    response = conn.getresponse()
    data = response.read()
    print(data)
    conn.close()
except Exception as e:
    print("[Errno {0}] {1}".format(e.errno, e.strerror))
