#include "stdafx.h"
#include "TwoPass.h"


/***************************************************************************************
Function:  区域生长算法
Input:     src 待处理原图像 pt 初始生长点 th 生长的阈值条件
Output:    实质区是白色，其他区域是黑色
Description: 生长结果区域标记为白色(255),背景色为黑色(0)
Return:    Mat
Others:    NULL
***************************************************************************************/
Mat RegionGrow(Mat src, Point2i pt, int th)
{
	Point2i ptGrowing;						//待生长点位置
	int nGrowLable = 0;								//标记是否生长过
	int nSrcValue = 0;								//生长起点灰度值
	int nCurValue = 0;								//当前生长点灰度值
	Mat matDst = Mat::zeros(src.size(), CV_8UC1);	//创建一个空白区域，填充为黑色
	//生长方向顺序数据
	int DIR[8][2] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 } };
	vector<Point2i> vcGrowPt;						//生长点栈
	vcGrowPt.push_back(pt);							//将生长点压入栈中
	matDst.at<uchar>(pt.y, pt.x) = 255;				//标记生长点
	nSrcValue = src.at<uchar>(pt.y, pt.x);			//记录生长点的灰度值

	while (!vcGrowPt.empty())						//生长栈不为空则生长
	{
		pt = vcGrowPt.back();						//取出一个生长点
		vcGrowPt.pop_back();

		//分别对八个方向上的点进行生长
		for (int i = 0; i<9; ++i)
		{
			ptGrowing.x = pt.x + DIR[i][0];
			ptGrowing.y = pt.y + DIR[i][1];
			//检查是否是边缘点
			if (ptGrowing.x < 0 || ptGrowing.y < 0 || ptGrowing.x >(src.cols - 1) || (ptGrowing.y > src.rows - 1))
				continue;

			nGrowLable = matDst.at<uchar>(ptGrowing.y, ptGrowing.x);		//当前待生长点的灰度值

			if (nGrowLable == 0)					//如果标记点还没有被生长
			{
				nCurValue = src.at<uchar>(ptGrowing.y, ptGrowing.x);
				if (abs(nSrcValue - nCurValue) < th)					//在阈值范围内则生长
				{
					matDst.at<uchar>(ptGrowing.y, ptGrowing.x) = 255;		//标记为白色
					vcGrowPt.push_back(ptGrowing);					//将下一个生长点压入栈中
				}
			}
		}
	}
	return matDst.clone();
}

const int max_size = 1000;
int parent[max_size] = { 0 };

// 找到label x的根节点
int find(int x, int parent[]){
	assert(x < max_size);
	int i = x;
	while (0 != parent[i])
		i = parent[i];
	return i;
}

// 将label x 和 label y合并到同一个连通域
void union_label(int x, int y, int parent[]){
	assert(x < max_size && y < max_size);
	int i = x;
	int j = y;
	while (0 != parent[i])
		i = parent[i];
	while (0 != parent[j])
		j = parent[j];
	if (i != j)
		parent[i] = j;
}


