#include "MO445.h"
#include <stdio.h>

int main(int argc,char **argv){
	CImage *cimg1 = NULL;
	FeatureVector1D *fvBIC1 = NULL;

	cimg1 = ReadCImage(argv[1]);
	fprintf(stderr,"Extracting BIC ... ");
	fvBIC1 = BIC_ExtractionAlgorithm(cimg1);
	
	char str1[100];
	char str2[100];
 	strcpy(str1, argv[1]);
	strcpy(str2, ".txt");
	strcat(str1, str2);

	WriteFeatureVector1D(fvBIC1, str1);

	return 0;
}
