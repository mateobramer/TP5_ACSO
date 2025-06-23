import os

import cv2
import numpy as np
from tqdm import tqdm


def getPatches(img: np.ndarray, patchSize: int, scale: int, offset: int, stride: int) -> list[np.ndarray]:
    """
    Extract non-overlapping patches from an image.
    :param img (np.ndarray): image to extract patches from
    :param patchSize (int): final size of each patch
    :param scale (int): scale*patchSize is the size of the patch extracted from the image, which is 
        then resized to patchSize.
    :param offset (int): offset from the borders of the image that are not considered for patch extraction
    :param stride (int): stride between patches
    """
    patches = []
    for i in range(offset, img.shape[0] - scale*patchSize - offset, scale*patchSize + stride):
        for j in range(offset, img.shape[1] - scale*patchSize - offset, scale*patchSize + stride):
            patch = img[i:i+scale*patchSize, j:j+scale*patchSize]
            patch = cv2.resize(patch, (patchSize, patchSize))
            patches.append(patch)
    return patches


def createPatchesData(inputDir: str, outputDir: str, categories: list, patchSize: int, scale: int, offset: int, stride: int, verbose: bool = False) -> None:
    """
    Create patches from images in the data directory and save them in the output directory.
    :param inputDir (str): directory containing the images
    :param outputDir (str): directory to save the patches
    :param categories (list): list of categories to consider
    :param patchSize (int): final size of each patch
    :param scale (int): scale*patchSize is the size of the patch extracted from the image, which is 
        then resized to patchSize.
    :param offset (int): offset from the borders of the image that are not considered for patch extraction
    :param stride (int): stride between patches
    :param verbose (bool): if True, print the number of patches created for
    """
    for category in categories:
        path = os.path.join(inputDir, category)
        outputCategory = os.path.join(outputDir, category)
        if not os.path.exists(outputCategory):
            os.makedirs(outputCategory)
        nPatches = 0
        for img in tqdm(os.listdir(path)):
            imgArray = cv2.imread(os.path.join(path, img))
            patches = getPatches(imgArray, patchSize, scale, offset, stride)
            for j, patch in enumerate(patches):
                cv2.imwrite(os.path.join(outputCategory, f"{img[:-4]}_{j}.jpg"), patch, [int(cv2.IMWRITE_JPEG_QUALITY), 89])
            nPatches += len(patches)
        if verbose:
            print(f"{nPatches} patches were created for the {category} category.")


def main():
    dataDir = "./entregas"
    categories = [
        "Coleus neochilus",
        "Salvia microphylla",
        "Salvia uliginosa",
    ]
    offset = 0       # Extraer patches desde el borde
    stride = 20      # Mover la ventana en pasos más pequeños, genera más patches
    scale = 2        # Extraer parches más pequeños para que entren más en la imagen
    patchSize = 96   # Tamaño final del patch

    inputDir = os.path.join(dataDir, "Bramer")
    outputDir = os.path.join(dataDir, "patches_mios")
    createPatchesData(inputDir, outputDir, categories, patchSize, scale, offset, stride, verbose=True)


if __name__ == "__main__":
    main()