void myConnectedComponentLabelingTwoPass(Mat& binary, Mat& label)
{
	CV_Assert(binary.data);
	CV_Assert(binary.depth() == CV_8U);
	CV_Assert(binary.channels() == 1);

	// create label image
	label.create(binary.size(), binary.type());

	// initialize label value
	int label_value = 0;

	// first pass
	for (int y = 1; y < binary.rows; y++)
	{
		uchar* p_binary = binary.ptr<uchar>(y);
		uchar* p_binary_up = binary.ptr<uchar>(y - 1);
		uchar* p_label = label.ptr<uchar>(y);
		uchar* p_label_up = label.ptr<uchar>(y - 1);
		for (int x = 0; x < label.cols; x++)
		{
			p_label[x] = 0;
		}
		for (int x = 1; x < binary.cols; x++)
		{
			if (255 == p_binary[x])
			{
				if (0 == p_binary[x - 1] && 0 == p_binary_up[x])
				{
					label_value += 1;
					p_label[x] = (int)label_value;
				}
				else if (255 == p_binary[x - 1] && 0 == p_binary_up[x])
					p_label[x] = p_label[x - 1];
				else if (0 == p_binary[x - 1] && 255 == p_binary_up[x])
					p_label[x] = p_label_up[x];
				else if (255 == p_binary[x - 1] && 255 == p_binary_up[x])
				{
					p_label[x] = p_label[x - 1];
					union_label(p_label[x - 1], p_label_up[x], parent);
				}
				else
				{
					cout << " p_binary[x-1] = " << (int)p_binary[x - 1] <<
						" p_binary_up[x] = " << (int)p_binary_up[x] << endl;
				}
			}
		}
	}

	// second pass
	for (int y = 1; y < binary.rows - 1; y++)
	{
		uchar* p_label = label.ptr<uchar>(y);
		for (int x = 1; x < binary.cols - 1; x++)
		{
			if (p_label[x] > 0)
			{
				p_label[x] = find(p_label[x], parent);
			}
		}
	}

}

//  Connected Component Analysis/Labeling -- Color Labeling 
//  Author:  www.icvpr.com  
//  Blog  :  http://blog.csdn.net/icvpr 
cv::Scalar icvprGetRandomColor()
{
	uchar r = (uchar)255 * (rand() / (1.0 + RAND_MAX));
	uchar g = (uchar)255 * (rand() / (1.0 + RAND_MAX));
	uchar b = (uchar)255 * (rand() / (1.0 + RAND_MAX));
	return cv::Scalar(b, g, r);
}

void icvprLabelColor(const cv::Mat& _labelImg, cv::Mat& _colorLabelImg)
{
	if (_labelImg.empty() ||
		_labelImg.type() != CV_8UC1)
	{
		return;
	}

	std::map<int, cv::Scalar> colors;

	int rows = _labelImg.rows;
	int cols = _labelImg.cols;

	_colorLabelImg.release();
	_colorLabelImg.create(rows, cols, CV_8UC3);
	_colorLabelImg = cv::Scalar::all(0);

	for (int i = 0; i < rows; i++)
	{
		uchar* data_src = (uchar*)_labelImg.ptr<uchar>(i);
		uchar* data_dst = _colorLabelImg.ptr<uchar>(i);
		for (int j = 0; j < cols; j++)
		{
			int pixelValue = data_src[j];
			if (pixelValue > 0)
			{
				if (colors.count(pixelValue) <= 0)
				{
					colors[pixelValue] = icvprGetRandomColor();
				}
				cv::Scalar color = colors[pixelValue];
				*data_dst++ = (uchar)color[0];
				*data_dst++ = (uchar)color[1];
				*data_dst++ = (uchar)color[2];
			}
			else
			{
				data_dst++;
				data_dst++;
				data_dst++;
			}
		}
	}
}
void icvprLabelColor2(const cv::Mat& _labelImg, cv::Mat& _colorLabelImg)
{
	if (_labelImg.empty() ||
		_labelImg.type() != CV_32SC1)
	{
		return ;
	}

	std::map<int, cv::Scalar> colors ;

	int rows = _labelImg.rows ;
	int cols = _labelImg.cols ;

	_colorLabelImg.release() ;
	_colorLabelImg.create(rows, cols, CV_8UC3) ;
	_colorLabelImg = cv::Scalar::all(0) ;

	for (int i = 0; i < rows; i++)
	{
		const int* data_src = (int*)_labelImg.ptr<int>(i) ;
		uchar* data_dst = _colorLabelImg.ptr<uchar>(i) ;
		for (int j = 0; j < cols; j++)
		{
			int pixelValue = data_src[j] ;
			if (pixelValue > 1)
			{
				if (colors.count(pixelValue) <= 0)
				{
					colors[pixelValue] = icvprGetRandomColor() ;
				}
				cv::Scalar color = colors[pixelValue] ;
				*data_dst++ = (uchar)color[0];
				*data_dst++ = (uchar)color[1];
				*data_dst++ = (uchar)color[2];
			}
			else
			{
				data_dst++;
				data_dst++;
				data_dst++;
			}
		}
	}
}
#if 0
int main(){
	// load image
	char* imageName = "binary.png";
	Mat image;
	image = imread(imageName, 1);
	if (!image.data){
		cout << "No image data" << endl;
		getchar();
		return -1;
	}

	// convert to gray
	cvtColor(image, image, CV_RGB2GRAY);// convert 3-channel image to 1-channel image

	// threshold
	Mat thresh;
	threshold(image, thresh, 100, 255, THRESH_BINARY);

	// label
	Mat label;
	myConnectedComponentLabelingTwoPass(thresh, label);

	// color labeling
	Mat color_label;
	icvprLabelColor(label, color_label);

	// show
	imshow("image", image);
	imshow("thresh", thresh);
	imshow("label", label);
	imshow("color_label", color_label);
	waitKey(0);
}
#endif


