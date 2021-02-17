# AutoCornerFinder

This is a modification of autoConerFinder tool, originally provided by Davide Scaramuzza in his [toolbox](https://sites.google.com/site/scarabotix/ocamcalib-toolbox/ocamcalib-toolbox-download-page). Several changes have been made in order to adapt to OpenCV's upgrade. The modification is guaranteed to compile and (*most importantly!*) work with [opencv-3.4.12](https://opencv.org/releases/).

#### Compilation
Before compilation, install opencv-3.4.12 and replace in the beginning of the Makfile: 
> CV_INC_PATH=your_opencv_include_path
> 
> CV_LINK_PATH=your_opencv_link_path

then run `make`.

#### Usage
This code is sopposed to work with Scaramuzza's Matlab ToolBox, which is also quite aged. To run this program without Matkab, just put the picture path in `pictures.txt`, line-by-line, then run.

#### Results
The results will be found in `./cToMatlab` directory.
