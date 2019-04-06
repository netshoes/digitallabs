#ifndef _MO445_H_
#define _MO445_H_

#define LOW 0
#define HIGH 1
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef __cplusplus
#ifndef _WIN32
#ifndef __cplusplus
typedef enum boolean {false,true} bool;
#endif
#else
typedef unsigned short ushort;
#endif
#endif

#ifndef MAX
#define MAX(x,y) (((x) > (y))?(x):(y))
#endif

#ifndef MIN
#define MIN(x,y) (((x) < (y))?(x):(y))
#endif

#define SetTieBreak(a,b) a->C.tiebreak=b 

#define MSG1  "Cannot allocate memory space"
#define MSG2  "Cannot open file"
#define SIZE 64
#define PI  3.1415926536
#define ROUND(x) ((x < 0)?(int)(x-0.5):(int)(x+0.5))
#define NIL  -1
#define WHITE       0 
#define GRAY        1
#define BLACK       2
#define INCREASING  1
#define DECREASING  0
#define HISTOGRAMSIZE 180
#define LIFOBREAK 1
#define FIFOBREAK 0
#define INTERIOR    0
#define EXTERIOR    1
#define HEAP_DAD(i) ((i - 1) / 2)
#define HEAP_LEFTSON(i) (2 * i + 1)
#define HEAP_RIGHTSON(i) (2 * i + 2)
#define BOTH        2

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

/* Common operations functions ************************/
int NCFgets(char *s, int m, FILE *f); 
int *AllocIntArray(int n);
double *AllocDoubleArray(int n);
float  *AllocFloatArray(int n); 
char   *AllocCharArray(int n);  /* It allocates 1D array of n characters */

/* Errors functions *********************************/
void Error(char *msg,char *func); 

/* Data structures **********************************/
/* BIC */
typedef struct {
  int color;
  int frequency;
}Property;

typedef struct {
  unsigned long colorH[SIZE];
  unsigned long lowH[SIZE];
  unsigned long highH[SIZE];
}VisualFeature;

typedef struct {
  unsigned char colorH[SIZE];
  unsigned char lowH[SIZE];
  unsigned char highH[SIZE];
}CompressedVisualFeature;

/* pgm images */
typedef struct _image {
  int *val;
  int ncols,nrows;
  int *tbrow;
} Image;

/* ppm images */
typedef struct cimage {
  Image *C[3];
} CImage;

typedef struct _FeatureVector1D {
  double *X;
  int n; 
} FeatureVector1D;

typedef struct _FeatureVector2D {
  double *X;
  double *Y;
  int n; 
} FeatureVector2D;

typedef struct _curve {
  double *X;
  double *Y;
  int n; 
} Curve;

typedef struct _curve3d { /* 3D Curve */
  double *X;
  double *Y;
  double *Z;
  int n; 
} Curve3D;

typedef struct _adjrel {
  int *dx;
  int *dy;
  int n;
} AdjRel;

typedef struct _pixel {
  int x,y;
} Pixel;

typedef struct{
  int length;   /* length of the boundary */
  int *X;       /* X values of each boundary point from 0 to length-1   */
  int *Y;       /* Y values of each boundary point from 0 to length-1 */
} boundary_type;

typedef struct{
  int length;   /* length of the boundary  */
  int *mean;    /* mean value BAS function  */
  int *second;  
  int *third;
} representation_type;

typedef struct _DImage{
  double *val;
  int ncols,nrows;
  int *tbrow;
} DImage;

typedef struct _tensorscale{
  DImage *anisotropy;
  DImage *orientation;
  DImage *thickness;

  int m_pairs;
}TensorScale;

typedef struct _vector{
  float x;
  float y;
  float z;
} Vector, Point, Vertex;

typedef struct _node { 
  int  next;  /* next node */
  int  prev;  /* prev node */
  char color; /* WHITE=0, GRAY=1, BLACK=2 */
} Node;