void cvMaxLable(IplImage* imgin, IplImage* imgout)
{
	int x, y;
	for (y = 0; y < imgin->height; y++)
	{
		uchar* ptr = (uchar*)(imgin->imageData + y * imgin->widthStep);
		for (x = 0; x < imgin->width; x++)
		{
			if (x == 0 || y == 0 || x == imgin->width - 1)
				ptr[x] = 0;
		}
	}
	twopass((unsigned char *)imgin->imageData, (unsigned char *)imgout->imageData, imgin->width, imgout->height, imgin->widthStep);
}
#define BLACK 0
#define WHITE 255
void twopass(unsigned char* image, unsigned char* imageget, int X_SIZE, int Y_SIZE, int X_STEP)
{
	int i, j, cnt = 0;
	int max = 0, sec = 0;
	int lmax;
	int lsec;
	int label[40960] = { 0 };
	int labelcnt[40960] = { 0 };
	int lcnt[40960] = { 0 };
	int *plabel = label;
	int num, numt;
	int l = 0;
	unsigned char a, b, c, d, e;
	//	int **pimage;int *imageout;
	//	int *pimage[Y_SIZE][X_SIZE];  
	//  int imageout[Y_SIZE][X_SIZE];
	//	*(image + i*Y_SIZE + j) 
	int **imageout;
	int ***pimage;
	imageout = (int **)malloc(Y_SIZE*sizeof(int));

	pimage = (int ***)malloc(Y_SIZE*sizeof(int *));
	for (i = 0; i<Y_SIZE; i++)
	{
		imageout[i] = (int *)malloc(X_SIZE*sizeof(int));
		pimage[i] = (int **)malloc(X_SIZE*sizeof(int *));
	}

	for (i = 1; i<Y_SIZE; i++)
		for (j = 1; j<X_SIZE - 1; j++)
		{
			if ((i == 1) && (j == 1))
			{
				pimage[i - 1][j - 1] = NULL;
				pimage[i - 1][j] = NULL;
				pimage[i - 1][j + 1] = NULL;
				pimage[i][j - 1] = NULL;
			}
			else if ((i == 1) && (j != 1) && (j != X_SIZE - 2))
			{
				pimage[i - 1][j + 1] = NULL;
			}
			else if ((i == 1) && (j == X_SIZE - 2))
			{
				pimage[i - 1][j + 1] = NULL;
				pimage[i][j + 1] = NULL;
			}
			else if ((i != 1) && (j == 1))
			{
				pimage[i][j - 1] = NULL;
			}
			else if ((i != 1) && (j == X_SIZE - 2))
			{
				pimage[i][j + 1] = NULL;
			}

			if (*(image + i*X_STEP + j) == BLACK)
			{
				pimage[i][j] = NULL;
			}
			else
			{
				a = *(image + (i - 1)*X_STEP + j - 1);
				b = *(image + (i - 1)*X_STEP + j);
				c = *(image + (i - 1)*X_STEP + j + 1);
				d = *(image + i*X_STEP + j - 1);
				e = *(image + i*X_STEP + j);

				if ((a == 0) && (b == 0) && (c == 0) && (d == 0))
				{
					plabel++; cnt++;
					pimage[i][j] = plabel;
				}//0  
				else if ((a == 0) && (b == 0) && (c != 0) && (d == 0))
				{
					pimage[i][j] = pimage[i - 1][j + 1];
				}//3  
				else if ((a == 0) && (b == 0) && (c == 0) && (d != 0))
				{
					pimage[i][j] = pimage[i][j - 1];
				}//4  
				else if ((a != 0) && (b == 0) && (c != 0))
				{
					pimage[i][j] = pimage[i - 1][j - 1];

					numt = *(pimage[i - 1][j - 1]);
					if (numt == 0)
					{
						numt = (((int)pimage[i - 1][j - 1] - (int)label) / sizeof(int));
					}
					else
					{
						while (1)
						{
							if (label[numt] == 0) break;
							else numt = label[numt];
						}
					}

					num = *(pimage[i - 1][j + 1]);
					if (num == 0)
					{
						num = (((int)pimage[i - 1][j + 1] - (int)label) / sizeof(int));
					}
					else
					{
						while (1)
						{
							if (label[num] == 0) break;
							else num = label[num];
						}
					}

					if (numt != num) label[num] = numt;
				}//6  
				else if ((a == 0) && (b != 0))
				{
					pimage[i][j] = pimage[i - 1][j];
				}//8  
				else if ((a == 0) && (b == 0) && (c != 0) && (d != 0))
				{
					pimage[i][j] = pimage[i - 1][j + 1];

					numt = *(pimage[i][j - 1]);
					if (numt == 0)
					{
						numt = (((int)pimage[i][j - 1] - (int)label) / sizeof(int));
					}
					else
					{
						while (1)
						{
							if (label[numt] == 0) break;
							else numt = label[numt];
						}
					}

					num = *(pimage[i - 1][j + 1]);
					if (num == 0)
					{
						num = (((int)pimage[i - 1][j + 1] - (int)label) / sizeof(int));
					}
					else
					{
						while (1)
						{
							if (label[num] == 0) break;
							else num = label[num];
						}
					}

					if (numt != num) label[num] = numt;
				}//10  
				else
				{
					pimage[i][j] = pimage[i - 1][j - 1];
				}//15

			}
		}

	for (i = 0; i<Y_SIZE; i++)
	{
		for (j = 0; j<X_SIZE; j++)
		{
			if (pimage[i][j] != NULL)
			{
				num = *(pimage[i][j]);
				if (num == 0)
				{
					num = (((int)pimage[i][j] - (int)label) / sizeof(int));
				}
				else
				{
					while (1)
					{
						if (label[num] == 0) break;
						else num = label[num];
					}
				}
				if (labelcnt[num] == 0)
				{
					l++;
					labelcnt[num] = l;
				}
				imageout[i][j] = labelcnt[num];
				lcnt[labelcnt[num]]++;
			}
			else imageout[i][j] = 0;
		}
	}

	for (i = 0; i<l + 1; i++)
	{
		if (lcnt[i]>max)
		{
			max = lcnt[i];
			lmax = i;
		}
	}

	for (i = 0; i<l; i++)
		if ((lcnt[i]>sec) && (lcnt[i]<max))
		{
			sec = lcnt[i];
			lsec = i;
		}


	for (i = 0; i<Y_SIZE; i++)
		for (j = 0; j<X_SIZE; j++)
		{
			if (imageout[i][j] == lmax) *(imageget + i*X_STEP + j) = WHITE;
			else *(imageget + i*X_STEP + j) = BLACK;
		}
	/*	int **imageout;
	int ***pimage;
	imageout = (int **)malloc(Y_SIZE*sizeof(int));

	pimage = (int ***)malloc(Y_SIZE*sizeof(int *));
	for(i=0;i<Y_SIZE;i++)
	{
	imageout[i] = (int *)malloc(X_SIZE*sizeof(int));
	pimage[i] = (int **)malloc(X_SIZE*sizeof(int *));
	}*/
	for (i = 0; i<Y_SIZE; i++)
	{
		free(imageout[i]);
		free(pimage[i]);
	}
	free(imageout);
	free(pimage);

}

