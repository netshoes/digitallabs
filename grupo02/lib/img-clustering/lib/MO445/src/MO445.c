#include "MO445.h"

/* Common operations functions ************************/
int NCFgets(char *s, int m, FILE *f) {
  while(fgets(s,m,f)!=NULL)
    if (s[0]!='#') return 1;
  return 0;
}

int *AllocIntArray(int n)
{
  int *v=NULL;
  v = (int *) calloc(n,sizeof(int));
  if (v == NULL)
    Error(MSG1,"AllocIntArray");
  return(v);
}

float *AllocFloatArray(int n)
{
  float *v=NULL;
  v = (float *) calloc(n,sizeof(float));
  if (v == NULL)
    Error(MSG1,"AllocFloatArray");
  return(v);
}

double *AllocDoubleArray(int n)
{
  double *v=NULL;
  v = (double *) calloc(n,sizeof(double));
  if (v == NULL)
    Error(MSG1,"AllocDoubleArray");
  return(v);
}

char *AllocCharArray(int n)
{
  char *v=NULL;
  v = (char *) calloc(n,sizeof(char));
  if (v == NULL)
    Error(MSG1,"AllocCharArray");
  return(v);
}

/* Errors functions *********************************/
void Error(char *msg,char *func){ 
  fprintf(stderr,"Error:%s in %s\n",msg,func);
  exit(-1);
}

/*Working with images *****************************/
/* pgm images */
Image *CreateImage(int ncols, int nrows)
{
  Image *img=NULL;
  int i;

  img = (Image *) calloc(1,sizeof(Image));
  if (img == NULL){
    Error(MSG1,"CreateImage");
  }

  img->val   = AllocIntArray(nrows*ncols);
  img->tbrow = AllocIntArray(nrows);

  img->tbrow[0]=0;
  for (i=1; i < nrows; i++)
    img->tbrow[i]=img->tbrow[i-1]+ncols;
  img->ncols = ncols;
  img->nrows = nrows;
 
 return(img);
}

void DestroyImage(Image **img){
  Image *aux;

  aux = *img;
  if(aux != NULL){
    if (aux->val != NULL)   free(aux->val); 
    if (aux->tbrow != NULL) free(aux->tbrow);
    free(aux);    
    *img = NULL;
  }
}

Image *ReadImage(char *filename)
{
  FILE *fp=NULL;
  unsigned char *value=NULL;
  char type[10];
  int  i,ncols,nrows,n;
  Image *img=NULL;
  char z[256];

  fp = fopen(filename,"rb");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  fscanf(fp,"%s\n",type);
  if((strcmp(type,"P5")==0)){
    NCFgets(z,255,fp);
    sscanf(z,"%d %d\n",&ncols,&nrows);
    n = ncols*nrows;
    NCFgets(z,255,fp);
    sscanf(z,"%d\n",&i);
    value = (unsigned char *)calloc(n,sizeof(unsigned char));
    if (value != NULL){
      fread(value,sizeof(unsigned char),n,fp);
    }else{
      fprintf(stderr,"Insufficient memory in ReadImage\n");
      exit(-1);
    }
    fclose(fp);
    img = CreateImage(ncols,nrows);
    for (i=0; i < n; i++)
      img->val[i]=(int)value[i];
    free(value);
  }else{
    if((strcmp(type,"P2")==0)){
      NCFgets(z,255,fp);
      sscanf(z,"%d %d\n",&ncols,&nrows);
      n = ncols*nrows;
      NCFgets(z,255,fp);
      sscanf(z,"%d\n",&i);
      img = CreateImage(ncols,nrows);
      for (i=0; i < n; i++)
	fscanf(fp,"%d",&img->val[i]);
      fclose(fp);
    }else{
      fprintf(stderr,"Input image must be P2 or P5\n");
      exit(-1);
    }
  }

  return(img);
}

Image *MBB(Image *img)
{
  int x,y;
  Pixel left,right;
  Image *mbb=NULL;
  
  left.x  = img->ncols-1;
  left.y  = img->nrows-1;
  right.x = 0;
  right.y = 0;
  
  for (y=0; y < img->nrows; y++)
    for (x=0; x < img->ncols; x++){
      if (img->val[x+img->tbrow[y]] > 0){
	if (x < left.x)
	  left.x = x;
	if (y < left.y)
	  left.y = y;
	if (x > right.x)
	  right.x = x;
	if (y > right.y)
	  right.y = y;	
      }
    }
  
  mbb = ROI(img,left.x,left.y,right.x,right.y);

  return(mbb);	
}

Image *ROI(Image *img, int xl, int yl, int xr, int yr)
{
  int x,y,p,i;
  Image *roi=NULL;
  
  if (ValidPixel(img,xl,yl)&&ValidPixel(img,xr,yr)&&
      (xl <= xr)&&(yl <= yr)        )
    {
      roi = CreateImage(xr-xl+1,yr-yl+1);
      i=0;
      for (y=yl; y <= yr; y++)
	for (x=xl; x <= xr; x++){
	  p = x + img->tbrow[y];
	  roi->val[i] = img->val[p];
	  i++;
	}
    } 
  
  return(roi);	
}

Image *AddFrame(Image *img, int sz, int value)
{
  Image *fimg;
  int y,*dst,*src,nbytes,offset;

  fimg = CreateImage(img->ncols+(2*sz),img->nrows+(2*sz));
  SetImage(fimg,value);
  nbytes = sizeof(int)*img->ncols;
  offset = sz+fimg->tbrow[sz];
  for (y=0,src=img->val,dst=fimg->val+offset; y < img->nrows;y++,src+=img->ncols,dst+=fimg->ncols){
    memcpy(dst,src,nbytes);
  }
  return(fimg);
}

void SetImage(Image *img, int value)
{ 
  int i,n;
  n = img->ncols*img->nrows;
  for (i=0; i < n; i++){
    img->val[i]=value;
  }
}

void WriteImage(Image *img,char *filename)
{
  FILE *fp;
  int i, n, Imax;

  fp = fopen(filename,"wb");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  n    = img->ncols*img->nrows;
  if ((Imax=MaximumValue(img))==INT_MAX){
    Warning("Image with infinity values","WriteImage");
    Imax = INT_MIN;
    for (i=0; i < n; i++) 
      if ((img->val[i] > Imax)&&(img->val[i]!=INT_MAX))
	Imax = img->val[i];
    fprintf(fp,"P2\n");
    fprintf(fp,"%d %d\n",img->ncols,img->nrows);
    fprintf(fp,"%d\n",Imax+1);
  } else {
    fprintf(fp,"P2\n");
    fprintf(fp,"%d %d\n",img->ncols,img->nrows);
    if (Imax==0) Imax++;
    fprintf(fp,"%d\n",Imax);
  }
 
  for (i=0; i < n; i++) {
    if (img->val[i]==INT_MAX)
      fprintf(fp,"%d ",Imax+1);
    else
      fprintf(fp,"%d ",img->val[i]);
    if (((i+1)%17) == 0)
      fprintf(fp,"\n");
  }

  fclose(fp);
}

/* ppm images */
CImage *CreateCImage(int ncols, int nrows)
{
  CImage *cimg=NULL;
  int i;

  cimg = (CImage *) calloc(1, sizeof(CImage));
  for (i=0; i < 3; i++) 
    cimg->C[i] = CreateImage(ncols,nrows);
  return(cimg);
}

void    DestroyCImage(CImage **cimg)
{
  CImage *tmp;
  int i;

  tmp = *cimg;
  if (tmp != NULL) {
    for (i=0; i < 3; i++) 
      DestroyImage(&(tmp->C[i]));
    free(tmp);
    *cimg = NULL;
  }
}

CImage *ReadCImage(char *filename)
{
  CImage *cimg=NULL;
  FILE *fp=NULL;
  char type[10];
  int  i,ncols,nrows,n;
  char z[256];

  fp = fopen(filename,"rb");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  fscanf(fp,"%s\n",type);
  if((strcmp(type,"P6")==0)){
    NCFgets(z,255,fp);
    sscanf(z,"%d %d\n",&ncols,&nrows);
    n = ncols*nrows;
    NCFgets(z,255,fp);
    sscanf(z,"%d\n",&i);
    cimg = CreateCImage(ncols,nrows);
    for (i=0; i < n; i++){
      cimg->C[0]->val[i] = fgetc(fp);
      cimg->C[1]->val[i] = fgetc(fp);
      cimg->C[2]->val[i] = fgetc(fp);
    }
    fclose(fp);
  }else{
    fprintf(stderr,"Input image must be P6\n");
    exit(-1);
  }

  return(cimg);
}

/* auxiliary functions  ****************************/
Curve *CreateCurve(int n)
{
  Curve *curve=NULL;

  curve = (Curve *) calloc(1,sizeof(Curve));
  if (curve != NULL) {
    curve->X = AllocDoubleArray(n);
    curve->Y = AllocDoubleArray(n);
    curve->n = n;
  } else {
    Error(MSG1,"CreateCurve");
  }
  return(curve);
}

void DestroyCurve(Curve **curve)
{
  Curve *aux;

  aux = *curve;
  if (aux != NULL){
    if (aux->X != NULL) free(aux->X);
    if (aux->Y != NULL) free(aux->Y);
    free(aux);
    *curve = NULL;
  }
}

FeatureVector1D *CreateFeatureVector1D(int n)
{
  FeatureVector1D *desc=NULL;
  
  desc = (FeatureVector1D *) calloc(1,sizeof(FeatureVector1D));
  if (desc != NULL) {
    desc->X = AllocDoubleArray(n);
    desc->n = n;
  } else {
    Error(MSG1,"CreateFeatureVector");
  }
  return(desc);
}

FeatureVector1D  *CurveTo1DFeatureVector(Curve *curve){
  FeatureVector1D *fv;

  fv = CreateFeatureVector1D(curve->n);
  memcpy(fv->X,curve->Y,curve->n*sizeof(double));

 return fv;
}

FeatureVector2D  *CurveTo2DFeatureVector(Curve *curve){
  FeatureVector2D *fv;

  fv = CreateFeatureVector2D(curve->n);
  memcpy(fv->X,curve->X,curve->n*sizeof(double));
  memcpy(fv->Y,curve->Y,curve->n*sizeof(double));

 return fv;
}

void DestroyFeatureVector1D(FeatureVector1D **desc)
{
  FeatureVector1D *aux;
  
  aux = *desc;
  if (aux != NULL){
    if (aux->X != NULL) {
      free(aux->X);
    }
    free(aux);
    *desc = NULL;
  }
}

void WriteFeatureVector1D(FeatureVector1D *desc,char *filename)
{
  FILE *fp;
  int i;
  
  fp = fopen(filename,"w");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  for (i=0; i < desc->n; i++)
    fprintf(fp,"%f\n",desc->X[i]);
  
  fclose(fp);
}


AdjRel *CreateAdjRel(int n)
{
  AdjRel *A=NULL;

  A = (AdjRel *) calloc(1,sizeof(AdjRel));
  if (A != NULL){
    A->dx = AllocIntArray(n);
    A->dy = AllocIntArray(n);
    A->n  = n;
  } else {
    Error(MSG1,"CreateAdjRel");
  }

  return(A);
}

void DestroyAdjRel(AdjRel **A)
{
  AdjRel *aux;

  aux = *A;
  if (aux != NULL){
    if (aux->dx != NULL) free(aux->dx);
    if (aux->dy != NULL) free(aux->dy);
    free(aux);
    *A = NULL;
  }   
}

AdjRel *Circular(float r)
{
  AdjRel *A=NULL;
  int i,j,k,n,dx,dy,r0,r2,d,i0=0;
  float *da,*dr,aux;

  n=0;

  r0 = (int)r;
  r2  = (int)(r*r);
  for(dy=-r0;dy<=r0;dy++)
    for(dx=-r0;dx<=r0;dx++)
      if(((dx*dx)+(dy*dy)) <= r2)
	n++;
	
  A = CreateAdjRel(n);
  i=0;
  for(dy=-r0;dy<=r0;dy++)
    for(dx=-r0;dx<=r0;dx++)
      if(((dx*dx)+(dy*dy)) <= r2){
	A->dx[i]=dx;
	A->dy[i]=dy;
	if ((dx==0)&&(dy==0))
	  i0 = i;
	i++;
      }

  da = AllocFloatArray(A->n);
  dr = AllocFloatArray(A->n);
  for (i=0; i < A->n; i++) {
    dx = A->dx[i];
    dy = A->dy[i];
    dr[i] = (float)sqrt((dx*dx) + (dy*dy));
    if (i != i0){ 
      da[i] = atan2(-dy,-dx)*180.0/PI;
      if (da[i] < 0.0)
	da[i] += 360.0;
    }
  }
  da[i0] = 0.0;
  dr[i0] = 0.0;

  aux    = da[i0];
  da[i0] = da[0];
  da[0]  = aux;
  aux    = dr[i0];
  dr[i0] = dr[0];
  dr[0]  = aux;
  d         = A->dx[i0];
  A->dx[i0] = A->dx[0];
  A->dx[0]  = d;
  d         = A->dy[i0];
  A->dy[i0] = A->dy[0];
  A->dy[0]  = d;

   for (i=1; i < A->n-1; i++){
    k = i;
    for (j=i+1; j < A->n; j++)
      if (da[j] < da[k]){
	k = j;
      }
    aux   = da[i];
    da[i] = da[k];
    da[k] = aux;
    aux   = dr[i];
    dr[i] = dr[k];
    dr[k] = aux;
    d   = A->dx[i];
    A->dx[i] = A->dx[k];
    A->dx[k] = d;
    d        = A->dy[i];
    A->dy[i] = A->dy[k];
    A->dy[k] = d;
  }

    for (i=1; i < A->n-1; i++){
    k = i;
    for (j=i+1; j < A->n; j++)
      if ((dr[j] < dr[k])&&(da[j]==da[k])){
	k = j;
      }
    aux   = dr[i];
    dr[i] = dr[k];
    dr[k] = aux;
    d        = A->dx[i];
    A->dx[i] = A->dx[k];
    A->dx[k] = d;
    d        = A->dy[i];
    A->dy[i] = A->dy[k];
    A->dy[k] = d;
  }

  free(dr);
  free(da);

  return(A);
}

AdjRel *LeftSide(AdjRel *A)
{
  AdjRel *L=NULL;
  int i;
  float d;

  L = CreateAdjRel(A->n);
  for (i=0; i < L->n; i++){
    d  = sqrt(A->dx[i]*A->dx[i] + A->dy[i]*A->dy[i]);
    if (d != 0){
      L->dx[i] = ROUND(((float)A->dx[i]/2.0)+((float)A->dy[i]/d));
      L->dy[i] = ROUND(((float)A->dy[i]/2)-((float)A->dx[i]/d));
    }
  }
  
  return(L);
}


AdjRel *RightSide(AdjRel *A)
{
  AdjRel *R=NULL;
  int i;
  float d;

  R = CreateAdjRel(A->n);
  for (i=0; i < R->n; i++){
    d  = sqrt(A->dx[i]*A->dx[i] + A->dy[i]*A->dy[i]);
    if (d != 0){
      R->dx[i] = ROUND(((float)A->dx[i]/2.0)-((float)A->dy[i]/d));
      R->dy[i] = ROUND(((float)A->dx[i]/d)+((float)A->dy[i]/2.0));
    }
  }

  return(R);
}

bool ValidContPoint(Image *bin, AdjRel *L, AdjRel *R, int p)
{
  int i,q,n,left,right;
  Pixel u,v,l,r;
  bool found=false;

  u.x = p%bin->ncols;
  u.y = p/bin->ncols;
  n   = L->n;

  for (i=0; i < n; i++) {
    v.x = u.x + L->dx[i];
    v.y = u.y + L->dy[i];
    if (ValidPixel(bin,v.x,v.y)){
      q = v.x + bin->tbrow[v.y];
      if ((bin->val[q]==1)&&(p!=q)){
	l.x = u.x + L->dx[i]; 
	l.y = u.y + L->dy[i];
	r.x = u.x + R->dx[i]; 
	r.y = u.y + R->dy[i];	
	if (ValidPixel(bin,l.x,l.y))
	  left = l.x + bin->tbrow[l.y];
	else
	  left = -1;
	if (ValidPixel(bin,r.x,r.y))
	  right = r.x + bin->tbrow[r.y];
	else
	  right = -1;
	if (((left!=-1)&&(right!=-1)&&(bin->val[left]!=bin->val[right]))||
	    ((left==-1)&&(right!=-1)&&(bin->val[right]==1)) ||
	    ((right==-1)&&(left!=-1)&&(bin->val[left]==1))){
	  found = true;
	  break;
	}
      }
    }
  }
  
  return(found);
}

bool ValidPixel(Image *img, int x, int y)
{
  if ((x >= 0)&&(x < img->ncols)&&
      (y >= 0)&&(y < img->nrows))
    return(true);
  else
    return(false);
}

