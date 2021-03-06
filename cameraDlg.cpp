// cameraDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "camera.h"
#include "cameraDlg.h"
#include "afxdialogex.h"
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//qfh add
#define use_unicode 0

#include "conio.h" //for AllocConsole();

#include "MessageLogContext.h"

using namespace std;
//qfh add end


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CcameraDlg 对话框

CcameraDlg::CcameraDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CAMERA_DIALOG, pParent)
	, m_nRadio_select_mode(-1)
	, m_nRadio_addr_type(-1) //we can set 0/1/2 to pick the radio as default
	, m_nRadio_data_type(-1)
	, m_userSetFormat(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcameraDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBOBOXEX, m_Comboboxex); //qfh add
	DDX_Control(pDX, IDC_RADIO_SENSOR, m_sensor_radio_btn);
	DDX_Radio(pDX, IDC_RADIO_SENSOR_INIT, m_nRadio_select_mode);
	DDX_Radio(pDX, IDC_RADIO_ADDR1, m_nRadio_addr_type);
	DDX_Radio(pDX, IDC_RADIO_DATA1, m_nRadio_data_type);
	DDX_Text(pDX, IDC_EDITBROWSE_INPUTFILE, m_SelectedPath); //not use DDX_Control

	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CcameraDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
#if defined(use_drop_list)
	ON_BN_CLICKED(IDC_RADIO_SENSOR, &CcameraDlg::OnBnClickedRadioSensor)
	ON_BN_CLICKED(IDC_RADIO_ACTUATOR, &CcameraDlg::OnBnClickedRadioActuator)
	ON_BN_CLICKED(IDC_RADIO_EEPROM, &CcameraDlg::OnBnClickedRadioEeprom)
	ON_CBN_DROPDOWN(IDC_COMBOBOXEX, &CcameraDlg::OnCbnDropdownComboboxex)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX, &CcameraDlg::OnCbnSelchangeComboboxex)
#else
	ON_BN_CLICKED(IDC_RADIO_SENSOR_INIT, &CcameraDlg::OnBnClickedRadioSensor)
	ON_BN_CLICKED(IDC_RADIO_SENSOR_RES, &CcameraDlg::OnBnClickedRadioSensor)
	ON_BN_CLICKED(IDC_RADIO_ACTUATOR, &CcameraDlg::OnBnClickedRadioSensor)
	ON_BN_CLICKED(IDC_RADIO_EEPROM, &CcameraDlg::OnBnClickedRadioSensor)
#endif
	ON_EN_CHANGE(IDC_EDITBROWSE_INPUTFILE, &CcameraDlg::OnEnChangeEditbrowseInputfile)
	ON_BN_CLICKED(IDC_BUTTON_TXT2XML, &CcameraDlg::OnBnClickedButtonTxt2xml)
	ON_BN_CLICKED(IDC_BUTTON_XML2TXT, &CcameraDlg::OnBnClickedButtonXml2txt)
	ON_BN_CLICKED(ID_QUIT, &CcameraDlg::OnBnClickedQuit)

	ON_EN_CHANGE(IDC_EDIT_FORMAT, &CcameraDlg::OnEnChangeEditGetUserFormat) /// When we parse, it will be called twice...
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_RADIO_ADDR1, &CcameraDlg::OnBnClickedRadioAddr1)
	ON_BN_CLICKED(IDC_RADIO_ADDR2, &CcameraDlg::OnBnClickedRadioAddr1)
	ON_BN_CLICKED(IDC_RADIO_ADDR3, &CcameraDlg::OnBnClickedRadioAddr1)
	ON_BN_CLICKED(IDC_RADIO_DATA1, &CcameraDlg::OnBnClickedRadioData1)
	ON_BN_CLICKED(IDC_RADIO_DATA2, &CcameraDlg::OnBnClickedRadioData1)
	ON_BN_CLICKED(IDC_RADIO_DATA3, &CcameraDlg::OnBnClickedRadioData1)
	ON_WM_DROPFILES()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CcameraDlg 消息处理程序

BOOL CcameraDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//qfh start
#if !defined(NDEBUG) //if it is debug mode
	AllocConsole();
	CDBG("initCousoleWindow");
	//#else
	freopen_s(&mLogFile, "log.txt", "w", stdout); //redirect stdout to log.txt  
	printf("%s %d printf\r\n", __FUNCTION__, __LINE__);
	//freopen("CON", "w", stdout); //recover stdout (for Windows)
