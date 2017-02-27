#ifndef _MY_RANDOM
#define _MY_RANDOM

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

#include "mclmcr.h"  
#include "matrix.h"  
#include "mclcppclass.h"  
#include "random_walker.h"


#include "thin.h"
#include "TwoPass.h"

#include "package_tracking/BlobTracking.h"
#include "package_bgs/PBAS/PixelBasedAdaptiveSegmenter.h"

using namespace std;
using namespace cv;

bool findPoint(vector<vector<Point> > Pointss, Point point, int m, int n);
void randomWalker(Mat src, Mat foreground, vector<vector<Point> > GauPoints, Mat& dst);
int selectforeground(Mat src, Mat& dst);
#endif