Image *LabelContPixel(Image *bin)
{
  Image *bndr=NULL;
  Image *color=NULL,*pred=NULL,*label=NULL;
  int p=0,q,r,i,j,n,left=0,right=0,*LIFO,last,l;
  AdjRel *A,*L,*R;
  Pixel u,v,w;
  
  A     = Circular(1.0);
  n     = bin->ncols*bin->nrows;
  bndr  = CreateImage(bin->ncols,bin->nrows);
  for (p=0; p < n; p++){
    if (bin->val[p]==1){
      u.x = p%bin->ncols;
      u.y = p/bin->ncols;
      for (i=1; i < A->n; i++){
	v.x = u.x + A->dx[i];
	v.y = u.y + A->dy[i];
	if (ValidPixel(bin,v.x,v.y)){
	  q = v.x + bin->tbrow[v.y];
	  if (bin->val[q]==0){
	    bndr->val[p]=1;
	    break;
	  }
	} else {
	    bndr->val[p]=1;
	    break;
	}
      }
    }
  }  
  DestroyAdjRel(&A);

  A      = Circular(1.5);
  L      = LeftSide(A);
  R      = RightSide(A);
  label  = CreateImage(bndr->ncols,bndr->nrows);
  color  = CreateImage(bndr->ncols,bndr->nrows);
  pred   = CreateImage(bndr->ncols,bndr->nrows);
  n      = bndr->ncols*bndr->nrows;
  LIFO   = AllocIntArray(n);
  last   = NIL;
  for (j=0; j < n; j++){
    if ((bndr->val[j]==1)
	&&(color->val[j]!=BLACK)
	&&ValidContPoint(bin,L,R,j)){
      last++;
      LIFO[last]    = j;
      color->val[j] = GRAY;
      pred->val[j] = j;
      while(last != NIL){
	p = LIFO[last]; last--;	
	color->val[p]=BLACK;
	u.x = p%bndr->ncols;
	u.y = p/bndr->ncols;
	for (i=1; i < A->n; i++){
	  v.x = u.x + A->dx[i];
	  v.y = u.y + A->dy[i];
	  if (ValidPixel(bndr,v.x,v.y)){
	    q = v.x + bndr->tbrow[v.y];
	    if ((q==j)&&(pred->val[p]!=j)){
	      last = NIL;
	      break;
	    }
	    
	    w.x = u.x + L->dx[i]; 
	    w.y = u.y + L->dy[i];
	    if (ValidPixel(bndr,w.x,w.y))
	      left = w.x + bndr->tbrow[w.y];
	    else
	      left = -1;
	    w.x = u.x + R->dx[i]; 
	    w.y = u.y + R->dy[i];
	    if (ValidPixel(bndr,w.x,w.y))
	      right = w.x + bndr->tbrow[w.y];
	    else
	      right = -1;
	    
	    if ((bndr->val[q]==1)&&
		(color->val[q] != BLACK)&&
		(((left!=-1)&&(right!=-1)&&(bin->val[left]!=bin->val[right]))||
		 ((left==-1)&&(right!=-1)&&(bin->val[right]==1)) ||
		 ((right==-1)&&(left!=-1)&&(bin->val[left]==1)))){ 
	      pred->val[q] = p;
	      if (color->val[q] == WHITE){
		last++;
		LIFO[last] = q;
		color->val[q]=GRAY;
	      }
	    } 
	  }
	}	
      }
      r = p;
      l = 1;
      while(pred->val[p]!=p){
	label->val[p] = l;
	p = pred->val[p];
	l++;
      }
      if (r != p) {
	label->val[p] = l;
      }
    }
  }

  DestroyAdjRel(&A);
  DestroyAdjRel(&L);
  DestroyAdjRel(&R);
  DestroyImage(&bndr);
  DestroyImage(&color);
  DestroyImage(&pred);
  free(LIFO);
  return(label);
}

Curve3D *CreateCurve3D(int n)
{
  Curve3D *curve=NULL;

  curve = (Curve3D *) calloc(1,sizeof(Curve3D));
  if (curve != NULL) {
    curve->X = AllocDoubleArray(n);
    curve->Y = AllocDoubleArray(n);
    curve->Z = AllocDoubleArray(n);
    curve->n = n;
  } else {
    Error(MSG1,"CreateCurve3D");
  }
  return(curve);
}

void DestroyCurve3D(Curve3D **curve)
{
  Curve3D *aux;

  aux = *curve;
  if (aux != NULL){
    if (aux->X != NULL) free(aux->X);
    if (aux->Y != NULL) free(aux->Y);
    if (aux->Z != NULL) free(aux->Z);
    free(aux);
    *curve = NULL;
  }
}

void SortCurve3D(Curve3D *curve, int left, int right, char order)
{
  int pivot;
 
  if (left < right) {
    pivot = PartCurve3D(curve,left,right,order);
    SortCurve3D(curve,left,pivot-1,order);
    SortCurve3D(curve,pivot+1,right,order); 
  }
}

int PartCurve3D (Curve3D *curve, int left, int right, char order)
{
  double z;
  int i;
  double X,Y,Z;
 
  z = curve->Z[left];
  i = left;
 
  do {
    if (order == INCREASING){
      while ((curve->Z[left] <= z)&&(left <= right)) left++;
      while (curve->Z[right]  > z) right--;
    } else { /* order = DECREASING */
      while ((curve->Z[left] >= z)&&(left <= right)) left++;
      while (curve->Z[right]  < z) right--;
    }
    if (left < right){
      X = curve->X[left];
      Y = curve->Y[left];
      Z = curve->Z[left];
      curve->X[left]  = curve->X[right];
      curve->Y[left]  = curve->Y[right];
      curve->Z[left]  = curve->Z[right];
      curve->X[right] = X;
      curve->Y[right] = Y;
      curve->Z[right] = Z;
      left++; right--;
    }
  } while (left <= right);

  left = i;

  if (left != right){
    X = curve->X[left];
    Y = curve->Y[left];
    Z = curve->Z[left];
    curve->X[left]  = curve->X[right];
    curve->Y[left]  = curve->Y[right];
    curve->Z[left]  = curve->Z[right];
    curve->X[right] = X;
    curve->Y[right] = Y;
    curve->Z[right] = Z;
  }

  return (right);
}

int FFT(int dir, long nn, double *x, double *y)
{
   int m;
   long i,i1,j,k,i2,l,l1,l2;
   double c1,c2,tx,ty,t1,t2,u1,u2,z;

   m = (int)(log(nn)/log(2)+.00001);
   
   i2 = nn >> 1;
   j = 0;
   for (i=0;i<nn-1;i++) {
      if (i < j) {
         tx = x[i];
         ty = y[i];
         x[i] = x[j];
         y[i] = y[j];
         x[j] = tx;
         y[j] = ty;
      }
      k = i2;
      while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
   }

   c1 = -1.0;
   c2 = 0.0;
   l2 = 1;
   for (l=0;l<m;l++) {
      l1 = l2;
      l2 <<= 1;
      u1 = 1.0;
      u2 = 0.0;
      for (j=0;j<l1;j++) {
         for (i=j;i<nn;i+=l2) {
            i1 = i + l1;
            t1 = u1 * x[i1] - u2 * y[i1];
            t2 = u1 * y[i1] + u2 * x[i1];
            x[i1] = x[i] - t1;
            y[i1] = y[i] - t2;
            x[i] += t1;
            y[i] += t2;
         }
         z =  u1 * c1 - u2 * c2;
         u2 = u1 * c2 + u2 * c1;
         u1 = z;
      }
      c2 = sqrt((1.0 - c1) / 2.0);
      if (dir == 1)
         c2 = -c2;
      c1 = sqrt((1.0 + c1) / 2.0);
   }

   if (dir == -1) {
      for (i=0;i<nn;i++) {
         x[i] /= (double)nn;
         y[i] /= (double)nn;
      }
   }

   return(0);
}

Image *LabelContour(Image *bin)
{
  Image *bndr=NULL;
  Image *color=NULL,*pred=NULL,*label=NULL;
  int p=0,q,r,i,j,left=0,right=0,n,*LIFO,last,l=1;
  AdjRel *A,*L,*R;
  Pixel u,v,w;
  
  A     = Circular(1.0);
  n     = bin->ncols*bin->nrows;
  bndr  = CreateImage(bin->ncols,bin->nrows);
  for (p=0; p < n; p++){
    if (bin->val[p]==1){
		  
      u.x = p%bin->ncols;
      u.y = p/bin->ncols;
      for (i=1; i < A->n; i++){
	v.x = u.x + A->dx[i];
	v.y = u.y + A->dy[i];
	if (ValidPixel(bin,v.x,v.y)){
	  q = v.x + bin->tbrow[v.y];
	  if (bin->val[q]==0){
	    bndr->val[p]=1;
	    break;
	  }
	} else {
	    bndr->val[p]=1;
	    break;
	}
      }
    }
  }
  DestroyAdjRel(&A);

  A      = Circular(1.5);
  L      = LeftSide(A);
  R      = RightSide(A);
  label  = CreateImage(bndr->ncols,bndr->nrows);
  color  = CreateImage(bndr->ncols,bndr->nrows);
  pred   = CreateImage(bndr->ncols,bndr->nrows);
  LIFO   = AllocIntArray(n);
  last   = NIL;
  for (j=0; j < n; j++){
    if ((bndr->val[j]==1)&&
	(color->val[j]!=BLACK)&&
	ValidContPoint(bin,L,R,j)){      
      last++; LIFO[last]    = j;
      color->val[j] = GRAY;
      pred->val[j] = j;
      while(last != NIL){
	p = LIFO[last];	last--;	
	color->val[p]=BLACK;
	u.x = p%bndr->ncols;
	u.y = p/bndr->ncols;
	for (i=1; i < A->n; i++){
	  v.x = u.x + A->dx[i];
	  v.y = u.y + A->dy[i];
	  if (ValidPixel(bndr,v.x,v.y)){
	    q = v.x + bndr->tbrow[v.y];
	    if ((q==j)&&(pred->val[p]!=j)){
	      last = NIL;
	      break;
	    }
	    w.x = u.x + L->dx[i]; 
	    w.y = u.y + L->dy[i];
	    if (ValidPixel(bndr,w.x,w.y))
	      left = w.x + bndr->tbrow[w.y];
	    else
	      left = -1;
	    w.x = u.x + R->dx[i]; 
	    w.y = u.y + R->dy[i];
	    if (ValidPixel(bndr,w.x,w.y))
	      right = w.x + bndr->tbrow[w.y];
	    else
	      right = -1;
	    
	    if ((bndr->val[q]==1)&&
		(color->val[q] != BLACK)&&
		(((left!=-1)&&(right!=-1)&&(bin->val[left]!=bin->val[right]))||
		 ((left==-1)&&(right!=-1)&&(bin->val[right]==1)) ||
		 ((right==-1)&&(left!=-1)&&(bin->val[left]==1))) ) {
	      pred->val[q] = p;
	      if (color->val[q] == WHITE){
		last++; LIFO[last] = q;
		color->val[q]=GRAY;
	      }
	    } 
	  }
	}	
      }
      r = p;
      while(pred->val[p]!=p){
	label->val[p] = l;
	p = pred->val[p];
      } 
      if (r != p){ 
	label->val[p] = l;
	l++;
      }
    }
  }

  DestroyAdjRel(&A);
  DestroyAdjRel(&L);
  DestroyAdjRel(&R);
  DestroyImage(&bndr);
  DestroyImage(&color);
  DestroyImage(&pred);
  free(LIFO);
  return(label);
}

void Warning(char *msg,char *func){ 
 fprintf(stdout,"Warning:%s in %s\n",msg,func);
}

Image *Scale(Image *img, float Sx, float Sy) 
{
  float S[2][2],x,y,d1,d2,d3,d4,Ix1,Ix2,If;
  Image *scl;
  Pixel u,v,prev,next;
  
  if (Sx == 0.0) Sx = 1.0;
  if (Sy == 0.0) Sy = 1.0;

  S[0][0] = 1.0/Sx;
  S[0][1] = 0;
  S[1][0] = 0;
  S[1][1] = 1.0/Sy;

  scl = CreateImage((int)(img->ncols*fabs(Sx) + 0.5),(int)(img->nrows*fabs(Sy) + 0.5)); 

  for (v.y=0; v.y < scl->nrows; v.y++)
    for (v.x=0; v.x < scl->ncols; v.x++){
      x = ((v.x-scl->ncols/2.)*S[0][0] + (v.y-scl->nrows/2.)*S[0][1]) 
	+ img->ncols/2.;
      y = ((v.x-scl->ncols/2.)*S[1][0] + (v.y-scl->nrows/2.)*S[1][1]) 
	+ img->nrows/2.;
      u.x = (int)(x+0.5);
      u.y = (int)(y+0.5);
      if (ValidPixel(img,u.x,u.y)){
	if (x < u.x) {
	  next.x = u.x;
	  prev.x = u.x - 1;
	} else {
	  next.x = u.x + 1;
	  prev.x = u.x;
	}
	d1 = next.x - x;
	d2 = x - prev.x;
	if (y < u.y) {
	  next.y = u.y;
	  prev.y = u.y - 1;
	} else {
	  next.y = u.y + 1;
	  prev.y = u.y;
	}
	d3 = next.y - y;
	d4 = y - prev.y;

	if (ValidPixel(img,prev.x,prev.y)&&ValidPixel(img,next.x,prev.y))
	  Ix1 = d1*img->val[prev.x+img->tbrow[prev.y]] + 
	    d2*img->val[next.x+img->tbrow[prev.y]];
	else
	  Ix1 = img->val[u.x+img->tbrow[u.y]];

	if (ValidPixel(img,prev.x,next.y)&&ValidPixel(img,next.x,next.y))
	  Ix2 = d1*img->val[prev.x+img->tbrow[next.y]] + 
	    d2*img->val[next.x+img->tbrow[next.y]];
	else
	  Ix2 = img->val[u.x+img->tbrow[u.y]];
	
	If = d3*Ix1 + d4*Ix2;

	scl->val[v.x+scl->tbrow[v.y]] = (int)If;
      }
    }
  
  return(scl);
}

DImage *CreateDImage(int ncols, int nrows)
{
  DImage *dimg=NULL;
  int i;

  dimg = (DImage *) calloc(1,sizeof(DImage));
  if (dimg == NULL){
    Error(MSG1,"CreateDImage");
  }

  dimg->val   = AllocDoubleArray(nrows*ncols);
  dimg->tbrow = AllocIntArray(nrows);

  dimg->tbrow[0]=0;
  for (i=1; i < nrows; i++)
    dimg->tbrow[i]=dimg->tbrow[i-1]+ncols;
  dimg->ncols = ncols;
  dimg->nrows = nrows;
 
 return(dimg);
}

void DestroyDImage(DImage **dimg)
{
  DImage *aux;

  aux = *dimg;
  if(aux != NULL){
    if (aux->val != NULL)   free(aux->val); 
    if (aux->tbrow != NULL) free(aux->tbrow);
    free(aux);    
    *dimg = NULL;
  }
}

Image *CopyImage(Image *img)
{
  Image *imgc;

  imgc = CreateImage(img->ncols,img->nrows);
  memcpy(imgc->val,img->val,img->ncols*img->nrows*sizeof(int));
  
  return(imgc);
}

int MaximumValue(Image *img)
{
  unsigned int i, n, r;
  int max;

  max = img->val[0];
  n = img->ncols*img->nrows - 1;
  r = n%4;
  n -= r;
  for (i=1; i < n; i+=4) {
    if (img->val[i] > max)
      max = img->val[i];
    if (img->val[i+1] > max)
      max = img->val[i+1];
    if (img->val[i+2] > max)
      max = img->val[i+2];
    if (img->val[i+3] > max)
      max = img->val[i+3];
  }
  while (r != 0) {
    if (img->val[i+r-1] > max)
      max = img->val[i+r-1];
    --r;
  }

  return(max);
}

Queue *CreateQueue(int nbuckets, int nelems)
{
  Queue *Q=NULL;

  Q = (Queue *) malloc(1*sizeof(Queue));
  
  if (Q != NULL) {
    Q->C.first = (int *)malloc(nbuckets * sizeof(int));
    Q->C.last  = (int *)malloc(nbuckets * sizeof(int));
    Q->C.nbuckets = nbuckets;
    if ( (Q->C.first != NULL) && (Q->C.last != NULL) ){
      Q->L.elem = (Node *)malloc(nelems*sizeof(Node));
      Q->L.nelems = nelems;
      if (Q->L.elem != NULL){
	ResetQueue(Q);
      } else
	Error(MSG1,"CreateQueue");	
    } else
      Error(MSG1,"CreateQueue");
  } else 
    Error(MSG1,"CreateQueue");
  
  return(Q);
}

void DestroyQueue(Queue **Q)
{
  Queue *aux;

  aux = *Q;
  if (aux != NULL) {
    if (aux->C.first != NULL) free(aux->C.first);
    if (aux->C.last  != NULL) free(aux->C.last);
    if (aux->L.elem  != NULL) free(aux->L.elem);
    free(aux);
    *Q = NULL;
  }
}

void InsertQueue(Queue *Q, int bucket, int elem)
{
  if (Q->C.first[bucket] == NIL){ 
    Q->C.first[bucket]   = elem;  
    Q->L.elem[elem].prev = NIL;
  }else {
    Q->L.elem[Q->C.last[bucket]].next = elem;
    Q->L.elem[elem].prev = Q->C.last[bucket];
  }
  
  Q->C.last[bucket]     = elem;
  Q->L.elem[elem].next  = NIL;
  Q->L.elem[elem].color = GRAY;
}

int RemoveQueue(Queue *Q)
{
  int elem=NIL, next, prev;
  int last;

  /** moves to next element or returns EMPTY queue **/
  if (Q->C.first[Q->C.current] == NIL) {
    last = Q->C.current;
    
    Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets);
    
    while ((Q->C.first[Q->C.current] == NIL) && (Q->C.current != last)) {
      Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets);
    }
    
    if (Q->C.first[Q->C.current] == NIL)
      return NIL;
  }

  if (Q->C.tiebreak == LIFOBREAK) {
    elem = Q->C.last[Q->C.current];
  
    prev = Q->L.elem[elem].prev;
    if (prev == NIL) {         /* there was a single element in the list */
      Q->C.last[Q->C.current] = Q->C.first[Q->C.current]  = NIL;    
    }
    else {
      Q->C.last[Q->C.current] = prev;
      Q->L.elem[prev].next = NIL;
    }
  } else { /* Assume FIFO policy for breaking ties */
    elem = Q->C.first[Q->C.current];
  
    next = Q->L.elem[elem].next;
    if (next == NIL) {         /* there was a single element in the list */
      Q->C.first[Q->C.current] = Q->C.last[Q->C.current]  = NIL;    
    }
    else {
      Q->C.first[Q->C.current] = next;
      Q->L.elem[next].prev = NIL;
    }
  }

  Q->L.elem[elem].color = BLACK;

  return elem;
}

