// MarkDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Mark.h"
#include "MarkDlg.h"

//#include "vld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//
// ��ͷ���д������õ�����������
// Debug�£�ֱ�����ӿ��� opencv_core248d.lib ��·������VS���������ã�
// Release�£����Ӷ�Ӧ�ľ�̬���� opencv_core248.lib ��
// �Ͳ���ÿ�ε���Ŀ��������ˣ����޸�OpenCV�汾��·��ʱ��ֱ�Ӹ�define����

#define CV_VERSION_ID "248"
#define CV_SLIB_DIR "C:/opencv/build/x86/vc12/staticlib/"

#ifdef _DEBUG
#define cvLIB(name) "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) CV_SLIB_DIR "opencv_" name CV_VERSION_ID
#define exLIB(name) CV_SLIB_DIR name
#endif

#pragma comment(lib, cvLIB("core"))
#pragma comment(lib, cvLIB("highgui"))
#pragma comment(lib, cvLIB("imgproc"))

#ifndef _DEBUG
#pragma comment(lib, exLIB("libjasper"))
#pragma comment(lib, exLIB("libjpeg"))
#pragma comment(lib, exLIB("libpng"))
#pragma comment(lib, exLIB("libtiff"))
#pragma comment(lib, exLIB("zlib"))
#pragma comment(lib, exLIB("ilmimf"))
#pragma comment(lib, "vfw32.lib")
#endif

//////////////////////////////////////////////////////////////////////////

using namespace cv;

#define IsCTRLPressed() ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 ) 


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMarkDlg �Ի���




CMarkDlg::CMarkDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMarkDlg::IDD, pParent)
, m_frameNum(0.00)
, m_frameCount(0.00)
, m_step(5)
, isSeeking(false)
, isDrawing(false)
, labelExists(false)
, fixedLabel(false)
, fixedSave(false)
, saveOriScale(true)
, labelNum(1)
, srcMode(0)
, ptStart((0,0))
, ptEnd((0,0))
, ptTemp((0,0))
, saveMode(2)
, m_savePath(_T(""))
, m_imgsPath(_T(""))
, currFileName(_T(""))
, fixedSize(35,40)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMarkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC, m_pic);
	DDX_Text(pDX, IDC_FRAMENUM, m_frameNum);
	DDX_Text(pDX, IDC_STEP, m_step);
	DDV_MinMaxInt(pDX, m_step, 1, 500);
	DDX_Text(pDX, IDC_FRMCOUNT, m_frameCount);
	DDX_Text(pDX, IDC_SAVEPATH, m_savePath);
	DDX_Text(pDX, IDC_IMGSPATH, m_imgsPath);
	DDX_Control(pDX, IDC_IFWHEEL, m_ifWheel);
}

BEGIN_MESSAGE_MAP(CMarkDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OPENPIC, &CMarkDlg::OnBnClickedOpenpic)
	ON_BN_CLICKED(IDC_OPENVIDEO, &CMarkDlg::OnBnClickedOpenvideo)
	ON_BN_CLICKED(IDC_NEXTFRAME, &CMarkDlg::OnBnClickedNextframe)
	ON_BN_CLICKED(IDC_PREVFRAME, &CMarkDlg::OnBnClickedPrevframe)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_SAVE, &CMarkDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_SETSAVEPATH, &CMarkDlg::OnBnClickedSetSavePath)

	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_SETIMGPATH, &CMarkDlg::OnBnClickedSetimgpath)
	ON_BN_CLICKED(IDC_NEXTIMG, &CMarkDlg::OnBnClickedNextimg)
	ON_BN_CLICKED(IDC_OPENSVPATH, &CMarkDlg::OnBnClickedOpensvpath)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_SETTINGS, &CMarkDlg::OnBnClickedSettings)
	ON_BN_CLICKED(IDC_CLEAR, &CMarkDlg::OnBnClickedClear)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CMarkDlg ��Ϣ�������

BOOL CMarkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_SHOWNORMAL);
	CenterWindow();

	// ��ʼ������
	m_pic.GetWindowRect(&rcImg);//��ȡPicture�ؼ������Ļ���Ͻǵ�����
	ScreenToClient(rcImg);	//��ȡPicture�ؼ���ԶԻ���ͻ������Ͻǵ�����
	rcImg.DeflateRect(-11,-11,11,11);

	// ��ʼ��Ĭ��ͼƬ��źͱ���·��
	TCHAR savePath[MAX_PATH];
	TCHAR imgsPath[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH,savePath);
	GetCurrentDirectoryW(MAX_PATH,imgsPath);
	wcscat_s(savePath,MAX_PATH,_T("\\ImgDst"));
	wcscat_s(imgsPath,MAX_PATH,_T("\\ImgSrc"));
	m_savePath = (CString)savePath;
	m_imgsPath = (CString)imgsPath;
	SetDlgItemText(IDC_SAVEPATH,m_savePath);
	SetDlgItemText(IDC_IMGSPATH,m_imgsPath);

	// ini�ļ���ʼ��
	GetModuleFileNameA(0, inipath, MAX_PATH);
	inipath[strlen(inipath)-3] = '\0';
	strcat_s(inipath,"ini");					// ͨ���滻��չ�����ֵõ�INI����·��
	if((GetFileAttributesA(inipath))==-1)		// ��ȡINI�ļ�
		WriteINI(inipath);
	ReadINI(inipath);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMarkDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMarkDlg::OnPaint()
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
		CDialog::OnPaint();
	}
	if (!imgShow.empty())
		ShowImage(imgShow,IDC_PIC,1);
	if (labelExists)
		reDrawRect(IDC_PIC);
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMarkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// ��Mat���͹���CImage��
void MatToCImage( Mat &mat, CImage &cImage)
{
	//create new CImage
	int width    = mat.cols;
	int height   = mat.rows;
	int channels = mat.channels();

	cImage.Destroy(); //clear
	cImage.Create(width, 
		height, //positive: left-bottom-up   or negative: left-top-down
		8*channels ); //numbers of bits per pixel

	//copy values
	uchar* ps;
	uchar* pimg = (uchar*)cImage.GetBits(); //A pointer to the bitmap buffer

	//The pitch is the distance, in bytes. represent the beginning of 
	// one bitmap line and the beginning of the next bitmap line
	int step = cImage.GetPitch();

	for (int i = 0; i < height; ++i)
	{
		ps = (mat.ptr<uchar>(i));
		for ( int j = 0; j < width; ++j )
		{
			if ( channels == 1 ) //gray
			{
				*(pimg + i*step + j) = ps[j];
			}
			else if ( channels == 3 ) //color
			{
				for (int k = 0 ; k < 3; ++k )
				{
					*(pimg + i*step + j*3 + k ) = ps[j*3 + k];
				}            
			}
		}    
	}
}


