#include "stdafx.h"
#include "myRandom.h"

//#pragma comment(lib,"random_walker.lib")  
int selectforeground(Mat src, Mat& dst)
{
	src.copyTo(dst);
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (src.at<uchar>(i, j) != 255)
				dst.at<uchar>(i, j) = 0;
		}
	}
	return 1;
}

void randomWalker(Mat src, Mat foreground, vector<vector<Point> > GauPoints, Mat& dst)
{
	Mat gray;
	if (src.channels() == 3)
		cvtColor(src, gray, CV_BGR2GRAY);
	else
		src.copyTo(gray);
	int X = src.rows;
	int Y = src.cols;
	int numSeeds = 0;

	for (int i = 0; i < GauPoints.size(); i++)
		numSeeds += GauPoints[i].size();

	int numLabels = GauPoints.size();

	//cout << numSeeds << " " << numLabels << endl;
	//Point p1(130, 150), p2(200, 200), p3(50, 50);

	if (numSeeds > 0 && numLabels>1)
	{
		Mat doubleImgae;
		Mat grayT = gray.t();
		//numSeeds += 4;
		//numLabels += 1;
		Point p0[4] = { Point(10, 10), Point(Y - 10, X - 10), Point(10, X - 10), Point(Y - 10, 10) };
		mwArray nwImg(X, Y, mxDOUBLE_CLASS);
		mwArray nwSeeds(1, numSeeds, mxDOUBLE_CLASS);
		mwArray nwLabels(1, numSeeds, mxDOUBLE_CLASS);
		mwArray nwMask(Y, X, mxDOUBLE_CLASS);
		mwSize dims[3] = { X, Y, numLabels };
		mwArray nwProbabilities(3, dims, mxDOUBLE_CLASS);
		mwArray nwBeta(1, 1, mxDOUBLE_CLASS);


		//double seeds[3] = { 33174, 51144, 12594 };
		//double labels[3] = { 1, 2, 3 };

		double* seeds = new double[numSeeds];
		double* labels = new double[numSeeds];

		int count = 0;

		for (int i = 0; i < GauPoints.size() - 1; i++)
		{
			for (int j = 0; j < GauPoints[i].size(); j++)
			{
				seeds[count] = (GauPoints[i][j].x - 1)*X + GauPoints[i][j].y;
				labels[count] = i + 2;
				count++;
			}
		}
		//cout << GauPoints[GauPoints.size() - 1].size() << endl;
		for (int i = 0; i < GauPoints[GauPoints.size() - 1].size(); i++)
		{
			seeds[count] = (GauPoints[GauPoints.size() - 1][i].x - 1)*X + GauPoints[GauPoints.size() - 1][i].y;// (p0[i].x - 1)*X + p0[i].y;
			//cout << GauPoints[GauPoints.size() - 1][i].x << " " << GauPoints[GauPoints.size() - 1][i].y << endl;
			labels[count] = 1;
			count++;
		}

		double beta = 90;
		nwSeeds.SetData(seeds, numSeeds);
		nwLabels.SetData(labels, numSeeds);
		nwImg.SetData(grayT.data, src.rows*src.cols);
		nwBeta.SetData(&beta, 1);


		random_walker(1, nwMask, nwProbabilities, nwImg, nwSeeds, nwLabels, nwBeta);

		int numMask = nwMask.NumberOfDimensions();

		Mat result(Size(src.size()), CV_64FC1, Scalar(0));
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols - 1; j++)
			{
				double x = nwMask.Get(1, i + 1 + j*src.rows);
				result.at<double>(i, j) = x;
			}
		}
		Mat resultImage(Size(src.size()), CV_8UC3, Scalar(0, 0, 0));

		std::map<int, cv::Scalar> colors;
		for (int i = 1; i < numLabels; i++)
		{
			int pixelValue = i;
			if (pixelValue > 0)
			{
				if (colors.count(pixelValue) <= 0)
				{
					colors[pixelValue] = icvprGetRandomColor();
				}
			}
		}
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols - 1; j++)
			{
				if ((int)result.at<double>(i, j) != numLabels)
				{
					if (foreground.at<uchar>(i, j) == 255)
					{
						resultImage.at<Vec3b>(i, j)[0] = colors[(int)result.at<double>(i, j)][0];
						resultImage.at<Vec3b>(i, j)[1] = colors[(int)result.at<double>(i, j)][1];
						resultImage.at<Vec3b>(i, j)[2] = colors[(int)result.at<double>(i, j)][2];
					}
					else
					{
						resultImage.at<Vec3b>(i, j)[0] = 0; //colors[(int)result.at<double>(i, j)][0];
						resultImage.at<Vec3b>(i, j)[1] = 0; //colors[(int)result.at<double>(i, j)][1];
						resultImage.at<Vec3b>(i, j)[2] = 0; //colors[(int)result.at<double>(i, j)][2];
					}
				}
				else
				{
					resultImage.at<Vec3b>(i, j)[0] = 0; //colors[(int)result.at<double>(i, j)][0];
					resultImage.at<Vec3b>(i, j)[1] = 0; //colors[(int)result.at<double>(i, j)][1];
					resultImage.at<Vec3b>(i, j)[2] = 0; //colors[(int)result.at<double>(i, j)][2];
				}
			}
		}

		//imshow("resultImage", resultImage);
		//waitKey(0);
		resultImage.copyTo(dst);
	}
}



bool findPoint(vector<vector<Point> > Pointss, Point point, int m, int n)
{
	bool ifFind = false;
	for (int i = 0; i < Pointss.size(); i++)
	{
		if (ifFind == true)
			break;
		for (int j = 0; j < Pointss[i].size(); j++)
		{
			if (m != i || n != j)
			{
				if (point.x == Pointss[i][j].x&&point.y == Pointss[i][j].y)
				{
					ifFind = true;
					break;
				}
			}
		}
	}
	return ifFind;
}