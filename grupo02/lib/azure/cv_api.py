from io import BytesIO

import requests

from PIL import Image
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt

# If you are using a Jupyter notebook, uncomment the following line.
# %matplotlib inline
# Replace <Subscription Key> with your valid subscription key.
subscription_key = "d36e250b867a4360baafc18ac5efd849"
assert subscription_key

# You must use the same region in your REST call as you used to get your
# subscription keys. For example, if you got your subscription keys from
# westus, replace "westcentralus" in the URI below with "westus".
#
# Free trial subscription keys are generated in the westcentralus region.
# If you use a free trial subscription key, you shouldn't need to change
# this region.
# vision_base_url = "https://brazilsouth.api.cognitive.microsoft.com/vision/v1.0/"
vision_base_url = "https://westcentralus.api.cognitive.microsoft.com/vision/v2.0/"

analyze_url = vision_base_url + "analyze"

# Set image_url to the URL of an image that you want to analyze.
image_url = "https://http2.mlstatic.com/D_Q_NP_837742-MLB26143612771_102017-Q.jpg"

headers = {'Ocp-Apim-Subscription-Key': subscription_key }
params = {'visualFeatures': 'Categories,Description,Color'}
data = {'url': image_url}
response = requests.post(analyze_url, headers=headers, params=params, json=data)
response.raise_for_status()

# The 'analysis' object contains various fields that describe the image. The most
# relevant caption for the image is obtained from the 'description' property.
analysis = response.json()
print(analysis)
image_caption = analysis["description"]["captions"][0]["text"].capitalize()

# Display the image and overlay it with the caption.
image = Image.open(BytesIO(requests.get(image_url).content))
plt.imshow(image)
plt.axis("off")
_ = plt.title(image_caption, size="x-large", y=-0.1)