void UpdateQueue(Queue *Q, int elem, int from, int to)
{
  RemoveQueueElem(Q, elem, from);
  InsertQueue(Q, to, elem);
}

int EmptyQueue(Queue *Q)
{
  int last;
  if (Q->C.first[Q->C.current] != NIL)
    return 0;
  
  last = Q->C.current;
  
  Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets);
  
  while ((Q->C.first[Q->C.current] == NIL) && (Q->C.current != last)) {
    Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets); 
  }
  
  return (Q->C.first[Q->C.current] == NIL);
}

void ResetQueue(Queue *Q)
{
  unsigned int i;

  Q->C.current  = 0;
  SetTieBreak(Q,FIFOBREAK);
#if defined(__i386__) && (NIL==-1 || NIL==0)
  i = Q->C.nbuckets*sizeof(int);
  memset(Q->C.first, NIL, i);
  memset(Q->C.last,  NIL, i);
  memset(Q->L.elem,  NIL, Q->L.nelems*sizeof(Node));
  for (i=0; i<Q->L.nelems; ++i)
    Q->L.elem[i].color = WHITE;
#else
  for (i=0; i < Q->C.nbuckets; ++i)
    Q->C.first[i]=Q->C.last[i]=NIL;
  for (i=0; i < Q->L.nelems; ++i) {
    Q->L.elem[i].next =  Q->L.elem[i].prev = NIL;
    Q->L.elem[i].color = WHITE;
  }
#endif
}

void RemoveQueueElem(Queue *Q, int elem, int bucket)
{
  int prev,next;

  prev = Q->L.elem[elem].prev;
  next = Q->L.elem[elem].next;
  
  /* if elem is the first element */
  if (Q->C.first[bucket] == elem) {
    Q->C.first[bucket] = next;
    if (next == NIL) /* elem is also the last one */
      Q->C.last[bucket] = NIL;
    else
      Q->L.elem[next].prev = NIL;
  }
  else{   /* elem is in the middle or it is the last */
    Q->L.elem[prev].next = next;
    if (next == NIL) /* if it is the last */
      Q->C.last[bucket] = prev;
    else 
      Q->L.elem[next].prev = prev;
  }

  Q->L.elem[elem].color = BLACK;
}

Curve *SamplePolynom(Polynom *P, double from, double to, int nbins)
{
  Curve *curve=NULL;
  double x = from,val;
  double inc = (to-from)/nbins;
  int i,p;
  
  if ((from <= to)&&(nbins > 0)) {
    curve = CreateCurve(nbins);
    for (p=0; p < nbins; p++){
      val=0.0;
      for (i=0; i <= P->n; i++)
	val += pow(x,i)*P->coef[i];
      curve->X[p] = x;
      curve->Y[p] = val;
      x +=inc;
    }
  }
  return(curve);
}

Polynom *CreatePolynom(int degree)
{
  Polynom *P=NULL;

  P = (Polynom *) calloc(1,sizeof(Polynom));  
  P->coef   = AllocDoubleArray(degree + 1);
  P->n      = degree;
  return(P);
}

void DestroyPolynom(Polynom **P)
{
  Polynom *aux=NULL;

  aux = *P;
  if(aux != NULL){
    if(aux->coef != NULL) free(aux->coef);
    free(aux);
    *P = NULL;
  }
}

Polynom *DerivPolynom(Polynom *P)
{
  Polynom *D;
  int i,j;

  D = CreatePolynom(P->n-1);
  j = 0;
  for (i=1; i <= P->n; i++){
    D->coef[j] = i*P->coef[i];
    j++;
  }
  return(D);
}


Polynom *Regression(Curve *curve, int degree)
{
  Polynom *P=NULL;
  double *A=NULL,*B=NULL;
  int i,j,k;
  double sum, m;

  /* Compute Non-Linear System: A*P=B, where P are the coefficients of
     the polynomial */

  A = AllocDoubleArray((degree+1)*(degree+1));
  B = AllocDoubleArray(degree+1);

  for (i=1; i<= 2*degree; i++){
    sum = 0.0;
    for (k=0; k < curve->n; k++){
      sum += pow(curve->X[k], i);
    }
    if (i<=degree){
      for (j=0; j<=i; j++){ 
	A[(i-j) + j*(degree+1)] = sum;
      }
    }
    else {
      for (j= (i-degree); j<= degree; j++){ 
	A[(i-j) + j*(degree+1)] = sum;
      }
    }
  }
  A[0]= curve->n;
  
  for (i=0; i<=degree; i++){
    sum = 0.0;
    for (k=0; k < curve->n; k++){
      sum += pow(curve->X[k], i)*curve->Y[k];
    }
    B[i] = sum;
  }

  /* Gauss's Regression Method */

  for(k = 0; k < degree; k++){ /* Triangulation of A */
    for(i = k+1; i<= degree; i++){
      m = A[i*(degree+1)+k]/A[k*(degree+1)+k];
      A[i*(degree+1)+k] = 0.0;
      for (j = k+1; j<= degree; j++){
	A[i*(degree+1)+j] = A[i*(degree+1)+j] - m *A[k*(degree+1)+j];
      }
      B[i] = B[i] - m * B[k];
    }
  }
    
  P = CreatePolynom(degree);

  P->coef[degree] = B[degree]/A[degree*(degree+1) + degree];
  for(k=degree-1; k>=0; k--){
    sum= 0.0;
    for(j=k+1; j<=degree; j++){
      sum += A[k*(degree+1)+j]*P->coef[j];
    }
    P->coef[k] = (B[k]-sum)/A[k*(degree+1)+k];
  }
 
  free(A);
  free(B);
  
  return(P);
}

void   DeAnnotate(AnnImg **aimg)
{
  AnnImg *aux;
  
  aux = *aimg;
  if (aux != NULL){
    DestroyImage(&(aux->cost));
    DestroyImage(&(aux->label));
    DestroyImage(&(aux->pred));
    DestroySet(&(aux->seed));
    free(aux);
    *aimg = NULL;
  }
}

Polynom *MSFractal(Image *bin, 
		   int maxdist, 
		   int degree, 
		   double lower, 
		   double higher,
		   int reg,
		   double from,
		   double to)
{
  Curve *hist=NULL,*haux=NULL,*ahist=NULL, *aux_ahist=NULL,*loglog=NULL;
  AnnImg *aimg=NULL;
  AdjRel *A=NULL;
  Image *mbb=NULL,*nbin=NULL;
  Polynom *P=NULL,*D=NULL;
  int n,i,j,maxcost=maxdist*maxdist;
    
  mbb  = MBB(bin);
  nbin = AddFrame(mbb,maxdist,0);
  DestroyImage(&mbb);

  /* Compute Euclidean IFT */

  A = Circular(1.5);
  aimg = Annotate(nbin,NULL,nbin);
  iftDilation(aimg, A);
  DestroyAdjRel(&A);

  /* Compute MS Fractal */

  hist = Histogram(aimg->cost);

  /* Compute non-zero points */

  n = 0;
  for (i=1; i < maxcost; i++)
    if (hist->Y[i] != 0)
      n++;

  haux = CreateCurve(n);
  j=0;
  for (i=1; i < maxcost; i++)
    if (hist->Y[i] != 0){
      haux->X[j] = log(sqrt((double)i));
      haux->Y[j] = hist->Y[i];
      j++;
    }
  
  /* Accumulate values */
  ahist = CreateCurve(n);
  ahist->X[0] = haux->X[0];
  ahist->Y[0] = haux->Y[0];
  for (i=1; i < n; i++) {
    ahist->X[i] = haux->X[i];
    ahist->Y[i] = ahist->Y[i-1] + haux->Y[i];
  }

  /* Compute log(Y) */
  for (i=0; i < n; i++)
    ahist->Y[i] = log((double)ahist->Y[i]);
  
  j=0;
 
  for (i=0; i < n; i++)
    if ((ahist->X[i]>from)&&((ahist->X[i]<to)))
      j++;
  
  aux_ahist = CreateCurve(j);
  
  j=0;
  for (i=0; i < n; i++)
    if ((ahist->X[i]>from)&&((ahist->X[i]<to))){
      aux_ahist->X[j] = ahist->X[i];
      aux_ahist->Y[j] = ahist->Y[i];
      j++;
    }
  
  
  /* Compute Regression */
  P = Regression(/*ahist*/aux_ahist,degree);
  
  /* Print loglog curve */
  if (reg){
    loglog = SamplePolynom(P,lower, higher, 100);
  }
  
  /* Compute Fractal Curve */
  D = DerivPolynom(P);  

  DestroyCurve(&hist);
  DestroyCurve(&haux);
  DestroyCurve(&ahist);
  DestroyCurve(&aux_ahist);
  DestroyCurve(&loglog);
  DestroyPolynom(&P);
  DeAnnotate(&aimg);
  DestroyImage(&nbin);
  return(D);
}

AnnImg *Annotate(Image *img, Image *cost, Image *label)
{
  AnnImg *aimg=NULL;
  int p,n;

  aimg = (AnnImg *) calloc(1,sizeof(AnnImg));
  if (aimg == NULL)
    Error(MSG1,"Annotate");

  aimg->img   = img;
  aimg->cost  = CreateImage(img->ncols,img->nrows);    
  aimg->label = CreateImage(img->ncols,img->nrows);
  aimg->pred  = CreateImage(img->ncols,img->nrows);
  aimg->root = NULL;
  aimg->seed  = NULL;

  n = img->ncols*img->nrows;

  if ((cost == NULL)&&(label == NULL))
    for (p=0; p < n; p++){
      aimg->cost->val[p]  = INT_MAX;
      aimg->label->val[p] = 0;
      aimg->pred->val[p]  = p;
    }
  else
    if ((cost == NULL)&&(label != NULL))
      for (p=0; p < n; p++){
	aimg->pred->val[p]  = p;
	if (label->val[p] > 0) {
	  aimg->cost->val[p]  = 0;
	  aimg->label->val[p] = label->val[p];
	  InsertSet(&(aimg->seed),p);
	} else {
	  aimg->cost->val[p]  = INT_MAX;
	  aimg->label->val[p] = 0;
	}
      }
    else
      if ((cost != NULL)&&(label == NULL))
	for (p=0; p < n; p++){
	  aimg->cost->val[p]  = cost->val[p];
	  aimg->label->val[p] = p;
	  aimg->pred->val[p]  = p;
	  InsertSet(&(aimg->seed),p);
	}
      else
	if ((cost != NULL)&&(label != NULL))	
	  for (p=0; p < n; p++){
	    aimg->pred->val[p]  = p;
	    aimg->label->val[p] = label->val[p];
	    if (label->val[p] > 0) {
	      aimg->cost->val[p]  = cost->val[p];
	      InsertSet(&(aimg->seed),p);
	    } else { 
	      aimg->cost->val[p]  = INT_MAX;
	    }
	  }
  
  return(aimg);
}

void InsertSet(Set **S, int elem)
{
  Set *p=NULL;

  p = (Set *) calloc(1,sizeof(Set));
  if (p == NULL) Error(MSG1,"InsertSet");
  if (*S == NULL){
    p->elem  = elem;
    p->next  = NULL;
  }else{
    p->elem  = elem;
    p->next  = *S;
  }
  *S = p;
}

int RemoveSet(Set **S)
{
  Set *p;
  int elem=NIL;
  
  if (*S != NULL){
    p    =  *S;
    elem = p->elem;
    *S   = p->next;
    //printf("RemoveSet before free");
    free(p);
    //printf(" RemoveSet after free: elem is %d\n",elem);
    //if(*S != NULL) printf(" *S->elem is %d\n",(*S)->elem);
  }
  return(elem);
}

void DestroySet(Set **S)
{
  Set *p;
  while(*S != NULL){
    p = *S;
    *S = p->next;
    free(p);
  }
}

int FrameSize(AdjRel *A)
{
  int sz=INT_MIN,i=0;

  for (i=0; i < A->n; i++){
    if (fabs(A->dx[i]) > sz) 
      sz = fabs(A->dx[i]);
    if (fabs(A->dy[i]) > sz) 
      sz = fabs(A->dy[i]);
  }
  return(sz);
}

AdjRel *ComplAdj(AdjRel *A1, AdjRel *A2)
{
  AdjRel *A;
  int i,j,n;
  char *subset=NULL;

  if (A1->n > A2->n){
    A  = A1;
    A1 = A2;
    A2 = A;
  }

  A = NULL;
  subset = AllocCharArray(A2->n);
  n = 0;
  for (i=0; i < A1->n; i++) 
    for (j=0; j < A2->n; j++)
      if ((A1->dx[i]==A2->dx[j])&&(A1->dy[i]==A2->dy[j])){	  
	subset[j] = 1;
	n++;
	break;
      }
  n = A2->n - n;
  
  if (n == 0) /* A1 == A2 */    
    return(NULL);

  A = CreateAdjRel(n);
  j=0;
  for (i=0; i < A2->n; i++) 
    if (subset[i] == 0){
      A->dx[j] = A2->dx[i];
      A->dy[j] = A2->dy[i];
      j++;
    }

  free(subset);

  return(A);
}

Heap *CreateHeap(int n, int *cost){
  Heap *H=NULL;
  int i;

  if (cost == NULL){
    Warning("Cannot create heap without cost map","CreateHeap");
    return(NULL);
  }

  H = (Heap *) calloc(1,sizeof(Heap));
  if (H != NULL) {
    H->n     = n;
    H->cost  = cost;
    H->color = (char *) calloc(n,sizeof(char));
    H->pixel = (int *) calloc(n,sizeof(int));
    H->pos   = (int *) calloc(n,sizeof(int));
    H->last = -1;
    if ((H->color == NULL) || (H->pos == NULL) || (H->pixel == NULL))
      Error(MSG1,"CreateHeap");
    for (i=0; i < H->n; i++){
      H->color[i]=WHITE;
      H->pos[i]=-1;
      H->pixel[i]=-1;
    }    
  } 
  else
    Error(MSG1,"CreateHeap");
  
  return(H);
}

void DestroyHeap(Heap **H){
  Heap *aux;
  
  aux = *H;
  if (aux != NULL) {
    if (aux->pixel != NULL) free(aux->pixel);
    if (aux->color != NULL) free(aux->color);
    if (aux->pos != NULL)   free(aux->pos);
    free(aux);
    *H = NULL;
  }
}

bool InsertHeap(Heap *H, int pixel){
  if (!IsFullHeap(H)){
    H->last ++;
    H->pixel[H->last] = pixel;
    H->color[pixel]   = GRAY;
    H->pos[pixel]     = H->last;
    GoUpHeap(H,H->last); 
    return(true);
  } else 
    return(false);
}

bool RemoveHeap(Heap *H, int *pixel){
  if (!IsEmptyHeap(H)){
    *pixel = H->pixel[0];
    H->pos[*pixel]=-1;
    H->color[*pixel] = BLACK;
    H->pixel[0] = H->pixel[H->last];
    H->pos[H->pixel[0]] = 0;
    H->last--;
    GoDownHeap(H,0);
    return(true);
  } else 
    return(false);
}

bool IsEmptyHeap(Heap *H){
  if (H->last == -1)
    return(true);
  else
    return(false);
}

void GoDownHeap(Heap *H, int i){
  int least, left=HEAP_LEFTSON(i), right=HEAP_RIGHTSON(i);

  if ((left <= H->last)&&(H->cost[H->pixel[left]] < H->cost[H->pixel[i]]))
    least = left;
  else
    least = i;

  if ((right <= H->last)&&(H->cost[H->pixel[right]] < H->cost[H->pixel[least]]))
    least = right;

  if (least != i){
    Change(&H->pixel[least],&H->pixel[i]);
    H->pos[H->pixel[i]]=i;
    H->pos[H->pixel[least]]=least;
    GoDownHeap(H,least);
 }
}

bool IsFullHeap(Heap *H){
  if (H->last == (H->n-1))
    return(true);
  else
    return(false);
}

bool HeapIsEmpty(Heap *H){
  return IsEmptyHeap(H);
}

void Change(int *a, int *b){
  int c;    
  c  = *a;
  *a = *b;
  *b = c;
}

void GoUpHeap(Heap *H, int i){
  int j = HEAP_DAD(i);

  while((i > 0) && (H->cost[H->pixel[j]] > H->cost[H->pixel[i]])){
    Change(&H->pixel[j],&H->pixel[i]);
    H->pos[H->pixel[i]]=i;
    H->pos[H->pixel[j]]=j;
    i = j;
    j = HEAP_DAD(i);
  }
}

void DestroyAdjPxl(AdjPxl **N)
{
  AdjPxl *aux;

  aux = *N;
  if (aux != NULL){
    if (aux->dp != NULL) free(aux->dp);
    free(aux);
    *N = NULL;
  }
}

Image *CompPaths(Image *pred)
{
  Image *seed=NULL;
  int p,n;

  seed = CopyImage(pred);

  n = seed->ncols*seed->nrows;
  for (p=0; p < n; p++) 
    seed->val[p] = Seed(seed,p);
  return(seed);
}

int Seed(Image *pred, int p)
{
  if (pred->val[p]==p)
    return(p);
  else 
    return(Seed(pred,pred->val[p]));    
}