#endif

	m_SelectedPath = "";
	fn = NULL;
	memset(&user_data, 0, sizeof(USER_DATA));

	m_rect.SetRect(0, 0, 0, 0);//初始化对话框大小
	GetClientRect(&m_rect); //获取对话框的大小

	m_sensor_radio_btn.SetCheck(1); // default selected
	m_nRadio_select_mode = SENSOR_INIT;
	CButton* radio_select_mode = (CButton*)GetDlgItem(IDC_RADIO_SENSOR_INIT);
	radio_select_mode->SetCheck(1);

	CButton* radio_addr_type = (CButton*)GetDlgItem(IDC_RADIO_ADDR2);
	radio_addr_type->SetCheck(1);
	m_nRadio_addr_type = 1;

	CButton* radio_data_type = (CButton*)GetDlgItem(IDC_RADIO_DATA2);
	radio_data_type->SetCheck(1);
	m_nRadio_data_type = 1;

	//如果把 CComboBoxEx 属性从下拉列表改成 drop down, 则这个有效果
	CString ComSelItem(_T("init mode"));
	m_Comboboxex.SetCurSel(0);
	m_Comboboxex.SetWindowText(ComSelItem);//

	//属性-->常规-->字符集-->使用多字节字符集 /可以解决各种编译问题

	m_p_Edit = (CEdit*)GetDlgItem(IDC_EDIT_FORMAT);
	m_p_Edit->SetLimitText(1024); //设置显示最大字符数, UINT_MAX = -1
	m_p_Edit->SetWindowText(_T("My Camera!"));//LPCTSTR
	m_p_Edit->ModifyStyle(0, ES_MULTILINE);

#if !use_unicode
	CDBG("we are using multi byte "); // print in the console windows
	string buf;
	for (int i = 0; defaultCueForser != NULL, i < 5; i++)
	{
		CDBG("%s", defaultCueForser[i].c_str());
		buf += defaultCueForser[i];
	}

	for (int i = 0; i < 15; i++)
	{
		CDBG("%s", default_sensor_init_format_string[i].c_str());
		buf += default_sensor_init_format_string[i];
		buf += "\r\n";
	}


#if defined(_WIN64) || (defined(NDEBUG) && defined(WIN32))
	CDBG("we are using X64 building or W32 debug");
	wchar_t *wChar = doMultiByteToWideChar(buf);
	::SetDlgItemText(AfxGetApp()->m_pMainWnd->m_hWnd, IDC_EDIT_FORMAT, wChar);// 需要一次性写入
	delete wChar;
#else
#if defined(WIN32)
	CDBG("we are using W32 release");
	char tmp_c[1024 * 12];
	//strncpy_s(tmp_c, buf.c_str(), strlen(buf.c_str())); // ok
	strncpy_s(tmp_c, sizeof(tmp_c), buf.c_str(), strlen(buf.c_str()));
	SetDlgItemText(IDC_EDIT_FORMAT, tmp_c); // 需要一次性写入
#endif
#endif

#else
	CDBG("we are using unicode "); // print in the console windows

	string buf;
	for (int i = 0; i < 15; i++)
	{
		CDBG("%s", default_sensor_init_format_string[i].c_str());
		CString goods_code;
		char c[1024];
		strcpy(c, default_sensor_init_format_string[i].c_str());
		For unicode
			goods_code.Format(_T("%s\r\n"), CStringW(c));  //goods_code.Format(_T("%s"), c); //网上说会出现乱码
		m_p_Edit->SetWindowText(goods_code);
		m_p_Edit->SetWindowText(_T(str.c_str()));// https://blog.csdn.net/daixiangzi/article/details/79718334 解决了这个问题
	}
	basic_string<TCHAR> pNewPathname = _T("fmt.txt");
	CFile file;
	if (file.Open(pNewPathname.c_str(), CFile::modeRead))
	{
		char  *pBuf;
		int  iLen = file.GetLength();
		pBuf = new  char[iLen + 1];
		file.Read(pBuf, iLen);
		pBuf[iLen] = 0;

		file.Close();
		basic_string<TCHAR> pBuf_c = _T(pBuf);

		m_p_Edit->SetWindowText(pBuf_c.c_str());
		delete pBuf;
	}
	else {
		CDBG("open file error");
	}
#endif
	//qfh end

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

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CcameraDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CcameraDlg::OnPaint()
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
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CcameraDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#if defined(use_drop_list)
void CcameraDlg::OnBnClickedRadioSensor()
{
	// TODO: 在此添加控件通知处理程序代码
	//MessageBox(L"sensor");
	UpdateData(true);
	CDBG("you select %d", m_nRadio_select_mode);

	CDBG("you are selected sensor");
	m_nRadio_select_mode = SENSOR_INIT;

	CButton* radio = (CButton*)GetDlgItem(IDC_RADIO_SENSOR);
	radio->SetCheck(1);

	//如果把 CComboBoxEx 属性从下拉列表改成 drop down, 则这个有效果
	CString ComSelItem(_T("init mode"));//获取当前下拉框的值 	
	m_Comboboxex.SetCurSel(0);
	m_Comboboxex.SetWindowText(ComSelItem);//将m_comBox的默认值设置为选中项的值
}

