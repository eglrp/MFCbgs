
// MFCbgsDlg.h : ͷ�ļ�
//

#pragma once
#include "CvvImage.h"

#include <strstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
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
#include "myRandom.h"

#include "package_tracking/BlobTracking.h"
#include "package_bgs/PBAS/PixelBasedAdaptiveSegmenter.h"
#include "package_analysis/VehicleCouting.h"
#include "afxwin.h"

using namespace std;
using namespace cv;

// CMFCbgsDlg �Ի���
class CMFCbgsDlg : public CDialogEx
{
// ����
public:
	CMFCbgsDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCBGS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	int showImage(Mat image, UINT ID);
	bool getBgsMethodName();

	Mat frame;
	VideoCapture video;

	int frameNum;
	int minThinSize;
	int dilateSize;
	int minCanny, maxCanny;
	double areaThres;
	double updateBackgroundImageRate;

	BlobTracking* blobTracking;
	Ptr<BackgroundSubtractorMOG2> bg_model;
	VehicleCouting* vehicleCouting;
	IBGS* bgs;

	CString backImgFile;

	bool started;

	CString methodName;

	static DWORD Thread(LPVOID *lpvParam);
	afx_msg void ThreadProcess();
	afx_msg void StopThread();
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	CWinThread* thread;
	afx_msg void OnBnClickedButtonopenvideo();
	CComboBox m_algorithm;
	afx_msg void OnBnClickedButtonstart();
	CStatic m_log;
	afx_msg void OnBnClickedButtonstop();
};