typedef struct _doublylinkedlists {
  Node *elem;  /* all possible doubly-linked lists of the circular queue */
  int nelems;  /* total number of elements */
} DoublyLinkedLists; 

typedef struct _circularqueue { 
  int *first;  /* list of the first elements of each doubly-linked list */
  int *last;   /* list of the last  elements of each doubly-linked list  */
  int nbuckets;  /* number of buckets in the circular queue */
  int current;   /* current bucket */
  char tiebreak; /* 1 is LIFO, 0 is FIFO (default) */
} CircularQueue;

typedef struct _queue { /* Priority queue by Dial implemented as
                           proposed by A. Falcao */
  CircularQueue C;
  DoublyLinkedLists L;
} Queue;

typedef struct _polynom { /* Polynomial */
  double *coef; /* a0*x^0 + a1*x^1 + ... + an*x^n */ 
  int n; /* degree n */
} Polynom;

typedef struct _set {
  int elem;
  struct _set *next;
} Set;

typedef struct _annimg {
  Image *img;
  Image *grad;
  Image *cost;
  Image *label;
  Image *pred;
  Image *root;
  Set   *seed;
} AnnImg;

typedef struct _heap {
  int *cost;
  char *color;
  int *pixel;
  int *pos;
  int last;
  int n;
} Heap;

typedef struct _adjpxl {
  int *dp;
  int n;
} AdjPxl;

/*Working with images *****************************/

/* pgm images */
Image  *CreateImage(int ncols,int nrows);
void    DestroyImage(Image **img);
Image  *ReadImage(char *filename);
Image  *MBB(Image *img);
Image  *ROI(Image *img, int xl, int yl, int xr, int yr);
Image  *AddFrame(Image *img, int sz, int value);
void    SetImage(Image *img, int value);
void WriteImage(Image *img,char *filename);

/* ppm images */
CImage *CreateCImage(int ncols, int nrows);
void    DestroyCImage(CImage **cimg);
CImage *ReadCImage(char *filename);

