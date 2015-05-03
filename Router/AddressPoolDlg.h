#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#define INVALID_CIDR 33

// AddressPoolDlg dialog

class AddressPoolDlg : public CDialog
{
	DECLARE_DYNAMIC(AddressPoolDlg)

public:
	AddressPoolDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AddressPoolDlg();

// Dialog Data
	enum { IDD = IDD_NATPOOL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CIPAddressCtrl m_firstIP;
	CIPAddressCtrl m_lastIP;
	CIPAddressCtrl m_mask;
	CEdit m_cidr;
	CSpinButtonCtrl m_cidrspin;
	BYTE GetCIDR(void);
	void SetMask(BYTE cidr);
public:
	afx_msg void OnIpnFieldchangedMask(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeCidr();
	afx_msg void OnBnClickedOk();
};