void icvprCcaByTwoPass(const cv::Mat& _binImg, cv::Mat& _lableImg)
{
	// connected component analysis (4-component)
	// use two-pass algorithm
	// 1. first pass: label each foreground pixel with a label
	// 2. second pass: visit each labeled pixel and merge neighbor labels
	// 
	// foreground pixel: _binImg(x,y) = 1
	// background pixel: _binImg(x,y) = 0


	if (_binImg.empty() ||
		_binImg.type() != CV_8UC1)
	{
		return;
	}

	// 1. first pass

	_lableImg.release();
	_binImg.convertTo(_lableImg, CV_32SC1);

	int label = 1;  // start by 2
	std::vector<int> labelSet;
	labelSet.push_back(0);   // background: 0
	labelSet.push_back(1);   // foreground: 1

	int rows = _binImg.rows - 1;
	int cols = _binImg.cols - 1;
	for (int i = 1; i < rows; i++)
	{
		int* data_preRow = _lableImg.ptr<int>(i - 1);
		int* data_curRow = _lableImg.ptr<int>(i);
		for (int j = 1; j < cols; j++)
		{
			if (data_curRow[j] == 1)
			{
				std::vector<int> neighborLabels;
				neighborLabels.reserve(2);
				int leftPixel = data_curRow[j - 1];
				int upPixel = data_preRow[j];
				if (leftPixel > 1)
				{
					neighborLabels.push_back(leftPixel);
				}
				if (upPixel > 1)
				{
					neighborLabels.push_back(upPixel);
				}

				if (neighborLabels.empty())
				{
					labelSet.push_back(++label);  // assign to a new label
					data_curRow[j] = label;
					labelSet[label] = label;
				}
				else
				{
					std::sort(neighborLabels.begin(), neighborLabels.end());
					int smallestLabel = neighborLabels[0];
					data_curRow[j] = smallestLabel;

					// save equivalence
					for (size_t k = 1; k < neighborLabels.size(); k++)
					{
						int tempLabel = neighborLabels[k];
						int& oldSmallestLabel = labelSet[tempLabel];
						if (oldSmallestLabel > smallestLabel)
						{
							labelSet[oldSmallestLabel] = smallestLabel;
							oldSmallestLabel = smallestLabel;
						}
						else if (oldSmallestLabel < smallestLabel)
						{
							labelSet[smallestLabel] = oldSmallestLabel;
						}
					}
				}
			}
		}
	}

	// update equivalent labels
	// assigned with the smallest label in each equivalent label set
	for (size_t i = 2; i < labelSet.size(); i++)
	{
		int curLabel = labelSet[i];
		int preLabel = labelSet[curLabel];
		while (preLabel != curLabel)
		{
			curLabel = preLabel;
			preLabel = labelSet[preLabel];
		}
		labelSet[i] = curLabel;
	}


	// 2. second pass
	for (int i = 0; i < rows; i++)
	{
		int* data = _lableImg.ptr<int>(i);
		for (int j = 0; j < cols; j++)
		{
			int& pixelLabel = data[j];
			pixelLabel = labelSet[pixelLabel];
		}
	}

	for (int i = 0; i < _binImg.rows; i++)
	{
		for (int j = 0; j < _binImg.cols; j++)
		{
			if (_binImg.at<uchar>(i, j) == 255)
			{
				cout << i << " " << j << " " << (int)(_lableImg.at<double>(i, j)*255) << endl;
			}
		}
	}
}

