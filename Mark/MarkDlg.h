// MarkDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include <atlimage.h>
#include <opencv2/opencv.hpp>
#include "SetDlg.h"
#include <queue>

// CMarkDlg �Ի���
class CMarkDlg : public CDialog
{
// ����
public:
	CMarkDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MARK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
//	std::queue<CPoint> ptQueue1;	// �����ע��������������
//	std::queue<CPoint> ptQueue2;	// �����ע�����յ��������
	std::queue<cv::Rect> rcQueue;	// �����ע���ζ���
	std::queue<cv::Mat> imgQueue;	// �����עͼ�����
	HICON m_hIcon;
	double m_frameNum;		// ��ǰ֡��
	double m_frameCount;	// ����֡��
	int m_step;				// ��֡���
	CStatic m_pic;			// ������ʾ��ͼƬ�ؼ�
	cv::Mat currPic;		// ��ǰͼ��
	cv::Mat imgShow;		// ������ʾ��ͼ��
	cv::VideoCapture myCap;	// ��Ƶ����
	CRect rcImg;			// ��ͼ�������
	CRect rcLabel;			// ��ע�����
	CRect rcTemp;			// ��ʱ����
	CPoint ptStart;			// ��ע�����ϵ�
	CPoint ptEnd;			// ��ע�����µ�
	CPoint ptTemp;			// ��ʱ��
	bool isDrawing;			// ָʾ����Ƿ��ڱ�ע��
	bool labelExists;		// ָʾ��ǰ�Ƿ��б�ע
	bool saveOriScale;		// �����Ƿ��ڱ����ע��ʱ��ԭͼ��������
	int labelNum;			// ����ı�ע��ţ��ѷ���
	int srcMode;			// ͼƬԴģʽ��0Ϊ���ţ�1Ϊ��Ƶ��2Ϊ�ļ���
	CString m_savePath;		// �洢�����ע��·��
	CString m_imgsPath;		// �洢ͼ��Դ�ļ��е�·��
	CString currFileName;	// ��ǰ�򿪵�����·���ļ���
	CButton m_ifWheel;		// �Ƿ���Ӧ��������������Ƶ����
	bool isSeeking;			// ָʾ��ǰ�Ƿ��ڱ����ļ���
	CFileFind ff;			// �����ļ���
	CSetDlg Settings;		// ѡ��Ի�����
	int saveMode;			// ����ģʽ��ָʾ�����ʽ
	//��saveMode��0Ϊ��עͼ��1ΪTXT��¼��2Ϊͼ�ͼ�¼���У�3Ϊԭͼ��4Ϊȫͼ���ֲ����ǣ�
	bool fixedLabel;		// �Ƿ�������ָ����С��ע�Ĺ���
	bool fixedSave;			// �Ƿ���ִ��ָ����С��עʱ�Զ�����
	cv::Size fixedSize;		// ��ע��ָ����С
	char inipath[MAX_PATH];	// ini�ļ�·��


	// ���ɵ���Ϣӳ�亯��
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
