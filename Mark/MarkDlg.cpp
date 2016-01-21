// MarkDlg.cpp : 实现文件
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
// 开头几行代码设置的链接条件：
// Debug下，直接链接库如 opencv_core248d.lib （路径已在VS环境中设置）
// Release下，链接对应的静态库如 opencv_core248.lib 等
// 就不必每次到项目属性里改了，且修改OpenCV版本或路径时，直接改define即可

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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMarkDlg 对话框




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


// CMarkDlg 消息处理程序

BOOL CMarkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_SHOWNORMAL);
	CenterWindow();

	// 初始化代码
	m_pic.GetWindowRect(&rcImg);//获取Picture控件相对屏幕左上角的坐标
	ScreenToClient(rcImg);	//获取Picture控件相对对话框客户区左上角的坐标
	rcImg.DeflateRect(-11,-11,11,11);

	// 初始化默认图片存放和保存路径
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

	// ini文件初始化
	GetModuleFileNameA(0, inipath, MAX_PATH);
	inipath[strlen(inipath)-3] = '\0';
	strcat_s(inipath,"ini");					// 通过替换扩展名部分得到INI完整路径
	if((GetFileAttributesA(inipath))==-1)		// 读取INI文件
		WriteINI(inipath);
	ReadINI(inipath);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMarkDlg::OnPaint()
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
		CDialog::OnPaint();
	}
	if (!imgShow.empty())
		ShowImage(imgShow,IDC_PIC,1);
	if (labelExists)
		reDrawRect(IDC_PIC);
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMarkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// 由Mat类型构建CImage类
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


// 自文件路径全名得到文件名，第二参数决定是否留下扩展名
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


// 选择文件夹
CString CMarkDlg::GetPath()
{
	TCHAR szDir[MAX_PATH];//用来存储路径的字符串
	CString strPath = _T("");
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = this->m_hWnd;
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = szDir;
	bInfo.lpszTitle = _T("请选择路径: ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	bInfo.lpfn = NULL;
	bInfo.lParam = 0;
	bInfo.iImage = 0;

	LPITEMIDLIST lpDlist; //用来保存返回信息的IDList
	lpDlist = SHBrowseForFolder(&bInfo) ; //显示选择对话框
	if(lpDlist != NULL)   //用户按了确定按钮
	{
		SHGetPathFromIDList(lpDlist, szDir);//把项目标识列表转化成字符串
		strPath = szDir; //将TCHAR类型的字符串转换为CString类型的字符串
	}
	return strPath;
}




// 操纵打开文件对话框，输入打开文件类型和默认路径，返回文件完整路径
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

	if (dlg.DoModal()==IDOK)	// 从“打开文件”对话框中提取文件路径
	{
		FileName=dlg.GetPathName();
	}
	return FileName;
}


// 用于显示Mat格式的图像到Picture控件上
int CMarkDlg::ShowImage(Mat image, int nID, bool ifFlush)
{
	if (image.empty())
		return 0;

	// 构造画图区域矩形
	GetDlgItem(nID)->GetClientRect(rcImg);
	CDC* pDC = GetDlgItem(nID)->GetDC();
	if (pDC==NULL)
		return -1;

	// 根据参数选择是否画图前先填充区域矩形
	if (ifFlush)
		pDC->FillSolidRect(rcImg,RGB(128,128,128));

	// 在显示前根据图像和显示区域的大小设置缩放
	int newTop, newLeft;

	if ((image.rows<=rcImg.Height())&&(image.cols<=rcImg.Width()))
	{
		// 若图片比绘图区域小，则保持原大小显示并将其置于中央
		newTop = (rcImg.Height()-image.rows)/2;
		newLeft = (rcImg.Width()-image.cols)/2;
		rcImg.SetRect(newLeft,newTop,newLeft+image.cols,newTop+image.rows);
		imgShow = image;
	}
	else
	{
		// 构造图像在绘图控件中按比例缩放后的大小
		double newWidth, newHeight;
		double picScale = (double)image.cols/(double)image.rows;
		double rcScale = (double)rcImg.Width()/(double)rcImg.Height();
		if (picScale>rcScale)
		{
			newWidth = rcImg.Width();
			newHeight = newWidth/picScale;
			newTop = rcImg.top + (rcImg.Height()-(int)newHeight)/2;
			// 重新定义画图矩形大小
			rcImg.SetRect(rcImg.left,newTop,rcImg.right,newTop+(int)newHeight);
		}
		else // 按高宽比不同而有不同计算
		{
			newHeight = rcImg.Height();
			newWidth = newHeight*picScale;
			newLeft = rcImg.left + (rcImg.Width()-(int)newWidth)/2;
			rcImg.SetRect(newLeft,rcImg.top,newLeft+(int)newWidth,rcImg.bottom);
		}

		// 构造新的uchar图像以供显示
		Size dsize = Size((int)newWidth,(int)newHeight);
		imgShow = Mat(dsize,CV_8UC3);
		resize(image,imgShow,dsize,0,0,INTER_LINEAR);
	}

	// 转换为CImage类的图像（调用函数）
	CImage cimg;
	MatToCImage(imgShow,cimg);
	// 显示到控件
	cimg.Draw(pDC->m_hDC,rcImg);
	// 释放资源
	ReleaseDC(pDC);
	cimg.Destroy();
	return 1;
}


