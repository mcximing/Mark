// MarkDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include <atlimage.h>
#include <opencv2/opencv.hpp>
#include "SetDlg.h"
#include <queue>

// CMarkDlg 对话框
class CMarkDlg : public CDialog
{
// 构造
public:
	CMarkDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MARK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
//	std::queue<CPoint> ptQueue1;	// 保存标注矩形起点坐标队列
//	std::queue<CPoint> ptQueue2;	// 保存标注矩形终点坐标队列
	std::queue<cv::Rect> rcQueue;	// 保存标注矩形队列
	std::queue<cv::Mat> imgQueue;	// 保存标注图像队列
	HICON m_hIcon;
	double m_frameNum;		// 当前帧数
	double m_frameCount;	// 所有帧数
	int m_step;				// 跳帧间隔
	CStatic m_pic;			// 用于显示的图片控件
	cv::Mat currPic;		// 当前图像
	cv::Mat imgShow;		// 用于显示的图像
	cv::VideoCapture myCap;	// 视频输入
	CRect rcImg;			// 画图区域矩形
	CRect rcLabel;			// 标注框矩形
	CRect rcTemp;			// 临时矩形
	CPoint ptStart;			// 标注框左上点
	CPoint ptEnd;			// 标注框右下点
	CPoint ptTemp;			// 临时点
	bool isDrawing;			// 指示鼠标是否在标注中
	bool labelExists;		// 指示当前是否有标注
	bool saveOriScale;		// 决定是否在保存标注的时候按原图比例保存
	int labelNum;			// 保存的标注编号，已废弃
	int srcMode;			// 图片源模式，0为单张，1为视频，2为文件夹
	CString m_savePath;		// 存储保存标注的路径
	CString m_imgsPath;		// 存储图像源文件夹的路径
	CString currFileName;	// 当前打开的完整路径文件名
	CButton m_ifWheel;		// 是否响应鼠标滚轮来控制视频进退
	bool isSeeking;			// 指示当前是否在遍历文件夹
	CFileFind ff;			// 查找文件类
	CSetDlg Settings;		// 选项对话框类
	int saveMode;			// 保存模式，指示输出形式
	//（saveMode：0为标注图，1为TXT记录，2为图和记录兼有，3为原图，4为全图带局部覆盖）
	bool fixedLabel;		// 是否开启按照指定大小标注的功能
	bool fixedSave;			// 是否在执行指定大小标注时自动保存
	cv::Size fixedSize;		// 标注框指定大小
	char inipath[MAX_PATH];	// ini文件路径


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void SaveWholeImg(int Mode);
	void SaveTXT(CString txtpath, char* c, int d);
	int isImageFile();
	void NextImage();
	cv::Rect CreateRect(CPoint pt1, CPoint pt2, bool zoom);
	void CreateLabelImg(CPoint pt1, CPoint pt2);
	void SaveLabel(int Mode);
	void ClearLabel();
	void JumpFrame(bool direct);
	void reDrawRect(int nID);
	void DrawRect(int nID, CPoint pt);
	int OpenVideo(CString currFileName);
	int OpenImage(CString currFileName);
	int ShowImage(cv::Mat image, int nID, bool ifFlush);
	CString FindFile(int FileType, CString dir);
	CString GetPath();
	void WriteIntValue(char* lpAppName, char* lpKeyName, int Value, char* lpFileName);
	void WriteINI(char * path);
	void ReadINI(char * path);

	afx_msg void OnBnClickedOpenpic();
	afx_msg void OnBnClickedOpenvideo();	
	afx_msg void OnBnClickedNextframe();
	afx_msg void OnBnClickedPrevframe();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedSetSavePath();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedSetimgpath();
	afx_msg void OnBnClickedNextimg();
	afx_msg void OnBnClickedOpensvpath();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedSettings();
	afx_msg void OnBnClickedClear();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