/* auxiliary functions  ****************************/
Curve   *CreateCurve(int n);
void     DestroyCurve(Curve **curve);
FeatureVector1D  *CurveTo1DFeatureVector(Curve *curve);
FeatureVector1D *CreateFeatureVector1D(int n);
void            DestroyFeatureVector1D(FeatureVector1D **desc);
void            WriteFeatureVector1D(FeatureVector1D *desc,char *filename);
AdjRel *CreateAdjRel(int n);
void    DestroyAdjRel(AdjRel **A);
AdjRel *LeftSide(AdjRel *A);
AdjRel *RightSide(AdjRel *A);
AdjRel *Circular(float r);
bool    ValidPixel(Image *img, int x, int y);
bool   ValidContPoint(Image *bin, AdjRel *L, AdjRel *R, int p);
Image *LabelContPixel(Image *bin);
Curve3D *CreateCurve3D(int n);
void     DestroyCurve3D(Curve3D **curve);
void     SortCurve3D(Curve3D *curve, int left, int right, char order);
int      PartCurve3D (Curve3D *curve, int left, int right, char order);
int FFT(int dir, long nn, double *x, double *y);
Image *LabelContour(Image *bin);
void Warning(char *msg,char *func); 
Image *Scale(Image *img, float Sx, float Sy) ;
DImage *CreateDImage(int ncols, int nrows);
void    DestroyDImage(DImage **dimg);
void    DestroyImage(Image **img);
int     MaximumValue(Image *img);
Queue *CreateQueue(int nbuckets, int nelems);
void   DestroyQueue(Queue **Q);
int    EmptyQueue(Queue *Q);
void   InsertQueue(Queue *Q, int bucket, int elem);
int    RemoveQueue(Queue *Q);
void   UpdateQueue(Queue *Q, int elem, int from, int to);
void ResetQueue(Queue *Q);
void RemoveQueueElem(Queue *Q, int elem, int bucket);
Curve   *SamplePolynom(Polynom *P, double from, double to, int nbins);
Polynom *CreatePolynom(int degree);
void     DestroyPolynom(Polynom **P);
Polynom *DerivPolynom(Polynom *P);
Polynom *Regression(Curve *curve, int degree);
Polynom *MSFractal(Image *bin,int maxdist,int degree,double lower,double higher,int reg,double from,double to);
AnnImg *Annotate(Image *img, Image *cost, Image *label);
AdjRel *ComplAdj(AdjRel *A1, AdjRel *A2);
void InsertSet(Set **S, int elem);
int  RemoveSet(Set **S);
int Seed(Image *pred, int p);
Image *CompPaths(Image *pred);
Image *Perimeter(Image *bin);
Curve3D *Saliences(Image *bin, int maxdist);
Image *MSSkel(Image *bin, char side);
void DestroyAdjPxl(AdjPxl **N);
Curve3D *CompSaliences(AnnImg *aimg, int maxcost);
Image *RemFrame(Image *fimg, int sz);
AdjPxl *AdjPixels(Image *img, AdjRel *A);
Image *Abs(Image *img);
Curve3D *RemSaliencesByAngle(Curve3D *curve,int radius, int angle);
Image *LabelBinComp(Image *bin, AdjRel *A);
Curve3D *SkelSaliences(Image *skel, int maxdist, int angle) ;
Image *Skeleton(Image *msskel, float perc);
Image *CompMSSkel(AnnImg *aimg);
void     iftDilation(AnnImg *aimg, AdjRel *A); /* by Dial */
void InvertXY(Curve *curve);
Curve *CopyCurve(Curve *curve);
void SortCurve(Curve *curve, int left, int right, char order);
int PartCurve (Curve *curve, int left, int right, char order);
Curve *Histogram(Image *img);
void DestroySet(Set **S);
void   DeAnnotate(AnnImg **aimg);
int FrameSize(AdjRel *A);
void Change(int *a, int *b);
Heap *CreateHeap(int n, int *cost);
void DestroyHeap(Heap **H);
bool InsertHeap(Heap *H, int pixel);
bool RemoveHeap(Heap *H, int *pixel);
void GoUpHeap(Heap *H, int i);
bool IsEmptyHeap(Heap *H);
bool HeapIsEmpty(Heap *H);
void GoDownHeap(Heap *H, int i);
bool IsFullHeap(Heap *H);
void GoUpHeap(Heap *H, int i);
FeatureVector2D  *CurveTo2DFeatureVector(Curve *curve);
/* Descriptor functions ***************************/

/* BIC */
/* auxiliary functions */
Curve *BIC(CImage *img);
void Write_visual_features(char *filename,char *dbname, CompressedVisualFeature *cvf);
CompressedVisualFeature *Extract_visual_features(CImage *img);
double gray_level_BIC(Image *img1, Image *img2);

/* Fourier Descriptor */
/* auxiliary functions */
double Cabs(double x, double y);
Curve *Image2Curve(Image *img);
Curve *FourierDescriptor(Image *img);

/* Moments Invariant */
/* auxiliary functions */
double  MomentPQ(int p, int q, Image *img, int max);
Curve *MomentInv(Image *img);
Curve *MomentInvariant(Image *img); // contorno e objeto inteiro

/* BAS */
/* auxiliary functions */
Curve           *BAS(Image *in,int rsp,int nsamples);

/* Tensor Scale */
/* auxiliary functions */
TensorScale *CreateBinaryTensorScale(Image *bin, int m_pairs);
void DestroyTensorScale(TensorScale **ts);
float *TSOrientationHistogram(TensorScale *ts);
Image *TSEDistTrans(Image *bin);

/* MultiScale Fractal Dimension*/
/* auxiliary functions */
Curve *PolynomToFractalCurve(Polynom *P, double lower, double higher, int nbins);
Curve *ContourMSFractal(Image *in);