void CcameraDlg::OnBnClickedRadioEeprom()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nRadio_select_mode = EEPROM;
	//MessageBox(L"EEPROM");
	CDBG("you are selected eeprom");
}

void CcameraDlg::OnBnClickedRadioActuator()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nRadio_select_mode = ACTUATOR;
	CDBG("you are selected actuator");
	//MessageBox(L"ACTUATOR");
}

void CcameraDlg::OnCbnDropdownComboboxex()
{
	// TODO: 在此添加控件通知处理程序代码
#if use_unicode
	CString strTemp;
	int iCount = ((CComboBox*)GetDlgItem(IDC_COMBOBOXEX))->GetCount();//取得目前已经有的行数
	if (iCount < 1)//防止重复多次添加
	{
		((CComboBox*)GetDlgItem(IDC_COMBOBOXEX))->ResetContent();
		CString strFont[5] = { _T("宋体"),_T("楷体"),_T("仿宋"),_T("黑体"),_T("华文细黑") };

		for (int i = 0; i < 5; i++)
		{

			m_Combobox.AddString(strFont[i]);
			//m_Combobox.InsertString(i, strFont[i]);
			//MessageBox(strFont[i]);
		}
	}

	int index = m_Combobox.FindStringExact(0, _T("楷体")); //0表示从索引为0的选项开始查找.如果找到有叫three
	m_Combobox.SetCurSel(index); //不过我们不太可能去记住每个选项的索引,所以先查找下它的索引是多少
#endif

	int iCount = ((CComboBox*)GetDlgItem(IDC_COMBOBOXEX))->GetCount();//取得目前已经有的行数
	if (iCount < 1)//防止重复多次添加
	{
		((CComboBox*)GetDlgItem(IDC_COMBOBOXEX))->ResetContent();
		COMBOBOXEXITEM   Item;
		Item.mask = CBEIF_TEXT;
		Item.pszText = _T("init mode");
		Item.iItem = 0;//index 
		m_Comboboxex.InsertItem(&Item);
		Item.mask = CBEIF_TEXT;
		Item.pszText = _T("res mode");
		Item.iItem = 1;//index
		m_Comboboxex.InsertItem(&Item);
	}
}

void CcameraDlg::OnCbnSelchangeComboboxex()
{
	// TODO: 在此添加控件通知处理程序代码
	if (0 <= m_nRadio_select_mode && m_nRadio_select_mode <= 1) {
		CString ComSelItem(_T(""));//获取当前下拉框的值 
		m_Comboboxex.GetLBText(m_Comboboxex.GetCurSel(), ComSelItem);//获取CComBox下拉框当前选中的值
		m_Comboboxex.SetWindowText(ComSelItem);//将m_comBox的默认值设置为选中项的值
		int nIndex = m_Comboboxex.GetCurSel(); //当前选中的项
		m_Comboboxex.SetCurSel(nIndex); //设置第nIndex项为显示的内容
		//MessageBox(ComSelItem);
		m_nRadio_select_mode = nIndex;
		CDBG("m_nRadio_select_mode=%2x %s", m_nRadio_select_mode, nIndex ? "res mode" : "init mode");
	}
	else {
		CString ComSelItem(_T("please select sensor radio first"));//获取当前下拉框的值 
		MessageBox(ComSelItem);
	}
}

