
// MFCbgsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCbgs.h"
#include "MFCbgsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma comment(lib,"random_walker.lib")  
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

int SizeH = 0;					//ͼƬ�߶�
int SizeW = 0;					//ͼƬ���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMFCbgsDlg �Ի���



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


// CMFCbgsDlg ��Ϣ�������

BOOL CMFCbgsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	frameNum = 0;
	minThinSize = 20;
	dilateSize = 35;
	minCanny = 60, maxCanny = 120;
	areaThres = 0.005;
	updateBackgroundImageRate = 0.01;

	if (!random_walkerInitialize())
	{
		MessageBox(L"random_walker�㷨��ʼ����");
		//exit(0);
	}

	m_algorithm.AddString(L"PBAS");
	m_algorithm.AddString(L"��ϸ�˹ģ��");

	m_algorithm.SetCurSel(0); //���õ�nIndex��Ϊ��ʾ������

	((CButton*)GetDlgItem(IDC_CHECKBACK))->SetCheck(TRUE);

	CEdit* pBoxOne;
	pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT2);
	//��ֵ
	pBoxOne-> SetWindowText( _T("0.005 ") );

	started = false;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCbgsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL isOpen = TRUE;     //�Ƿ��(����Ϊ����)  
	CString defaultDir = L"E:\\";   //Ĭ�ϴ򿪵��ļ�·��  
	CString fileName = L"";         //Ĭ�ϴ򿪵��ļ���  
	CString filter = L"�ļ� (*.mp4; *.avi; *.wav; *.rmvb; *.mpeg)|*.mp4;*.avi;*.rmvb;*.mpeg||";   //�ļ����ǵ�����
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	openFileDlg.GetOFN().lpstrInitialDir = L"E:\\FileTest\\raw.avi";
	openFileDlg.m_ofn.lpstrTitle = L"����Ƶ";
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
	else if (methodName == "��ϸ�˹ģ��")
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
					AfxMessageBox(L"��ѡ��Ԥ�豳��");
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
		else if (methodName == "��ϸ�˹ģ��")
		{
			bg_model->apply(frame, mask, updateBackgroundImageRate);
			bg_model->getBackgroundImage(backGround);
		}
		if (!mask.empty())
		{
			//�ҶȻ�

			//��ֵ�˲�
			Mat medianMask;
			medianBlur(mask, medianMask, 9);

			if (methodName == "��ϸ�˹ģ��")
			{
				// ����
				Mat element = getStructuringElement(MORPH_RECT, Size(dilateSize, dilateSize));
				dilate(medianMask, foreground, cv::Mat());

				//canny��ȡ��Ե
				Mat cannyMask, cannyBackground;
				Canny(medianMask, cannyMask, minCanny, maxCanny);
				Canny(backGround, cannyBackground, minCanny, maxCanny);
			}

			else
			{
					medianMask.copyTo(foreground);
			}
			//ȥ����Ӱ
			foreground = foreground;// &cannyImage;

			Mat foreground1;
			selectforeground(foreground, foreground1);


			//�����Ǽ���ȡ
			//thinning(foreground, thin1);
			foreground.copyTo(thin1);
			chao_thinimage(thin1);

			//�Ǽܶ�ֵ��
		    cv::threshold(thin1, thin1, 128, 255, THRESH_BINARY);

			//��ͨ�Ǽ���ȡ
			vector<vector<Point>> contours;
			cv::findContours(thin1, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

			//̫�̵���ͨ�Ǽ�ȥ��
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

				//ѡȡ�Ǽܱ�ǵ�
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

				//�Ǽܱ�ǵ��ɫ
				Mat gauImg;
				frame.copyTo(gauImg);
				gauPointsLabelColor(GauPoints, gauImg);

				//��������㷨
				Mat randomWalkImage;

				randomWalker(frame, foreground, GauPoints, randomWalkImage);

				//��ȡ����
				vector<vector<Point>> resultContours;
				cv::findContours(foreground1, resultContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
				/* ��ÿ������������͹��*/
				vector<vector<Point> >poly(resultContours.size());
				for (int i = 0; i < resultContours.size(); i++)
				{
					approxPolyDP(Mat(resultContours[i]), poly[i], 1, true);
				}

				//����������ܳ��˲�
				vector<vector<Point>> resultPolys;
				double areaFrame = frame.cols*frame.rows;
				for (int i = 0; i < (int)poly.size(); i++)
				{
					//cout << frame.cols*frame.rows<<" "<<i << "�����  " << contourArea(poly[i]) << "�ܳ���  " << arcLength(poly[i], true) << endl;
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
		if (methodName == "��ϸ�˹ģ��")
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