// ���ļ�·��ȫ���õ��ļ������ڶ����������Ƿ�������չ��
CString GetTitleFromFileName(CString FileName, BOOL Ext)
{
	int Where;
	Where = FileName.ReverseFind(_T('\\')); 
	CString FileTitle = FileName.Right(FileName.GetLength() - 1 - Where);
	if (!Ext)
	{
		int Which = FileTitle.ReverseFind('.');
		if (Which != -1)
			FileTitle = FileTitle.Left(Which);
	}
	return FileTitle;
}


// ѡ���ļ���
CString CMarkDlg::GetPath()
{
	TCHAR szDir[MAX_PATH];//�����洢·�����ַ���
	CString strPath = _T("");
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = this->m_hWnd;
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = szDir;
	bInfo.lpszTitle = _T("��ѡ��·��: ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	bInfo.lpfn = NULL;
	bInfo.lParam = 0;
	bInfo.iImage = 0;

	LPITEMIDLIST lpDlist; //�������淵����Ϣ��IDList
	lpDlist = SHBrowseForFolder(&bInfo) ; //��ʾѡ��Ի���
	if(lpDlist != NULL)   //�û�����ȷ����ť
	{
		SHGetPathFromIDList(lpDlist, szDir);//����Ŀ��ʶ�б�ת�����ַ���
		strPath = szDir; //��TCHAR���͵��ַ���ת��ΪCString���͵��ַ���
	}
	return strPath;
}




// ���ݴ��ļ��Ի���������ļ����ͺ�Ĭ��·���������ļ�����·��
CString CMarkDlg::FindFile(int FileType, CString dir)
{
	CString FileName = _T("");
	CString TypeName = _T("All Files (*.*) |*.*||");
	if (FileType==0)
	{
		TypeName = _T("image files (*.bmp; *.png; *.jpg) |*.bmp; *.png; *.jpg; *.jpeg | All Files (*.*) |*.*||");
	}
	else if(FileType==1)
	{
		TypeName = _T("video files (*.avi; *.mpg; *.mp4; *.wmv) |*.avi; *.mpg; *.mp4; *.wmv | All Files (*.*) |*.*||");
	}
	CFileDialog dlg(TRUE,0,0,OFN_HIDEREADONLY,TypeName,this);
	if (dir!=_T(""))
		dlg.m_ofn.lpstrInitialDir = dir;	

	if (dlg.DoModal()==IDOK)	// �ӡ����ļ����Ի�������ȡ�ļ�·��
	{
		FileName=dlg.GetPathName();
	}
	return FileName;
}


// ������ʾMat��ʽ��ͼ��Picture�ؼ���
int CMarkDlg::ShowImage(Mat image, int nID, bool ifFlush)
{
	if (image.empty())
		return 0;

	// ���컭ͼ�������
	GetDlgItem(nID)->GetClientRect(rcImg);
	CDC* pDC = GetDlgItem(nID)->GetDC();
	if (pDC==NULL)
		return -1;

	// ���ݲ���ѡ���Ƿ�ͼǰ������������
	if (ifFlush)
		pDC->FillSolidRect(rcImg,RGB(128,128,128));

	// ����ʾǰ����ͼ�����ʾ����Ĵ�С��������
	int newTop, newLeft;

	if ((image.rows<=rcImg.Height())&&(image.cols<=rcImg.Width()))
	{
		// ��ͼƬ�Ȼ�ͼ����С���򱣳�ԭ��С��ʾ��������������
		newTop = (rcImg.Height()-image.rows)/2;
		newLeft = (rcImg.Width()-image.cols)/2;
		rcImg.SetRect(newLeft,newTop,newLeft+image.cols,newTop+image.rows);
		imgShow = image;
	}
	else
	{
		// ����ͼ���ڻ�ͼ�ؼ��а��������ź�Ĵ�С
		double newWidth, newHeight;
		double picScale = (double)image.cols/(double)image.rows;
		double rcScale = (double)rcImg.Width()/(double)rcImg.Height();
		if (picScale>rcScale)
		{
			newWidth = rcImg.Width();
			newHeight = newWidth/picScale;
			newTop = rcImg.top + (rcImg.Height()-(int)newHeight)/2;
			// ���¶��廭ͼ���δ�С
			rcImg.SetRect(rcImg.left,newTop,rcImg.right,newTop+(int)newHeight);
		}
		else // ���߿�Ȳ�ͬ���в�ͬ����
		{
			newHeight = rcImg.Height();
			newWidth = newHeight*picScale;
			newLeft = rcImg.left + (rcImg.Width()-(int)newWidth)/2;
			rcImg.SetRect(newLeft,rcImg.top,newLeft+(int)newWidth,rcImg.bottom);
		}

		// �����µ�ucharͼ���Թ���ʾ
		Size dsize = Size((int)newWidth,(int)newHeight);
		imgShow = Mat(dsize,CV_8UC3);
		resize(image,imgShow,dsize,0,0,INTER_LINEAR);
	}

	// ת��ΪCImage���ͼ�񣨵��ú�����
	CImage cimg;
	MatToCImage(imgShow,cimg);
	// ��ʾ���ؼ�
	cimg.Draw(pDC->m_hDC,rcImg);
	// �ͷ���Դ
	ReleaseDC(pDC);
	cimg.Destroy();
	return 1;
}


// �����κ���
void CMarkDlg::DrawRect(int nID, CPoint pt)
{
	CDC * pRDC = GetDlgItem(nID)->GetDC();	// ��ͼDC
	pRDC->SelectStockObject(NULL_BRUSH);	// ˢ��͸��

	CPen myPen(PS_SOLID, 2, RGB(255,0,0));	// ��������
	DeleteObject(pRDC->SelectObject(myPen));

	rcTemp = CRect(ptStart,ptTemp); // �ϴε��ú������ľ���
	rcLabel = CRect(ptStart,pt);	// ����Ҫ���ľ���

	pRDC->SetROP2(R2_NOTXORPEN);	// �����ϴεľ���
	pRDC->Rectangle(rcTemp);		// �滭���
	pRDC->Rectangle(rcLabel);

	CString winTitle = _T("ͼ���ע - ");
	winTitle += GetTitleFromFileName(currFileName,TRUE);
	// �ڳ��򴰿ڱ�����ʵʱ��ʾ��ǰ���εĳ���
	winTitle.AppendFormat(_T(" - %d X %d"),abs(rcLabel.Width()),abs(rcLabel.Height()));
	SetWindowTextW(winTitle);

	ReleaseDC(pRDC);
	ptTemp = pt;
}


// �ػ�����͵ľ���
void CMarkDlg::reDrawRect(int nID)
{
	CDC * pRDC = GetDlgItem(nID)->GetDC();	// ��ͼDC
	pRDC->SelectStockObject(NULL_BRUSH);	// ˢ��͸��
	CPen myPen(PS_SOLID, 2, RGB(255,0,0));	// ��������
	DeleteObject(pRDC->SelectObject(myPen));
	pRDC->Rectangle(rcLabel);
	ReleaseDC(pRDC);
}


// ������ʼ��ͽ��������ɱ�ע�����õľ���
cv::Rect CMarkDlg::CreateRect(CPoint pt1, CPoint pt2, bool zoom)
{
	// �Ƚ�x1,y1,x2,y2����Դ�С����������
	int rx = (pt2.x<pt1.x)?pt2.x:pt1.x;
	int ry = (pt2.y<pt1.y)?pt2.y:pt1.y;
	int rw = ((pt2.x>pt1.x)?pt2.x:pt1.x) - rx;
	int rh = ((pt2.y>pt1.y)?pt2.y:pt1.y) - ry;

	if (zoom)
	{
		// ��ԭͼ�ߴ�������ʾʱ�����ţ���������㣬���Ż�ȥ
		double Xscale = (double)currPic.cols/(double)imgShow.cols;
		double Yscale = (double)currPic.rows/(double)imgShow.rows;
		double scale = Xscale>Yscale ? Xscale:Yscale;
		if (scale>1)
		{
			rx = (int)(rx*scale);
			ry = (int)(ry*scale);
			rw = (int)(rw*scale);
			rh = (int)(rh*scale);
		}
	}

	return Rect(rx,ry,rw,rh);
}


// �������ű�ע��ȡ������ͼ��
void CMarkDlg::CreateLabelImg(CPoint pt1, CPoint pt2)
{
	if (imgShow.empty())
	{
		// AfxMessageBox(_T("���ȴ�ͼƬ����Ƶ���б�ע"),MB_OK,0);
		return;
	}
	if (!labelExists)
	{
		AfxMessageBox(_T("����ѡ����ע���ο�"),MB_OK,0);
		return;
	}
	// ������ע����
	cv::Rect rcRoi = CreateRect(pt1,pt2,saveOriScale);

	// �����������Դͼ��
	IplImage img1;
	if (saveOriScale)
		img1 = currPic;
	else
		img1 = imgShow;

	// ���ROIС��Դͼ���С
	if (rcRoi.width>img1.width || rcRoi.height>img1.height)	
	{
		AfxMessageBox(_T("���ý�ͼ������ִ��󣬽�����"),MB_OK,0);
		return;
	}

	// д���ע���ζ���
	rcQueue.push(rcRoi);

	// ���ø���Ȥ����ROI
	cvSetImageROI(&img1, rcRoi);

	// ����Ŀ��ͼ�񣬴�ʱcvGetSize���ص��Ѿ���ROI����Ĳ���
	IplImage *img2 = cvCreateImage(cvGetSize(&img1),
		img1.depth,
		img1.nChannels);

	// ת��ΪMat��ʽ��д�����
	Mat newLabelImg;
	newLabelImg = Mat(img2, false);
	imgQueue.push(newLabelImg);

	// ����
	cvCopy(&img1, img2, NULL);
	// ����ROI
	cvResetImageROI(&img1);
	// �˴��ͷ�img2ָ����Դ�ͷŻᵼ�±���ʱ�����Ȳ��ͷſ�����ϣ��Mat���Զ����������������ͷ�
	// cvReleaseImage(&img2);
}



// �����ע���ļ�
// ���������ͼƬԴ���ͣ�0Ϊ���ţ�1Ϊ��Ƶ��2Ϊ�ļ���
void CMarkDlg::SaveLabel(int Mode)
{
	if (imgShow.empty())
	{
		AfxMessageBox(_T("���ȴ�ͼƬ����Ƶ���б�ע"),MB_OK,0);
		return;
	}
	if (!labelExists)
	{
		AfxMessageBox(_T("����ѡ����ע���ο�"),MB_OK,0);
		return;
	}

	int labelCount = imgQueue.size();
	USES_CONVERSION;
	CString LogChar = _T(""); // ����TXT�ļ����ַ���
	CString TXTPath = _T(""); // TXT�ļ�����
	CString PathName = _T("");// ·�����ļ�ȫ��
	CString FileName = _T("");// ���ļ�����������չ����
	CString tempStr = _T(""); // ��ʱ
	CTime tm = CTime::GetCurrentTime();	// �õ�ʱ��
	CString TimeStr = tm.Format("_20%y_%m_%d_%H_%M_%S");
	//	FileName.Format(_T("Img_%.3d"),labelNum);
	tempStr = GetTitleFromFileName(currFileName,FALSE);
	FileName = tempStr + TimeStr; // �����ļ������ʱ���

	switch(Mode) // ����ͼƬԴģʽѡ�񱣴淽ʽ
	{
	case 0:		// ����ͼƬ���򿪱����ļ��Ի�����б���
		{
			// ������Ϊ1��2���򱣴��¼�ļ�
			if (saveMode!=0)
			{
				// ֱ�ӱ��浽ͼƬԴĿ¼��
				int namelen = (GetTitleFromFileName(currFileName,TRUE)).GetLength();
				CString NewTXTPath = currFileName.Left(currFileName.GetLength()-namelen);
				NewTXTPath += tempStr;
				NewTXTPath += _T(".txt");
				// �����¼��LogCharǰ����ͼ����
				cv::Rect rcRoi;
				LogChar = tempStr;
				LogChar.AppendFormat(_T(" %d"), labelCount);
				for(int j=0; j<labelCount; j++)	// ��ͬһ��ͼ�����б�ע��¼Ϊһ��
				{
					rcRoi = rcQueue.front();
					LogChar.AppendFormat(_T(" %d %d %d %d"),
						rcRoi.x,rcRoi.y,rcRoi.width,rcRoi.height);
					rcQueue.pop();
				}
				char * c = T2A(LogChar);
				SaveTXT(NewTXTPath, c, CStringA(LogChar).GetLength());
			}
			// ������Ϊ0��2���򱣴��עͼ��
			if (saveMode!=1)
			{
				for(int i=0; i<labelCount; i++)	// ���α����б������б�ע�ļ�
				{
					CString tpName = FileName;
					tpName.AppendFormat(_T("_%.2d"),i);
					CFileDialog dlg( FALSE, _T("jpg"), tpName,	// Ĭ���ļ�����������չ����
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						_T("JPG�ļ�(*.jpg)|*.jpg |BMP�ļ�(*.bmp)|*.bmp |PNG�ļ�(*.png)|*.png ||"));
					if (dlg.DoModal()==IDOK)
					{
						PathName = dlg.GetPathName();	// ·��ȫ��
						imwrite(T2A(PathName),imgQueue.front());
						//labelNum++;
					}
					imgQueue.pop();
				}
			}
			break;
		}
	case 1:		// ��Ƶ�ļ��е�֡���Զ�����
		{
			if (m_savePath!=_T(""))
			{
				CreateDirectoryW(m_savePath,NULL);	// ��鱣��·��
				// ���챣�����ļ�������
				PathName = m_savePath + _T("\\");
				PathName += tempStr;
				PathName += _T("_dst");
				if(PathName.GetLength()>MAX_PATH-40)// ���·�����Ƿ����
					PathName.Left(MAX_PATH-40);
				// �������α�������ļ���
				CreateDirectoryW(PathName,NULL);

				// ������Ϊ1��2���򱣴��¼�ļ�
				if (saveMode!=0)
				{
					// ����TXT�ļ���ȫ����tempStr��ʱ���������Ƶ�ļ���
					TXTPath = PathName + _T("\\");
					TXTPath += tempStr;
					TXTPath += _T(".txt");
					// ����ƵԴĿ¼��Ҳ����һ��
					int namelen = (GetTitleFromFileName(currFileName,TRUE)).GetLength();
					CString NewTXTPath = currFileName.Left(currFileName.GetLength()-namelen);
					NewTXTPath += tempStr;
					NewTXTPath += _T(".txt");
					// �����¼��LogCharǰ������Ƶ��-֡��
					cv::Rect rcRoi;
					LogChar = tempStr + _T("-");
					LogChar.AppendFormat(_T("%d %d"), (int)m_frameNum, labelCount);
					for(int j=0; j<labelCount; j++)	// ��ͬһ��ͼ�����б�ע��¼Ϊһ��
					{
						rcRoi = rcQueue.front();
						LogChar.AppendFormat(_T(" %d %d %d %d"),
							rcRoi.x,rcRoi.y,rcRoi.width,rcRoi.height);
						rcQueue.pop();
					}
					char * c = T2A(LogChar);
					SaveTXT(TXTPath, c, CStringA(LogChar).GetLength());
					SaveTXT(NewTXTPath, c, CStringA(LogChar).GetLength());
				}
				// ������Ϊ0��2���򱣴��עͼ��
				if (saveMode!=1)
				{
					// �������֡��ָʾ���ͼƬ�ļ�·��ȫ�����˲��������PathName����TXTPath�Ĺ���Ӧ�ڴ�ǰ��
					PathName += _T("\\");
					PathName += FileName;
					tempStr.Format(_T("_n%d"),(int)m_frameNum);	// ������Ƶ֡����
					PathName += tempStr;

					CString tpName;
					for(int i=0; i<labelCount; i++)	// ���α����б������б�ע�ļ�
					{
						tpName = PathName;
						tpName.AppendFormat(_T("_%.2d"),i);		// ����ͬһ֡�еĲ�ͬ��ע�����99
						tpName += _T(".jpg");
						imwrite(T2A(tpName),imgQueue.front());
						imgQueue.pop();
						labelNum++;
					}
				}
			}
			else
				AfxMessageBox(_T("���󣺱���·��Ϊ��"),MB_OK,0);
			break;
		}
	case 2:		// ���ͼ���ļ��У��Զ�����
		{
			if (m_savePath!=_T(""))
			{
				CreateDirectoryW(m_savePath,NULL);//��鱣���ļ��У�û���򴴽�
				tempStr = GetTitleFromFileName(m_imgsPath,TRUE);//�õ���ǰ������ļ�����

				// �ڵ�ǰ����·�����½��ļ��У������ļ�·��ȫ��
				PathName = m_savePath + _T("\\");
				PathName += tempStr;
				PathName += _T("_dst");
				if(PathName.GetLength()>MAX_PATH-40)// ���·�����Ƿ����
					PathName.Left(MAX_PATH-40);
				//�������α�������ļ���
				CreateDirectoryW(PathName,NULL);

				// ������Ϊ1��2���򱣴��¼�ļ�
				if (saveMode!=0)
				{
					// ����TXT�ļ���ȫ����tempStr��ʱ�������ͼƬ�ļ�����
					TXTPath = PathName + _T("\\");
					TXTPath += tempStr;
					TXTPath += _T(".txt");
					// ��ͼƬԴĿ¼��Ҳ����һ��
					CString NewTXTPath = m_imgsPath + _T("\\");
					NewTXTPath += tempStr;
					NewTXTPath += _T(".txt");
					// �����¼��LogCharǰ�����ļ�����\ͼƬ��
					cv::Rect rcRoi;
					LogChar = tempStr + _T("\\");
					LogChar += GetTitleFromFileName(currFileName,TRUE);
					LogChar.AppendFormat(_T(" %d"), labelCount);
					for(int j=0; j<labelCount; j++)	// ��ͬһ��ͼ�����б�ע��¼Ϊһ��
					{
						rcRoi = rcQueue.front();
						LogChar.AppendFormat(_T(" %d %d %d %d"),
							rcRoi.x,rcRoi.y,rcRoi.width,rcRoi.height);
						rcQueue.pop();
					}
					char * c = T2A(LogChar);
					SaveTXT(TXTPath, c, CStringA(LogChar).GetLength());
					SaveTXT(NewTXTPath, c, CStringA(LogChar).GetLength());
				}
				// ������Ϊ0��2���򱣴��עͼ��
				if (saveMode!=1)
				{
					// ���챣��ͼƬ�ļ������˲��������PathName����TXTPath�Ĺ���Ӧ�ڴ�ǰ��
					PathName += _T("\\");
					PathName += FileName;
					CString tpName;
					for(int i=0; i<labelCount; i++)	// ���α����б������б�ע�ļ�
					{
						tpName = PathName;
						tpName.AppendFormat(_T("_%.2d"),i);		// ����ͬһ֡�еĲ�ͬ��ע�����99
						tpName += _T(".jpg");
						imwrite(T2A(tpName),imgQueue.front());
						imgQueue.pop();
						labelNum++;
					}
				}
			}
			else
				AfxMessageBox(_T("���󣺱���·��Ϊ��"),MB_OK,0);
			break;
		}
	default:
		break;
	}
}


// �����ǰ���б�ע���ο������
void CMarkDlg::ClearLabel()
{
	if (!imgShow.empty())
		ShowImage(imgShow,IDC_PIC,0); // ������ʾ��ǰͼƬ
	else
		Invalidate();
	// ������ݶ���
	while(!rcQueue.empty())
		rcQueue.pop();
	while(!imgQueue.empty())
		imgQueue.pop();
	labelExists = false;
}


// ��֡����
void CMarkDlg::JumpFrame(bool direct)
{
	if (!myCap.isOpened())
	{
		AfxMessageBox(_T("����������Ƶ�ļ���"),MB_OK,0);
		return;
	}
	// ȡ��ȫ��֡��
	m_frameCount = myCap.get(CV_CAP_PROP_FRAME_COUNT);
	SetDlgItemInt(IDC_FRMCOUNT,(int)m_frameCount);

	// ȡ�õ�ǰ֡��
	m_frameNum = GetDlgItemInt(IDC_FRAMENUM);
	m_step = GetDlgItemInt(IDC_STEP);

	if (direct)	// �����>>
		m_frameNum += m_step;
	else		// ��ǰ��<<
		m_frameNum -= m_step;

	if (m_frameNum<0)
	{
		AfxMessageBox(_T("�Ѿ�������Ƶ��ͷ��"),MB_OK,0);
		m_frameNum = 0;
	}
	if (m_frameNum>=m_frameCount)
	{
		AfxMessageBox(_T("�Ѿ�������Ƶ��β��"),MB_OK,0);
		m_frameNum = m_frameCount-1;
	}
	myCap.set(CV_CAP_PROP_POS_FRAMES, m_frameNum);
	SetDlgItemInt(IDC_FRAMENUM,(int)m_frameNum);
	myCap >> currPic;

	ShowImage(currPic,IDC_PIC,0);
	// �ͷ���һ��ͼռ�õ����ݱ���
	while(!rcQueue.empty())
		rcQueue.pop();
	while(!imgQueue.empty())
		imgQueue.pop();
	labelExists = false;
}


// ��ȫ·������ͼ���ļ������򿪵���ͼƬ�ͱ����ļ��еĺ�������
int CMarkDlg::OpenImage(CString currFileName)
{
	USES_CONVERSION;
	char * pFileName = T2A(currFileName);
	currPic =  imread(pFileName,1);
	if (currPic.empty())
	{
		AfxMessageBox(_T("����ͼƬʧ�ܣ�"),MB_OK,0);
		currPic.release();	// ȷ���ͷ���Դ������ػ�ʱ����
		imgShow.release();
		m_pic.GetWindowRect(&rcImg);// ����rcImg,ͬ��ʼһ��
		ScreenToClient(rcImg);
		rcImg.DeflateRect(-11,-11,11,11);
		return 0;
	}
	// ��ʾ
	ShowImage(currPic,IDC_PIC,1);
	// �ڱ�������ʾ��ǰ�򿪵��ļ���
	CString winTitle = _T("ͼ���ע - ");
	winTitle += GetTitleFromFileName(currFileName,TRUE);
	SetWindowTextW(winTitle);

	// �ͷ���һ��ͼռ�õ����ݱ���
	while(!rcQueue.empty())
		rcQueue.pop();
	while(!imgQueue.empty())
		imgQueue.pop();
	labelExists = false;
	return 1;
}


// ��ȫ·��������Ƶ�ļ�������ť����Ƶ���϶�����Ƶ�ĺ�������
int CMarkDlg::OpenVideo(CString currFileName)
{
	USES_CONVERSION;
	char * pFileName = T2A(currFileName);
	if (myCap.isOpened())
		myCap.release();
	myCap.open(pFileName);

	if (!myCap.isOpened())
	{
		AfxMessageBox(_T("������Ƶʧ�ܣ�"),MB_OK,0);
		currPic.release();	// ȷ���ͷ���Դ������ػ�ʱ����
		imgShow.release();
		m_pic.GetWindowRect(&rcImg);// ����rcImg,ͬ��ʼһ��
		ScreenToClient(rcImg);
		rcImg.DeflateRect(-11,-11,11,11);
		return 0;
	} 

	// ȡ��ȫ��֡��
	m_frameCount = myCap.get(CV_CAP_PROP_FRAME_COUNT);
	SetDlgItemInt(IDC_FRMCOUNT,(int)m_frameCount);

	// ȡ�õ�ǰ֡��
	m_frameNum = GetDlgItemInt(IDC_FRAMENUM);
	//if (m_frameNum>=m_frameCount)
	//{
	//	AfxMessageBox(_T("��ǰ֡��������֡����ת����Ƶ��ͷ"),MB_OK,0);
	//	SetDlgItemInt(IDC_FRAMENUM,0);
	//	m_frameNum = 0;
	//}
	myCap.set(CV_CAP_PROP_POS_FRAMES, m_frameNum);
	myCap >> currPic;
	// ��ʾ��ͼƬ��
	ShowImage(currPic,IDC_PIC,1);
	// �ڱ�������ʾ��ǰ�򿪵��ļ���
	CString winTitle = _T("ͼ���ע - ");
	winTitle += GetTitleFromFileName(currFileName,TRUE);
	SetWindowTextW(winTitle);
	// �ͷ���һ��ռ�õ����ݱ���
	while(!rcQueue.empty())
		rcQueue.pop();
	while(!imgQueue.empty())
		imgQueue.pop();
	labelExists = false;
	isSeeking = false;
	m_ifWheel.SetCheck(BST_CHECKED);
	return 1;
}



// �򿪵���ͼƬ��ť��Ӧ
void CMarkDlg::OnBnClickedOpenpic()
{
	m_ifWheel.SetCheck(BST_UNCHECKED);
	if (myCap.isOpened())
		myCap.release();

	// ���ַ�����ʽת��һ�£���Ϊ��Unicode
	currFileName = FindFile(0,_T(""));
	if (OpenImage(currFileName)==0)	// ��ͼ����ʧ�ܷ���
		return;
	isSeeking = false;
	srcMode = 0;
}


// ����Ƶ��ť��Ӧ
void CMarkDlg::OnBnClickedOpenvideo()
{
	// ��ʼ��֡��Ϊ0
	SetDlgItemInt(IDC_FRAMENUM,0);
	// ����Ƶ�ļ�
	currFileName = FindFile(1,_T(""));
	if (OpenVideo(currFileName)==0)	// ��ͼ����ʧ�ܷ���
		return;
	srcMode = 1;
}



// �����֡
void CMarkDlg::OnBnClickedNextframe()
{
	JumpFrame(true);
}

// ��ǰ��֡
void CMarkDlg::OnBnClickedPrevframe()
{
	JumpFrame(false);
}


// ����ƶ������л�����
void CMarkDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	point.x -= 11;		// ������ʲôԭ�����ǲ�11����
	point.y -= 11;

	// ����ڻ�ͼ״̬�������ͼ�������ڣ��򻭾���
	if (isDrawing)
	{
		// �������ͼ���������޶�����ֵ
		if (point.x > rcImg.right)
			point.x = rcImg.right;
		if (point.y > rcImg.bottom)
			point.y = rcImg.bottom;
		if (point.x < rcImg.left)
			point.x = rcImg.left;
		if (point.y < rcImg.top)
			point.y = rcImg.top;

		DrawRect(IDC_PIC,point);
	}


	//// ������굱ǰ����ֵ
	//point.x -=  rcImg.left;	//point��ȡ���������ԶԻ���ͻ������Ͻǵ����꣬��ȥrcImg.left��
	//point.y -=  rcImg.top;	//rcImg.top�󣬼�Ϊ������Picture�ؼ����Ͻǵ�����

	////������λ��Picture�ؼ�֮�⣬����ʾ������
	//if ( point.x>(rcImg.Width()) || point.y>(rcImg.Height())
	//	|| point.x<0 || point.y<0 )  
	//	point = (-1,-1);

	//SetDlgItemInt(IDC_X, (int)ptStart.x);
	//SetDlgItemInt(IDC_Y, (int)ptStart.y);

	CDialog::OnMouseMove(nFlags, point);
}