#else
void CcameraDlg::OnBnClickedRadioSensor()
{
	// TODO: 在此添加控件通知处理程序代码
	//MessageBox(L"sensor");
	UpdateData(true);
	CDBG("you select %d", m_nRadio_select_mode);
	if (m_userSetFormat == TRUE) {

	}
	else {
		string buf;
		string *p_default = NULL;
		int lineCnt = 0;

		for (int i = 0; defaultCueForser != NULL, i < 5; i++)
		{
			CDBG("%s", defaultCueForser[i].c_str());
			buf += defaultCueForser[i];
		}
		if (m_nRadio_select_mode == SENSOR_INIT) {
			p_default = default_sensor_init_format_string;
			lineCnt = 15;
		}
		else if (m_nRadio_select_mode == SENSOR_RES) {
			p_default = default_sensor_res_format_string;
			lineCnt = 15;
		}
		else if (m_nRadio_select_mode == EEPROM) {
			p_default = default_eeprom_memory_format_string;
			lineCnt = 21;
		}
		else if (m_nRadio_select_mode == ACTUATOR) {
			return;
		}

		for (int i = 0; p_default != NULL, i < lineCnt; i++)
		{
			CDBG("%s", p_default[i].c_str());
			buf += p_default[i];
			buf += "\r\n";
		}

		char *tmp_c = new char[1024 * (lineCnt + 5)];
		strncpy_s(tmp_c, 1024 * (lineCnt + 5), buf.c_str(), strlen(buf.c_str()) + 1);

#if defined(_WIN64) || (defined(NDEBUG) && defined(WIN32))
		CDBG("we are using X64 building or W32 debug %s  sizeof(buf.c_str() = %d (buf.c_str()) = %d",
			buf.c_str(), sizeof(buf.c_str()), strlen(buf.c_str())); //sizeof(buf.c_str()) = 8 in x64
		wchar_t *wChar = doMultiByteToWideChar(tmp_c);
		::SetDlgItemText(AfxGetApp()->m_pMainWnd->m_hWnd, IDC_EDIT_FORMAT, wChar);// 需要一次性写入
		delete wChar;
#else
#if defined(WIN32)
		CDBG("we are using win32 release %s  sizeof(buf.c_str() = %d (buf.c_str()) = %d",
			buf.c_str(), sizeof(buf.c_str()), strlen(buf.c_str())); //sizeof(buf.c_str()) = 4 in x86
		SetDlgItemText(IDC_EDIT_FORMAT, tmp_c);// 需要一次性写入
#endif
#endif
		delete tmp_c;
	}
}
#endif

void CcameraDlg::OnEnChangeEditGetUserFormat()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	CFile m_File;
	CString FileName;
	CString strTemp;

	int i = 0;
	int lineLen = 0;
	int lineCnt = 0;
	int nEditBoxBufSize = 0;

	CTime t = CTime::GetCurrentTime();
	FileName = t.Format("%Y-%m-%d-%H-%M-%S");
	FileName += _T("_format_file.txt"); //使用系统日期作为文件名 
	FileName = "format_file.txt";
	//FileName = "E:\\C++\\camera\\Debug\\format_file.txt";

	m_p_Edit = (CEdit*)GetDlgItem(IDC_EDIT_FORMAT);

#if 0
	FILE * resoutOutFile = NULL;
	resoutOutFile = fopen(FileName, "w+");
	if (resoutOutFile == NULL)
	{
		CDBG("openfile error");
	}
	else
		CDBG("openfile OK");
#if 0
	int err = fopen_s(&resoutOutFile, FileName, "w+")-----> return 13, access issue
		if (err != 0) {
			CDBG("open file fail err %d", err);
		}
#endif
#else
	if (m_File.Open(FileName, CFile::modeCreate | CFile::modeWrite | CFile::modeRead | CFile::typeBinary))
