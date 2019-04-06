The purpose of this package is to provide the source code of some
image descriptors for the image analysis course (MO445). Each
descriptor is represented by a feature vector and a similarity
(distance) function, and its corresponding paper can be found in the
subdirectory docs.

Follow the steps:

1 - Extract the files in MO445.tar.gz.

2 - Go to the folder MO445 and execute the Makefile

3 - Go to the folder examples (MO445/examples), which contains one
example file "test.c" and two images to execute the program.

4 - Execute the Makefile. It will generate a executable file "test"

5 - Run ./test

6 - Some files corresponding to the descriptors will be generated, such as:

- bas_<figureName.txt> -> Descriptor Beam Angle Statistics (BAS) (SHAPE)
- bic_<figureName.txt>.txt -> Border/Interior Pixel Classification (BIC) (COLOR/TEXTURE)
- moments_<figureName.txt>.txt -> Moment Invariants (MI) (SHAPE)
- fourier_<figureName.txt>.txt -> Fourier Descriptor (SHAPE)
- tensorscale_<figureName.txt>.txt -> Tensor Scale Descriptor (SHAPE)
- multiscales_<figureName.txt>.txt -> Multiscale Fractal Dimension (SHAPE)
- contoursaliences_<figureName.txt>.txt -> Contour Saliences (SHAPE)
- segmentsaliences_<figureName.txt>.txt -> Segment Saliences (SHAPE)

All descriptors have two basic functions: extraction and
similarity (distance). Example for BAS:

- FeatureVector1D *BAS_ExtractionAlgorithm(Image *in,int rsp,int nsamples);
- double BAS_DistanceAlgorithm(FeatureVector1D *c1, FeatureVector1D *c2);

The extraction algorithm extracts the feature vector for an image and
the similarity function returns the distance between two images. Fell
free to modify the feature vector size.

The figures and the results obtained can be found in ./figs and
./results directories, respectively.

If you have questions, please contact Joao Paulo Papa
(jpaulo@ic.unicamp.br) or Alexandre Falcao (afalcao@ic.unicamp.br)

Enjoy!