// �������
void CMarkDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	ptStart.x = point.x - 11;	// У��11������
	ptStart.y = point.y - 11;

	// �����������ͼ�������ڣ����趨��ͼ��ʼ
	if ( ptStart.x<rcImg.right && ptStart.y<rcImg.bottom
		&& ptStart.x>rcImg.left && ptStart.y>rcImg.top )
	{
		isDrawing = true;			// ��ʼ������
		ptTemp = ptStart;			// ʹ��һ�β�����Ч
		// ��������ı�ע����
		// ClearLabel();
	}

	CDialog::OnLButtonDown(nFlags, point);
}


// ���̧��
void CMarkDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	point.x -= 11;
	point.y -= 11;

	ptEnd = ptTemp;				// ��������������

	ptStart.x -= rcImg.left;	// ת������Ϊ����ڻ�ͼ������ε�����
	ptStart.y -= rcImg.top;
	ptEnd.x -= rcImg.left;
	ptEnd.y -= rcImg.top;

	if ((ptEnd.x-ptStart.x)==0 
		|| (ptStart.y-ptEnd.y)==0) // ���û���¾��Σ�ֻ����һ��
	{
		if (fixedLabel)
		{
			ptStart.x = point.x - fixedSize.width/2;
			ptStart.y = point.y - fixedSize.height/2;
			ptEnd.x = ptStart.x + fixedSize.width; // ֮����ֱ�ӼӶ����Ǵ��м��һ�룬�ǿ��ǵ������߳�������
			ptEnd.y = ptStart.y + fixedSize.height;
			rcLabel = CRect(ptStart,ptEnd);	// ����Ҫ���ľ���
			reDrawRect(IDC_PIC);
			labelExists = true;

			ptStart.x -= rcImg.left;	// ת������Ϊ����ڻ�ͼ������ε�����
			ptStart.y -= rcImg.top;
			ptEnd.x -= rcImg.left;
			ptEnd.y -= rcImg.top;

			CreateLabelImg(ptStart, ptEnd);	// ��������ͼ��

			if (fixedSave)	// ���ѡ�����Զ����棬��ֱ�ӱ����ע��
				OnBnClickedSave();
		}
		else
			labelExists = false;
	}
	else if (isDrawing) // ̧�����ʱ�����վ��ν����Ϊ��ɫ
	{
		reDrawRect(IDC_PIC);
		labelExists = true;
		CreateLabelImg(ptStart, ptEnd);	// ��������ͼ��
	}
	isDrawing = false;

	//// ��������ͼ��������
	//if(point.x<rcImg.right && point.y<rcImg.bottom
	//	&& point.x>rcImg.left && point.y>rcImg.top )
	//{	
	//}

	//// ���λ��Picture�ؼ�֮�⣬��������Ϊ-1
	//if ( ptStart.x>(rcImg.Width()) || ptStart.y>(rcImg.Height())
	//	|| ptStart.x<0 || ptStart.y<0 )  
	//	ptStart = (-1,-1);

	CDialog::OnLButtonUp(nFlags, point);
}


