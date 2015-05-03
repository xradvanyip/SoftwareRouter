#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// StaticNatDlg dialog

class StaticNatDlg : public CDialog
{
	DECLARE_DYNAMIC(StaticNatDlg)

public:
	StaticNatDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~StaticNatDlg();

// Dialog Data
	enum { IDD = IDD_STATICNAT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_modecombo;
	CIPAddressCtrl m_globalIP;
	CIPAddressCtrl m_localIP;
	CButton m_pfwdcheck;
	CComboBox m_protcombo;
	CEdit m_globalPort;
	CSpinButtonCtrl m_globalPortSpin;
	CEdit m_localPort;
	CSpinButtonCtrl m_localPortSpin;
public:
	afx_msg void OnBnClickedPortFWcheck();
	afx_msg void OnBnClickedOk();
};
