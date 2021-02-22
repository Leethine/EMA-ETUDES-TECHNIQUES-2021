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

int point_index[2];

int main(int argc, char *argv[])
{   
  struct ocam_model o, o_cata; // our ocam_models for the fisheye and catadioptric cameras
  get_ocam_model(&o, "./calib_results_fisheye.txt");

  int height = o.height;
  int width  = o.width;
  
  float sf = readsf();
  float R = o.r;
  float Nxc = o.xc;
  float Nyc = o.yc;
  float Nz  = -R/sf;
  
  cv::Mat img_fisheye = cv::imread("test_fisheye.jpg");
  cv::Mat img_persp = cv::imread("undistorted_perspective.jpg");
  cv::Mat src_gray;
  
  cv::cvtColor( img_persp, src_gray, cv::COLOR_BGR2GRAY );
  cv::Mat dst_norm, dst_norm_scaled;
  
  int thresh = 200;
  int max_thresh = 255;
  
  int blockSize = 4;
  int apertureSize = 3;
  double k = 0.04;
  
  cv::Mat dst = cv::Mat::zeros( src_gray.size(), CV_32FC1 );
  cv::cornerHarris( src_gray, dst, blockSize, apertureSize, k );
  cv::normalize( dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat() );
  cv::convertScaleAbs( dst_norm, dst_norm_scaled );
    
  for( int i = 0; i < dst_norm.rows ; i++ ) {
    for( int j = 0; j < dst_norm.cols; j++ ) {
      if( (int) dst_norm.at<float>(i,j) > thresh ) {
        cv::circle( img_persp, cv::Point(j,i), 6,  cv::Scalar(255,255,255), 2, 8, 0 );
      }
    }
  }

  cv::imshow( "Corners", img_persp );
  cv::waitKey(-1);
  
  CvMat* mapx_persp = cvCreateMat(1024, 1280, CV_32FC1);
  CvMat* mapy_persp = cvCreateMat(1024, 1280, CV_32FC1);
  create_perspecive_undistortion_LUT( mapx_persp, mapy_persp, &o, sf );
  
  float * data_mapx = mapx_persp->data.fl;
  float * data_mapy = mapy_persp->data.fl;
    
  double px, py;

  for( int i = 0; i < dst_norm.rows ; i++ ) {
    for( int j = 0; j < dst_norm.cols; j++ ) {
      if( (int) dst_norm.at<float>(i,j) > thresh ) {
        px = *(data_mapx + width*i + j);
        py = *(data_mapy + width*i + j);
        
        cv::circle( img_fisheye, cv::Point(px,py), 6,  cv::Scalar(255,255,255), 2, 8, 0 );
      }
    }
  }

  cv::imshow( "Corners", img_fisheye );
  cv::waitKey(-1);
  
  cvReleaseMat(&mapx_persp);
  cvReleaseMat(&mapy_persp);
    
  return 0;
}