// ���水ť
void CMarkDlg::OnBnClickedSave()
{
	if (saveMode>2)
		SaveWholeImg(srcMode);
	else
		SaveLabel(srcMode);
}


// �����ǰ��ע������
void CMarkDlg::OnBnClickedClear()
{
	ClearLabel();
}


// ˫��������Ӧ
void CMarkDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!fixedLabel)
		OnBnClickedSave();

	CDialog::OnLButtonDblClk(nFlags, point);
}



// �Ҽ������ǰ��ע������
void CMarkDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	OnBnClickedClear();

	CDialog::OnRButtonDown(nFlags, point);
}


// ����ӳ�䲿�ֲ���
BOOL CMarkDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN) //�а�������ʱ
	{
		if (IsCTRLPressed())	//CTRL����ʱ
		{
			switch(pMsg->wParam)
			{
			case 'S':
				OnBnClickedSave();
				break;

			case VK_RIGHT:
				JumpFrame(true);
				GetDlgItem(IDC_NEXTFRAME)->SetFocus();
				break;

			case VK_LEFT:
				JumpFrame(false);
				//				GetDlgItem(IDC_PREVFRAME)->SetFocus();
				break;

			default:
				break;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


// ����ָ������Ŀ¼·��
void CMarkDlg::OnBnClickedSetSavePath()
{
	CString path = GetPath();
	// �û�ѡȡ�ļ��У�����·��
	if (path!=_T(""))
	{
		m_savePath = path;
		SetDlgItemText(IDC_SAVEPATH,m_savePath);
	}
}


// ����ָ��ͼ�������ļ���
void CMarkDlg::OnBnClickedSetimgpath()
{
	CString path = GetPath();
	// �û�ѡȡ�ļ��У�����·��
	if (path!=_T(""))
	{
		m_imgsPath = path;
		SetDlgItemText(IDC_IMGSPATH,m_imgsPath);
		isSeeking = false; // ������ǰ����
	}
}


// ������ӳ����֡����
BOOL CMarkDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_ifWheel.GetCheck())
	{
		if (myCap.isOpened() && (zDelta == 120))
			JumpFrame(false);
		if (myCap.isOpened() && (zDelta == -120))
			JumpFrame(true);
		if (isSeeking && (zDelta == -120))
			NextImage();
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


// ������չ���ж��Ƿ�Ϊͼ���ļ�
int CMarkDlg::isImageFile()
{
	int dot,len;
	CString ext = _T("");
	// ����ϲ����ļ��ĺ���ʹ�ã�ff��CFileFind�����
	currFileName = ff.GetFilePath();
	dot = currFileName.ReverseFind('.');
	len = currFileName.GetLength();
	ext = currFileName.Right(len-dot-1);

	if (ext==_T("jpg") || ext==_T("bmp") || ext==_T("png") || ext==_T("jpeg") || 
		ext==_T("JPG") || ext==_T("BMP") || ext==_T("PNG") || ext==_T("JPEG"))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


// �����ȡ�ļ�������һ��ͼ��
void CMarkDlg::OnBnClickedNextimg()
{
	if (m_imgsPath==_T(""))
	{
		AfxMessageBox(_T("ͼ��Դ·���쳣���������ã�"),MB_OK,0);
		return;
	}
	// ���޴��ļ����򴴽�����ʱû��ͼƬ
	if (CreateDirectoryW(m_imgsPath,NULL)!=0)
	{
		AfxMessageBox(_T("��Ĭ��·���Ҳ�����ͼ���ļ��У���ȷ���ļ���λ�á�"),MB_OK,0);
		return;
	}

	USES_CONVERSION;
	CString pathWild = m_imgsPath + _T("\\*.*");	// ��Ѱ�ҵ��ļ�·��ȫ��
	CString currName = _T("");						// ��ǰ�ҵ����ļ���

	if (!isSeeking)	// �統ǰû�ڲ��ң�����һ���²���
	{
		if (myCap.isOpened()) // �統ǰ������Ƶ�ļ���ر�֮
			myCap.release();

		if (ff.FindFile(pathWild)==0)
		{
			AfxMessageBox(_T("ͼ��Դ·���쳣���������ã�"),MB_OK,0);
			ff.Close();
			return;
		}
		do // �������е�Ŀ¼��./..����һ��ʵ�ʵ��ļ�
		{
			if (ff.FindNextFile()==0)	// ��һ���ļ��������һ��ʱ
			{
				if (isImageFile()!=0)	// ����ͼ���ļ����֮
				{
					OpenImage(currFileName);	// ��ͼ��
					AfxMessageBox(_T("����������һ���ļ���"),MB_OK,0);
					srcMode = 2;
				}
				else
					AfxMessageBox(_T("�Ҳ���ͼ���ļ�(*.jpg /*.bmp /*.png)��"),MB_OK,0);
				isSeeking = false;
				ff.Close();
				return;
			}
		} while (ff.IsDirectory() || ff.IsDots());

		if (isImageFile()!=0)	// ���һ���ļ���ͼ���ļ����֮
		{
			OpenImage(currFileName);
			srcMode = 2;
		}
		else	// ��һ������ͼ���ļ�����һ��
			NextImage();
		isSeeking = true;	// ��־��ʼ����
		m_ifWheel.SetCheck(BST_CHECKED);
	}
	else	// �統ǰ�Ѿ��ڱ�����������ֱ����һ��
		NextImage();
}


// �ļ�������һ��ͼƬ
void CMarkDlg::NextImage()
{
	while(ff.FindNextFile()!=0)	// �ҵ���һ��ͼ���ļ����������һ����
	{
		if (isImageFile()!=0)	// ��ͼ���ļ���򿪺󷵻�
		{
			OpenImage(currFileName);
			srcMode = 2;
			return;
		}
	}
	if (isImageFile()!=0)	// �ж����һ���ļ��Ƿ�Ϊͼ��
	{
		OpenImage(currFileName);	// �����
		srcMode = 2;
	}
	AfxMessageBox(_T("����������һ���ļ���"),MB_OK,0);
	isSeeking = false;
	ff.Close();
}



// ֱ�Ӵ򿪱����ע���ļ���
void CMarkDlg::OnBnClickedOpensvpath()
{
	if (m_savePath!=_T(""))	// ��鱣��·��
		ShellExecute(NULL,_T("open"), m_savePath,NULL,NULL,SW_SHOWNORMAL);
}


// ����TXT��������Write�Ĳ�����д��һ���ַ���������䳤�� 
void CMarkDlg::SaveTXT(CString txtpath, char* c, int d)
{
	//	CTime tm=CTime::GetCurrentTime();
	//	CString timelog;
	//	timelog.Format(_T("Time: %02d-%02d %02d:%02d \r\n"),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute());
	CFile file;
	if(!file.Open(txtpath,CFile::modeReadWrite))
	{
		file.Open(txtpath,CFile::modeCreate | CFile::modeReadWrite);
	}
	file.SeekToEnd();
	//file.Write("------------------\r\n",20);
	file.Write(c,d);
	file.Write("\r\n",2);
	file.Close();
}


// ��Ƶ��ͼ����ʵ�֣�ֱ�ӱ��浱ǰ��ʾ��ԭͼ��saveMode>2��
void CMarkDlg::SaveWholeImg(int Mode)
{
	if (currPic.empty())	// �ж�����
	{
		AfxMessageBox(_T("���ȴ�ͼƬ��һ֡��Ƶ"),MB_OK,0);
		return;
	}
	if (saveMode==4)	// �ֲ���ɫ�����
	{
		if (!labelExists)
		{
			AfxMessageBox(_T("����ѡ����ע���ο�"),MB_OK,0);
			return;
		}

		// ��������
		cv::Rect rcRoi = CreateRect(ptStart,ptEnd,1);
		cv::rectangle(currPic,rcRoi,CV_RGB(0,0,0),CV_FILLED,8,0);
	}

	CString PathName = _T("");// ·�����ļ�ȫ��
	CString FileName = _T("");// ���ļ�����������չ����
	CString tempStr = _T(""); // ��ʱ
	CTime tm = CTime::GetCurrentTime();	// �õ�ʱ��
	CString TimeStr = tm.Format("_20%y_%m_%d_%H_%M_%S");
	tempStr = GetTitleFromFileName(currFileName,FALSE);
	FileName = tempStr + TimeStr; // �����ļ������ʱ���

	USES_CONVERSION;

	switch(Mode) // ����ͼƬԴģʽѡ�񱣴淽ʽ
	{
	case 0:		// ����ͼƬ���򿪱����ļ��Ի�����б���
		{
			CFileDialog dlg(FALSE,
				_T("jpg"),
				FileName,	// Ĭ���ļ�����������չ����
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				_T("JPG�ļ�(*.jpg)|*.jpg |BMP�ļ�(*.bmp)|*.bmp |PNG�ļ�(*.png)|*.png ||"));
			if (dlg.DoModal()==IDOK)
			{
				PathName = dlg.GetPathName();	// ·��ȫ��
				imwrite(T2A(PathName),currPic);
			}
			break;
		}
	case 1:		// ��Ƶ�ļ��е�֡���Զ�����
		{
			if (m_savePath!=_T(""))
			{
				CreateDirectoryW(m_savePath,NULL);	// ��鱣��·��
				// ���챣�����ļ�������
				PathName = m_savePath + _T("\\");
				PathName += tempStr;
				PathName += _T("_dst");
				if(PathName.GetLength()>MAX_PATH-40)// ���·�����Ƿ����
					PathName.Left(MAX_PATH-40);
				// �������α�������ļ���
				CreateDirectoryW(PathName,NULL);

				// �������֡��ָʾ���ͼƬ�ļ�·��ȫ��
				PathName += _T("\\");
				PathName += FileName;
				tempStr.Format(_T("_n%d"),(int)m_frameNum);
				PathName += tempStr;
				PathName += _T(".jpg");

				imwrite(T2A(PathName),currPic);
			}
			else
				AfxMessageBox(_T("���󣺱���·��Ϊ��"),MB_OK,0);
			break;
		}
	case 2:		// ���ͼ���ļ��У��Զ�����
		{
			if (m_savePath!=_T(""))
			{
				CreateDirectoryW(m_savePath,NULL);//��鱣���ļ��У�û���򴴽�
				tempStr = GetTitleFromFileName(m_imgsPath,TRUE);//�õ���ǰ������ļ�����

				// �ڵ�ǰ����·�����½��ļ��У������ļ�·��ȫ��
				PathName = m_savePath + _T("\\");
				PathName += tempStr;
				PathName += _T("_dst");
				if(PathName.GetLength()>MAX_PATH-40)// ���·�����Ƿ����
					PathName.Left(MAX_PATH-40);
				//�������α�������ļ���
				CreateDirectoryW(PathName,NULL);

				// ���챣��ͼƬ�ļ���
				PathName += _T("\\");
				PathName += FileName;
				PathName += _T(".jpg");

				imwrite(T2A(PathName),currPic);
			}
			else
				AfxMessageBox(_T("���󣺱���·��Ϊ��"),MB_OK,0);
			break;
		}
	default:
		break;
	}
}



// ��Ӧ�϶��ļ�����
void CMarkDlg::OnDropFiles(HDROP hDropInfo)
{
	CString dropPath = _T("");
	wchar_t path[200];
	DragQueryFile(hDropInfo, 0, path, sizeof(path));
	dropPath = path;
	DragFinish(hDropInfo);
	//	AfxMessageBox(dropPath,MB_OK,0);

	int dot,len;
	CString ext = _T("");
	dot = dropPath.ReverseFind('.');
	len = dropPath.GetLength();

	// �Ͻ��������ļ���
	if (GetFileAttributes(dropPath) == FILE_ATTRIBUTE_DIRECTORY)
	{
		m_imgsPath = dropPath;
		SetDlgItemText(IDC_IMGSPATH,m_imgsPath);
		isSeeking = false; // ȡ����һ�����
		OnBnClickedNextimg();
		return;
	}

	// û����չ�����ַ���Ϊ�գ�ֱ�ӷ���
	if (dot==-1 || len==0)
		return;

	// ��ȡ��չ��
	ext = dropPath.Right(len-dot-1);

	// ��ͼƬ�ļ�
	if (ext==_T("jpg") || ext==_T("bmp") || ext==_T("png") || ext==_T("jpeg") || 
		ext==_T("JPG") || ext==_T("BMP") || ext==_T("PNG") || ext==_T("JPEG"))
	{
		currFileName = dropPath;
		OpenImage(currFileName);
		isSeeking = false;
		srcMode = 0;
	}
	// ����Ƶ�ļ�
	if (ext==_T("avi") || ext==_T("mpg") || ext==_T("mp4") || ext==_T("wmv") || 
		ext==_T("AVI") || ext==_T("MPG") || ext==_T("MP4") || ext==_T("WMV"))
	{
		currFileName = dropPath;
		OpenVideo(currFileName);
		srcMode = 1;
	}

	CDialog::OnDropFiles(hDropInfo);
}


// ����ѡ��Ի�����в�������
void CMarkDlg::OnBnClickedSettings()
{
	// ����ǰ���ø�ֵ���ӶԻ�����ʾ
	ReadINI(inipath);

	Settings.m_radio = saveMode;
	Settings.ifFixed = fixedLabel;
	Settings.ifAutoSave = fixedSave;
	Settings.m_width = fixedSize.width;
	Settings.m_height = fixedSize.height;
	Settings.ifSaveOrigin = saveOriScale;

	// ȡ����������ֵ
	if (Settings.DoModal()==IDOK)
	{
		saveMode = Settings.m_radio;
		fixedLabel = Settings.ifFixed;
		fixedSave = Settings.ifAutoSave;
		fixedSize.width = Settings.m_width;
		fixedSize.height = Settings.m_height;
		saveOriScale = Settings.ifSaveOrigin;

		WriteINI(inipath);
	}
}


// ����д��INI�ļ��ķ�װ����
void CMarkDlg::WriteIntValue(char* lpAppName, char* lpKeyName, int Value, char* lpFileName)
{
	char ValBuf[16];
	sprintf_s(ValBuf,16,"%d",Value);
	WritePrivateProfileStringA(lpAppName, lpKeyName, ValBuf, lpFileName );
}


// д��INI�ļ�
void CMarkDlg::WriteINI(char * path)
{
	WriteIntValue("Configuration", "ifFixed", (int)fixedLabel, path);
	WriteIntValue("Configuration", "ifAutoSave", (int)fixedSave, path);
	WriteIntValue("Configuration", "ifSaveOrigin", (int)saveOriScale, path);

	WriteIntValue("Configuration", "SaveMode", saveMode, path);
	WriteIntValue("Configuration", "fixedSizeWidth", fixedSize.width, path);
	WriteIntValue("Configuration", "fixedSizeHeight", fixedSize.height, path);
}


// ��ȡINI�ļ��Ի�ȡ���ò���
void CMarkDlg::ReadINI(char * path)
{
	fixedLabel = (GetPrivateProfileIntA("Configuration", "ifFixed", 0, path))>0;
	fixedSave = (GetPrivateProfileIntA("Configuration", "ifAutoSave", 0, path))>0;
	saveOriScale = (GetPrivateProfileIntA("Configuration", "ifSaveOrigin", 1, path))>0;

	saveMode = GetPrivateProfileIntA("Configuration", "SaveMode", 2, path);
	fixedSize.width = GetPrivateProfileIntA("Configuration", "fixedSizeWidth", 35, path);
	fixedSize.height = GetPrivateProfileIntA("Configuration", "fixedSizeHeight", 40, path);

	if (saveMode>4)
		saveMode = 2;
}

