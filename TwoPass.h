#ifndef TWOPASS_H
#define TWOPASS_H

#include <iostream>
#include <vector>
#include <map>

//#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;

Mat RegionGrow(Mat src, Point2i pt, int th);//ÇøÓòÉú³¤

void myConnectedComponentLabelingTwoPass(Mat& binary, Mat& label);//TwoPass
void icvprLabelColor(const cv::Mat& _labelImg, cv::Mat& _colorLabelImg);

void twopass(unsigned char* image, unsigned char* imageget, int X_SIZE, int Y_SIZE, int X_STEP);
void cvMaxLable(IplImage* imgin, IplImage* imgout);

void icvprCcaByTwoPass(const cv::Mat& _binImg, cv::Mat& _lableImg);
void icvprLabelColor2(const cv::Mat& _labelImg, cv::Mat& _colorLabelImg);

void contoursLabelColor(vector<vector<Point> > contours, Mat& _colorLabelImg, Mat& temp);
void gauPointsLabelColor(vector<vector<Point> > contours, Mat& _colorLabelImg);

void selectGaugePoints(vector<vector<Point> > contours, Mat temp, vector<vector<Point> >& GauPoints);

cv::Scalar icvprGetRandomColor();
#endif