void contoursLabelColor(vector<vector<Point> > contours, Mat& _colorLabelImg,Mat& temp)
{
	std::map<int, cv::Scalar> colors;
	for (int i = 0; i < contours.size(); i++)
	{
		int pixelValue = contours[i].size();
		if (pixelValue > 0)
		{
			if (colors.count(pixelValue) <= 0)
			{
				colors[pixelValue] = icvprGetRandomColor();
			}
			cv::Scalar color = colors[pixelValue];
			for (int j = 0; j < contours[i].size(); j++)
			{
				_colorLabelImg.at<Vec3b>(contours[i][j].y, contours[i][j].x)[0] = (uchar)color[0];
				_colorLabelImg.at<Vec3b>(contours[i][j].y, contours[i][j].x)[1] = (uchar)color[1];
				_colorLabelImg.at<Vec3b>(contours[i][j].y, contours[i][j].x)[2] = (uchar)color[2];
				temp.at<uchar>(contours[i][j].y, contours[i][j].x) = pixelValue;
			}
		}
	}
}

void gauPointsLabelColor(vector<vector<Point> > contours, Mat& _colorLabelImg)
{
	std::map<int, cv::Scalar> colors;
	for (int i = 0; i < contours.size()-1; i++)
	{
		int pixelValue = contours[i].size();
		if (pixelValue > 0)
		{
			if (colors.count(pixelValue) <= 0)
			{
				colors[pixelValue] = icvprGetRandomColor();
			}
			cv::Scalar color = colors[pixelValue];
			for (int j = 0; j < contours[i].size(); j++)
			{
				circle(_colorLabelImg, Point(contours[i][j].x, contours[i][j].y), 2, color, 2);
				//_colorLabelImg.at<Vec3b>(contours[i][j].y, contours[i][j].x)[0] = (uchar)color[0];
				//_colorLabelImg.at<Vec3b>(contours[i][j].y, contours[i][j].x)[1] = (uchar)color[1];
				//_colorLabelImg.at<Vec3b>(contours[i][j].y, contours[i][j].x)[2] = (uchar)color[2];
			}
		}
	}
}

