#pragma once
#include "afxcmn.h"


// RipTimersDlg dialog

class RipTimersDlg : public CDialog
{
	DECLARE_DYNAMIC(RipTimersDlg)

public:
	RipTimersDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~RipTimersDlg();

// Dialog Data
	enum { IDD = IDD_RIP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CSpinButtonCtrl m_updatespin;
	CSpinButtonCtrl m_invalidspin;
	CSpinButtonCtrl m_flushspin;
	CSpinButtonCtrl m_holddownspin;
public:
	afx_msg void OnBnClickedOk();
};
