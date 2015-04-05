#pragma once
#pragma comment(lib, "iphlpapi.lib")

#include <IPHlpApi.h>
#include "afxwin.h"


// CInitDlg dialog

class CInitDlg : public CDialog
{
	DECLARE_DYNAMIC(CInitDlg)

public:
	CInitDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInitDlg();

// Dialog Data
	enum { IDD = IDD_INIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	static PIP_ADAPTER_ADDRESSES AdaptersInfoBuffer;
	CComboBox m_int1box;
	CComboBox m_int2box;
	CArray<PCHAR,PCHAR> IfNames;
	CArray<PWCHAR,PWCHAR> IfDescs;
	CArray<PBYTE,PBYTE> IfMACs;
	DWORD FillAdaptersBuffer(void);
public:
	afx_msg void OnBnClickedOk();
};