Image *Perimeter(Image *bin)
{
  int p,n;
  Image *cont,*perim;
  Curve *hist;

  cont  = LabelContour(bin);
  n     = cont->ncols*cont->nrows;
  perim = CreateImage(cont->ncols,cont->nrows);
  hist  = Histogram(cont);
  for (p=0; p < n; p++)
    if (cont->val[p] > 0)
      perim->val[p] = hist->Y[cont->val[p]];

  DestroyCurve(&hist);
  DestroyImage(&cont);

  return(perim);
}

Curve3D *Saliences(Image *bin, int maxdist) 
{  
  Image *cont=NULL;
  AdjRel *A=NULL;
  AnnImg *aimg=NULL;
  Curve3D *saliences=NULL;
  
  /* Compute Euclidean IFT */
  
  cont    = LabelContPixel(bin);
  aimg    = Annotate(bin,NULL,cont); 
  A       = Circular(1.5);
  iftDilation(aimg,A);
  saliences = CompSaliences(aimg, maxdist*maxdist);
  
  DestroyImage(&cont);
  DestroyAdjRel(&A);
  DeAnnotate(&aimg);
  
  return(saliences);
}

Image *MSSkel(Image *bin, char side)
{
  Image *msskel,*cont;
  AdjRel *A;
  AnnImg *aimg;
  int p,n;

  /* Compute Euclidean IFT */

  cont   = LabelContPixel(bin);
  aimg   = Annotate(bin,NULL,cont); 
  A      = Circular(1.5);
  n      = aimg->img->ncols*aimg->img->nrows;
  switch (side) {
  case INTERIOR:
    for(p = 0; p < n; p++)
      if (aimg->img->val[p] == 0){
	aimg->cost->val[p] = 0;
      }
    break;
  case EXTERIOR:
    for(p = 0; p < n; p++)
      if (aimg->img->val[p] != 0){
	aimg->cost->val[p] = 0;
      }
    break;
  case BOTH:
  default:    
    ;
  }
  iftDilation(aimg,A);
  DestroyAdjRel(&A);
  DestroyImage(&cont);
  
  /* Compute MS Skeletons */

  msskel = CompMSSkel(aimg);
  DeAnnotate(&aimg);

  return(msskel);
}

Curve3D *CompSaliences(AnnImg *aimg, int maxcost)
{
  Image *cont=NULL;
  double *inter=NULL,*exter=NULL;  
  int p,n,i,Lmax;
  Curve3D *saliences=NULL;
  
  n       = aimg->img->ncols*aimg->img->nrows;
  cont = CreateImage(aimg->img->ncols,aimg->img->nrows);
  for(p=0;p<n; p++){
    if (aimg->pred->val[p]==p){
      cont->val[p]=aimg->label->val[p];
    }
  }
  
  Lmax    = MaximumValue(aimg->label);
  inter   = AllocDoubleArray(Lmax);
  exter   = AllocDoubleArray(Lmax);
  
  
  /* Compute influence areas */

  for (p=0; p < n; p++){
    if ((aimg->label->val[p] > 0)&&(aimg->cost->val[p] <= maxcost)) {
      if (aimg->img->val[p] != 0){
	inter[aimg->label->val[p]-1]++;
      } else {
	exter[aimg->label->val[p]-1]++;
      }
    }
  }
  
  /* Compute saliences */
  saliences  = CreateCurve3D(Lmax);
  
  for (p=0; p < n; p++){
    if (cont->val[p] > 0){
      i = cont->val[p]-1;
      saliences->X[i] = (double)(p%cont->ncols);
      saliences->Y[i] = (double)(p/cont->ncols);
      if (exter[i] > inter[i]){
	saliences->Z[i] = exter[i];
      }else{
	if (exter[i] < inter[i]){
	  saliences->Z[i] = -inter[i];
	}else{
	  saliences->Z[i] = 0.0;
	}
      }
    }
  }

  DestroyImage(&cont);
  free(inter);
  free(exter);

  return(saliences);
}

Image *RemFrame(Image *fimg, int sz)
{
  Image *img;
  int y,*dst,*src,nbytes,offset;

  img = CreateImage(fimg->ncols-(2*sz),fimg->nrows-(2*sz));
  nbytes = sizeof(int)*img->ncols;
  offset = sz+fimg->tbrow[sz];
  for (y=0,src=fimg->val+offset,dst=img->val; y < img->nrows;y++,src+=fimg->ncols,dst+=img->ncols){
    memcpy(dst,src,nbytes);
  }
  return(img);
}

AdjPxl *AdjPixels(Image *img, AdjRel *A)
{
  AdjPxl *N;
  int i;

  N = (AdjPxl *) calloc(1,sizeof(AdjPxl));
  if(N != NULL){
    N->dp = AllocIntArray(A->n);
    N->n  = A->n;
    for (i=0; i < N->n; i++)
      N->dp[i] = A->dx[i] + img->ncols*A->dy[i];
  }else{
    Error(MSG1,"AdjPixels");  
  }

  return(N);
}

Image *Abs(Image *img)
{
  Image *absimg=NULL;
  int p,n;
  
  n = img->ncols*img->nrows;
  absimg = CreateImage(img->ncols,img->nrows);
  for (p=0; p < n; p++)
    absimg->val[p] = abs(img->val[p]);
  return(absimg);
}

Curve3D *RemSaliencesByAngle(Curve3D *curve,int radius, int angle)
{
  Curve3D *scurve;
  double area;
  int i;

  scurve = CreateCurve3D(curve->n+1);
  for (i=0; i < curve->n; i++){ 
    scurve->X[i] = curve->X[i];
    scurve->Y[i] = curve->Y[i];
    scurve->Z[i] = curve->Z[i];
  }
  
  area = ((double)angle*PI*radius*radius/360.0);
  for (i=0; i < scurve->n; i++){ 
    if (fabs(scurve->Z[i]) <= area)
      scurve->Z[i] = 0.0;      
  }

  return(scurve);
}

Image *LabelBinComp(Image *bin, AdjRel *A)
{
  Image *label=NULL,*flabel=NULL,*fbin=NULL;
  int i,j,n,sz,p,q,l=1;
  AdjPxl *N=NULL;
  int *FIFO=NULL;
  int first=0,last=0;
  
  sz  = FrameSize(A);
  fbin = AddFrame(bin,sz,INT_MIN);
  flabel = CreateImage(fbin->ncols,fbin->nrows);
  N  = AdjPixels(fbin,A);
  n  = fbin->ncols*fbin->nrows;
  FIFO  = AllocIntArray(n);
  for (j=0; j < n; j++){
    if ((fbin->val[j]==1)&&(flabel->val[j]==0)){
      flabel->val[j]=l;
      FIFO[last]=j;      
      last++;      
      while(first != last){
	p = FIFO[first];
	first++;
	for (i=1; i < N->n; i++){
	  q = p + N->dp[i];
	  if ((fbin->val[q]==1)&&(flabel->val[q] == 0)){
	    flabel->val[q] = flabel->val[p];
	    FIFO[last] = q;
	    last++;
	  }
	}
      }
      l++;
      first=last=0;
    }
  }
  
  label = RemFrame(flabel,sz);
  DestroyAdjPxl(&N);
  DestroyImage(&fbin);
  DestroyImage(&flabel);
  free(FIFO);

  return(label);
}

Curve3D *SkelSaliences(Image *skel, int maxdist, int angle) 
{  
  Image *cont=NULL;
  AdjRel *A=NULL;
  AnnImg *aimg=NULL;
  Curve3D *saliences=NULL;
  Curve3D *auxsalie=NULL;

  /* Compute Euclidean IFT */
  A         = Circular(0.0);
  cont      = LabelBinComp(skel, A);
  aimg      = Annotate(skel,NULL,cont);
  DestroyAdjRel(&A); 

  A         = Circular(1.5);
  iftDilation(aimg,A);
  auxsalie  = CompSaliences(aimg, maxdist*maxdist);
  saliences = RemSaliencesByAngle(auxsalie,maxdist,angle);
  DestroyImage(&cont);
  DestroyAdjRel(&A);
  DeAnnotate(&aimg);
  DestroyCurve3D(&auxsalie);
  return(saliences);
}

Image *Skeleton(Image *msskel, float perc)
{
  Image *skel = NULL;
  int p ,n, thres;
  
  skel  = Abs(msskel);
  thres = (int)((MaximumValue(skel)*perc)/100.0);      
  n = skel->ncols*skel->nrows;
  for (p=0; p < n; p++)
    if (skel->val[p] >= thres) 
      skel->val[p]=1;
    else
      skel->val[p]=0;
  
  return(skel);
}

Image *CompMSSkel(AnnImg *aimg)
{
  int i,p,q,n,maxd1,maxd2,d1,d2,MaxD;
  Pixel u,v;
  int sign=1,s2;
  Image *msskel,*cont=NULL,*perim=NULL,*seed=NULL;
  AdjRel *A;

  /* Compute MS Skeletons */

  cont   = LabelContour(aimg->img);
  perim  = Perimeter(aimg->img);
  seed   = CompPaths(aimg->pred);
  A      = Circular(1.0);
  n      = aimg->label->ncols*aimg->label->nrows;
  msskel = CreateImage(aimg->label->ncols,aimg->label->nrows);

  MaxD = INT_MIN;
  for (p=0; p < n; p++) {
    if (aimg->pred->val[p] != p) {  /* It eliminates the countors and
                                       already takes into account the
                                       side option */
      u.x = p%aimg->label->ncols;
      u.y = p/aimg->label->ncols;
      maxd1 = maxd2 = INT_MIN;
      for (i=1; i < A->n; i++){
	v.x = u.x + A->dx[i];
	v.y = u.y + A->dy[i];
	if (ValidPixel(aimg->label,v.x,v.y)){
	  q = v.x + aimg->label->tbrow[v.y];
	  if (cont->val[seed->val[p]] == cont->val[seed->val[q]]){ 
	    d2   = aimg->label->val[q]-aimg->label->val[p];
	    s2   = 1;
	    //	    if (d2 > (perim->val[seed->val[p]]-1-d2)){
	    if (d2 > (perim->val[seed->val[p]]-d2)){
	      s2 = -1;
	      //	      d2 = (perim->val[seed->val[p]]-1-d2);
	      d2 = (perim->val[seed->val[p]]-d2);
	    } 
	    if (d2 > maxd2){
	      maxd2 = d2;
	      sign  = s2;
	    }
	  } else {
	    d1 = cont->val[seed->val[q]] - cont->val[seed->val[p]];
	    if (d1 > maxd1) 
	      maxd1 = d1;
	  }
	}
      }
      if (maxd1 > 0) {
	msskel->val[p] = INT_MAX;
      } else {
	msskel->val[p] = sign*maxd2;
	if (msskel->val[p] > MaxD)
	  MaxD = msskel->val[p];    
      }
    }
  }

  for (p=0; p < n; p++) { /* Set up SKIZ */
    if (msskel->val[p] == INT_MAX)
      msskel->val[p] = MaxD + 1;
  }

  DestroyImage(&cont);
  DestroyImage(&perim);
  DestroyImage(&seed);
  DestroyAdjRel(&A);

  return(msskel);
}

void iftDilation(AnnImg *aimg, AdjRel *A)
{
  Image *Dx=NULL,*Dy=NULL;
  Queue *Q=NULL;
  int i,p,q,n,sz;
  Pixel u,v;
  int *sq=NULL,tmp=INT_MAX,dx,dy;
  char *color=NULL;

  if (aimg->seed == NULL)
    return;

  n  = MAX(aimg->img->ncols,aimg->img->nrows);
  sq = AllocIntArray(n);
  for (i=0; i < n; i++) 
    sq[i]=i*i;

  Dx = CreateImage(aimg->img->ncols,aimg->img->nrows);
  Dy = CreateImage(aimg->img->ncols,aimg->img->nrows);
  n  = aimg->img->ncols*aimg->img->nrows;
  color = AllocCharArray(n);
  sz = FrameSize(A);
  Q  = CreateQueue(2*sz*(sz+aimg->img->ncols+aimg->img->nrows),n);
  
  while (aimg->seed != NULL){
    p=RemoveSet(&(aimg->seed));
    InsertQueue(Q,aimg->cost->val[p]%Q->C.nbuckets,p);
    color[p]=GRAY;
  }

  while(!EmptyQueue(Q)) {
    p=RemoveQueue(Q);
    color[p]=BLACK;
    u.x = p%aimg->img->ncols;
    u.y = p/aimg->img->ncols;
    for (i=1; i < A->n; i++){
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      if (ValidPixel(aimg->img,v.x,v.y)){
	q = v.x + aimg->img->tbrow[v.y];
	if (color[q] != BLACK){
	  dx  = Dx->val[p] + abs(v.x-u.x);
	  dy  = Dy->val[p] + abs(v.y-u.y);
	  tmp = sq[dx] + sq[dy];
	  if (tmp < aimg->cost->val[q])
	    {
	      if (color[q] == WHITE){
		InsertQueue(Q,tmp%Q->C.nbuckets,q);
		color[q]=GRAY;
	      }else
		UpdateQueue(Q,q,aimg->cost->val[q]%Q->C.nbuckets,tmp%Q->C.nbuckets);
	      aimg->cost->val[q]  = tmp;
	      aimg->pred->val[q]  = p;
	      aimg->label->val[q] = aimg->label->val[p];
	      Dx->val[q] = dx;
	      Dy->val[q] = dy;
	    }
	}
      }    
    }
  }
  free(color);
  free(sq);
  DestroyQueue(&Q);
  DestroyImage(&Dx);
  DestroyImage(&Dy);
}

Curve *CopyCurve(Curve *curve)
{
  Curve *curvec;

  curvec = CreateCurve(curve->n);
  memcpy(curvec->X,curve->X,curve->n*sizeof(double));
  memcpy(curvec->Y,curve->Y,curve->n*sizeof(double));
  return(curvec);
}

Curve *Histogram(Image *img)
{
  int i,p,n,nbins;
  Curve *hist=NULL;

  nbins = MaximumValue(img)+1;
  hist  = CreateCurve(nbins);
  n     = img->ncols*img->nrows;
  for (p=0; p < n; p++)
    hist->Y[img->val[p]]++;
  for (i=0; i < nbins; i++) 
    hist->X[i] = i;

  return(hist);
}

void SortCurve(Curve *curve, int left, int right, char order)
{
  int pivot;
 
  if (left < right) {
    pivot = PartCurve(curve,left,right,order);
    SortCurve(curve,left,pivot-1,order);
    SortCurve(curve,pivot+1,right,order); 
  }
}

int PartCurve (Curve *curve, int left, int right, char order)
{
  double y;
  int i;
  double X,Y;
 
  y = curve->Y[left];
  i = left;
  do {
    if (order == INCREASING){
      while ((curve->Y[left] <= y)&&(left <= right)) left++; 
      while (curve->Y[right]  > y) right--;
    } else { /* order = DECREASING */
      while ((curve->Y[left] >= y)&&(left <= right)) left++; 
      while (curve->Y[right]  < y) right--;
    }
    if (left < right){
      X = curve->X[left];
      Y = curve->Y[left];
      curve->X[left]  = curve->X[right];
      curve->Y[left]  = curve->Y[right];
      curve->X[right] = X;
      curve->Y[right] = Y;
      left++; right--;
    }
  } while (left <= right);

  left = i;
  if (left != right){
    X = curve->X[left];
    Y = curve->Y[left];
    curve->X[left]  = curve->X[right];
    curve->Y[left]  = curve->Y[right];
    curve->X[right] = X;
    curve->Y[right] = Y;
  }

  return (right);
}

void InvertXY(Curve *curve)
{
  double tmp;
  int i;
  for (i=0; i<curve->n; i++){
    tmp = curve->X[i];
    curve->X[i] = curve->Y[i];
    curve->Y[i] = tmp;
  }
}

/* Descriptor functions ***************************/

/* BIC */
int *Quantize_colors(CImage *img, int color_dim){
  unsigned long i;
  unsigned long r, g, b;
  unsigned long fator_g, fator_b;
  int *color, n;
  
  n = img->C[0]->nrows * img->C[0]->ncols;  

  color = (int *) calloc(n, sizeof(int));
  if(color==NULL){
    printf("\nOut of memory \n");
    exit(-1);
  }
  
  fator_g = color_dim;
  fator_b = fator_g*color_dim;
  
  for(i=0; i<n; i++){
    r = color_dim*img->C[0]->val[i]/256;
    g = color_dim*img->C[1]->val[i]/256;
    b = color_dim*img->C[2]->val[i]/256;
    
    color[i] = (r + fator_g*g + fator_b*b);
  }
  return color;
}

unsigned char Compute_log(float value){
  unsigned char result;
  
  value = 255. * value;
  if(value==0.)       result=0;
  else if(value<1.)   result=1;
  else if(value<2.)   result=2;
  else if(value<4.)   result=3;
  else if(value<8.)   result=4;
  else if(value<16.)  result=5;
  else if(value<32.)  result=6;
  else if(value<64.)  result=7;
  else if(value<128.) result=8;
  else                result=9;
  
  return result;
}

void Compress_histogram(unsigned char *ch, unsigned long *h, 
			unsigned long max, int size){
  int i;
  unsigned char v;
  
  for(i=0; i<size; i++){
    v = Compute_log((float) h[i] / (float) max);
    ch[i] = (unsigned char)(48 + v);
  }
}

void Compute_frequency_property(Image *img, Property *ppt){
  
  unsigned long x, y, p, q;
  int i, border;
  AdjRel *A;
  Pixel v;
  
  A = Circular(1.0);
  
  for(y=0L; y<img->nrows; y++){
    for(x=0L; x<img->ncols; x++){
      p = x + img->tbrow[y];
      border=FALSE;
      for (i=1; i < A->n; i++){
	v.x = x + A->dx[i];
	v.y = y + A->dy[i];
	if (ValidPixel(img,v.x,v.y)){
	  q = v.x + img->tbrow[v.y];
	  if(ppt[p].color!=ppt[q].color){ 
	    border=TRUE;
	    break;
	  }
	}
      }
      if(border==FALSE) 
	ppt[p].frequency=LOW;
      else ppt[p].frequency=HIGH;
    }
  }
  DestroyAdjRel(&A);
}

