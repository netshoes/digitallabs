import cv2
import numpy as np
from subprocess import call


def extract_bic(path):
    """
    Extract bic features.
    """

    # Read image.
    img = cv2.imread(path)
    img = cv2.resize(img, (256, 256))
    path = path.split(".")
    save = path[0] + ".ppm"
    cv2.imwrite(save, img)

    # Predict segmented image.
    call(["./extract_bic", save])
    feature_path = save.replace("ppm", "ppm.txt")
    feature = np.loadtxt(feature_path)
    call(["rm", feature_path])
    call(["rm", save])

    return feature


if __name__ == '__main__':
    # Read image.
    path = "D24-1415-310_detalhe1.jpg"
    print(extract_bic(path))