#endif
	{
		lineCnt = m_p_Edit->GetLineCount(); //取得edit控件中有多少行

#if !use_unicode
//First way
#if 0
		CString cs_buf;
		m_p_Edit->GetWindowText(cs_buf);
		GetDlgItemText(IDC_EDIT_FORMAT, cs_buf);// 需要一次性获取

		string s_buf = CT2A(cs_buf.GetBuffer());
		char  *tmp_c = NULL;
		nEditBoxBufSize = lineCnt * 1024;
		tmp_c = new  char[nEditBoxBufSize];

		if (tmp_c != NULL) {
			strncpy_s(tmp_c, nEditBoxBufSize, s_buf.c_str(), strlen(s_buf.c_str()) + 1); //lineCnt=1 sizeof(tmp_c)=4 strlen(tmp_c) = 0 sizeof(s_buf) = 28

			CDBG("pBuf = :\n %s \n lineCnt=%d sizeof(tmp_c)=%d strlen(pBuf) = %d sizeof(s_buf) = %d",
				tmp_c, lineCnt, sizeof(tmp_c), strlen(tmp_c), sizeof(s_buf));
			if (strlen(tmp_c) > 0) {
				m_File.Write(cs_buf, cs_buf.GetLength()); //把整个buf 写入文件， 但不方便我们按行保存到项目 buf 变量中
				m_File.Write(_T("\r\n"), 2); //换行
			}
			delete tmp_c;
		}
#endif
		//save each line to a buf or a file
		for (i = 0; i < lineCnt; i++)
		{
			strTemp.Empty();
			lineLen = m_p_Edit->LineLength(m_p_Edit->LineIndex(i));
			CDBG("lineLen = : %d  for line = %d", lineLen, i);
			if (lineLen == 0)
			{
				continue;
			}
			m_p_Edit->GetLine(i, strTemp.GetBuffer(lineLen), lineLen); //第 i 行的字符串给strTemp

			strTemp.ReleaseBuffer();

			lineLen = strTemp.GetLength();
			strTemp = strTemp.Left(lineLen); //从左边1开始获取前 lineLen 个字符

			string s = CT2A(strTemp.GetBuffer());
			CDBG("lineLen = : %d  s.length = %d", lineLen, s.length());

#if 0
			m_File.Write(s.c_str(), strTemp.GetLength());
			if (i != lineCnt - 1)
				m_File.Write(_T("\r\n"), 2);
			strncpy_s(m_userFormatString[i], sizeof(m_userFormatString[i]), s.c_str(), lineLen);
			CDBG("copy done ! s.c_str()=%s strlen(s.c_str())=%d sizeof(s.c_str())=%d", s.c_str(), strlen(s.c_str()), sizeof(s.c_str()));
#else
			if (lineLen != s.length())
			{
				MessageBox(__T("Take care of the string ... size is differ"));
			}

			//way3
			//回车键，ASCII码是13，控制字符CR，转义字符\r。 换行键，ASCII码是10，控制字符LF，转义字符\n。空格键，ASCII码是32。	
			char *tmp_c = NULL;
			bool flag = FALSE;
			int k = 0;
			for (k = 0; k < s.length(); k++)
			{
				CDBG("pos = : %d %c %d", k, s[k], s[k]);

				if (s[k] == '\n' || s[k] == '\t' || s[k] == '\r')
					CDBG("pos = : %d %c %d", k, s[k], s[k]);

				if (flag == TRUE)
				{
					s[k] = '\0';
					continue;
				}
				if (s[k] < 0) { //some case the value is < 0
					s[k] = '\0'; //s[k] = 32; //to fix some line has special value with 0 or empty space
					flag = TRUE;
					lineLen = k;
				}
			}

			CDBG("finish copy to s file flag = %d lineLen = %d strlen(s)=%d", flag, lineLen, strlen(s.c_str()));

			tmp_c = (char *)malloc(lineLen + 1);
			//s.copy(tmp_c, lineLen, 0);
			//strncpy won't add '\0' automatically, strncpy_s(tmp_c, lineLen, s.c_str(), lineLen); has error  because  s.c_str() include '\0', which size == (lineLen+1)
			strncpy_s(tmp_c, lineLen + 1, s.c_str(), lineLen);
			tmp_c[lineLen] = '\0';
			//we should use sizeof() , don't use sizeof(tmp_c),  sizeof(tmp_c) = 8 in x64 and sizeof(tmp_c) = 4 in x86
			CDBG("copy done !! tmp_c=%s strlen(tmp_c)=%d sizeof(tmp_c)=%d", tmp_c, strlen(tmp_c), sizeof(tmp_c));
			strncpy_s(m_userFormatString[i], sizeof(m_userFormatString[i]), tmp_c, lineLen);

			m_userFormatString[i][lineLen + 1] = '\0';
			DBG("m_userFormatString[i]=%s", s.c_str());
			DBG("m_userFormatString[i]=%s", m_userFormatString[i]);
#if 0
			if (resoutOutFile != NULL) {
				CDBG("writing = %s ", s.c_str());
				fwrite(s.c_str(), s.length(), 1, resoutOutFile);
			}
#else
			m_File.Write(tmp_c, lineLen);
			if (i != lineCnt - 1)
				m_File.Write(_T("\r\n"), 2);
			delete tmp_c;
#endif
#endif

		}
		m_File.Flush();
		m_File.Close();
		user_data.nUserFormatStringLine = lineCnt;
		if (lineCnt > 0)
			m_userSetFormat = TRUE;
		m_p_Edit->LineScroll(m_p_Edit->GetLineCount());//滚动条置底
#endif
	}
	else {
		CDBG("open file %s fail", FileName);
	}
}

//Group radio refer:  https://blog.csdn.net/songyimin1208/article/details/54946994
void CcameraDlg::OnBnClickedRadioAddr1()
{
	// TODO: 在此添加控件通知处理程序代码
	//UINT  select = GetCheckedRadioButton(IDC_RADIO_ADDR1, IDC_RADIO_ADDR2);

	UpdateData(true);
	CDBG("you select %d", m_nRadio_addr_type);
#ifdef MSG_BOX
	switch (m_nRadio_addr_type)
	{
	case 0:
		MessageBox(("Thisis a IDC_RADIO_ADDR1!"));
		break;
	case 1:
		MessageBox(("Thisis a IDC_RADIO_ADDR2!"));
		break;
	case 2:
		MessageBox(("Thisis a IDC_RADIO_ADDR3!"));
		break;
	default:
		MessageBox(("Please select!"));
		break;
	}
#endif
}