Property *Compute_pixels_properties(CImage *img)
{
  Property *p;
  int *color, i, n;
  
  n = img->C[0]->nrows * img->C[0]->ncols;  
  
  p = (Property *) calloc(n, sizeof(Property));
  if(p==NULL){
    printf("\nOut of memory \n");
    exit(-1);
  }
  
  color = Quantize_colors(img, 4);
  for(i=0; i<n; i++) 
    p[i].color=color[i];
  Compute_frequency_property(img->C[0], p);
  
  free(color);
  return p;
}

VisualFeature *Compute_histograms(Property *p, int n){
  VisualFeature *vf = (VisualFeature *) calloc(1,sizeof(VisualFeature));
  unsigned long i;
  
  for(i=0; i<SIZE; i++){
    vf->colorH[i] = 0;
    vf->lowH[i] = 0;
    vf->highH[i] = 0;
  }
  
  for(i=0; i<n; i++){
    vf->colorH[p[i].color]++;
    
    if(p[i].frequency==LOW) 
      vf->lowH[p[i].color]++;
    else 
      vf->highH[p[i].color]++;
  }
  return vf;
}

CompressedVisualFeature *Compress_histograms(VisualFeature *vf, int npixels)
{
  CompressedVisualFeature *cvf =  (CompressedVisualFeature *) calloc(1,sizeof(CompressedVisualFeature));
  
  Compress_histogram(cvf->colorH, vf->colorH, npixels, SIZE);
  Compress_histogram(cvf->lowH, vf->lowH, npixels, SIZE);
  Compress_histogram(cvf->highH, vf->highH, npixels, SIZE);
  
  return cvf;
}

void Write_visual_features(char *filename,char *dbname, CompressedVisualFeature *cvf)
{
  FILE *file;
  int i;
  
  if((file = fopen(dbname, "a+t")) == NULL)
    {
      fprintf(stderr, "Can't open %s \n", dbname);
      exit(-1);
    }
  
  fprintf(file, "%s\t", filename);
  for(i=0;i<SIZE;i++)
    {
      fprintf(file, "%c%c", cvf->lowH[i], cvf->highH[i]);
       }
  fprintf(file, "\n");
  
  fclose(file);
}

CompressedVisualFeature *Extract_visual_features(CImage *img)
{
  Property *p;
  VisualFeature *vf;
  CompressedVisualFeature *cvf;
  int npixels;
  
  npixels = img->C[0]->nrows * img->C[0]->ncols;
  
  p = Compute_pixels_properties(img);
  vf = Compute_histograms(p, npixels);
  cvf = Compress_histograms(vf, npixels);
  
  free(p);
  return cvf;
}

Curve *BIC(CImage *img)
{
  Property *p;
  VisualFeature *vf;
  CompressedVisualFeature *cvf;
  int i, npixels;
  Curve *curve = CreateCurve(2*SIZE);
  
  npixels = img->C[0]->nrows * img->C[0]->ncols;
  
  p = Compute_pixels_properties(img);
  vf = Compute_histograms(p, npixels);
  cvf = Compress_histograms(vf, npixels);
  
  for (i=0; i<SIZE; i++){
    curve->X[i] = i;
    curve->Y[i] = cvf->lowH[i];
    curve->X[i+SIZE] = i+SIZE;
    curve->Y[i+SIZE] = cvf->highH[i];
  }

  free(p);
  free(vf);
  free(cvf);
  return curve;
}


double gray_level_BIC(Image *img1, Image *img2){
  Property *p1, *p2;
  VisualFeature *vf1, *vf2;
  CompressedVisualFeature *cvf1, *cvf2;
  int i, n1, n2;
  double dist;

  n1 = img1->nrows * img1->ncols;  
  n2 = img2->nrows * img2->ncols;  

  p1 = (Property *) calloc(n1, sizeof(Property));
  p2 = (Property *) calloc(n2, sizeof(Property));

  if((p1==NULL)||(p2==NULL)){
    printf("\nOut of memory \n");
    exit(-1);
  }
  
  for(i=0; i<n1; i++){
    p1[i].color=img1->val[i];
  }
  for(i=0; i<n2; i++){
    p2[i].color=img2->val[i];
  }
  
  Compute_frequency_property(img1,p1);
  Compute_frequency_property(img2,p2);
  
  vf1 = Compute_histograms(p1, n1);
  vf2 = Compute_histograms(p2, n2);
  
  cvf1 = Compress_histograms(vf1, n1);
  cvf2 = Compress_histograms(vf2, n2);
  
  dist = 0.0;
  for (i=0; i<SIZE; i++){
    dist += fabs(cvf1->lowH[i] - cvf2->lowH[i]);
    dist += fabs(cvf1->highH[i] - cvf2->highH[i]);
  }
  
  free(p1);
  free(p2);
  free(vf1);
  free(vf2);
  free(cvf1);
  free(cvf2);
  return dist;
}

FeatureVector1D *BIC_ExtractionAlgorithm(CImage *in){
 Curve *curve = NULL;
 FeatureVector1D *fv = NULL;
 curve = BIC(in);
 fv = CurveTo1DFeatureVector(curve);
 
 DestroyCurve(&curve);
 return fv;
}

double BIC_DistanceAlgorithm(FeatureVector1D *fv1, FeatureVector1D *fv2)
{
	return L1_Distance(fv1, fv2);
}

/* Fourier Descriptor */
double Cabs(double x, double y) {
  return (sqrt( (x*x) + (y*y)));
}

Curve *Image2Curve(Image *img) { /* img = binary image */
  Curve3D *curve; //ponto (x,y) e z = ordem no contorno
  Curve *curve2;
  Image *contour = NULL;
  int i;
  int npixels;
  int count = 0;
  int j = 0 ;
  
  contour = LabelContPixel(img);
  npixels = contour->ncols * contour->nrows;
  
  for (i = 0; i < npixels; i++){
    if (contour->val[i]!=0)
      count++;
  }
  
  curve = CreateCurve3D(count+1);
  
  for (i = 0; i < npixels; i++){
    if (contour->val[i]!=0){
      curve->X[j] = i % contour->ncols;
      curve->Y[j] = i / contour->ncols;
      curve->Z[j] = contour->val[i];
      j++;
    }
  }  
  
  SortCurve3D(curve, 0, (curve->n - 2), INCREASING);
  
  curve2 = CreateCurve(curve->n);
  for (i=0;i<curve->n;i++){
    curve2->X[i]=curve->X[i];
    curve2->Y[i]=curve->Y[i];
  }
  
  DestroyCurve3D(&curve);
  DestroyImage(&contour);
  return (curve2);
}

Curve *FourierDescriptor(Image *img) {
  Curve *c = NULL; 
  Curve *curve = NULL;
  Curve *mag = NULL;
  int i;
  int z; /* lixo */
  int tam;
  int nn = 0;
  double normfactor = 0;

  curve = Image2Curve(img);
   
  tam = curve->n;
  i = 1;
  nn = tam;
  while(nn != 1) {
    nn >>= 1;
    ++i;
  }
  
  for(; i; i--)
    nn <<= 1;
 
  if (nn < 128)
    nn = 128;
  
   c = CreateCurve(nn);
  
  for (i = 0; i < tam ; i++)
    {
      c->X[i] = (double) curve->X[i];	
      c->Y[i] = (double) curve->Y[i];
    }
  
  for (i = tam ; i < nn ; i++)
    {
      c->X[i] = 0;
      c->Y[i] = 0;
    }
  
  z = FFT(1, nn,c->X ,c->Y);

  mag = CreateCurve(126);
  normfactor = Cabs(c->X[0],c->Y[0]);
  
  for (i = 1; i <= 126; i++)
    {
      mag->X[i-1] = i - 1;
      mag->Y[i-1] = (double) (Cabs(c->X[i],c->Y[i])/normfactor);
    }
  
  DestroyCurve(&c);  
  DestroyCurve(&curve);  
  return (mag);
}

FeatureVector1D *FourierDescriptor_ExtractionAlgorithm(Image *in){
  Curve *curve = NULL;
  FeatureVector1D *fv = NULL;
  
  curve = FourierDescriptor(in);
  fv = CurveTo1DFeatureVector(curve);
  
  DestroyCurve(&curve);
  return fv;
}

double Fourier_DistanceAlgorithm(FeatureVector1D *fv1, FeatureVector1D *fv2){
	return EuclideanDistance(fv1, fv2);
}

/* Moments Invariant */
double  MomentPQ(int p, int q, Image *img, int max){
  int i, x, y;
  double sum = 0.0;
  
  for (i = 0; i < img->ncols * img->nrows; i++)
    {
      if (img->val[i] != 0 )
	{
	  x = 1 + i%img->ncols; /* don't do 0^0 */
	  y = 1 + i/img->ncols;      
	  sum = sum + (pow(x, p) * pow(y,q) * ((double) img->val[i]/max));
	}
    }
  
  return (sum);
}

Curve *MomentInv(Image *img) {
  Curve * curve;
  int i;
  int max;
  
  /* mPQ momentes of order (P+Q) */
  double  m00, m01, m10, m11, m20, m02, m12, m21, m03, m30;
  
  /* center moments */
  double xc, yc;
  
  /* normalized central moments (Eta) */
  double n00, n10, n01, n02, n20, n11, n12, n21, n03, n30; 
  
  /* invariant moments (Phi) */
  double f1, f2, f3, f4, f5, f6, f7;
  
  float g; /* gamma = (p+q)/2 + 1  */
  
  /* n = Upq/U00 */
 
  max = 0;
  
  for (i = 0; i < img->ncols *  img->nrows ; i++)
    {
      if (img->val[i] > max)
	max = img->val[i];
    }
  
  m00 = MomentPQ(0,0, img, max);
  m01 = MomentPQ(0,1, img, max);
  m10 = MomentPQ(1,0, img, max);
  m11 = MomentPQ(1,1, img, max); 
  m12 = MomentPQ(1,2, img, max); 
  m21 = MomentPQ(2,1, img, max); 
  m02 = MomentPQ(0,2, img, max);
  m20 = MomentPQ(2,0, img, max); 
  m03 = MomentPQ(0,3, img, max);
  m30 = MomentPQ(3,0, img, max);  
    
  xc = (double) m10 / m00;
  yc = (double) m01 / m00;
  
  n00 = 1.0; 
  
  n10 = 0.0;
  
  n01 = 0.0;
  
  g = 2.0; 
  
  n20 = (double) (m20 - (xc * m10)) / (pow(m00, g));
  
  n02 = (double) (m02 - (yc * m01)) / (pow(m00, g));
  
  n11 = (double) (m11 - (yc * m10)) / (pow(m00, g));
  
  g = 2.5;
  
  n30 = (double) (m30 - (3 * xc * m20) + (2 * (pow(xc,2)) * m10) ) / (pow(m00, g));   
  
  n12 = (double) (m12 - (2 * yc * m11) - (xc * m02) + (2 * pow(yc,2) * m10) ) / (pow(m00, g)) ;
  
  n21 = (double) (m21 - (2 * xc * m11) - (yc * m20) + (2 * pow(xc,2) * m01) ) / (pow(m00, g));
  
  n03 = (double) (m03 - (3 * yc * m02) + (2 * (pow(yc,2)) * m01)) / (pow(m00, g)); 
  
  f1 = (double) n20 + n02;
  
  f2 = (double) (pow((n20 - n02),2)) + (4 * (pow(n11,2)));
  
  f3 = (double) (pow((n30 - (3 * n12)),2)) + (pow( ( (3 * n21) - n03),2));
  
  f4 = (double) (pow((n30 + n12),2)) + (pow((n21 + n03),2));
  
  f5 = (double) ((n30 - (3 * n12)) * (n30 + n12) * ((pow((n30 + n12),2)) - (3 * (pow((n21 + n03),2))))) + 
    (((3 * n21) - n03) * (n21 + n03) * ((3 * (pow((n30 + n12),2))) - (pow((n21 + n03),2))));
  
  f6 = (double) ((n20 + n02) * ((pow((n30 + n12),2)) - (pow((n21 + n03),2)))) +
    ((4 * n11) * (n30 + n12) * (n21 + n03)); 
  
  f7 = (double) (((3 * n21) - n03) * (n30 + n12) * ((pow((n30 + n12),2)) - (3 * (pow((n21 + n03),2))))) +
    (((3 * n12) - n30) * (n21 + n03) * ((3 * (pow((n30 + n12),2))) - (pow((n21 + n03),2))));
  
  curve = CreateCurve(7);
  
  curve->X[0] = 0.0;
  curve->Y[0] = f1;
  
  curve->X[1] = 1.0;
  curve->Y[1] = f2;
  
  curve->X[2] = 2.0; 
  curve->Y[2] = f3;
  
  curve->X[3] = 3.0; 
  curve->Y[3] = f4;
  
  curve->X[4] = 4.0; 
  curve->Y[4] = f5;
  
  curve->X[5] = 5.0; 
  curve->Y[5] = f6;
  
  curve->X[6] = 6.0; 
  curve->Y[6] = f7;

  return (curve);
}

Curve *MomentInvariant(Image *img) { // contorno e objeto inteiro
  Image *contour = NULL;
  Curve *c1 = NULL;
  Curve *c2 = NULL;
  Curve *curve = NULL;
  int i;
  
  contour = LabelContour(img);
  c1 = MomentInv(contour);
  c2 = MomentInv(img);
  
  curve = CreateCurve(c1->n + c2->n);
  for (i=0; i<c1->n; i++){
    curve->X[i] = i;
    curve->Y[i] = c1->Y[i];
  }
  for (i=0; i<c2->n; i++){
    curve->X[i+c1->n] = i + c1->n;
    curve->Y[i+c1->n] = c2->Y[i];
  }
  
  DestroyCurve(&c1);
  DestroyCurve(&c2);
  DestroyImage(&contour);
    
  return (curve);    
}

FeatureVector1D *MomentInvariant_ExtractionAlgorithm(Image *in){
  Curve *curve = NULL;
  FeatureVector1D *fv = NULL;
  
  curve = MomentInvariant(in);
  fv = CurveTo1DFeatureVector(curve);

  DestroyCurve(&curve);
  return fv;
}

double MomentInvariant_DistanceAlgorithm(FeatureVector1D *fv1, FeatureVector1D *fv2){
	return 1-EuclideanDistance(fv1, fv2);
}

/* BAS */
/*resample curve*/
representation_type *resample(representation_type *curve, int nsamples)
{
  representation_type *rcurve;
  Image *img1, *img2;
  int x;
  
  img1= CreateImage(curve->length, 3);
  
  for (x=0; x<curve->length; x++){
    img1->val[x] = curve->mean[x];
    img1->val[x+img1->tbrow[1]] = curve->second[x];
    img1->val[x+img1->tbrow[2]] = curve->third[x];
  }
  
  img2 = Scale(img1, (((float) nsamples)/curve->length), 1);
  
  rcurve = (representation_type *) calloc(1, sizeof(representation_type));
  rcurve->length = nsamples;
  rcurve->mean = (int *) calloc(nsamples, sizeof(int));
  rcurve->second =(int *) calloc(nsamples, sizeof(int));
  rcurve->third = (int *) calloc(nsamples, sizeof(int));
  for (x=0; x<nsamples; x++){
    rcurve->mean[x] = img2->val[x];
    rcurve->second[x] = img2->val[x+img2->tbrow[1]];
    rcurve->third[x] = img2->val[x+img2->tbrow[2]];
  }
  
  DestroyImage(&img1);
  DestroyImage(&img2);
  return(rcurve);
}

double find_angle(deltax,deltay)
     int deltax;
     int deltay;
{
  double angle;
  double pi;
  
  pi=22.0/7.0; 
  
  if((deltax==0) && (deltay==0))
    angle=0.0;
  else{
    angle=atan((10.0*abs(deltax))/(10.0*abs(deltay)));
    angle=angle*180.0/pi;  
    if((deltax <= 0) && (deltay >= 0)) 
      angle=360.0-angle;
    else if((deltax <= 0) && (deltay <=0)) 
      angle=180.0 + angle;  
    else if((deltax >= 0) && (deltay <=0)) 
      angle=180.0 - angle;
  }
  
  return(angle);
}

