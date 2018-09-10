#include "MO445.h"
#include <stdio.h>

int main(int argc,char **argv){
	fprintf(stderr,"\nInstructions: ");
	fprintf(stderr,"\nFor the shape descriptors (Moments Invariant, Fourier Descriptor, BAS Descriptor abd Tensor Scale Descriptor) the image \nneeds to be in binary format (0 - background 1 - object)\n");

	CImage *cimg1 = NULL, *cimg2 = NULL;
	Image *img1 = NULL, *img2 = NULL;
	FeatureVector1D *fvBIC1 = NULL, *fvBIC2 = NULL, *fvMoments1 = NULL, *fvMoments2 = NULL;
	FeatureVector1D *fvFourier1 = NULL, *fvFourier2, *fvBAS1 = NULL, *fvBAS2 = NULL;
	FeatureVector1D	*fvTensor1 = NULL, *fvTensor2 = NULL, *fvMS1 = NULL, *fvMS2 = NULL;
	FeatureVector2D *fvCS1 = NULL, *fvCS2 = NULL;
	FeatureVector1D *fvSS1 = NULL, *fvSS2 = NULL;

	cimg1 = ReadCImage("figs/corel-guards.ppm");
	cimg2 = ReadCImage("figs/corel-locomotive.ppm");
	img1 = ReadImage("figs/mpeg7-bat.pgm");
	img2 = ReadImage("figs/mpeg7-bird.pgm");
		
	fprintf(stderr,"\nExtracting features from corel-guards.ppm and mpeg7-bat.pgm ...");
	fprintf(stderr,"\nExtracting BIC ... ");
	fvBIC1 = BIC_ExtractionAlgorithm(cimg1);
	WriteFeatureVector1D(fvBIC1, "results/bic_corel-guards.txt");

	fprintf(stderr,"\nExtracting MomentsInvariant ... ");
	fvMoments1 = MomentInvariant_ExtractionAlgorithm(img1);
	WriteFeatureVector1D(fvMoments1, "results/moments_mpeg7-bat.txt");

	fprintf(stderr,"\nExtracting Fourier Descriptor ... ");
	fvFourier1 = FourierDescriptor_ExtractionAlgorithm(img1);
	WriteFeatureVector1D(fvFourier1, "results/fourier_mpeg7-bat.txt");

	fprintf(stderr,"\nExtracting BAS ... ");
	fvBAS1 = BAS_ExtractionAlgorithm(img1, 0, 0);
	WriteFeatureVector1D(fvBAS1, "results/bas_mpeg7-bat.txt");

	fprintf(stderr,"\nExtracting Tensor Scale ... ");
	fvTensor1 = TensorScale_ExtractionAlgorithm(img1);
	WriteFeatureVector1D(fvTensor1, "results/tensorscale_mpeg7-bat.txt");

	fprintf(stderr,"\nExtracting Multiscale Fractal Dimension ... ");
	fvMS1 = MS_ExtractionAlgorithm(img1);
	WriteFeatureVector1D(fvMS1, "results/multiscale_mpeg7-bat.txt");

	fprintf(stderr,"\nExtracting Contour Saliences ... ");
	fvCS1 = CS_ExtractionAlgorithm(img1);
	WriteFeatureVector2D(fvCS1, "results/contoursaliences_mpeg7-bat.txt");

	fprintf(stderr,"\nExtracting Segment Saliences ... ");
	fvSS1 = SS_ExtractionAlgorithm(img1);
	WriteFeatureVector1D(fvSS1, "results/segmentsaliences_mpeg7-bat.txt");

	fprintf(stderr,"\n\nExtracting features from corel-locomotive.ppm and mpeg7-bird.pgm ...");
	fprintf(stderr,"\nExtracting BIC ... ");
	fvBIC2 = BIC_ExtractionAlgorithm(cimg2);
	WriteFeatureVector1D(fvBIC2, "results/bic_corel-locomotive.txt");

	fprintf(stderr,"\nExtracting MomentsInvariant ... ");
	fvMoments2 = MomentInvariant_ExtractionAlgorithm(img2);
	WriteFeatureVector1D(fvMoments2, "results/moments_mpeg7-bird.txt");

	fprintf(stderr,"\nExtracting Fourier Descriptor ... ");
	fvFourier2 = FourierDescriptor_ExtractionAlgorithm(img2);
	WriteFeatureVector1D(fvFourier2, "results/fourier_mpeg7-bird.txt");

	fprintf(stderr,"\nExtracting BAS ... ");
	fvBAS2 = BAS_ExtractionAlgorithm(img2, 0, 0);
	WriteFeatureVector1D(fvBAS2, "results/bas_mpeg7-bird.txt");

	fprintf(stderr,"\nExtracting Tensor Scale ... ");
	fvTensor2 = TensorScale_ExtractionAlgorithm(img2);
	WriteFeatureVector1D(fvTensor2, "results/tensorscale_mpeg7-bird.txt");

	fprintf(stderr,"\nExtracting Multiscale Fractal Dimension ... ");
	fvMS2 = MS_ExtractionAlgorithm(img2);
	WriteFeatureVector1D(fvMS2, "results/multiscale_mpeg7-bird.txt");

	fprintf(stderr,"\nExtracting Contour Saliences ... ");
	fvCS2 = CS_ExtractionAlgorithm(img2);
	WriteFeatureVector2D(fvCS2, "results/contoursaliences_mpeg7-bird.txt");

	fprintf(stderr,"\nExtracting Segment Saliences ... ");
	fvSS2 = SS_ExtractionAlgorithm(img2);
	WriteFeatureVector1D(fvSS2, "results/segmentsaliences_mpeg7-bird.txt");
	fprintf(stderr,"%lf",CS_DistanceAlgorithm(fvCS1, fvCS2));

	DestroyCImage(&cimg1);	DestroyImage(&img1);
	DestroyCImage(&cimg2);	DestroyImage(&img2);
	DestroyFeatureVector1D(&fvBIC1); DestroyFeatureVector1D(&fvBIC2);
	DestroyFeatureVector1D(&fvMoments1); DestroyFeatureVector1D(&fvMoments2);
	DestroyFeatureVector1D(&fvFourier1); DestroyFeatureVector1D(&fvFourier2);
	DestroyFeatureVector1D(&fvBAS1); DestroyFeatureVector1D(&fvBAS2);
	DestroyFeatureVector1D(&fvTensor1); DestroyFeatureVector1D(&fvTensor2);
	DestroyFeatureVector1D(&fvMS1); DestroyFeatureVector1D(&fvMS2);
	DestroyFeatureVector2D(&fvCS1); DestroyFeatureVector2D(&fvCS2);
	DestroyFeatureVector1D(&fvSS1); DestroyFeatureVector1D(&fvSS2);

	fprintf(stderr,"\n");

	return 0;
}