void CcameraDlg::OnBnClickedRadioData1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	CDBG("you select %d", m_nRadio_data_type);
#ifdef MSG_BOX

	switch (m_nRadio_data_type)
	{
	case 0:
		MessageBox(("Thisis a IDC_RADIO_DATA1!"));
		break;
	case 1:
		MessageBox(("Thisis a IDC_RADIO_DATA2!"));
		break;
	case 2:
		MessageBox(("Thisis a IDC_RADIO_DATA3!"));
		break;
	default:
		MessageBox(("Please select!"));
		break;
	}
#endif
}

void CcameraDlg::get_and_set_user_setting()
{
	CDBG("you select m_nRadio_addr_type=%d m_nRadio_data_type=%d m_nRadio_select_mode=%d",
		m_nRadio_addr_type, m_nRadio_data_type, m_nRadio_select_mode);
	user_data.nRadio_addr_type = m_nRadio_addr_type + 1;
	user_data.nRadio_data_type = m_nRadio_data_type + 1;
	user_data.nRadio_select_mode = m_nRadio_select_mode;
}

void CcameraDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UINT count;
	TCHAR filePath[MAX_PATH] = { 0 };

	count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);//从成功的拖放操作中检索文件的名称。并取代被拖拽文件的数目
	if (count == 1)//如果只拖拽一个文件夹
	{
		DragQueryFile(hDropInfo, 0, filePath, sizeof(filePath));//获得拖拽的文件名
		m_SelectedPath = filePath;
		UpdateData(FALSE);
		DragFinish(hDropInfo);//拖放成功后，释放内存
		//MessageBox(m_SelectedPath);
		CDBG("get drop file m_SelectedPath = %s", m_SelectedPath);

		CDialog::OnDropFiles(hDropInfo);
		return;

	}
	else//如果拖拽多个文件夹
	{
		//m_vectorFile.clear();
		for (UINT i = 0; i < count; i++)
		{
			int pathLen = DragQueryFile(hDropInfo, i, filePath, sizeof(filePath));
			m_SelectedPath = filePath;
			//m_vectorFile.push_back(filePath);
			//break;
			CString str;
			vector <string>::iterator pos;
			//在需要的地方，显示拖拽的文件名
			for (pos = m_vectorFile.begin(); pos != m_vectorFile.end(); pos++)
			{
				str = pos->c_str();
			}
		}

		UpdateData(FALSE);
		DragFinish(hDropInfo);
	}

	CDialog::OnDropFiles(hDropInfo);
}

void CcameraDlg::OnEnChangeEditbrowseInputfile()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
#if !use_unicode
	string pFileName;

#if 1 // use way1
	CMFCEditBrowseCtrl *prog = (CMFCEditBrowseCtrl*)GetDlgItem(IDC_EDITBROWSE_INPUTFILE);  //get edit browse control ID
	prog->EnableFileBrowseButton(_T("TXT"), _T("文本文件(*.txt)|*.txt|Word文件(*.doc)|*.doc|所有文件(*.*)|*.*||"));
	GetDlgItemText(IDC_EDITBROWSE_INPUTFILE, m_SelectedPath);

	CDBG("Editbrowse update strFilePath = %s with way1", pFileName.c_str());
	//or GetDlgItem(IDC_EDITBROWSE_INPUTFILE)->GetWindowText(m_SelectedPath);
	//MessageBox(m_SelectedPath);
#else
	// 设置过滤器   
	TCHAR szFilter[] = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);

	// 设置过滤器   
	TCHAR szFilter[] = _T("文本文件(*.txt)|*.txt|Word文件(*.doc)|*.doc|所有文件(*.*)|*.*||");
	// 构造保存文件对话框   
	CFileDialog fileDlg(FALSE, _T("doc"), _T("my"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

	// 显示保存文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		// 如果点击了文件对话框上的“保存”按钮，则将选择的文件路径显示到编辑框里   
		m_SelectedPath = fileDlg.GetPathName();
		SetDlgItemText(IDC_EDITBROWSE_INPUTFILE, m_SelectedPath);
	}
	pFileName = CT2A(m_SelectedPath.GetBuffer());
	CDBG("Editbrowse update strFilePath = %s with way2", pFileName.c_str());
#endif

#else
	//below for unicode
	//注意：以下n和len的值大小不同，n是按字符计算的，len是按字节计算的
	int n = m_SelectedPath.GetLength();
	//获取宽字节字符的大小，大小是按字节计算的
	int len = WideCharToMultiByte(CP_ACP, 0, m_SelectedPath, m_SelectedPath.GetLength(), NULL, 0, NULL, NULL);
	//为多字节字符数组申请空间，数组大小为按字节计算的宽字节字节大小
	char * pFileName = new char[len + 1];   //以字节为单位
	//宽字节编码转换成多字节编码          
	WideCharToMultiByte(CP_ACP, 0, m_SelectedPath, m_SelectedPath.GetLength(), pFileName, len, NULL, NULL);
	pFileName[n] = 0;   //pFileName[len + 1] = '/0' ; //多字节字符以'/0'结束, 使用 + 1 会导致有乱码
	CDBG("m_SelectedPath = %s %d %d", pFileName, n, len);
#endif
}