// 画矩形函数
void CMarkDlg::DrawRect(int nID, CPoint pt)
{
	CDC * pRDC = GetDlgItem(nID)->GetDC();	// 画图DC
	pRDC->SelectStockObject(NULL_BRUSH);	// 刷子透明

	CPen myPen(PS_SOLID, 2, RGB(255,0,0));	// 创建画笔
	DeleteObject(pRDC->SelectObject(myPen));

	rcTemp = CRect(ptStart,ptTemp); // 上次调用函数画的矩形
	rcLabel = CRect(ptStart,pt);	// 本次要画的矩形

	pRDC->SetROP2(R2_NOTXORPEN);	// 擦除上次的矩形
	pRDC->Rectangle(rcTemp);		// 随画随擦
	pRDC->Rectangle(rcLabel);

	CString winTitle = _T("图像标注 - ");
	winTitle += GetTitleFromFileName(currFileName,TRUE);
	// 在程序窗口标题上实时显示当前矩形的长宽
	winTitle.AppendFormat(_T(" - %d X %d"),abs(rcLabel.Width()),abs(rcLabel.Height()));
	SetWindowTextW(winTitle);

	ReleaseDC(pRDC);
	ptTemp = pt;
}


// 重画最后定型的矩形
void CMarkDlg::reDrawRect(int nID)
{
	CDC * pRDC = GetDlgItem(nID)->GetDC();	// 画图DC
	pRDC->SelectStockObject(NULL_BRUSH);	// 刷子透明
	CPen myPen(PS_SOLID, 2, RGB(255,0,0));	// 创建画笔
	DeleteObject(pRDC->SelectObject(myPen));
	pRDC->Rectangle(rcLabel);
	ReleaseDC(pRDC);
}


