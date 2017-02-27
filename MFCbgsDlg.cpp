
// MFCbgsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCbgs.h"
#include "MFCbgsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma comment(lib,"random_walker.lib")  
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

int SizeH = 0;					//图片高度
int SizeW = 0;					//图片宽度

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCbgsDlg 对话框



CMFCbgsDlg::CMFCbgsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCbgsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCbgsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_algorithm);
	DDX_Control(pDX, IDC_LOG, m_log);
}

BEGIN_MESSAGE_MAP(CMFCbgsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTONOPENVIDEO, &CMFCbgsDlg::OnBnClickedButtonopenvideo)
	ON_BN_CLICKED(IDC_BUTTONSTART, &CMFCbgsDlg::OnBnClickedButtonstart)
	ON_BN_CLICKED(IDC_BUTTONSTOP, &CMFCbgsDlg::OnBnClickedButtonstop)
END_MESSAGE_MAP()


// CMFCbgsDlg 消息处理程序

BOOL CMFCbgsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	frameNum = 0;
	minThinSize = 20;
	dilateSize = 35;
	minCanny = 60, maxCanny = 120;
	areaThres = 0.005;
	updateBackgroundImageRate = 0.01;

	if (!random_walkerInitialize())
	{
		MessageBox(L"random_walker算法初始错误！");
		//exit(0);
	}

	m_algorithm.AddString(L"PBAS");
	m_algorithm.AddString(L"混合高斯模型");

	m_algorithm.SetCurSel(0); //设置第nIndex项为显示的内容

	((CButton*)GetDlgItem(IDC_CHECKBACK))->SetCheck(TRUE);

	CEdit* pBoxOne;
	pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT2);
	//赋值
	pBoxOne-> SetWindowText( _T("0.005 ") );

	started = false;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCbgsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCbgsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCbgsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int CMFCbgsDlg::showImage(Mat image, UINT ID)
{
	if (image.empty())
		return 0;
	CDC* pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	SetRect(rect, rect.left, rect.top, rect.right, rect.bottom);

	IplImage img = image;
	CvvImage cimg;
	cimg.CopyOf(&img);
	cimg.DrawToHDC(hDC, &rect);
	ReleaseDC(pDC);
	return 1;
}

string CStringToString(CString cstr)
{
	string str;
	setlocale(LC_ALL, "chs");
	wchar_t wch[255];
	char temp[255];
	wcscpy(wch, cstr.GetString());
	wcstombs(temp, wch, 254);
	str.append(temp);
	return str;

}
void CMFCbgsDlg::OnBnClickedButtonopenvideo()
{
	// TODO:  在此添加控件通知处理程序代码
	BOOL isOpen = TRUE;     //是否打开(否则为保存)  
	CString defaultDir = L"E:\\";   //默认打开的文件路径  
	CString fileName = L"";         //默认打开的文件名  
	CString filter = L"文件 (*.mp4; *.avi; *.wav; *.rmvb; *.mpeg)|*.mp4;*.avi;*.rmvb;*.mpeg||";   //文件过虑的类型
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	openFileDlg.GetOFN().lpstrInitialDir = L"E:\\FileTest\\raw.avi";
	openFileDlg.m_ofn.lpstrTitle = L"打开视频";
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = defaultDir + "\\raw.avi";
	if (result == IDOK)
	{
		filePath = openFileDlg.GetPathName();
		video.release();
		video.open(CStringToString(filePath));
		while (1)
		{
			video >> frame;
			if (!frame.empty())
				break;
		}
		showImage(frame, IDC_STATICPICTURE1);
		SizeH = frame.rows;
		SizeW = frame.cols;
	}
}
bool CMFCbgsDlg::getBgsMethodName()
{
	int nIndex = m_algorithm.GetCurSel();
	if (nIndex != CB_ERR)
	{
		m_algorithm.GetLBText(nIndex, methodName);
		return true;
	}
	else
	{
		AfxMessageBox(L"Please, select one background subtraction method!");
		return false;
	}

}

