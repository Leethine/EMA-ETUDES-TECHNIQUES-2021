/*------------------------------------------------------------------------------
   Example code that shows the use of the 'cam2world" and 'world2cam" functions
   Shows also how to undistort images into perspective or panoramic images
   
   NOTE, IF YOU WANT TO SPEED UP THE REMAP FUNCTION I STRONGLY RECOMMEND TO INSTALL
   INTELL IPP LIBRARIES ( http://software.intel.com/en-us/intel-ipp/ )
   YOU JUST NEED TO INSTALL IT AND INCLUDE ipp.h IN YOUR PROGRAM
   
   Copyright (C) 2009 DAVIDE SCARAMUZZA, ETH Zurich  
   Author: Davide Scaramuzza - email: davide.scaramuzza@ieee.org
------------------------------------------------------------------------------*/

#include "ocam_functions.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <cstdlib>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/core/types_c.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/imgproc/imgproc_c.h>

#include <string.h>
#include <time.h>

double M[3];
double m[2];

int point_index[2];

static void onMouse(int event, int x, int y, int, void*);
void printP();
void printM();


int main(int argc, char *argv[])
{   
  struct ocam_model o, o_cata; // our ocam_models for the fisheye and catadioptric cameras
  get_ocam_model(&o, "./calib_results_fisheye.txt");
  get_ocam_model(&o_cata, "./calib_results_catadioptric.txt"); 
  
  int height = o.height;
  int width  = o.width;
  
  float sf = readsf();
  float R = o.r;
  float Nxc = o.xc;
  float Nyc = o.yc;
  float Nz  = -R/sf;
  
  double factor;
  
  cv::Mat img_fisheye = cv::imread("test_fisheye.jpg");
  cv::Mat img_persp = cv::imread("undistorted_perspective.jpg");
  
  cv::Mat img_contour = cv::Mat::zeros(height, width, CV_8UC1);
  cv::Mat img_contour2 = cv::Mat::zeros(height, width, CV_8UC1);
  cv::Canny	( img_persp, img_contour, 90, 100 );
  
  CvMat* mapx_persp = cvCreateMat(1024, 1280, CV_32FC1);
  CvMat* mapy_persp = cvCreateMat(1024, 1280, CV_32FC1);
  create_perspecive_undistortion_LUT( mapx_persp, mapy_persp, &o, sf );
  
  int i, j;
    
  //M[0] = (double)i - Nxc;
  //M[1] = (double)j - Nyc;
  //M[2] = -R/sf;
     
  //world2cam(m, M, &o);
  cv::namedWindow( "REPRO_TEST", 0 );
  cv::setMouseCallback( "REPRO_TEST", onMouse, 0 );
  
  //cv::circle(img_persp, cv::Point2f (j, i), 3, cv::Scalar(255,255,255), -1);
  cv::imshow("REPRO_TEST", img_persp);
  cv::waitKey(-1);
  
  i = point_index[0];
  j = point_index[1];
  
  std::cout << "i = " << i << ", j = " << j << std::endl;
  
  float * data_mapx = mapx_persp->data.fl;
  float * data_mapy = mapy_persp->data.fl;
  
  m[1] = *(data_mapx + width*i + j);
  m[0] = *(data_mapy + width*i + j);
  
  printP();
  //m[1] = i;
  //m[0] = j;
  
  cam2world(M, m, &o, true);
  
  printM();
  
  factor = Nz / M[2];
  
  M[0] *= factor;
  M[1] *= factor;
  
  double px, py;
  
  //px = M[0]+Nxc;
  //py = M[1]+Nyc;
  
  px = m[0];
  py = m[1];
  
  std::cout << "i = " << px << ", j = " << py << std::endl;
  
  cv::circle(img_fisheye, cv::Point2f (py,px), 3, cv::Scalar(255,255,255), -1);
  
  cv::imshow("REPRO_TEST", img_fisheye);
  cv::waitKey(-1);
  
  cvReleaseMat(&mapx_persp);
  cvReleaseMat(&mapy_persp);
  
  return 0;
}

void printP() {

  std::cout << "m[1] = " << m[1] << std::endl;
  std::cout << "m[0] = " << m[0] << std::endl; 

}

void printM() {
  
  std::cout << "M[0] = x = " << M[0] << std::endl;
  std::cout << "M[1] = y = " << M[1] << std::endl; 
  std::cout << "M[2] = z = " << M[2] << std::endl;
  
}

static void onMouse(int event, int x, int y, int, void*)
{
  // Action when left button is pressed
  if ((event == cv::EVENT_LBUTTONDOWN) )
  {
    point_index[0] = y;
    point_index[1] = x;
  }
}