representation_type *extract_feature(boun)
     boundary_type *boun;
{
  representation_type *curve_feature; 
  int i,j,x1,x2,x3,y1,y2,y3,curvelength;
  double angle_1,angle_2,curve,total,previous;
  int delta_x, delta_y,mean,second,third;
  int *bearing_array;
  
  curve_feature = (representation_type *) calloc(1, sizeof(representation_type));
  curve_feature->length=boun->length;
  curve_feature->mean= (int *) calloc(boun->length, sizeof(int));
  curve_feature->second=(int *) calloc(boun->length, sizeof(int));
  curve_feature->third=(int *) calloc(boun->length, sizeof(int));
  
  
  curvelength=(int)(boun->length/2);
  bearing_array=(int *) calloc((curvelength-1), sizeof(int));
  for(i=0; i<boun->length; i++){
    total=0.0;
    x1=boun->X[((i-1)+boun->length)%boun->length];
    y1=boun->Y[((i-1)+boun->length)%boun->length];
    x2=boun->X[i];
    y2=boun->Y[i];
    x3=boun->X[((i+1)+boun->length)%boun->length];
    y3=boun->Y[((i+1)+boun->length)%boun->length];
  
    delta_x=x1-x2;
    delta_y=-(y1-y2);
    angle_1=find_angle(delta_x,delta_y);
    delta_x=x3-x2;
    delta_y=-(y3-y2);
    angle_2=find_angle(delta_x,delta_y);
    if(angle_1 >= angle_2)
      curve=angle_1-angle_2;
    else
      curve=360.0 + angle_1-angle_2;
  
    total+=curve;
    bearing_array[0]=(int)curve;
    previous=curve;
    for(j=2; j<curvelength; j++){
      x1=boun->X[((i-j)+boun->length)%boun->length];
      y1=boun->Y[((i-j)+boun->length)%boun->length];
      x2=boun->X[i];
      y2=boun->Y[i];
      x3=boun->X[((i+j)+boun->length)%boun->length];
      y3=boun->Y[((i+j)+boun->length)%boun->length];
      delta_x=x1-x2;
      delta_y=-(y1-y2);
      angle_1=find_angle(delta_x,delta_y);
      delta_x=x3-x2;
      delta_y=-(y3-y2);
      angle_2=find_angle(delta_x,delta_y);
      if(angle_1 >= angle_2)
	curve=angle_1-angle_2;
      else
	curve=360.0 + angle_1-angle_2;
      
      if(j > 3){
	if(((curve-previous) > 180))
	  curve=curve-360.0;
	else
	  if(((previous-curve) > 180))
	    curve=curve+360.0;
      }
      
      bearing_array[j-1]=(int)curve; 
      total+=curve;
      previous=curve;
    }
    
    mean=(int)(total/(double)(curvelength-1));
    total=0.0;
    for(j=0;j<curvelength-1; j++)
      total+=pow((bearing_array[j]-mean),2.0);
    second=pow(total/(double)(curvelength-2),0.5);
    total=0.0;
    for(j=0;j<curvelength-1; j++)
      total+=pow(abs(bearing_array[j]-mean),3.0);
    third=pow(total/(double)(curvelength-2),(1.0/3.0)); 
  
    curve_feature->mean[i]=mean;
    curve_feature->second[i]=second;
    curve_feature->third[i]=third;
  }    
  free(bearing_array);
  return(curve_feature);
}

Curve *BAS(Image *in,int rsp,int nsamples){
  Curve *featurevector = NULL;
  Curve *contour = NULL;
  Curve *moment1 = NULL;
  Curve *moment2 = NULL;
  Curve *moment3 = NULL;
  
  boundary_type *bound;
  representation_type *curve, *rcurve;
  int i;

  contour = Image2Curve(in);
  bound = (boundary_type *) calloc(1, sizeof(boundary_type));
  bound->length = contour->n;
  bound->X = (int *)calloc(bound->length, sizeof(int));
  bound->Y = (int *)calloc(bound->length, sizeof(int));
  for (i=0; i<bound->length; i++){
    bound->X[i] = (int)contour->X[i];
    bound->Y[i] = (int)contour->Y[i];
  }
  
  curve = extract_feature(bound);

  if(rsp == 0)
  {
  	rcurve = resample(curve, curve->length);
	nsamples = curve->length;
  }
  else
	rcurve = resample(curve, nsamples);

  moment1 = CreateCurve(nsamples);
  moment2 = CreateCurve(nsamples);
  moment3 = CreateCurve(nsamples);
  
  featurevector = CreateCurve(3*nsamples);
  for (i=0; i<3*nsamples; i++){
    featurevector->X[i] = (double)i;
  }
  
  for (i=0; i<nsamples; i++){
    moment1->X[i]= moment2->X[i]= moment3->X[i]=(double)i;
    featurevector->Y[i] = moment1->Y[i]= (double) rcurve->mean[i];
    featurevector->Y[nsamples+i] = moment2->Y[i]= (double) rcurve->second[i];
    featurevector->Y[2*nsamples+i] = moment3->Y[i]= (double) rcurve->third[i];
  }

  free(bound->X);
  free(bound->Y);
  free(bound);
  free(curve->mean);
  free(curve->second);
  free(curve->third);
  free(curve);
  free(rcurve->mean);
  free(rcurve->second);
  free(rcurve->third);
  free(rcurve);
  DestroyCurve(&contour);
  DestroyCurve(&moment1);
  DestroyCurve(&moment2);
  DestroyCurve(&moment3);

  return featurevector;
}

FeatureVector1D *BAS_ExtractionAlgorithm(Image *in,int rsp,int nsamples){
 Curve *curve = NULL;
 FeatureVector1D *fv = NULL;
 
 curve = BAS(in,rsp,nsamples);
 fv = CurveTo1DFeatureVector(curve);
 
 DestroyCurve(&curve);
 return fv;
}


long min(Dist1,Dist2,Dist3)
     long Dist1;
     long Dist2;
     long Dist3;
{
  if((Dist1<=Dist2) && (Dist1<=Dist3)) 
    return(Dist1);
  else if((Dist2<=Dist1) && (Dist2<=Dist3)) 
    return(Dist2);
  /*else if((Dist3<=Dist1) && (Dist3<=Dist2)) */
  return(Dist3);
}


long Cum_Dist_Optimal(fv1,fv2,dim1,dim2,DISTANCE)
     representation_type fv1;
     representation_type fv2;
     int dim1;
     int dim2;
     long *DISTANCE;
{
  long temp_dist;
  int i,j;
  int penalty;
  
  penalty=300;
  /* OPTIMAL CORRESPONDENCE OF STRINGS
   */
  DISTANCE[0*(dim2+1)+0]=0;
  for(j=1;j<=dim2;j++)
    DISTANCE[0*(dim2+1)+j]=j * penalty;
  
  for(i=1;i<=dim1;i++)
    DISTANCE[i*(dim2+1)+0]=i * penalty;
  
  for(i=1;i<=dim1;i++)
    for(j=1;j<=dim2;j++)
      if(abs(i-j) < (5)){
	temp_dist=abs(fv1.mean[i-1]-fv2.mean[j-1]) +
	  abs(fv1.second[i-1]-fv2.second[j-1]) +
	  abs(fv1.third[i-1]-fv2.third[j-1]);
	
	DISTANCE[i*(dim2+1)+j]= temp_dist +
	  min(DISTANCE[(i-1)*(dim2+1)+(j-1)],
	      DISTANCE[(i-1)*(dim2+1)+(j)] + penalty,
	      DISTANCE[(i)*(dim2+1)+(j-1)] + penalty); 
      }
  return(DISTANCE[(dim1)*(dim2+1)+(dim2)]/dim2);
}

long find_distance(representation_type fv1, representation_type fv2,
		   int dim1, int dim2)
{
  long distance,k,i,j,temp_dist;
  representation_type temp_list1, temp_list2;
  long *DISTANCE;
  
	DISTANCE=(long *) calloc((dim1+1)*(dim2+1),sizeof(long));
  
  temp_list1.mean=(int *) calloc(dim2,sizeof(int));
  temp_list1.second=(int *) calloc(dim2,sizeof(int));
  temp_list1.third=(int *) calloc(dim2,sizeof(int));
  temp_list2.mean=(int *) calloc(dim2,sizeof(int));
  temp_list2.second=(int *) calloc(dim2,sizeof(int));
  temp_list2.third=(int *) calloc(dim2,sizeof(int));
  
  temp_dist=10000000;
  
  for(i=0;i<dim1+1;i++)
    for(j=0;j<dim2+1;j++)
      DISTANCE[i*(dim2+1)+j]=10000000;
  
  for(k=0; k<dim2; k++){
    for(i=0;i<dim2;i++){
      temp_list1.mean[i]=fv2.mean[(i+k)%dim2];
      temp_list1.second[i]=fv2.second[(i+k)%dim2];
      temp_list1.third[i]=fv2.third[(i+k)%dim2]; 
    }   
    distance=Cum_Dist_Optimal(fv1,temp_list1,dim1,dim2,DISTANCE);
    if(temp_dist>distance) temp_dist=distance;
  }
  /***Taking the mirror of fv2 *****/
  
  for(i=0;i<dim2;i++){
    temp_list2.mean[i]=fv2.mean[(dim2-1)-i];
    temp_list2.second[i]=fv2.second[(dim2-1)-i];
    temp_list2.third[i]=fv2.third[(dim2-1)-i]; 
  }
  
  for(k=0; k<dim2; k++){
    for(i=0;i<dim2;i++){
      temp_list1.mean[i]=temp_list2.mean[(i+k)%dim2];
      temp_list1.second[i]=temp_list2.second[(i+k)%dim2];
      temp_list1.third[i]=temp_list2.third[(i+k)%dim2]; 
    }
    distance=Cum_Dist_Optimal(fv1,temp_list1,dim1,dim2,DISTANCE);
    if(temp_dist>distance) temp_dist=distance;
  }
  
  distance=temp_dist;
  
  free(temp_list1.mean);
  free(temp_list1.second);
  free(temp_list1.third);
  free(temp_list2.mean);
  free(temp_list2.second);
  free(temp_list2.third);
  free(DISTANCE);
  return(distance);
}

double BAS_DistanceAlgorithm(FeatureVector1D *c1, FeatureVector1D *c2){
  
  representation_type fv1, fv2;
  int n, m, i;
  long dist;
  
  n = c1->n/3;
  m = c2->n/3;
  fv1.mean=(int *) calloc(n,sizeof(int));
  fv1.second=(int *) calloc(n,sizeof(int));
  fv1.third=(int *) calloc(n,sizeof(int));
  fv2.mean=(int *) calloc(m,sizeof(int));
  fv2.second=(int *) calloc(m,sizeof(int));
  fv2.third=(int *) calloc(m,sizeof(int));  
  
  for (i=0; i<n; i++){
    fv1.mean[i] = (int)c1->X[i];
    fv1.second[i] = (int)c1->X[n + i];
    fv1.third[i] = (int)c1->X[2*n + i];
  }

  for (i=0; i<m; i++){
    fv2.mean[i] = (int)c2->X[i];
    fv2.second[i] = (int)c2->X[m + i];
    fv2.third[i] = (int)c2->X[2*m + i];
  }
  
  dist = find_distance(fv1,fv2, n, m);
  
  free(fv1.mean);
  free(fv1.second);
  free(fv1.third);
  free(fv2.mean);
  free(fv2.second);
  free(fv2.third);
  
  return dist;
}

/* Tensor Scale */
FeatureVector1D *TensorScale_ExtractionAlgorithm(Image *in){
	TensorScale *ts = NULL;
	FeatureVector1D *fv = NULL;
	float *hist = NULL;
	int i;
	
	ts = CreateBinaryTensorScale(in,24);
	hist = TSOrientationHistogram(ts);

	/*convertendo hist para featurevector1D*/
	fv = CreateFeatureVector1D(HISTOGRAMSIZE+1);
	for(i = 0; i <  HISTOGRAMSIZE+1; i++)
		fv->X[i] = hist[i];
	
	free(hist);
	DestroyTensorScale(&ts);

	return fv;
}

TensorScale *CreateBinaryTensorScale(Image *bin, int m_pairs){
  Image *edt,*edt2,*bin2;
  Point *epsilon;
  TensorScale *ts = NULL;
  Vector *tau;
  int i,j,k,p,v,vi,n;
  float x,y,xc,yc,taux, tauy, aux;
  float a2,b2,b1,teta,u1,v1,u2,v2,aa,acc,wt,w;
  float gSxy, gSy2_x2;
  float sin_teta, cos_teta;
  float *lt_sqrt;
  int d,d1,d2,dmax;
  int ncols,nrows;

  ncols = bin->ncols;
  nrows = bin->nrows;
  n = ncols*nrows;

  bin2 = CopyImage(bin);
  p = bin2->tbrow[nrows-1];
  for(i=0; i<ncols; i++){
    bin2->val[i] = 0;
    bin2->val[p+i] = 0;
  }
  for(i=0; i<nrows; i++){
    p = bin2->tbrow[i];
    bin2->val[p] = 0;
    bin2->val[p+ncols-1] = 0;
  }

  //------ Euclidean distance transform ----------------
  edt2 = TSEDistTrans(bin2);

  dmax = MaximumValue(edt2);
  lt_sqrt = (float *)malloc((dmax+1)*sizeof(float));
  for(i=0;i<=dmax;i++)
    lt_sqrt[i] = sqrtf((float)i);

  edt = CreateImage(ncols,nrows);
  for(p=0; p<n; p++){
    d = edt2->val[p];
    d = ROUND(lt_sqrt[d]);
    edt->val[p] = d;
  }

  //---------------------------------------------------

  ts = (TensorScale *)malloc(sizeof(TensorScale));
  ts->orientation = CreateDImage(ncols,nrows);
  ts->anisotropy  = CreateDImage(ncols,nrows);
  ts->thickness   = CreateDImage(ncols,nrows);
  ts->m_pairs     = m_pairs;

  tau = (Vector *)malloc(sizeof(Vector)*m_pairs);
  epsilon = (Point *)malloc(sizeof(Point)*m_pairs);

  teta = 0.0;
  for(i=0;i<m_pairs;i++){
    tau[i].x = cosf(teta);
    tau[i].y = sinf(teta);
    tau[i].z = 0.0;

    teta += ((float)PI/m_pairs); 
  }

  for(i=1;i<nrows-1;i++){
    for(j=1;j<ncols-1;j++){
      p = bin2->tbrow[i]+j;
      if(bin2->val[p] == 0) continue;

      vi = edt->val[p];
      //--------- Sample lines --------------------------------------
      gSxy = gSy2_x2 = 0.0;
      xc = j+0.5; yc = i+0.5;
      for(k=0;k<m_pairs;k++){
	taux = tau[k].x;
	tauy = tau[k].y;
	v = vi;
	d1 = d2 = 0;

	while(1){
	  x = v*taux;
	  y = v*tauy;

	  if(d1==0){
	    d1 = edt->val[(int)(xc+x) + edt->tbrow[(int)(yc+y)]];
	    if(d1 == 0)
	      break;
	  }
	  
	  if(d2==0){
	    d2 = edt->val[(int)(xc-x) + edt->tbrow[(int)(yc-y)]];
	    if(d2 == 0)
	      break;
	  }

	  d = (d1<d2)?d1:d2;
	  d1 -= d;
	  d2 -= d;
	  v += d;
	}

	epsilon[k].x = x;
	epsilon[k].y = -y;

	gSxy -= x*y;            //gSxy += x*(-y);
	gSy2_x2 += (y+x)*(y-x); //(y*y-x*x);
      }

      //-------------------- TETA -----------------------------------
      
      if(gSy2_x2==0.0){ 
	if(gSxy>0.0) teta=PI/2.0;
	else teta=-PI/2.0;
      }
      else{
	teta = atanf((gSxy+gSxy)/gSy2_x2);
	
	if(gSxy<0.0 && teta<0.0) teta+=PI;
	else if(gSxy>0.0 && teta>0.0) teta-=PI;
	else if(teta==0.0 && gSy2_x2>0.0) teta=PI;
      }
      teta /= 2.0;

      //----------------- A & B ---------------------------------
      b2   = (float)edt2->val[p];
      b1   = lt_sqrt[(int)b2];
      
      acc = wt = 0.0;
      sin_teta = sinf(teta);
      cos_teta = cosf(teta);
      for(k=0;k<m_pairs;k++){
	x = epsilon[k].x;
	y = epsilon[k].y;
	
	v1 = y*cos_teta + x*sin_teta;
	u1 = x*cos_teta - y*sin_teta;
	
	v2 = v1*v1;
	u2 = u1*u1;

	if(v2<b2){
	  aa = b2*u2/(b2-v2);
	  if(aa>=b2){
	    w = (v1<0.0)?(b1+v1):(b1-v1);
	    acc += w*aa;
	    wt  += w;
	  }
	}
      }

      if(wt>0.0)
	a2 = acc/wt;
      else
	a2 = b2;

      aux = 1.0-b2/a2;
      if(aux<0.0) aux = 0.0;
      ts->anisotropy->val[p] = sqrtf(aux);
      ts->thickness->val[p]  = b1; //sqrtf(b2);

      if(teta<0.0) teta+=(float)PI;
      if(teta>PI)  teta = PI;
      teta = PI-teta;

      ts->orientation->val[p] = teta;
    }
  }

  free(tau);
  free(epsilon);
  free(lt_sqrt);
  DestroyImage(&edt);
  DestroyImage(&edt2);
  DestroyImage(&bin2);

  return ts;
}

void DestroyTensorScale(TensorScale **ts){
  if(*ts != NULL){
    DestroyDImage(&((*ts)->orientation));
    DestroyDImage(&((*ts)->anisotropy));
    DestroyDImage(&((*ts)->thickness));
    free(*ts);
  }
  *ts = NULL;
}

float *TSOrientationHistogram(TensorScale *ts){
  float *hist;
  float ratio,sum;
  double an,th;
  int w,h,i,j,p,bin;
  
  ratio = (float)HISTOGRAMSIZE/PI;
  hist = (float *)malloc(sizeof(float)*(HISTOGRAMSIZE+1));
  memset(hist, 0, sizeof(float)*(HISTOGRAMSIZE+1));
  w = ts->anisotropy->ncols;
  h = ts->anisotropy->nrows;

  for(i=0; i<h; i++){
    for(j=0; j<w; j++){
      p = ts->anisotropy->tbrow[i]+j;
      an = ts->anisotropy->val[p];
      th = ts->thickness->val[p];

      if(th>0.0){
	bin = ROUND(ts->orientation->val[p]*ratio);  
	hist[bin] += an;
      }
    }
  }
  hist[0] += hist[HISTOGRAMSIZE];
  hist[HISTOGRAMSIZE] = 0.0;

  //Normalizaçao do histograma
  sum = 0.0;
  for(i=0;i<HISTOGRAMSIZE;i++)
    sum += hist[i];
  for(i=0;i<HISTOGRAMSIZE;i++)
    hist[i] /= sum;

  return hist;
}

