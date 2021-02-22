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
  /* Print ocam_model parameters                                         */
  /* --------------------------------------------------------------------*/  
  int i;
  printf("pol =\n");    
  for (i=0; i<o.length_pol; i++){
      printf("\t%e\n",o.pol[i]); 
  };    
  printf("\n");
  printf("invpol =\n"); 
  for (i=0; i<o.length_invpol; i++){
      printf("\t%e\n",o.invpol[i]);
  }; 
  printf("\n");  
  printf("\nxc = %f\nyc = %f\n\nwidth = %d\nheight = %d\n",o.xc,o.yc,o.width,o.height);
  
  return 0;
}
