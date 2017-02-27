#ifndef THIN_H
#define THIN_H

#include <iostream>
#include <vector>
#include <map>

//#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;

void thinningIteration(cv::Mat& img, int iter);
void thinning(const cv::Mat& src, cv::Mat& dst);

void chao_thinimage(Mat &srcimage);//单通道、二值化后的图像



#endif