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

int main(int argc, char *argv[])
{   
  /* --------------------------------------------------------------------*/
  /* Read the parameters of the omnidirectional camera from the TXT file */
  /* --------------------------------------------------------------------*/
  struct ocam_model o, o_cata; // our ocam_models for the fisheye and catadioptric cameras
  get_ocam_model(&o, "./calib_results_fisheye.txt");
  get_ocam_model(&o_cata, "./calib_results_catadioptric.txt");    

  /* --------------------------------------------------------------------*/  
  /* Allocate space for the unistorted images                            */
  /* --------------------------------------------------------------------*/  
  IplImage *src1         = cvLoadImage("./test_fisheye.jpg");      // source image 1
  
  CvSize size_persp_image = cvSize(cvGetSize(src1).width, cvGetSize(src1).height);
  IplImage *dst_persp    = cvCreateImage( size_persp_image, 8, 3 );   // undistorted perspective and panoramic image
  
  CvMat* mapx_persp = cvCreateMat(size_persp_image.height, size_persp_image.width, CV_32FC1);
  CvMat* mapy_persp = cvCreateMat(size_persp_image.height, size_persp_image.width, CV_32FC1);
  
  /* --------------------------------------------------------------------  */  
  /* Create Look-Up-Table for perspective undistortion                     */
  /* SF is kind of distance from the undistorted image to the camera       */
  /* (it is not meters, it is just a zoom fator)                           */
  /* Try to change SF to see how it affects the result                     */
  /* The undistortion is done on a  plane perpendicular to the camera axis */
  /* --------------------------------------------------------------------  */
  float sf = readsf();
  
  create_perspecive_undistortion_LUT( mapx_persp, mapy_persp, &o, sf );
  
  /* --------------------------------------------------------------------*/  
  /* Undistort using specified interpolation method                      */
  /* Other possible values are (see OpenCV doc):                         */
  /* CV_INTER_NN - nearest-neighbor interpolation,                       */
  /* CV_INTER_LINEAR - bilinear interpolation (used by default)          */
  /* CV_INTER_AREA - resampling using pixel area relation. It is the preferred method for image decimation that gives moire-free results. In case of zooming it is similar to CV_INTER_NN method. */
  /* CV_INTER_CUBIC - bicubic interpolation.                             */
  /* --------------------------------------------------------------------*/
  cvRemap( src1, dst_persp, mapx_persp, mapy_persp, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0) );
  
  std::cout << "Saving Image..." << std::endl;
  
  cv::Mat m = cv::cvarrToMat(dst_persp);
  cv::imwrite("undistorted_perspective.jpg", m);
  //cvSaveImage("undistorted_perspective.jpg",dst_persp);
  printf("Image %s saved\n", "undistorted_perspective.jpg");

  /* --------------------------------------------------------------------*/    
  /* Free memory                                                         */
  /* --------------------------------------------------------------------*/  
  cvReleaseImage(&src1);
  cvReleaseImage(&dst_persp);
  cvReleaseMat(&mapx_persp);
  cvReleaseMat(&mapy_persp);

  return 0;
}