Image *TSEDistTrans(Image *bin){
  Image *Dx=NULL,*Dy=NULL,*cost;
  Queue *Q=NULL;
  int i,p,q,n;
  Pixel u,v;
  int *sq=NULL,tmp=INT_MAX,dx,dy;
  AdjRel *A;

  n  = MAX(bin->ncols,bin->nrows);
  sq = AllocIntArray(n);
  for (i=0; i < n; i++) 
    sq[i]=i*i;

  A = Circular(1.5);
  cost = CreateImage(bin->ncols, bin->nrows);
  Dx = CreateImage(bin->ncols,bin->nrows);
  Dy = CreateImage(bin->ncols,bin->nrows);  
  n  = bin->ncols*bin->nrows;
  Q = CreateQueue(bin->ncols+bin->nrows,n);

  for(p = 0; p < n; p++){
    if(bin->val[p] > 0)
      cost->val[p] = INT_MAX;	  
    else{
      cost->val[p]=0;    
      InsertQueue(Q,cost->val[p]%Q->C.nbuckets,p);
    }
  }
  
  while(!EmptyQueue(Q)) {
    p   = RemoveQueue(Q);
    u.x = p%bin->ncols;
    u.y = p/bin->ncols;
    for (i=1; i < A->n; i++){
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      if(ValidPixel(bin,v.x,v.y)){
	q = v.x + bin->tbrow[v.y];
	if (cost->val[p] < cost->val[q]){
	  dx  = Dx->val[p] + abs(v.x-u.x);
	  dy  = Dy->val[p] + abs(v.y-u.y);
	  tmp = sq[dx] + sq[dy];
	  if (tmp < cost->val[q]){
	    if (cost->val[q] == INT_MAX)
	      InsertQueue(Q,tmp%Q->C.nbuckets,q);
	    else
	      UpdateQueue(Q,q,cost->val[q]%Q->C.nbuckets,tmp%Q->C.nbuckets);
	    cost->val[q]  = tmp;
	    Dx->val[q] = dx;
	    Dy->val[q] = dy;
	  }
	}
      }
    }
  }

  free(sq);
  DestroyQueue(&Q);
  DestroyAdjRel(&A);
  DestroyImage(&Dx);
  DestroyImage(&Dy);
  
  return(cost);
}

double TensorScale_DistanceAlgorithm(FeatureVector1D *fv1, FeatureVector1D *fv2){
	double result;
	int offset;

	result = (double) TSHistogramMatch(fv1, fv2, &offset);

	return (1-result);
}

float TSHistogramMatch(FeatureVector1D *fv1, FeatureVector1D *fv2, int *offset){
  float *newhist;
  float *newh1,*newh2, *hist1, *hist2;
  int i,j,p;
  float max,correlacao;
  float score;
  float dabs,aux;
  //int maxoffset;
  FILE *file1,*file2,*file3;

  newhist = (float *)malloc(sizeof(float)*2*HISTOGRAMSIZE);
  newh1=newhist;
  newh2=newhist+HISTOGRAMSIZE;

  hist1 = (float *)malloc(sizeof(float)*HISTOGRAMSIZE+1);
  hist2 = (float *)malloc(sizeof(float)*HISTOGRAMSIZE+1);
  
  for(i = 0; i < HISTOGRAMSIZE+1; i++)
  {
	  hist1[i] = fv1->X[i];
	  hist2[i] = fv2->X[i];
  }


  //Ajuste no histograma
  newh1[0] = (2.0*hist1[0]+hist1[HISTOGRAMSIZE-1]+hist1[1])/4.0;
  newh2[0] = (2.0*hist2[0]+hist2[HISTOGRAMSIZE-1]+hist2[1])/4.0;
  for(i=1;i<HISTOGRAMSIZE-1;i++){
    newh1[i] = (2.0*hist1[i]+hist1[i-1]+hist1[i+1])/4.0;
    newh2[i] = (2.0*hist2[i]+hist2[i-1]+hist2[i+1])/4.0;
  }
  newh1[HISTOGRAMSIZE-1] = (2.0*hist1[HISTOGRAMSIZE-1]+hist1[HISTOGRAMSIZE-2]+hist1[0])/4.0;
  newh2[HISTOGRAMSIZE-1] = (2.0*hist2[HISTOGRAMSIZE-1]+hist2[HISTOGRAMSIZE-2]+hist2[0])/4.0;

  //Correlacao
  //maxoffset = ROUND((24.0*HISTOGRAMSIZE)/180.0); // 24 graus.
  *offset=0;
  max=0.0;
  for(i=0;i<HISTOGRAMSIZE;i++){
    correlacao=0.0;
    //if(i==maxoffset) i=HISTOGRAMSIZE-maxoffset; //angulo entre -24 e 24.
    for(p=i,j=0;j<HISTOGRAMSIZE;j++,p++){
      if(p==HISTOGRAMSIZE) p=0;
      correlacao+=(newh1[p]*newh2[j]);
    }

    if(correlacao>max){
      max=correlacao;
      *offset=i;
    }
  }

  file1 = fopen("histogram1d.txt","w");
  file2 = fopen("histogram2.txt","w");
  file3 = fopen("histogram1.txt","w");
  dabs = 0.0;
  for(p=*offset,j=0;j<HISTOGRAMSIZE;j++,p++){
    if(p==HISTOGRAMSIZE) p=0;

    fprintf(file1,"%d %f\n",j,newh1[p]);
    fprintf(file2,"%d %f\n",j,newh2[j]);
    fprintf(file3,"%d %f\n",j,newh1[j]);

    aux=(newh1[p]-newh2[j]);
    aux=(aux<0.0)?(-aux):(aux);
    dabs+=aux;
  }
  score = 1.0 - dabs;

  free(newhist);
  free(hist1);
  free(hist2);
  fclose(file1);
  fclose(file2);
  fclose(file3);

  return score;
}

/* Multiscale Fractal Dimension*/
Curve *PolynomToFractalCurve(Polynom *P, double lower, double higher, int nbins){
  int i;
  Curve *descriptor = NULL;

  descriptor = SamplePolynom(P, lower, higher, nbins);
  for (i=0; i < descriptor->n; i++) 
    descriptor->Y[i] = 2.0 - descriptor->Y[i];
  
  return descriptor;
}

Curve *ContourMSFractal(Image *in)
{
  Image *cont = NULL;
  Curve *descriptor = NULL;
  Polynom *P;
  double lower = 1.0;
  double higher = 5.0;
  int nbins = 100;
  int degree = 10;
  
  cont = LabelContour(in);
  P = MSFractal(cont, 256, degree, lower, higher, 0, 0.0, 6.0);
  descriptor = PolynomToFractalCurve(P, lower, higher, nbins);
  
  DestroyPolynom(&P);
  DestroyImage(&cont);
  return (descriptor);
}

FeatureVector1D *MS_ExtractionAlgorithm(Image *img){
 Curve *curve = NULL;
 FeatureVector1D *fv = NULL;
 
 curve = ContourMSFractal(img);
 fv = CurveTo1DFeatureVector(curve);
 
 DestroyCurve(&curve);
 return fv;
}

double MS_DistanceAlgorithm(FeatureVector1D *fv1, FeatureVector1D *fv2){
	return EuclideanDistance(fv1, fv2);
}

/* Contour Saliences */
void DescInvertXY(FeatureVector2D *desc)
{
  double tmp;
  int i;
  for (i=0; i<desc->n; i++){
    tmp = desc->X[i];
    desc->X[i] = desc->Y[i];
    desc->Y[i] = tmp;
  }
}

FeatureVector2D *CreateFeatureVector2D(int n)
{
  FeatureVector2D *desc=NULL;
  
  desc = (FeatureVector2D *) calloc(1,sizeof(FeatureVector2D));
  if (desc != NULL) {
    desc->X = AllocDoubleArray(n);
    desc->Y = AllocDoubleArray(n);
    desc->n = n;
  } else {
    Error(MSG1,"CreateFeatureVector");
  }
  return(desc);
}

double ContSalieDistance(FeatureVector2D *D1, FeatureVector2D *D2){
  
  int i;
  int n = MIN(D1->n, D2->n);
  double deltaX, dist, eps = 0.2;
  
  dist = 0.0;
  for (i=0; i<n; i++){
    deltaX = fabs(D1->X[i]-D2->X[i]);
    if (deltaX<=eps){
      dist = dist + sqrt(pow(D1->X[i] - D2->X[i],2)+
			 pow(D1->Y[i] - D2->Y[i],2));
    }else
      dist = dist + fabs(D1->Y[i]) + fabs(D2->Y[i]);    
  }
  
  for (i=n; i<D1->n; i++){
    dist = dist + fabs(D1->Y[i]);
  }
  
  for (i=n; i<D2->n; i++){
    dist = dist + fabs(D2->Y[i]);
  }
  
  return dist;
}

FeatureVector2D *CircularRotation(FeatureVector2D *descriptor, double delta){
  
  FeatureVector2D *c = CreateFeatureVector2D(descriptor->n);
  int i;
  
  for (i=0; i<descriptor->n; i++){
    c->Y[i] = descriptor->X[i] + delta;
    if (c->Y[i]<0.0)
      c->Y[i] = 1.0 + c->Y[i];
    if (c->Y[i]>1.0)
      c->Y[i] = 1.0 - c->Y[i];
    c->X[i] = descriptor->Y[i];
  }
  SortFeatureVector2D(c, 0, (c->n-1), INCREASING);
  DescInvertXY(c);
  return c;
}

void DestroyFeatureVector2D(FeatureVector2D **desc)
{
  FeatureVector2D *aux;
  
  aux = *desc;
  if (aux != NULL){
    if (aux->X != NULL) free(aux->X);
    if (aux->Y != NULL) free(aux->Y);
    free(aux);
    *desc = NULL;
  }
}

void SortFeatureVector2D(FeatureVector2D *desc, int left, int right, char order)
{
  int pivot;
  
  if (left < right) {
    pivot = PartFeatureVector2D(desc,left,right,order);
    SortFeatureVector2D(desc,left,pivot-1,order);
    SortFeatureVector2D(desc,pivot+1,right,order); 
  }
}

int PartFeatureVector2D (FeatureVector2D *desc, int left, int right, char order)
{
  double y;
  int i;
  double X,Y;
  
  y = desc->Y[left];
  i = left;
  
  do {
    if (order == INCREASING){
      while ((desc->Y[left] <= y)&&(left <= right)) left++;
      while (desc->Y[right]  > y) right--;
    } else { /* order = DECREASING */
      while ((desc->Y[left] >= y)&&(left <= right)) left++;
      while (desc->Y[right]  < y) right--;
    }
    if (left < right){
      X = desc->X[left];
      Y = desc->Y[left];
      desc->X[left]  = desc->X[right];
      desc->Y[left]  = desc->Y[right];
      desc->X[right] = X;
      desc->Y[right] = Y;
      left++; right--;
    }
  } while (left <= right);
  
  left = i;
  
  if (left != right){
    X = desc->X[left];
    Y = desc->Y[left];
    desc->X[left]  = desc->X[right];
    desc->Y[left]  = desc->Y[right];
    desc->X[right] = X;
    desc->Y[right] = Y;
  }
  
  return (right);
}

FeatureVector2D *CopyFeatureVector2D(FeatureVector2D *desc)
{
  FeatureVector2D *descc;
  
  descc = CreateFeatureVector2D(desc->n);
  memcpy(descc->X,desc->X,desc->n*sizeof(double));
  memcpy(descc->Y,desc->Y,desc->n*sizeof(double));
  
  return(descc);
}

double Matching(FeatureVector2D *descriptor1, FeatureVector2D *descriptor2, int order)
{
  FeatureVector2D *d1 = NULL;
  FeatureVector2D *d2 = NULL;
  FeatureVector2D *D1  = NULL;
  FeatureVector2D *D2  = NULL;
  double max1, max2;
  double dist, distance = INT_MAX;
  int i,j;
  
  d1 = CopyFeatureVector2D(descriptor1);
  d2 = CopyFeatureVector2D(descriptor2);
  SortFeatureVector2D(d1, 0, (d1->n - 1), order);
  SortFeatureVector2D(d2, 0, (d2->n - 1), order);
  
  max1 = fabs(d1->Y[0]);
  max2 = fabs(d2->Y[0]);
  
  i = 0;
  while ((i<d1->n)&&
	 ((fabs(d1->Y[i]) - max1)<=(fabs(0.2*  max1)))){
    j = 0;
    while((j<d2->n)&&
	  ((fabs(d2->Y[j]) - max2)<=(fabs(0.2 * max2)))){
      if (d1->Y[i]*d2->Y[j]>0.0){
	D1 = CircularRotation(descriptor1, -d1->X[i]);
	D2 = CircularRotation(descriptor2, -d2->X[j]);
	dist = ContSalieDistance(D1, D2);
	//WriteInstance(i, j, descriptor1, descriptor2, d1, d2, D1, D2, -d1->X[i], -d2->X[j], dist);
	if (dist < distance)
	  distance = dist;
	DestroyFeatureVector2D(&D1);
	DestroyFeatureVector2D(&D2);
      }
      j++;
    }
    i++;
  }
  
  DestroyFeatureVector2D(&d1);
  DestroyFeatureVector2D(&d2);
  return distance;
}

void iftFastDilation(AnnImg *aimg, AdjRel *A)
{
  Image *Dx=NULL,*Dy=NULL;
  Queue *Q=NULL;
  Heap *H=NULL;
  int i,p,q,n,sz;
  Pixel u,v;
  int *sq=NULL,tmp=INT_MAX,dx,dy;
  bool cuisenaire;
  AdjRel *A8=Circular(1.5),*CA=NULL;
  char *color=NULL;

  if (aimg->seed == NULL)
    return;
  
  n  = MAX(aimg->img->ncols,aimg->img->nrows);
  sq = AllocIntArray(n);
  for (i=0; i < n; i++) 
    sq[i]=i*i;
  
  Dx = CreateImage(aimg->img->ncols,aimg->img->nrows);
  Dy = CreateImage(aimg->img->ncols,aimg->img->nrows);
  n  = aimg->img->ncols*aimg->img->nrows;
  color = AllocCharArray(n);
  sz = FrameSize(A);
  Q  = CreateQueue(2*sz*(sz+aimg->img->ncols+aimg->img->nrows),n);
  
  /* Compute IFT with 8-Adjacency */
  
  while (aimg->seed != NULL){
    p=RemoveSet(&(aimg->seed));
    InsertQueue(Q,aimg->cost->val[p]%Q->C.nbuckets,p);
    color[p]=GRAY;
  }
  
  while(!EmptyQueue(Q)) {
    p=RemoveQueue(Q);
    color[p]=BLACK;
    u.x = p%aimg->img->ncols;
    u.y = p/aimg->img->ncols;
    cuisenaire=true;
    for (i=1; i < A8->n; i++){
      v.x = u.x + A8->dx[i];
      v.y = u.y + A8->dy[i];
      if (ValidPixel(aimg->img,v.x,v.y)){
	q = v.x + aimg->img->tbrow[v.y];
	if (color[q] != BLACK){
	  dx  = Dx->val[p] + abs(v.x-u.x);
	  dy  = Dy->val[p] + abs(v.y-u.y);
	  tmp = sq[dx] + sq[dy];
	  if (tmp < aimg->cost->val[q]){
	    if (color[q] == WHITE){
	      InsertQueue(Q,tmp%Q->C.nbuckets,q);
	      color[q] = GRAY;
	      }else
		UpdateQueue(Q,q,aimg->cost->val[q]%Q->C.nbuckets,tmp%Q->C.nbuckets);
	    aimg->cost->val[q]  = tmp;
	    aimg->pred->val[q]  = p;
	    aimg->label->val[q] = aimg->label->val[p];
	    Dx->val[q] = dx;
	    Dy->val[q] = dy;
	    cuisenaire = false;
	  }
	} 
      }    
    }
    if (cuisenaire)    
      InsertSet(&(aimg->seed),p); 
  }
  
  DestroyQueue(&Q);
  free(color);
  
  /* Compute IFT with Complementary Adjacency */
  
  if (A8->n < A->n) {
    
    CA = ComplAdj(A8,A);
    H  = CreateHeap(n,aimg->cost->val);

    while (aimg->seed != NULL){
      p=RemoveSet(&(aimg->seed));
      InsertHeap(H,p);
    }

    while(!HeapIsEmpty(H)) {
      RemoveHeap(H,&p);
      u.x = p%aimg->img->ncols;
      u.y = p/aimg->img->ncols;
      for (i=0; i < CA->n; i++){
	v.x = u.x + CA->dx[i];
	v.y = u.y + CA->dy[i];
	if (ValidPixel(aimg->img,v.x,v.y)){
	  q = v.x + aimg->img->tbrow[v.y];
	  if (color[q]!=BLACK){
	    dx  = Dx->val[p] + abs(v.x-u.x);
	    dy  = Dy->val[p] + abs(v.y-u.y);
	    tmp = sq[dx] + sq[dy];
	    if (tmp < aimg->cost->val[q]) 
	      {
		aimg->cost->val[q]  = tmp;
		aimg->pred->val[q]  = p;
		aimg->label->val[q] = aimg->label->val[p];
		Dx->val[q] = dx;
		Dy->val[q] = dy;
		if (color[q] == WHITE){
		  InsertHeap(H,q);
		}else
		  GoUpHeap(H,H->pos[q]);
	      }
	  }
	}    
      }
    }
    DestroyAdjRel(&CA);
    DestroyHeap(&H);
  }

  DestroyAdjRel(&A8);

  free(sq);
  DestroyImage(&Dx);
  DestroyImage(&Dy);
}