void selectGaugePoints(vector<vector<Point> > contours, Mat temp, vector<vector<Point> >& GauPoints)
{
	//cout << contours.size() << endl;
	for (int i = 0; i < contours.size(); i++)
	{
		vector<Point> labelGau;
		for (int j = 0; j < contours[i].size(); j++)
		{
			int pixelValue = (int)temp.at<uchar>(contours[i][j].y, contours[i][j].x);
			if (pixelValue != 0)
			{
				int num = 0;
				/*if ((int)temp.at<uchar>(contours[i][j].y - 1, contours[i][j].x) == pixelValue)
				{
					num++;
				}
				if ((int)temp.at<uchar>(contours[i][j].y, contours[i][j].x-1) == pixelValue)
				{
					num++;
				}
				if ((int)temp.at<uchar>(contours[i][j].y+1, contours[i][j].x) == pixelValue)
				{
					num++;
				}
				if ((int)temp.at<uchar>(contours[i][j].y, contours[i][j].x+1) == pixelValue)
				{
					num++;
				}*/
				
				for (int m = contours[i][j].y - 1; m <= contours[i][j].y + 1; m++)
				{
					for (int n = contours[i][j].x - 1; n <= contours[i][j].x + 1; n++)
					{
						if (m == contours[i][j].y && n == contours[i][j].x)
						{

						}
						else
						{
							if ((int)temp.at<uchar>(m, n) == pixelValue)
							{
								num++;
							}
						}
					}
				}
				if (num==1||num == 3)
				{
					//cout << num << endl;
					labelGau.push_back(contours[i][j]);
				}
			}
		}
		GauPoints.push_back(labelGau);
	}
}
