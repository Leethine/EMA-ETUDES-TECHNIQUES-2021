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

int main(int argc, char *argv[])
{   
  struct ocam_model o, o_cata; // our ocam_models for the fisheye and catadioptric cameras
  get_ocam_model(&o, "./calib_results_fisheye.txt");
 
  int th1 = 90;
  int th2 = 100;
  
  int height = o.height;
  int width  = o.width;
  
  float sf = readsf();
  float R = o.r;
  float Nxc = o.xc;
  float Nyc = o.yc;
  float Nz  = -R/sf;
  
  float px, py;
  
  cv::Mat img_fisheye = cv::imread("test_fisheye.jpg");
  cv::Mat img_persp = cv::imread("undistorted_perspective.jpg");
  
  cv::Mat img_contour = cv::Mat::zeros(height, width, CV_8UC1);
  cv::Canny	( img_persp, img_contour, th1, th2 );

  for (int i=0; i < height; i++) {
    for (int j=0; j < width; j++) {
      if ( img_contour.at<uchar>(i,j) != 0 ) {
        cv::circle(img_persp, cv::Point2f (j,i), 1, cv::Scalar(255,255,255), -1); 
      }
    }
  }
  
  cv::imshow("Edge Detection", img_persp);
  cv::waitKey(-1);
  
  CvMat* mapx_persp = cvCreateMat(height, width, CV_32FC1);
  CvMat* mapy_persp = cvCreateMat(height, width, CV_32FC1);
  create_perspecive_undistortion_LUT( mapx_persp, mapy_persp, &o, sf );
  
  float * data_mapx = mapx_persp->data.fl;
  float * data_mapy = mapy_persp->data.fl;
  
  for (int i=0; i < height; i++) {
    for (int j=0; j < width; j++) {
      if ( img_contour.at<uchar>(i,j) != 0 ) {
        px = *(data_mapx + width*i + j);
        py = *(data_mapy + width*i + j);
        cv::circle(img_fisheye, cv::Point2f (px,py), 1, cv::Scalar(255,255,255), -1); 
        
        //std::cout << img_contour.at<uchar>(j,i); 
      }
    }
  }
  
  cv::imshow("Edge Detection", img_fisheye);
  cv::waitKey(-1);
  
  cvReleaseMat(&mapx_persp);
  cvReleaseMat(&mapy_persp);
  
  return 0;
}
