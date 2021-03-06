
// cameraDlg.h: 头文件
//

#pragma once
#include "Format.h"

// CcameraDlg 对话框
class CcameraDlg : public CDialog
{
	// 构造
public:
	CcameraDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERA_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioSensor();
#if defined(use_drop_list)
	afx_msg void OnBnClickedRadioActuator();
	afx_msg void OnBnClickedRadioEeprom();
	afx_msg void OnCbnDropdownComboboxex();
	afx_msg void OnCbnSelchangeComboboxex();
#endif
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEnChangeEditbrowseInputfile();
	afx_msg void OnEnChangeEditGetUserFormat();
	afx_msg void OnBnClickedButtonTxt2xml();
	afx_msg void OnBnClickedButtonXml2txt();
	afx_msg void OnBnClickedQuit();
	afx_msg void OnBnClickedRadioAddr1();
	afx_msg void OnBnClickedRadioData1();

private:
	Format m_fmt;
	CComboBoxEx m_Comboboxex;	//It is not CComboBox m_Combobox;
	CButton m_sensor_radio_btn;
	CEdit *m_p_Edit;
	void(*fn)(string filename, USER_DATA user_data); // 如果是类的非静态成员函数，在外面想用指针的方式比较麻烦，这个可以声明 void(Format::*fn)(string str);
	CString m_SelectedPath;
	string m_pFileName;
	FILE * mLogFile;
	CRect m_rect;
private:
	int m_nRadio_select_mode;// if change to static, link problem
	int m_nRadio_addr_type;
	int m_nRadio_data_type;
	bool m_userSetFormat;
	vector<string> m_vectorFile;
public:
	void get_and_set_user_setting();
	void ChangeSize(UINT nID, int x, int y);
	void OnSize(UINT nType, int cx, int cy);
	void repaint(UINT id, int last_Width, int now_Width, int last_Height, int now_Height);
};
extern string default_eeprom_memory_format_string[21];
extern string default_sensor_init_format_string[15];
extern string default_sensor_res_format_string[15];
extern string defaultCueForser[5];
extern USER_DATA user_data;
extern char m_userFormatString[64][1024];
extern wchar_t* doMultiByteToWideChar(const string & cStr);
extern wchar_t* doMultiByteToWideChar(const char* cStr);