// 根据起始点和结束点生成标注框所用的矩形
cv::Rect CMarkDlg::CreateRect(CPoint pt1, CPoint pt2, bool zoom)
{
	// 比较x1,y1,x2,y2的相对大小，创建矩形
	int rx = (pt2.x<pt1.x)?pt2.x:pt1.x;
	int ry = (pt2.y<pt1.y)?pt2.y:pt1.y;
	int rw = ((pt2.x>pt1.x)?pt2.x:pt1.x) - rx;
	int rh = ((pt2.y>pt1.y)?pt2.y:pt1.y) - ry;

	if (zoom)
	{
		// 若原图尺寸过大而显示时有缩放，则进行运算，缩放回去
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


// 创建单张标注截取的区域图像
void CMarkDlg::CreateLabelImg(CPoint pt1, CPoint pt2)
{
	if (imgShow.empty())
	{
		// AfxMessageBox(_T("请先打开图片或视频进行标注"),MB_OK,0);
		return;
	}
	if (!labelExists)
	{
		AfxMessageBox(_T("请先选出标注矩形框"),MB_OK,0);
		return;
	}
	// 创建标注矩形
	cv::Rect rcRoi = CreateRect(pt1,pt2,saveOriScale);

	// 拷贝供处理的源图像
	IplImage img1;
	if (saveOriScale)
		img1 = currPic;
	else
		img1 = imgShow;

	// 检查ROI小于源图像大小
	if (rcRoi.width>img1.width || rcRoi.height>img1.height)	
	{
		AfxMessageBox(_T("设置截图区域出现错误，将返回"),MB_OK,0);
		return;
	}

	// 写入标注矩形队列
	rcQueue.push(rcRoi);

	// 设置感兴趣区域ROI
	cvSetImageROI(&img1, rcRoi);

	// 创建目标图像，此时cvGetSize返回的已经是ROI区域的参数
	IplImage *img2 = cvCreateImage(cvGetSize(&img1),
		img1.depth,
		img1.nChannels);

	// 转换为Mat格式并写入队列
	Mat newLabelImg;
	newLabelImg = Mat(img2, false);
	imgQueue.push(newLabelImg);

	// 复制
	cvCopy(&img1, img2, NULL);
	// 重置ROI
	cvResetImageROI(&img1);
	// 此处释放img2指针资源释放会导致保存时出错。先不释放看看，希望Mat能自动管理，或待保存后再释放
	// cvReleaseImage(&img2);
}



// 保存标注到文件
// 输入参数：图片源类型，0为单张，1为视频，2为文件夹
void CMarkDlg::SaveLabel(int Mode)
{
	if (imgShow.empty())
	{
		AfxMessageBox(_T("请先打开图片或视频进行标注"),MB_OK,0);
		return;
	}
	if (!labelExists)
	{
		AfxMessageBox(_T("请先选出标注矩形框"),MB_OK,0);
		return;
	}

	int labelCount = imgQueue.size();
	USES_CONVERSION;
	CString LogChar = _T(""); // 存入TXT文件的字符串
	CString TXTPath = _T(""); // TXT文件名称
	CString PathName = _T("");// 路径加文件全名
	CString FileName = _T("");// 仅文件名（不带扩展名）
	CString tempStr = _T(""); // 临时
	CTime tm = CTime::GetCurrentTime();	// 得到时间
	CString TimeStr = tm.Format("_20%y_%m_%d_%H_%M_%S");
	//	FileName.Format(_T("Img_%.3d"),labelNum);
	tempStr = GetTitleFromFileName(currFileName,FALSE);
	FileName = tempStr + TimeStr; // 构造文件标题加时间戳

	switch(Mode) // 根据图片源模式选择保存方式
	{
	case 0:		// 单张图片，打开保存文件对话框进行保存
		{
			// 若设置为1或2，则保存记录文件
			if (saveMode!=0)
			{
				// 直接保存到图片源目录里
				int namelen = (GetTitleFromFileName(currFileName,TRUE)).GetLength();
				CString NewTXTPath = currFileName.Left(currFileName.GetLength()-namelen);
				NewTXTPath += tempStr;
				NewTXTPath += _T(".txt");
				// 保存记录，LogChar前边是图像名
				cv::Rect rcRoi;
				LogChar = tempStr;
				LogChar.AppendFormat(_T(" %d"), labelCount);
				for(int j=0; j<labelCount; j++)	// 将同一张图的所有标注记录为一行
				{
					rcRoi = rcQueue.front();
					LogChar.AppendFormat(_T(" %d %d %d %d"),
						rcRoi.x,rcRoi.y,rcRoi.width,rcRoi.height);
					rcQueue.pop();
				}
				char * c = T2A(LogChar);
				SaveTXT(NewTXTPath, c, CStringA(LogChar).GetLength());
			}
			// 若设置为0或2，则保存标注图像
			if (saveMode!=1)
			{
				for(int i=0; i<labelCount; i++)	// 依次保存列表中所有标注文件
				{
					CString tpName = FileName;
					tpName.AppendFormat(_T("_%.2d"),i);
					CFileDialog dlg( FALSE, _T("jpg"), tpName,	// 默认文件名（不带扩展名）
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						_T("JPG文件(*.jpg)|*.jpg |BMP文件(*.bmp)|*.bmp |PNG文件(*.png)|*.png ||"));
					if (dlg.DoModal()==IDOK)
					{
						PathName = dlg.GetPathName();	// 路径全名
						imwrite(T2A(PathName),imgQueue.front());
						//labelNum++;
					}
					imgQueue.pop();
				}
			}
			break;
		}
	case 1:		// 视频文件中的帧，自动保存
		{
			if (m_savePath!=_T(""))
			{
				CreateDirectoryW(m_savePath,NULL);	// 检查保存路径
				// 构造保存子文件夹名称
				PathName = m_savePath + _T("\\");
				PathName += tempStr;
				PathName += _T("_dst");
				if(PathName.GetLength()>MAX_PATH-40)// 检查路径名是否过长
					PathName.Left(MAX_PATH-40);
				// 创建本次保存的子文件夹
				CreateDirectoryW(PathName,NULL);

				// 若设置为1或2，则保存记录文件
				if (saveMode!=0)
				{
					// 构造TXT文件的全名，tempStr此时保存的是视频文件名
					TXTPath = PathName + _T("\\");
					TXTPath += tempStr;
					TXTPath += _T(".txt");
					// 在视频源目录里也保存一份
					int namelen = (GetTitleFromFileName(currFileName,TRUE)).GetLength();
					CString NewTXTPath = currFileName.Left(currFileName.GetLength()-namelen);
					NewTXTPath += tempStr;
					NewTXTPath += _T(".txt");
					// 保存记录，LogChar前边是视频名-帧数
					cv::Rect rcRoi;
					LogChar = tempStr + _T("-");
					LogChar.AppendFormat(_T("%d %d"), (int)m_frameNum, labelCount);
					for(int j=0; j<labelCount; j++)	// 将同一张图的所有标注记录为一行
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
				// 若设置为0或2，则保存标注图像
				if (saveMode!=1)
				{
					// 构造加上帧数指示后的图片文件路径全名，此操作会更改PathName，故TXTPath的构造应在此前。
					PathName += _T("\\");
					PathName += FileName;
					tempStr.Format(_T("_n%d"),(int)m_frameNum);	// 加上视频帧参数
					PathName += tempStr;

					CString tpName;
					for(int i=0; i<labelCount; i++)	// 依次保存列表中所有标注文件
					{
						tpName = PathName;
						tpName.AppendFormat(_T("_%.2d"),i);		// 区分同一帧中的不同标注，最多99
						tpName += _T(".jpg");
						imwrite(T2A(tpName),imgQueue.front());
						imgQueue.pop();
						labelNum++;
					}
				}
			}
			else
				AfxMessageBox(_T("错误：保存路径为空"),MB_OK,0);
			break;
		}
	case 2:		// 浏览图像文件夹，自动保存
		{
			if (m_savePath!=_T(""))
			{
				CreateDirectoryW(m_savePath,NULL);//检查保存文件夹，没有则创建
				tempStr = GetTitleFromFileName(m_imgsPath,TRUE);//得到当前浏览的文件夹名

				// 在当前保存路径下新建文件夹，构造文件路径全名
				PathName = m_savePath + _T("\\");
				PathName += tempStr;
				PathName += _T("_dst");
				if(PathName.GetLength()>MAX_PATH-40)// 检查路径名是否过长
					PathName.Left(MAX_PATH-40);
				//创建本次保存的子文件夹
				CreateDirectoryW(PathName,NULL);

				// 若设置为1或2，则保存记录文件
				if (saveMode!=0)
				{
					// 构造TXT文件的全名，tempStr此时保存的是图片文件夹名
					TXTPath = PathName + _T("\\");
					TXTPath += tempStr;
					TXTPath += _T(".txt");
					// 在图片源目录里也保存一份
					CString NewTXTPath = m_imgsPath + _T("\\");
					NewTXTPath += tempStr;
					NewTXTPath += _T(".txt");
					// 保存记录，LogChar前边是文件夹名\图片名
					cv::Rect rcRoi;
					LogChar = tempStr + _T("\\");
					LogChar += GetTitleFromFileName(currFileName,TRUE);
					LogChar.AppendFormat(_T(" %d"), labelCount);
					for(int j=0; j<labelCount; j++)	// 将同一张图的所有标注记录为一行
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
				// 若设置为0或2，则保存标注图像
				if (saveMode!=1)
				{
					// 构造保存图片文件名，此操作会更改PathName，故TXTPath的构造应在此前。
					PathName += _T("\\");
					PathName += FileName;
					CString tpName;
					for(int i=0; i<labelCount; i++)	// 依次保存列表中所有标注文件
					{
						tpName = PathName;
						tpName.AppendFormat(_T("_%.2d"),i);		// 区分同一帧中的不同标注，最多99
						tpName += _T(".jpg");
						imwrite(T2A(tpName),imgQueue.front());
						imgQueue.pop();
						labelNum++;
					}
				}
			}
			else
				AfxMessageBox(_T("错误：保存路径为空"),MB_OK,0);
			break;
		}
	default:
		break;
	}
}


// 清除当前所有标注矩形框和数据
void CMarkDlg::ClearLabel()
{
	if (!imgShow.empty())
		ShowImage(imgShow,IDC_PIC,0); // 重新显示当前图片
	else
		Invalidate();
	// 清空数据队列
	while(!rcQueue.empty())
		rcQueue.pop();
	while(!imgQueue.empty())
		imgQueue.pop();
	labelExists = false;
}


// 跳帧函数
void CMarkDlg::JumpFrame(bool direct)
{
	if (!myCap.isOpened())
	{
		AfxMessageBox(_T("请先载入视频文件！"),MB_OK,0);
		return;
	}
	// 取得全部帧数
	m_frameCount = myCap.get(CV_CAP_PROP_FRAME_COUNT);
	SetDlgItemInt(IDC_FRMCOUNT,(int)m_frameCount);

	// 取得当前帧数
	m_frameNum = GetDlgItemInt(IDC_FRAMENUM);
	m_step = GetDlgItemInt(IDC_STEP);

	if (direct)	// 向后跳>>
		m_frameNum += m_step;
	else		// 向前跳<<
		m_frameNum -= m_step;

	if (m_frameNum<0)
	{
		AfxMessageBox(_T("已经到达视频开头！"),MB_OK,0);
		m_frameNum = 0;
	}
	if (m_frameNum>=m_frameCount)
	{
		AfxMessageBox(_T("已经到达视频结尾！"),MB_OK,0);
		m_frameNum = m_frameCount-1;
	}
	myCap.set(CV_CAP_PROP_POS_FRAMES, m_frameNum);
	SetDlgItemInt(IDC_FRAMENUM,(int)m_frameNum);
	myCap >> currPic;

	ShowImage(currPic,IDC_PIC,0);
	// 释放上一张图占用的数据变量
	while(!rcQueue.empty())
		rcQueue.pop();
	while(!imgQueue.empty())
		imgQueue.pop();
	labelExists = false;
}


// 据全路径名打开图像文件，被打开单张图片和遍历文件夹的函数调用
int CMarkDlg::OpenImage(CString currFileName)
{
	USES_CONVERSION;
	char * pFileName = T2A(currFileName);
	currPic =  imread(pFileName,1);
	if (currPic.empty())
	{
		AfxMessageBox(_T("载入图片失败！"),MB_OK,0);
		currPic.release();	// 确认释放资源，免得重画时出错
		imgShow.release();
		m_pic.GetWindowRect(&rcImg);// 重置rcImg,同开始一样
		ScreenToClient(rcImg);
		rcImg.DeflateRect(-11,-11,11,11);
		return 0;
	}
	// 显示
	ShowImage(currPic,IDC_PIC,1);
	// 在标题上显示当前打开的文件名
	CString winTitle = _T("图像标注 - ");
	winTitle += GetTitleFromFileName(currFileName,TRUE);
	SetWindowTextW(winTitle);

	// 释放上一张图占用的数据变量
	while(!rcQueue.empty())
		rcQueue.pop();
	while(!imgQueue.empty())
		imgQueue.pop();
	labelExists = false;
	return 1;
}


// 据全路径名打开视频文件，被按钮打开视频和拖动打开视频的函数调用
int CMarkDlg::OpenVideo(CString currFileName)
{
	USES_CONVERSION;
	char * pFileName = T2A(currFileName);
	if (myCap.isOpened())
		myCap.release();
	myCap.open(pFileName);

	if (!myCap.isOpened())
	{
		AfxMessageBox(_T("载入视频失败！"),MB_OK,0);
		currPic.release();	// 确认释放资源，免得重画时出错
		imgShow.release();
		m_pic.GetWindowRect(&rcImg);// 重置rcImg,同开始一样
		ScreenToClient(rcImg);
		rcImg.DeflateRect(-11,-11,11,11);
		return 0;
	} 

	// 取得全部帧数
	m_frameCount = myCap.get(CV_CAP_PROP_FRAME_COUNT);
	SetDlgItemInt(IDC_FRMCOUNT,(int)m_frameCount);

	// 取得当前帧数
	m_frameNum = GetDlgItemInt(IDC_FRAMENUM);
	//if (m_frameNum>=m_frameCount)
	//{
	//	AfxMessageBox(_T("当前帧数超过总帧数，转到视频开头"),MB_OK,0);
	//	SetDlgItemInt(IDC_FRAMENUM,0);
	//	m_frameNum = 0;
	//}
	myCap.set(CV_CAP_PROP_POS_FRAMES, m_frameNum);
	myCap >> currPic;
	// 显示到图片上
	ShowImage(currPic,IDC_PIC,1);
	// 在标题上显示当前打开的文件名
	CString winTitle = _T("图像标注 - ");
	winTitle += GetTitleFromFileName(currFileName,TRUE);
	SetWindowTextW(winTitle);
	// 释放上一次占用的数据变量
	while(!rcQueue.empty())
		rcQueue.pop();
	while(!imgQueue.empty())
		imgQueue.pop();
	labelExists = false;
	isSeeking = false;
	m_ifWheel.SetCheck(BST_CHECKED);
	return 1;
}



// 打开单张图片按钮响应
void CMarkDlg::OnBnClickedOpenpic()
{
	m_ifWheel.SetCheck(BST_UNCHECKED);
	if (myCap.isOpened())
		myCap.release();

	// 将字符串格式转换一下，因为是Unicode
	currFileName = FindFile(0,_T(""));
	if (OpenImage(currFileName)==0)	// 打开图像，如失败返回
		return;
	isSeeking = false;
	srcMode = 0;
}


// 打开视频按钮响应
void CMarkDlg::OnBnClickedOpenvideo()
{
	// 初始化帧数为0
	SetDlgItemInt(IDC_FRAMENUM,0);
	// 打开视频文件
	currFileName = FindFile(1,_T(""));
	if (OpenVideo(currFileName)==0)	// 打开图像，如失败返回
		return;
	srcMode = 1;
}



// 向后跳帧
void CMarkDlg::OnBnClickedNextframe()
{
	JumpFrame(true);
}

// 向前跳帧
void CMarkDlg::OnBnClickedPrevframe()
{
	JumpFrame(false);
}


// 鼠标移动过程中画矩形
void CMarkDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	point.x -= 11;		// 不晓得什么原理，就是差11个点
	point.y -= 11;

	// 如果在画图状态且鼠标在图像区域内，则画矩形
	if (isDrawing)
	{
		// 如果超出图像区域，则限定坐标值
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


	//// 计算鼠标当前坐标值
	//point.x -=  rcImg.left;	//point获取的是鼠标相对对话框客户区左上角的坐标，减去rcImg.left和
	//point.y -=  rcImg.top;	//rcImg.top后，即为鼠标相对Picture控件左上角的坐标

	////如果鼠标位于Picture控件之外，不显示其坐标
	//if ( point.x>(rcImg.Width()) || point.y>(rcImg.Height())
	//	|| point.x<0 || point.y<0 )  
	//	point = (-1,-1);

	//SetDlgItemInt(IDC_X, (int)ptStart.x);
	//SetDlgItemInt(IDC_Y, (int)ptStart.y);

	CDialog::OnMouseMove(nFlags, point);
}


// 左键按下
void CMarkDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	ptStart.x = point.x - 11;	// 校正11点的误差
	ptStart.y = point.y - 11;

	// 如果鼠标落点在图像区域内，则设定画图开始
	if ( ptStart.x<rcImg.right && ptStart.y<rcImg.bottom
		&& ptStart.x>rcImg.left && ptStart.y>rcImg.top )
	{
		isDrawing = true;			// 开始画矩形
		ptTemp = ptStart;			// 使第一次擦除无效
		// 清除其他的标注矩形
		// ClearLabel();
	}

	CDialog::OnLButtonDown(nFlags, point);
}


// 左键抬起
void CMarkDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	point.x -= 11;
	point.y -= 11;

	ptEnd = ptTemp;				// 保存矩形最终落点

	ptStart.x -= rcImg.left;	// 转换坐标为相对于画图区域矩形的坐标
	ptStart.y -= rcImg.top;
	ptEnd.x -= rcImg.left;
	ptEnd.y -= rcImg.top;

	if ((ptEnd.x-ptStart.x)==0 
		|| (ptStart.y-ptEnd.y)==0) // 如果没有新矩形，只单击一下
	{
		if (fixedLabel)
		{
			ptStart.x = point.x - fixedSize.width/2;
			ptStart.y = point.y - fixedSize.height/2;
			ptEnd.x = ptStart.x + fixedSize.width; // 之所以直接加而不是从中间加一半，是考虑到奇数边长的问题
			ptEnd.y = ptStart.y + fixedSize.height;
			rcLabel = CRect(ptStart,ptEnd);	// 本次要画的矩形
			reDrawRect(IDC_PIC);
			labelExists = true;

			ptStart.x -= rcImg.left;	// 转换坐标为相对于画图区域矩形的坐标
			ptStart.y -= rcImg.top;
			ptEnd.x -= rcImg.left;
			ptEnd.y -= rcImg.top;

			CreateLabelImg(ptStart, ptEnd);	// 创建区域图像

			if (fixedSave)	// 如果选择了自动保存，则直接保存标注。
				OnBnClickedSave();
		}
		else
			labelExists = false;
	}
	else if (isDrawing) // 抬起左键时将最终矩形结果画为红色
	{
		reDrawRect(IDC_PIC);
		labelExists = true;
		CreateLabelImg(ptStart, ptEnd);	// 创建区域图像
	}
	isDrawing = false;

	//// 如果落点在图形区域内
	//if(point.x<rcImg.right && point.y<rcImg.bottom
	//	&& point.x>rcImg.left && point.y>rcImg.top )
	//{	
	//}

	//// 如果位于Picture控件之外，设置坐标为-1
	//if ( ptStart.x>(rcImg.Width()) || ptStart.y>(rcImg.Height())
	//	|| ptStart.x<0 || ptStart.y<0 )  
	//	ptStart = (-1,-1);

	CDialog::OnLButtonUp(nFlags, point);
}


// 保存按钮
void CMarkDlg::OnBnClickedSave()
{
	if (saveMode>2)
		SaveWholeImg(srcMode);
	else
		SaveLabel(srcMode);
}


// 清除当前标注框及数据
void CMarkDlg::OnBnClickedClear()
{
	ClearLabel();
}


// 双击保存响应
void CMarkDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!fixedLabel)
		OnBnClickedSave();

	CDialog::OnLButtonDblClk(nFlags, point);
}