DWORD CMFCbgsDlg::Thread(LPVOID *lpvParam)
{
	CMFCbgsDlg *thr = (CMFCbgsDlg*)lpvParam;
	thr->ThreadProcess();
	return NULL;
}
void CMFCbgsDlg::ThreadProcess()
{
	if (methodName == "PBAS")
		bgs = new PixelBasedAdaptiveSegmenter;
	else if (methodName == "混合高斯模型")
		bg_model = createBackgroundSubtractorMOG2();

	CEdit* pBoxOne;
	pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT2);
	CString strArea;
	pBoxOne->GetWindowText(strArea);
	areaThres = _ttol(strArea);
	int frameNum = 0;

	blobTracking = new BlobTracking;
	vehicleCouting = new VehicleCouting;
	started = true;
	do
	{

		Mat randomWalkImage;
		Mat gauImg;
		Mat img_blob;
		Mat mask, foreground, thin1, thin2;
		Mat backGround;
		if (frameNum == 0)
		{
			if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECKBACK))
			{
				CString selectedPath;
				GetDlgItemText(IDC_MFCEDITBROWSE1, selectedPath);
				if (selectedPath == "")
				{
					AfxMessageBox(L"请选择预设背景");
				}
				frame = imread(CStringToString(selectedPath));
			}
			else
			{
				video >> frame;
				if (frame.empty())
					break;
			}
		}
		else
		{
			video >> frame;
			if (frame.empty())
				break;
		}
		if (frame.rows > 240)
		{
			resize(frame, frame, Size(frame.cols / (frame.cols / 240), 240));
		}
		if (methodName == "PBAS")
			bgs->process(frame, mask);
		else if (methodName == "混合高斯模型")
		{
			bg_model->apply(frame, mask, updateBackgroundImageRate);
			bg_model->getBackgroundImage(backGround);
		}
		if (!mask.empty())
		{
			//灰度化

			//中值滤波
			Mat medianMask;
			medianBlur(mask, medianMask, 9);

			if (methodName == "混合高斯模型")
			{
				// 膨胀
				Mat element = getStructuringElement(MORPH_RECT, Size(dilateSize, dilateSize));
				dilate(medianMask, foreground, cv::Mat());

				//canny提取边缘
				Mat cannyMask, cannyBackground;
				Canny(medianMask, cannyMask, minCanny, maxCanny);
				Canny(backGround, cannyBackground, minCanny, maxCanny);
			}

			else
			{
					medianMask.copyTo(foreground);
			}
			//去除阴影
			foreground = foreground;// &cannyImage;

			Mat foreground1;
			selectforeground(foreground, foreground1);


			//初步骨架提取
			//thinning(foreground, thin1);
			foreground.copyTo(thin1);
			chao_thinimage(thin1);

			//骨架二值化
		    cv::threshold(thin1, thin1, 128, 255, THRESH_BINARY);

			//连通骨架提取
			vector<vector<Point>> contours;
			cv::findContours(thin1, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

			//太短的连通骨架去除
			vector<vector<Point>>::iterator itr = contours.begin();
			while (itr != contours.end())
			{
				if (itr->size() <minThinSize)
				{
					itr = contours.erase(itr);
				}
				else
					itr++;
			}

			if (contours.size() > 0)
			{
				Mat labelImg(thin1.size(), CV_8UC3, Scalar(0, 0, 0));
				Mat temp(thin1.size(), CV_8U, Scalar(0));
				vector<vector<Point> > connectThins;
				for (int i = 0; i < contours.size(); i++)
				{
					vector<Point> connectThin;
					for (int j = 0; j < contours[i].size(); j++)
					{
						bool isFind = false;
						for (int m = 0; m < connectThin.size(); m++)
						{
							if (contours[i][j].x == connectThin[m].x&&contours[i][j].y == connectThin[m].y)
							{
								isFind = true;
								break;
							}
						}
						if (isFind == false)
							connectThin.push_back(contours[i][j]);
					}
					if (connectThin.size() > 0)
						connectThins.push_back(connectThin);
				}
				contoursLabelColor(connectThins, labelImg, temp);

				//选取骨架标记点
				vector<vector<Point> > GauPoints;
				selectGaugePoints(connectThins, temp, GauPoints);

				int countBack = 0;
				srand(time(0));
				vector<Point> backPoints;
				int x = 10;
				int y = 10;
				backPoints.push_back(Point(x, y));
				x = 10;
				y = frame.rows - 10;
				backPoints.push_back(Point(x, y));

				x = frame.cols - 10;
				y = 10;
				backPoints.push_back(Point(x, y));
				x = frame.cols - 10;
				y = frame.rows - 10;
				backPoints.push_back(Point(x, y));


				GauPoints.push_back(backPoints);

				//骨架标记点标色
				Mat gauImg;
				frame.copyTo(gauImg);
				gauPointsLabelColor(GauPoints, gauImg);

				//随机游走算法
				Mat randomWalkImage;

				randomWalker(frame, foreground, GauPoints, randomWalkImage);

				//提取轮廓
				vector<vector<Point>> resultContours;
				cv::findContours(foreground1, resultContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
				/* 对每个轮廓计算其凸包*/
				vector<vector<Point> >poly(resultContours.size());
				for (int i = 0; i < resultContours.size(); i++)
				{
					approxPolyDP(Mat(resultContours[i]), poly[i], 1, true);
				}

				//轮廓面积和周长滤波
				vector<vector<Point>> resultPolys;
				double areaFrame = frame.cols*frame.rows;
				for (int i = 0; i < (int)poly.size(); i++)
				{
					//cout << frame.cols*frame.rows<<" "<<i << "面积：  " << contourArea(poly[i]) << "周长：  " << arcLength(poly[i], true) << endl;
					if (contourArea(poly[i]) / areaFrame>areaThres)
					{
						resultPolys.push_back(poly[i]);
					}
				}

				Mat frameTrack;
				frame.copyTo(frameTrack);

				if (resultPolys.size() > 0)
					drawContours(frameTrack, resultPolys, -1, Scalar(255, 0, 0), 2);
				blobTracking->process(frameTrack, foreground1, img_blob);

				Mat out;
				img_blob.copyTo(out);
				vehicleCouting->setInput(img_blob);
				vehicleCouting->setTracks(blobTracking->getTracks());
				vehicleCouting->process(out);

				showImage(randomWalkImage, IDC_STATICPICTURE2);
				showImage(gauImg, IDC_STATICPICTURE3);
				showImage(out, IDC_STATICPICTURE4);
			}
			showImage(frame, IDC_STATICPICTURE1);
		}
		frameNum++;
		m_log.SetWindowTextW(L"Running...");
	} while (1);

	if (methodName == "PBAS")
	{
		delete bgs;
		bgs = NULL;
	}

	//AfxMessageBox(L"Thread is finished!");
	m_log.SetWindowTextW(L"Finished!");
	started = false;
}
void CMFCbgsDlg::OnBnClickedButtonstart()
{
	// TODO:  在此添加控件通知处理程序代码
	if (started == false)
	{
		m_log.SetWindowTextW(L"Checking...");

		if (!getBgsMethodName())
		{
			m_log.SetWindowTextW(L"Stopped...");
			return;
		}
		m_log.SetWindowTextW(L"Starting...");
		thread = AfxBeginThread((AFX_THREADPROC)CMFCbgsDlg::Thread, (LPVOID) this);
	}
	else
	{
		AfxMessageBox(L"Thread is already initialized!");
		return;
	}
}


void CMFCbgsDlg::OnBnClickedButtonstop()
{
	// TODO:  在此添加控件通知处理程序代码
	if (started)
	{
		m_log.SetWindowTextW(L"Stopping...");

		StopThread();

		if (methodName == "PBAS")
		{
			if (bgs != NULL)
			{
				delete bgs;
				bgs = NULL;
			}
		}
		if (methodName == "混合高斯模型")
		{
			if (!bg_model.empty())
			{
				//delete bg_model;
				//bg_model.~Ptr();
			}
		}

		m_log.SetWindowTextW(L"Stopped!");
		started = false;
	}
}

void CMFCbgsDlg::StopThread()
{
	DWORD exit_code = NULL;

	if (thread != NULL)
	{
		GetExitCodeThread(thread->m_hThread, &exit_code);

		if (exit_code == STILL_ACTIVE)
		{
			::TerminateThread(thread->m_hThread, 0);
			CloseHandle(thread->m_hThread);
		}

		thread->m_hThread = NULL;
		thread = NULL;
	}
}