void CcameraDlg::OnBnClickedQuit()
{
	// TODO: 在此添加控件通知处理程序代码
	//CcameraDlg dlg; //会导致初始化对象又被创建
	//CAboutDlg dlg; // 这个会弹出关于版本的对话框
	//if (IDOK == dlg.DoModal())
	{
		// 如果点击了文件对话框上的“保存”按钮，则将选择的文件路径显示到编辑框里   
		CDialog::OnOK();
		CDBG("quit");
	}
}

void CcameraDlg::OnBnClickedButtonTxt2xml()
{
	// TODO: 在此添加控件通知处理程序代码
	CDBG("processing ....");
	clock_t start, finish;
	double totaltime;
	start = clock();

	get_and_set_user_setting();
	//MessageBox(L"txt2xml");
	switch (m_nRadio_select_mode) {
	case SENSOR_INIT:
	case SENSOR_RES:
	case EEPROM:
		fn = &Format::txt2xml_for_modules;
		break;
	default:
		//MessageBox(L"There is something wrong before txt2xml");
		break;

	}
	m_pFileName = CT2A(m_SelectedPath.GetBuffer());
	CDBG("m_SelectedPath = %s m_nRadio_select_mode=%d", m_pFileName.c_str(), m_nRadio_select_mode);

	if (m_SelectedPath == "" || !fn || m_pFileName.find(".txt") == string::npos)
	{
		MessageBox(__T("please check that if it is a txt file"));
	}
	else {
		fn(m_pFileName, user_data);
	}
	finish = clock();
	totaltime = (double)(finish - start) * 1000 / CLOCKS_PER_SEC;

	char msgStr[1024];
	snprintf(msgStr, 1024, "process done ! it takes %f ms", totaltime);
	SDBG(msgStr);
	DBG("%s", msgStr);
	CDBG("%s", msgStr);
	fflush(mLogFile);
#if defined(_WIN64) || (defined(NDEBUG) && defined(WIN32))
	MessageBox(CString(msgStr));
#else
	MessageBox(__T(msgStr));
#endif
}

void CcameraDlg::OnBnClickedButtonXml2txt()
{
	// TODO: 在此添加控件通知处理程序代码
	CDBG("processing ....");
	clock_t start, finish;
	double totaltime;
	start = clock();

	get_and_set_user_setting();
	//MessageBox(L"xml2txt");
	switch (m_nRadio_select_mode) {
	case SENSOR_INIT:
	case SENSOR_RES:
	case EEPROM:
		fn = &Format::xml2txt_for_modules;
		break;
	default:
		//MessageBox(L"There is something wrong before xml2txt");
		break;
	}
	m_pFileName = CT2A(m_SelectedPath.GetBuffer());
	CDBG("m_SelectedPath = %s m_nRadio_select_mode=%d", m_pFileName.c_str(), m_nRadio_select_mode);

	if (m_SelectedPath == "" || !fn || m_pFileName.find(".xml") == string::npos)
	{
		MessageBox(__T("please check that if it is a xml file"));
	}
	else {
		CDBG("processing ....");
		fn(m_pFileName, user_data);
	}

	finish = clock();
	totaltime = (double)(finish - start) * 1000 / CLOCKS_PER_SEC;

	char msgStr[1024];
	snprintf(msgStr, 1024, "process done ! it takes %f ms", totaltime);
	SDBG(msgStr);
	DBG("%s", msgStr);
	CDBG("%s", msgStr);
	fflush(mLogFile);
#if defined(_WIN64) || (defined(NDEBUG) && defined(WIN32))
	MessageBox(CString(msgStr));
#else
	MessageBox(__T(msgStr));
#endif
}

void CcameraDlg::ChangeSize(UINT nID, int x, int y) //nID为控件ID，x,y分别为对话框的当前长和宽

{

	CWnd *pWnd;

	pWnd = GetDlgItem(nID);

	if (pWnd != NULL) //判断是否为空，因为在窗口创建的时候也会调用OnSize函数，但是此时各个控件还没有创建，Pwnd为空

	{

		CRect rec;

		pWnd->GetWindowRect(&rec); //获取控件变化前的大小

		ScreenToClient(&rec); //将控件大小装换位在对话框中的区域坐标

		rec.left = rec.left*x / m_rect.Width(); //按照比例调整空间的新位置

		rec.top = rec.top*y / m_rect.Height();

		rec.bottom = rec.bottom*y / m_rect.Height();

		rec.right = rec.right*x / m_rect.Width();

		pWnd->MoveWindow(rec); //伸缩控件

	}

}