Curve3D *SkelCont(Image *bin, int maxdist, int threshold, int angle, char side) {  
  Image   *contour=NULL;
  Image   *msskel=NULL;
  Image   *skel=NULL;
  Image   *bin_skel=NULL;
  AdjRel  *A=NULL;
  AnnImg  *aimg=NULL;
  Curve3D *contour_salie = NULL;
  Curve3D *skelsaliences = NULL;
  Curve3D *saliences = NULL;
  Pixel left, right;
  int i, j, p, q, n, label, imax, maxcont, max, min, imin, x, y, ne, ni, delta = 3; 
  double sum;
  
  contour   = LabelContPixel(bin);
  aimg      = Annotate(bin,NULL,contour); 
  A         = Circular(1.5);
  iftFastDilation(aimg,A);
  
  msskel   = MSSkel(bin, side);
  skel     = Skeleton(msskel, threshold);
  bin_skel = Skeleton(msskel, threshold);
  n        = bin->ncols*bin->nrows;
  contour_salie = Saliences(bin, maxdist);

  maxcont = MaximumValue(contour);

  for (p=0; p<n; p++){
    if (skel->val[p]!=0){
      q = Seed(aimg->pred, p);
      label = (aimg->label->val[q] + msskel->val[p]/2 + maxcont)%maxcont;
      skel->val[p] = label;
      if (side == INTERIOR){
	if (contour_salie->Z[label-1]<0.0){
	  max = INT_MIN;
	  imax = 0;
	  for (j=-5; j<5; j++){
	    if (contour_salie->Z[(label-1+j+contour_salie->n)%contour_salie->n] > max){
	      imax = (label-1+j+contour_salie->n)%contour_salie->n;
	      max = contour_salie->Z[imax];
	    }
	  }
	  skel->val[p] = imax + 1;
	}
	else {
	  skel->val[p] = MAX(label,1);
	}
      }
      else{ 
	if (side == EXTERIOR){
	  if (contour_salie->Z[label-1]>0.0){
	    min = INT_MAX;
	    imin = 0;
	    for (j=-5; j<5; j++){
	      if (contour_salie->Z[(label-1+j+contour_salie->n)%contour_salie->n] < min){
		imin = (label-1+j+contour_salie->n)%contour_salie->n;
		min = contour_salie->Z[imin];
	      }
	    }
	    skel->val[p] = imin + 1;
	  }
	  else {
	    skel->val[p] = MAX(label, 1);
	  }
	}
      }
    }
  }
  
  skelsaliences = SkelSaliences(bin_skel, maxdist, angle); 
  
  if (side==EXTERIOR){
    left.x  = bin->ncols-1;
    left.y  = bin->nrows-1;
    right.x = 0;
    right.y = 0;
    for (y=0; y < bin->nrows; y++)
      for (x=0; x < bin->ncols; x++){
	if (bin->val[x+bin->tbrow[y]] > 0){
	  if (x < left.x)
	    left.x = x;
	  if (y < left.y)
	    left.y = y;
	  if (x > right.x)
	    right.x = x;
	  if (y > right.y)
	    right.y = y;	
	}
      }
    
    for (i=0; i<skelsaliences->n; i++){
      if ((skelsaliences->X[i]<left.x)||
	  (skelsaliences->X[i]>right.x)||
	  (skelsaliences->Y[i]<left.y)||
	  (skelsaliences->Y[i]>right.y))
	skelsaliences->Z[i] = 0.0;
    }
  }
  
  SortCurve3D(skelsaliences, 0, (skelsaliences->n - 2), DECREASING);
  i=0;
  while (skelsaliences->Z[i]!=0.0)
    i++;
  saliences = CreateCurve3D(i);  
  for (i=0; i< saliences->n; i++){
    if (skelsaliences->Z[i]!=0.0){
      p = (int)skelsaliences->X[i]+bin->tbrow[(int)skelsaliences->Y[i]];
      saliences->X[i] = contour_salie->X[skel->val[p]-1];
      saliences->Y[i] = contour_salie->Y[skel->val[p]-1];
      if (side==INTERIOR){
	sum = 0.0;
	for (j=-delta; j<=delta; j++){
	  q = ((skel->val[p]-1) + j + maxcont) % maxcont;
	  if (contour_salie->Z[q]>0.0)
	    sum += contour_salie->Z[q];
	}
	saliences->Z[i] = sum;
      }      
      else{
	sum = 0.0;
	for (j=-delta; j<=delta; j++){
	  q = ((skel->val[p]-1) + j + maxcont) % maxcont;
	  if (contour_salie->Z[q]<0.0)
	    sum += contour_salie->Z[q];
	}
	saliences->Z[i] = sum;
      }
    }  
  }

  ne = 0;
  ni = 0;
  for (i=0; i<saliences->n; i++){
    if (saliences->Z[i]>0.0) 
      ni += saliences->Z[i];
    else
      if (saliences->Z[i]<0.0) 
	ne += fabs(saliences->Z[i]);
  }

  for (i=0; i<saliences->n; i++){
    if (saliences->Z[i]>0.0) 
      saliences->Z[i]/=ni;
    else
      if (saliences->Z[i]<0.0) 
	saliences->Z[i]/=ne;
  }

  
  DestroyImage(&contour);
  DestroyImage(&msskel);
  DestroyImage(&skel);
  DestroyImage(&bin_skel);
  DestroyCurve3D(&contour_salie);
  DestroyCurve3D(&skelsaliences);
  DestroyAdjRel(&A);
  DeAnnotate(&aimg);
  return(saliences);

}

Curve3D *iftContourSaliences(Image *bin,int threshold_in,int threshold_out,int angle_in,int angle_out)
{

  Curve3D *saliences = NULL;
  Curve3D *convex_saliences = NULL;
  Curve3D *concave_saliences = NULL;
  int i;
  int maxdist = 10;
  
  convex_saliences = SkelCont(bin,maxdist,threshold_in, angle_in, INTERIOR);
  concave_saliences = SkelCont(bin, maxdist, threshold_out, angle_out, EXTERIOR);
  saliences = CreateCurve3D(convex_saliences->n + concave_saliences->n);
  for (i=0; i<convex_saliences->n; i++){
    saliences->X[i] = convex_saliences->X[i];
    saliences->Y[i] = convex_saliences->Y[i];
    saliences->Z[i] = convex_saliences->Z[i];
  }
  for (i=convex_saliences->n; i<saliences->n;i++){
    saliences->X[i] = concave_saliences->X[i-convex_saliences->n];
    saliences->Y[i] = concave_saliences->Y[i-convex_saliences->n];
    saliences->Z[i] = concave_saliences->Z[i-convex_saliences->n];
  }
  
  DestroyCurve3D(&convex_saliences);
  DestroyCurve3D(&concave_saliences);
  return saliences;
}

Curve *ContourSaliences(Image *in)
{
  Curve3D *saliences = NULL;
  Curve *descriptor = NULL;
  Image *contour = NULL;
  int i, p, max;
  
  contour = LabelContPixel(in);
  saliences = iftContourSaliences(in, 5, 20, 50, 110);    
  
  descriptor = CreateCurve(saliences->n);
  max = MaximumValue(contour);
  for (i=0; i<saliences->n; i++){
    descriptor->X[i] = saliences->Z[i];
    p = (int)saliences->X[i]+contour->tbrow[(int)saliences->Y[i]];
    descriptor->Y[i] = (double)(((contour->val[p])-1))/max;
  }
  SortCurve(descriptor, 0, (descriptor->n - 1), INCREASING);
  InvertXY(descriptor);  
  
  DestroyCurve3D(&saliences);
  DestroyImage(&contour);
  
  return (descriptor);
}

void WriteFeatureVector2D(FeatureVector2D *desc,char *filename)
{
  FILE *fp;
  int i;
  
  fp = fopen(filename,"w");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  for (i=0; i < desc->n; i++)
    fprintf(fp,"%f\t%f\n",desc->X[i],desc->Y[i]);
  
  fclose(fp);
}

FeatureVector2D *CS_ExtractionAlgorithm(Image *img){
 Curve *curve = NULL;
 FeatureVector2D *fv = NULL;
 
 curve = ContourSaliences(img);
 fv = CurveTo2DFeatureVector(curve);
 
 DestroyCurve(&curve);
 return fv;
}

double CS_DistanceAlgorithm(FeatureVector2D *descriptor1, FeatureVector2D *descriptor2){
  double convex_distance = INT_MIN;
  double concave_distance = INT_MIN;
  
  convex_distance = Matching(descriptor1, descriptor2, DECREASING);
  concave_distance = Matching(descriptor1, descriptor2, INCREASING);
  return(MIN(convex_distance, concave_distance));
}

/* Segment Saliences */
Curve *SS_ExtractionAlgorithm_(Image *in, int maxdist, int nsamples, int side){
  Curve *inner = NULL;
  Curve *outer = NULL;
  Curve *diff = NULL;
  Curve *ninner = NULL;
  Curve *nouter = NULL;
  Curve *ndiff = NULL;
  Curve *output = NULL;

  Image *mbb = NULL;
  Image *bin = NULL;
  Image *contour = NULL;
  Image *segments = NULL;
  
  AdjRel *A=NULL;
  AnnImg *aimg= NULL;
  
  int p,i,Lmax, maxcost = maxdist*maxdist;
  double nin, nout, maxin, maxout;

  mbb  = MBB(in);
  bin = AddFrame(mbb,maxdist,0);

  DestroyImage(&mbb);
  
  segments = LabelContPixel(bin);

  /* Compute Euclidean IFT */
  contour    = LabelContPixel(bin);
  
  aimg    = Annotate(bin,NULL,contour); 
  A       = Circular(1.5);
  iftDilation(aimg,A);  
  
  Lmax    = MaximumValue(aimg->label);
  //printf("Lmax = %d\n", Lmax);
  inner   = CreateCurve(Lmax);
  outer   = CreateCurve(Lmax);
  diff    = CreateCurve(Lmax);
  
  for (i=0; i<Lmax; i++){
    diff->X[i] = inner->X[i] = outer->X[i]= (double)(i*nsamples)/Lmax;
  }  
  
  /* Compute influence areas */  
  nin = nout = 0.0;
  for (p=0; p < bin->ncols*bin->nrows; p++){
    if (segments->val[p] != 0){
      segments->val[p]=((((segments->val[p]*nsamples)/Lmax))/*%2*/)+1;
    }
    if ((aimg->label->val[p] > 0)&&(aimg->cost->val[p] <= maxcost)) {
      if (aimg->img->val[p] != 0){
	nin++;
	inner->Y[aimg->label->val[p]-1]++;
      } else {
	nout++;
	outer->Y[aimg->label->val[p]-1]++;
      }
    }
  }
  
  maxin = INT_MIN;
  maxout = INT_MIN;
  for (i=0; i<Lmax; i++){
    if (inner->Y[i] > maxin){
      maxin = inner->Y[i];
    }
    if (outer->Y[i] > maxout){
      maxout = outer->Y[i];
    }
  }
  
  for (i=0; i<Lmax; i++){
    inner->Y[i] /= nin;
    outer->Y[i] /= nout;
    diff->Y[i] = outer->Y[i] - inner->Y[i];
  }
  
  ninner   = CreateCurve(nsamples);
  nouter   = CreateCurve(nsamples);
  ndiff    = CreateCurve(nsamples);
  
  for (i=0; i<nsamples; i++){
    ninner->X[i] = nouter->X[i] = ndiff->X[i] = i;
  }
  for (i=0; i<Lmax; i++){
    ninner->Y[(int)inner->X[i]] += inner->Y[i];
    nouter->Y[(int)outer->X[i]] += outer->Y[i];
  }
  for (i=0; i<nsamples; i++){
    ndiff->Y[i] =  nouter->Y[i] - ninner->Y[i];
  }
  
  
  if (side == INTERIOR){
    output = CopyCurve(ninner);
  }
  else if (side==EXTERIOR){
    output = CopyCurve(nouter);
  }
  else if (side == BOTH){
    output = CopyCurve(ndiff);
  }
  else{
    printf("Invalid \"side\" option <%d>\n", side);
    exit(-1);
  }
  
  DestroyImage(&segments);
  DestroyCurve(&ninner);
  DestroyCurve(&nouter);
  DestroyCurve(&ndiff);

  DestroyImage(&contour);
  DestroyAdjRel(&A);
  DeAnnotate(&aimg);

  DestroyImage(&bin);
  DestroyImage(&mbb);
  DestroyCurve(&inner);
  DestroyCurve(&outer);
  DestroyCurve(&diff);

  return output;
}

FeatureVector1D *SS_ExtractionAlgorithm(Image *img){
 Curve *curve = NULL;
 FeatureVector1D *fv = NULL;
 
 curve = SS_ExtractionAlgorithm_(img, 5, 100, BOTH);
 fv = CurveTo1DFeatureVector(curve);
 
 DestroyCurve(&curve);
 return fv;
}

/***************SS SIMILARITY ALGORITHM*********************/
double SS_getMin(double Dist1, double Dist2, double Dist3){
  if((Dist1<=Dist2) && (Dist1<=Dist3)) 
    return(Dist1);
  else if((Dist2<=Dist1) && (Dist2<=Dist3)) 
    return(Dist2);
  //else if((Dist3<=Dist1) && (Dist3<=Dist2)) 
  return(Dist3);
}

double SS_OCS(FeatureVector1D *fv1, FeatureVector1D *fv2){
  
  int i,j, dim1 = fv1->n, dim2 = fv2->n;
  double temp_dist;
  double penalty;
  double *DISTANCE = NULL;
  
  DISTANCE=(double *) calloc((dim1+1)*(dim2+1),sizeof(double));

  penalty=20.0;
  /* OPTIMAL CORRESPONDENCE OF STRINGS
   */
  DISTANCE[0*(dim2+1)+0]=0;
  for(j=1;j<=dim2;j++)
    DISTANCE[0*(dim2+1)+j]=j * penalty;
  
  for(i=1;i<=dim1;i++)
    DISTANCE[i*(dim2+1)+0]=i * penalty;
  
  for(i=1;i<=dim1;i++)
    for(j=1;j<=dim2;j++)
      if(abs(i-j) < (5)){
	temp_dist=abs(fv1->X[i-1]-fv2->X[j-1]);
		
	DISTANCE[i*(dim2+1)+j]= temp_dist +
	  SS_getMin(DISTANCE[(i-1)*(dim2+1)+(j-1)],
		 DISTANCE[(i-1)*(dim2+1)+(j)] + penalty,
		 DISTANCE[(i)*(dim2+1)+(j-1)] + penalty); 
      }
  
  temp_dist = DISTANCE[(dim1)*(dim2+1)+(dim2)]/dim2;
  free(DISTANCE);
  
  return temp_dist;
}

double SS_OCSMatching(FeatureVector1D *fv_1, FeatureVector1D *fv_2){
  double distance,temp_dist;
  int i,k;
  FeatureVector1D *temp1, *temp2, *fv1, *fv2;
  
  fv1 = CreateFeatureVector1D(fv_1->n);
  fv2 = CreateFeatureVector1D(fv_2->n);
  for (i = 0; i<fv1->n; i++){
    fv1->X[i] = 100*fv_1->X[i];
    fv2->X[i] = 100*fv_2->X[i];
  }
  
  temp1 = CreateFeatureVector1D(fv2->n);
  temp2 = CreateFeatureVector1D(fv2->n);
  
  temp_dist=INT_MAX; 
  for(k=0; k<fv2->n; k++){
    for(i=0;i<fv2->n;i++){
      temp2->X[i]=fv2->X[(i+k)%fv2->n];
    }   
    distance= SS_OCS(fv1,temp2);
    if(temp_dist>distance) 
      temp_dist=distance;
  }
  /***Taking the mirror of fv2 *****/
  for(i=0;i<fv2->n;i++){
    temp2->X[i]=fv2->X[(fv2->n-1)-i];
  }
  
  for(k=0; k<fv2->n; k++){
    for(i=0;i<fv2->n;i++){
      temp1->X[i]= temp2->X[(i+k)%fv2->n];
    }
    distance=SS_OCS(fv1,temp1);
    if(temp_dist>distance) 
      temp_dist=distance;
  }
  
  distance=temp_dist;
  DestroyFeatureVector1D(&temp1);
  DestroyFeatureVector1D(&temp2);
  DestroyFeatureVector1D(&fv1);
  DestroyFeatureVector1D(&fv2);
  return(distance);
}

double SS_DistanceAlgorithm(FeatureVector1D *fv1d1, FeatureVector1D *fv1d2){
  
  double dist;

  dist = SS_OCSMatching(fv1d1, fv1d2);
    
  return dist;
}

/* Metrics to measure the similarity between feature vectors*/
double EuclideanDistance(FeatureVector1D *v1, FeatureVector1D *v2) { 
  int i;
  double sum = 0.0;
  double z = 0.0;
  
  for (i = 0; i < v1->n ; i++){
    z = v1->X[i] - v2->X[i]; 
    sum += z*z;
  }
  sum = sqrtf(sum);
  return (sum);
}

double L1_Distance(FeatureVector1D *v1, FeatureVector1D *v2) { 
  int i;
  double sum = 0.0;
  
  for (i = 0; i < v1->n ; i++){
    sum += fabs(v1->X[i] - v2->X[i]); 
  }
  return (sum);
}

double dLog(FeatureVector1D *fv1, FeatureVector1D *fv2)
{
	int i;
	double sum = 0.0;
	double q,d;
	
	for(i = 0; i < fv1->n; i++)
	{
	
		if(fv1->X[i] == 0)
			q = 0;
		else
			if((fv1->X[i] > 0) && (fv1->X[i] <= 1))
				q = 1;
			else
				q = log10(fv1->X[i])/log10(2);
		if(fv2->X[i] == 0)
			d = 0;
		else
			if((fv2->X[i] > 0) && (fv2->X[i] <= 1))
				d = 1;
			else
				d = log10(fv2->X[i])/log10(2);
		
		sum = sum + fabs(q-d);
	}

	
	return sum;
}