/* Contour Saliences */
/* auxiliary functions */
Curve3D *SkelCont(Image *bin, int maxdist, int threshold, int angle, char side);
Curve3D *iftContourSaliences(Image *bin,int threshold_in,int threshold_out,int angle_in,int angle_out);
Curve   *ContourSaliences(Image *in);
void DescInvertXY(FeatureVector2D *desc);
FeatureVector2D *CreateFeatureVector2D(int n);
double ContSalieDistance(FeatureVector2D *D1, FeatureVector2D *D2);
FeatureVector2D *CircularRotation(FeatureVector2D *descriptor, double delta);
void DestroyFeatureVector2D(FeatureVector2D **desc);
void SortFeatureVector2D(FeatureVector2D *desc, int left, int right, char order);
int PartFeatureVector2D (FeatureVector2D *desc, int left, int right, char order);
void WriteFeatureVector2D(FeatureVector2D *desc,char *filename);
FeatureVector2D *CopyFeatureVector2D(FeatureVector2D *desc);
void DestroyFeatureVector2D(FeatureVector2D **desc);
double Matching(FeatureVector2D *descriptor1, FeatureVector2D *descriptor2, int order);

/* Segment Saliences */
/* auxiliary functions */
Curve *SS_ExtractionAlgorithm_(Image *in, int maxdist, int nsamples, int side);
double SS_OCSMatching(FeatureVector1D *fv_1, FeatureVector1D *fv_2);
double SS_OCS(FeatureVector1D *fv1, FeatureVector1D *fv2);
double SS_getMin(double Dist1, double Dist2, double Dist3);

/* call functions *********************************/
/* BIC */
FeatureVector1D *BIC_ExtractionAlgorithm(CImage *in); /*BIC extractor*/
double BIC_DistanceAlgorithm(FeatureVector1D *fv1, FeatureVector1D *fv2); /*BIC similarity*/

/* Fourier Descriptor */
FeatureVector1D *FourierDescriptor_ExtractionAlgorithm(Image *in);/* in is a binary image*/
double Fourier_DistanceAlgorithm(FeatureVector1D *fv1, FeatureVector1D *fv2); /*fourier similarity*/

/* Moments Invariant */
FeatureVector1D *MomentInvariant_ExtractionAlgorithm(Image *in);/*in is a binary image*/
double MomentInvariant_DistanceAlgorithm(FeatureVector1D *fv1, FeatureVector1D *fv2);

/*  BAS */
FeatureVector1D *BAS_ExtractionAlgorithm(Image *in,int rsp,int nsamples);
double BAS_DistanceAlgorithm(FeatureVector1D *c1, FeatureVector1D *c2);

/*  Tensor Scale */
FeatureVector1D *TensorScale_ExtractionAlgorithm(Image *in);
double TensorScale_DistanceAlgorithm(FeatureVector1D *c1, FeatureVector1D *c2);

/* Multiscale Fractal Dimension */
FeatureVector1D *MS_ExtractionAlgorithm(Image *img);
double MS_DistanceAlgorithm(FeatureVector1D *fv1, FeatureVector1D *fv2); 

/* Contour Saliences */
FeatureVector2D *CS_ExtractionAlgorithm(Image *img);
double CS_DistanceAlgorithm(FeatureVector2D *descriptor1, FeatureVector2D *descriptor2);

/* Segment Saliences */
FeatureVector1D *SS_ExtractionAlgorithm(Image *img);
double SS_DistanceAlgorithm(FeatureVector1D *fv1d1, FeatureVector1D *fv1d2);

/* Metrics to measure the similarity between feature vectors*/
double EuclideanDistance(FeatureVector1D *v1, FeatureVector1D *v2);
double L1_Distance(FeatureVector1D *v1, FeatureVector1D *v2);
float TSHistogramMatch(FeatureVector1D *fv1, FeatureVector1D *fv2, int *offset);
double dLog(FeatureVector1D *fv1, FeatureVector1D *fv2);

#endif
