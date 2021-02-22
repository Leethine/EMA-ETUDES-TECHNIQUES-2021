/*------------------------------------------------------------------------------
   Example code that shows the use of the 'cam2world" and 'world2cam" functions
   Shows also how to undistort images into perspective or panoramic images
   Copyright (C) 2008 DAVIDE SCARAMUZZA, ETH Zurich  
   Author: Davide Scaramuzza - email: davide.scaramuzza@ieee.org
------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui.hpp>
#include "ocam_functions.h"


//------------------------------------------------------------------------------
int get_ocam_model(struct ocam_model *myocam_model, const char *filename)
{
 double *pol        = myocam_model->pol;
 double *invpol     = myocam_model->invpol; 
 double *xc         = &(myocam_model->xc);
 double *yc         = &(myocam_model->yc); 
 double *c          = &(myocam_model->c);
 double *d          = &(myocam_model->d);
 double *e          = &(myocam_model->e);
 double *r          = &(myocam_model->r);
 int    *width      = &(myocam_model->width);
 int    *height     = &(myocam_model->height);
 int *length_pol    = &(myocam_model->length_pol);
 int *length_invpol = &(myocam_model->length_invpol);
 FILE *f;
 char buf[CMV_MAX_BUF];
 int i;
 
 //Open file
 if(!(f=fopen(filename,"r")))
 {
   printf("File %s cannot be opened\n", filename);				  
   return -1;
 }
 
 //Read polynomial coefficients
 fgets(buf,CMV_MAX_BUF,f);
 fscanf(f,"\n");
 fscanf(f,"%d", length_pol);
 for (i = 0; i < *length_pol; i++)
 {
     fscanf(f," %lf",&pol[i]);
 }

 //Read inverse polynomial coefficients
 fscanf(f,"\n");
 fgets(buf,CMV_MAX_BUF,f);
 fscanf(f,"\n");
 fscanf(f,"%d", length_invpol);
 for (i = 0; i < *length_invpol; i++)
 {
     fscanf(f," %lf",&invpol[i]);
 }
 
 //Read center coordinates
 fscanf(f,"\n");
 fgets(buf,CMV_MAX_BUF,f);
 fscanf(f,"\n");
 fscanf(f,"%lf %lf %lf\n", xc, yc, r);

 //Read affine coefficients
 fgets(buf,CMV_MAX_BUF,f);
 fscanf(f,"\n");
 fscanf(f,"%lf %lf %lf\n", c,d,e);

 //Read image size
 fgets(buf,CMV_MAX_BUF,f);
 fscanf(f,"\n");
 fscanf(f,"%d %d", height, width);

 fclose(f);
 return 0;
}

//------------------------------------------------------------------------------
void cam2world(double point3D[3], double point2D[2], struct ocam_model *myocam_model, bool normalise)
{
 double *pol    = myocam_model->pol;
 double xc      = (myocam_model->xc);
 double yc      = (myocam_model->yc); 
 double c       = (myocam_model->c);
 double d       = (myocam_model->d);
 double e       = (myocam_model->e);
 int length_pol = (myocam_model->length_pol); 
 double invdet  = 1/(c-d*e); // 1/det(A), where A = [c,d;e,1] as in the Matlab file

 double xp = invdet*(    (point2D[0] - xc) - d*(point2D[1] - yc) );
 double yp = invdet*( -e*(point2D[0] - xc) + c*(point2D[1] - yc) );
  
 double r   = sqrt(  xp*xp + yp*yp ); //distance [pixels] of the point from the image center
 double zp  = pol[0];
 double r_i = 1;
 int i;
 
 for (i = 1; i < length_pol; i++)
 {
   r_i *= r;
   zp  += r_i*pol[i];
 }
 
 //normalize to unit norm
 double invnorm;
 if (normalise)
    invnorm = 1/sqrt( xp*xp + yp*yp + zp*zp );
 else
    invnorm = 1.0;
    
 point3D[0] = invnorm*xp;
 point3D[1] = invnorm*yp; 
 point3D[2] = invnorm*zp;
 
}

//------------------------------------------------------------------------------
void world2cam(double point2D[2], double point3D[3], struct ocam_model *myocam_model)
{
 double *invpol     = myocam_model->invpol; 
 double xc          = (myocam_model->xc);
 double yc          = (myocam_model->yc); 
 double c           = (myocam_model->c);
 double d           = (myocam_model->d);
 double e           = (myocam_model->e);
 int    width       = (myocam_model->width);
 int    height      = (myocam_model->height);
 int length_invpol  = (myocam_model->length_invpol);
 double norm        = sqrt(point3D[0]*point3D[0] + point3D[1]*point3D[1]);
 double theta       = atan(point3D[2]/norm);
 double t, t_i;
 double rho, x, y;
 double invnorm;
 int i;
  
  if (norm != 0) 
  {
    invnorm = 1/norm;
    t  = theta;
    rho = invpol[0];
    t_i = 1;

    for (i = 1; i < length_invpol; i++)
    {
      t_i *= t;
      rho += t_i*invpol[i];
    }

    x = point3D[0]*invnorm*rho;
    y = point3D[1]*invnorm*rho;
  
    point2D[0] = x*c + y*d + xc;
    point2D[1] = x*e + y   + yc;
  }
  else
  {
    point2D[0] = xc;
    point2D[1] = yc;
  }
}
//------------------------------------------------------------------------------
void create_perspecive_undistortion_LUT( CvMat *mapx, CvMat *mapy, struct ocam_model *ocam_model, float sf)
{
     int i, j;
     int width = mapx->cols; //New width
     int height = mapx->rows;//New height     
     float *data_mapx = mapx->data.fl;
     float *data_mapy = mapy->data.fl;
     
     float R = ocam_model->r;
     
     //float Nxc = height/2.0;
     //float Nyc = width/2.0;
     //float Nz  = -width/sf; // ?
     float Nxc = ocam_model->xc;
     float Nyc = ocam_model->yc;
     float Nz  = -R/sf; // ?
     
     double M[3];
     double m[2];

     for (i=0; i<height; i++) {
         for (j=0; j<width; j++)
         {   
             M[0] = ((float)i - Nxc);
             M[1] = ((float)j - Nyc);
             M[2] = Nz;
             world2cam(m, M, ocam_model);
             //*( data_mapx + i*width+j ) = 500;
             //*( data_mapy + i*width+j ) = 500;
             *( data_mapx + i*width+j ) = (float) m[1];
             *( data_mapy + i*width+j ) = (float) m[0];
         }
     }

}

/**
// USELESS FUNCTION
void reproject_perspecive_LUT( CvMat *mapx, CvMat *mapy, CvMat *mapx_inv, CvMat *mapy_inv, struct ocam_model *ocam_model, float sf)
{
     int i, j;
     int height = mapx_inv->cols; //New width
     int width = mapy_inv->rows;//New height     
     
     float *data_mapx = mapx->data.fl;
     float *data_mapy = mapy->data.fl;
     float *data_mapx_inv = mapx_inv->data.fl;
     float *data_mapy_inv = mapy_inv->data.fl;
     
     float Nxc = ocam_model->xc;
     float Nyc = ocam_model->yc;
     float Nz  = -R/sf; // ?
     double M[3];
     double M_rep[3];
     double m[2];
     
     int index_x, index_y;
     for (i=0; i<height; i++)
         for (j=0; j<width; j++)
         {   
             m[1] = *( data_mapx + i*width+j ) ;
             m[0] = *( data_mapy + i*width+j ) ;
             cam2world(M, m, ocam_model, true);

             M_rep[0] = Nz * M[0]/M[2];
             M_rep[1] = Nz * M[1]/M[2];
             M_rep[2] = Nz;
             
             //int pi, pj;
             //pi = (int) m[1];
             //pj = (int) m[0];
             
             //world2cam(m, M_rep, ocam_model);
             //index_x = (int) m[1];
             //index_y = (int) m[0];
             //*( data_mapx_inv + i * width + j ) = M[0];
             //*( data_mapy_inv + i * width + j ) = M[1];
             
             *( data_mapy_inv + i*width+j ) = M_rep[0] + Nxc;
             *( data_mapx_inv + i*width+j ) = M_rep[1] + Nyc;
             
             // f_MAPX(Px) = New_Px
             // finv_MAPX(New_Px) = Px_orig
         }
}

// Useless function
void create_reproject_LUT( CvMat * rep_LUTx, CvMat * rep_LUTy, struct ocam_model *ocam_model, float sf)
{
     int i, j;

     int height = ocam_model->height;
     int width = ocam_model->width;
     
     float * datax = rep_LUTx->data.fl;
     float * datay = rep_LUTy->data.fl;
     
     float Nxc = height/2.0;
     float Nyc = width/2.0;
     float Nz  = -width/sf; // ?
     double M[3];
     double M_rep[3];
     double m[2];
     
     int index_x, index_y;
     for (i=0; i<height; i++)
         for (j=0; j<width; j++)
         {   
             M[0] = ((float)i - Nxc);
             M[1] = ((float)j - Nyc);
             M[2] = Nz;
             M_rep[0] = ((float)i - Nxc);
             M_rep[1] = ((float)j - Nyc);
             M_rep[2] = Nz;
             
             world2cam(m, M_rep, ocam_model);
             index_x = (int) m[1];
             index_y = (int) m[0];
             
             if (index_x > height || index_x > width) std::cout << "Oh no..." << std::endl;
             //rep_LUT->at(index_x)->at(index_y)[0] = M[0];
             //rep_LUT->at(index_x)->at(index_y)[1] = M[1];
             
             *( datax + index_x*width + index_y ) = (float) M[0];
             *( datay + index_x*width + index_y ) = (float) M[1];
         }
}

//------------------------------------------------------------------------------
void create_panoramic_undistortion_LUT ( CvMat *mapx, CvMat *mapy, float Rmin, float Rmax, float xc, float yc )
{
     int i, j;
     float theta;
     int width = mapx->width;
     int height = mapx->height;     
     float *data_mapx = mapx->data.fl;
     float *data_mapy = mapy->data.fl;
     float rho;
     
     for (i=0; i<height; i++)
         for (j=0; j<width; j++)
         {
             theta = -((float)j)/width*2*M_PI; // Note, if you would like to flip the image, just inverte the sign of theta
             rho   = Rmax - (Rmax-Rmin)/height*i;
             *( data_mapx + i*width+j ) = yc + rho*sin(theta); //in OpenCV "x" is the
             *( data_mapy + i*width+j ) = xc + rho*cos(theta);             
         }
}
**/

void descart2angular(double PointXYZ[3], double PointANG[3], struct ocam_model *myocam_model) {
// Convert cartesian corrdinate to angular coordinate
// TODO
    double *pol        = myocam_model->pol; 
    double *invpol     = myocam_model->invpol; 
    double xc          = (myocam_model->xc);
    double yc          = (myocam_model->yc); 
    double c           = (myocam_model->c);
    double d           = (myocam_model->d);
    double e           = (myocam_model->e);
    int    width       = (myocam_model->width);
    int    height      = (myocam_model->height);
    int length_invpol  = (myocam_model->length_invpol);
    double norm        = sqrt(point3D[0]*point3D[0] + point3D[1]*point3D[1]);
    
    double theta       = atan(point3D[2]/norm);
    double t, t_i;
    double rho, x, y;
    double invnorm;
    int i;
  
}

float readsf(void) {
    FILE *f;
    char filename[] = "sf.txt";
    float sf;
 
    //Open file
    if(!(f=fopen(filename,"r")))
    {
        printf("File %s cannot be opened\n", filename);				  
        return -1;
    }
    else {
        fscanf(f, "%f", &sf);
    }
   
    fclose(f);
   
    return sf;
}