void CcameraDlg::repaint(UINT id, int last_Width, int now_Width, int last_Height, int now_Height)
{
	CRect rect;
	CWnd *wnd = NULL;
	wnd = GetDlgItem(id);
	if (NULL != wnd)
	{
		wnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.left = (long)((double)rect.left / (double)last_Width*(double)now_Width);
		rect.right = (long)((double)rect.right / (double)last_Width*(double)now_Width);
		rect.top = (long)((double)rect.top / (double)last_Height*(double)now_Height);
		rect.bottom = (long)((double)rect.bottom / (double)last_Height*(double)now_Height);
		wnd->MoveWindow(&rect);
	}
}

void CcameraDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType == 1) return;//最小化则什么都不做	
#if 0 //https://blog.csdn.net/renyongwang1988/article/details/16840735
	if (nType != SIZE_MINIMIZED) //判断窗口是不是最小化了，因为窗口最小化之后 ，窗口的长和宽会变成0，当前一次变化的                               //时就会出现除以0的错误操作

	{
		ChangeSize(IDC_EDIT_FORMAT, cx, cy);

		ChangeSize(IDC_STATIC_TRANS_MODE, cx, cy);
		ChangeSize(IDC_RADIO_SENSOR_INIT, cx, cy);
		ChangeSize(IDC_RADIO_SENSOR_RES, cx, cy);
		ChangeSize(IDC_RADIO_EEPROM, cx, cy);
		ChangeSize(IDC_RADIO_ACTUATOR, cx, cy);

		ChangeSize(IDC_STATIC_ADDR_TYPE, cx, cy);
		ChangeSize(IDC_RADIO_ADDR1, cx, cy);
		ChangeSize(IDC_RADIO_ADDR2, cx, cy);
		ChangeSize(IDC_RADIO_ADDR3, cx, cy);

		ChangeSize(IDC_STATIC_DATA_TYPE, cx, cy);
		ChangeSize(IDC_RADIO_DATA1, cx, cy);
		ChangeSize(IDC_RADIO_DATA2, cx, cy);
		ChangeSize(IDC_RADIO_DATA3, cx, cy);

		ChangeSize(IDC_STATIC_INPUTFILE, cx, cy);
		ChangeSize(IDC_EDITBROWSE_INPUTFILE, cx, cy);

		ChangeSize(IDC_BUTTON_TXT2XML, cx, cy);
		ChangeSize(IDC_BUTTON_XML2TXT, cx, cy);
		ChangeSize(ID_QUIT, cx, cy);

		GetClientRect(&m_rect); //最后要更新对话框的大小，当做下一次变化的旧坐标
	}
#else //https://blog.csdn.net/weixin_34249678/article/details/94083306?depth_1-utm_source=distribute.pc_relevant.none-task&utm_source=distribute.pc_relevant.none-task
	if (0 == m_rect.left && 0 == m_rect.right
		&& 0 == m_rect.top && 0 == m_rect.bottom)//第一次启动对话框时的大小变化不做处理
	{
	}
	else
	{
		if (0 == cx && 0 == cy)//如果是按下了最小化，则触发条件，这时不保存对话框数据
		{
			return;
		}
		CRect rectDlgChangeSize;
		GetClientRect(&rectDlgChangeSize);//存储对话框大小改变后对话框大小数据

		repaint(IDC_EDIT_FORMAT, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_STATIC_TRANS_MODE, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_RADIO_SENSOR_INIT, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());

		repaint(IDC_RADIO_SENSOR_RES, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_RADIO_EEPROM, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_RADIO_ACTUATOR, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());

		repaint(IDC_STATIC_ADDR_TYPE, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_RADIO_ADDR1, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_RADIO_ADDR2, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_RADIO_ADDR3, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_STATIC_DATA_TYPE, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_RADIO_DATA1, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_RADIO_DATA2, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_RADIO_DATA3, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());

		repaint(IDC_STATIC_INPUTFILE, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());

		repaint(IDC_EDITBROWSE_INPUTFILE, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_BUTTON_TXT2XML, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(IDC_BUTTON_XML2TXT, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
		repaint(ID_QUIT, m_rect.Width(), rectDlgChangeSize.Width(), m_rect.Height(), rectDlgChangeSize.Height());
	}
	GetClientRect(&m_rect); //save size of dialog

#endif

}