// 右键清除当前标注框及数据
void CMarkDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	OnBnClickedClear();

	CDialog::OnRButtonDown(nFlags, point);
}


// 键盘映射部分操作
BOOL CMarkDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN) //有按键按下时
	{
		if (IsCTRLPressed())	//CTRL按下时
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


// 设置指定保存目录路径
void CMarkDlg::OnBnClickedSetSavePath()
{
	CString path = GetPath();
	// 用户选取文件夹，更新路径
	if (path!=_T(""))
	{
		m_savePath = path;
		SetDlgItemText(IDC_SAVEPATH,m_savePath);
	}
}


// 设置指定图像所在文件夹
void CMarkDlg::OnBnClickedSetimgpath()
{
	CString path = GetPath();
	// 用户选取文件夹，更新路径
	if (path!=_T(""))
	{
		m_imgsPath = path;
		SetDlgItemText(IDC_IMGSPATH,m_imgsPath);
		isSeeking = false; // 结束当前查找
	}
}


// 鼠标滚轮映射跳帧方向
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


// 根据扩展名判断是否为图像文件
int CMarkDlg::isImageFile()
{
	int dot,len;
	CString ext = _T("");
	// 需配合查找文件的函数使用，ff是CFileFind类对象
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


// 点击读取文件夹中下一张图像
void CMarkDlg::OnBnClickedNextimg()
{
	if (m_imgsPath==_T(""))
	{
		AfxMessageBox(_T("图像源路径异常，请检查设置！"),MB_OK,0);
		return;
	}
	// 如无此文件夹则创建，此时没有图片
	if (CreateDirectoryW(m_imgsPath,NULL)!=0)
	{
		AfxMessageBox(_T("在默认路径找不到含图像文件夹，请确认文件夹位置。"),MB_OK,0);
		return;
	}

	USES_CONVERSION;
	CString pathWild = m_imgsPath + _T("\\*.*");	// 欲寻找的文件路径全名
	CString currName = _T("");						// 当前找到的文件名

	if (!isSeeking)	// 如当前没在查找，则开启一个新查找
	{
		if (myCap.isOpened()) // 如当前打开了视频文件则关闭之
			myCap.release();

		if (ff.FindFile(pathWild)==0)
		{
			AfxMessageBox(_T("图像源路径异常，请检查设置！"),MB_OK,0);
			ff.Close();
			return;
		}
		do // 跳过所有的目录和./..至第一个实际的文件
		{
			if (ff.FindNextFile()==0)	// 第一个文件就是最后一个时
			{
				if (isImageFile()!=0)	// 如是图像文件则打开之
				{
					OpenImage(currFileName);	// 打开图像
					AfxMessageBox(_T("已浏览至最后一个文件。"),MB_OK,0);
					srcMode = 2;
				}
				else
					AfxMessageBox(_T("找不到图像文件(*.jpg /*.bmp /*.png)！"),MB_OK,0);
				isSeeking = false;
				ff.Close();
				return;
			}
		} while (ff.IsDirectory() || ff.IsDots());

		if (isImageFile()!=0)	// 如第一个文件是图像文件则打开之
		{
			OpenImage(currFileName);
			srcMode = 2;
		}
		else	// 第一个不是图像文件就下一张
			NextImage();
		isSeeking = true;	// 标志开始查找
		m_ifWheel.SetCheck(BST_CHECKED);
	}
	else	// 如当前已经在遍历过程中则直接下一张
		NextImage();
}


// 文件夹中下一张图片
void CMarkDlg::NextImage()
{
	while(ff.FindNextFile()!=0)	// 找到下一个图像文件（不是最后一个）
	{
		if (isImageFile()!=0)	// 是图像文件则打开后返回
		{
			OpenImage(currFileName);
			srcMode = 2;
			return;
		}
	}
	if (isImageFile()!=0)	// 判断最后一个文件是否为图像
	{
		OpenImage(currFileName);	// 是则打开
		srcMode = 2;
	}
	AfxMessageBox(_T("已浏览至最后一个文件。"),MB_OK,0);
	isSeeking = false;
	ff.Close();
}



// 直接打开保存标注的文件夹
void CMarkDlg::OnBnClickedOpensvpath()
{
	if (m_savePath!=_T(""))	// 检查保存路径
		ShellExecute(NULL,_T("open"), m_savePath,NULL,NULL,SW_SHOWNORMAL);
}


// 保存TXT，参数是Write的参数，写入一个字符串，标记其长度 
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


// 视频截图功能实现，直接保存当前显示的原图像（saveMode>2）
void CMarkDlg::SaveWholeImg(int Mode)
{
	if (currPic.empty())	// 判断条件
	{
		AfxMessageBox(_T("请先打开图片或一帧视频"),MB_OK,0);
		return;
	}
	if (saveMode==4)	// 局部黑色块填充
	{
		if (!labelExists)
		{
			AfxMessageBox(_T("请先选出标注矩形框"),MB_OK,0);
			return;
		}

		// 画填充矩形
		cv::Rect rcRoi = CreateRect(ptStart,ptEnd,1);
		cv::rectangle(currPic,rcRoi,CV_RGB(0,0,0),CV_FILLED,8,0);
	}

	CString PathName = _T("");// 路径加文件全名
	CString FileName = _T("");// 仅文件名（不带扩展名）
	CString tempStr = _T(""); // 临时
	CTime tm = CTime::GetCurrentTime();	// 得到时间
	CString TimeStr = tm.Format("_20%y_%m_%d_%H_%M_%S");
	tempStr = GetTitleFromFileName(currFileName,FALSE);
	FileName = tempStr + TimeStr; // 构造文件标题加时间戳

	USES_CONVERSION;

	switch(Mode) // 根据图片源模式选择保存方式
	{
	case 0:		// 单张图片，打开保存文件对话框进行保存
		{
			CFileDialog dlg(FALSE,
				_T("jpg"),
				FileName,	// 默认文件名（不带扩展名）
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				_T("JPG文件(*.jpg)|*.jpg |BMP文件(*.bmp)|*.bmp |PNG文件(*.png)|*.png ||"));
			if (dlg.DoModal()==IDOK)
			{
				PathName = dlg.GetPathName();	// 路径全名
				imwrite(T2A(PathName),currPic);
			}
			break;
		}
	case 1:		// 视频文件中的帧，自动保存
		{
			if (m_savePath!=_T(""))
			{
				CreateDirectoryW(m_savePath,NULL);	// 检查保存路径
				// 构造保存子文件夹名称
				PathName = m_savePath + _T("\\");
				PathName += tempStr;
				PathName += _T("_dst");
				if(PathName.GetLength()>MAX_PATH-40)// 检查路径名是否过长
					PathName.Left(MAX_PATH-40);
				// 创建本次保存的子文件夹
				CreateDirectoryW(PathName,NULL);

				// 构造加上帧数指示后的图片文件路径全名
				PathName += _T("\\");
				PathName += FileName;
				tempStr.Format(_T("_n%d"),(int)m_frameNum);
				PathName += tempStr;
				PathName += _T(".jpg");

				imwrite(T2A(PathName),currPic);
			}
			else
				AfxMessageBox(_T("错误：保存路径为空"),MB_OK,0);
			break;
		}
	case 2:		// 浏览图像文件夹，自动保存
		{
			if (m_savePath!=_T(""))
			{
				CreateDirectoryW(m_savePath,NULL);//检查保存文件夹，没有则创建
				tempStr = GetTitleFromFileName(m_imgsPath,TRUE);//得到当前浏览的文件夹名

				// 在当前保存路径下新建文件夹，构造文件路径全名
				PathName = m_savePath + _T("\\");
				PathName += tempStr;
				PathName += _T("_dst");
				if(PathName.GetLength()>MAX_PATH-40)// 检查路径名是否过长
					PathName.Left(MAX_PATH-40);
				//创建本次保存的子文件夹
				CreateDirectoryW(PathName,NULL);

				// 构造保存图片文件名
				PathName += _T("\\");
				PathName += FileName;
				PathName += _T(".jpg");

				imwrite(T2A(PathName),currPic);
			}
			else
				AfxMessageBox(_T("错误：保存路径为空"),MB_OK,0);
			break;
		}
	default:
		break;
	}
}



// 响应拖动文件操作
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

	// 拖进来的是文件夹
	if (GetFileAttributes(dropPath) == FILE_ATTRIBUTE_DIRECTORY)
	{
		m_imgsPath = dropPath;
		SetDlgItemText(IDC_IMGSPATH,m_imgsPath);
		isSeeking = false; // 取消上一次浏览
		OnBnClickedNextimg();
		return;
	}

	// 没有扩展名或字符串为空，直接返回
	if (dot==-1 || len==0)
		return;

	// 提取扩展名
	ext = dropPath.Right(len-dot-1);

	// 是图片文件
	if (ext==_T("jpg") || ext==_T("bmp") || ext==_T("png") || ext==_T("jpeg") || 
		ext==_T("JPG") || ext==_T("BMP") || ext==_T("PNG") || ext==_T("JPEG"))
	{
		currFileName = dropPath;
		OpenImage(currFileName);
		isSeeking = false;
		srcMode = 0;
	}
	// 是视频文件
	if (ext==_T("avi") || ext==_T("mpg") || ext==_T("mp4") || ext==_T("wmv") || 
		ext==_T("AVI") || ext==_T("MPG") || ext==_T("MP4") || ext==_T("WMV"))
	{
		currFileName = dropPath;
		OpenVideo(currFileName);
		srcMode = 1;
	}

	CDialog::OnDropFiles(hDropInfo);
}


// 弹出选项对话框进行参数设置
void CMarkDlg::OnBnClickedSettings()
{
	// 将当前设置赋值进子对话框显示
	ReadINI(inipath);

	Settings.m_radio = saveMode;
	Settings.ifFixed = fixedLabel;
	Settings.ifAutoSave = fixedSave;
	Settings.m_width = fixedSize.width;
	Settings.m_height = fixedSize.height;
	Settings.ifSaveOrigin = saveOriScale;

	// 取得新设置数值
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


// 方便写入INI文件的封装函数
void CMarkDlg::WriteIntValue(char* lpAppName, char* lpKeyName, int Value, char* lpFileName)
{
	char ValBuf[16];
	sprintf_s(ValBuf,16,"%d",Value);
	WritePrivateProfileStringA(lpAppName, lpKeyName, ValBuf, lpFileName );
}


// 写入INI文件
void CMarkDlg::WriteINI(char * path)
{
	WriteIntValue("Configuration", "ifFixed", (int)fixedLabel, path);
	WriteIntValue("Configuration", "ifAutoSave", (int)fixedSave, path);
	WriteIntValue("Configuration", "ifSaveOrigin", (int)saveOriScale, path);

	WriteIntValue("Configuration", "SaveMode", saveMode, path);
	WriteIntValue("Configuration", "fixedSizeWidth", fixedSize.width, path);
	WriteIntValue("Configuration", "fixedSizeHeight", fixedSize.height, path);
}


// 读取INI文件以获取配置参数
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

