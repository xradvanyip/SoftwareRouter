#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Interface.h"

#define INVALID_CIDR 33

// IPAddrDlg dialog

class IPAddrDlg : public CDialog
{
	DECLARE_DYNAMIC(IPAddrDlg)

public:
	IPAddrDlg(Interface *i, CWnd* pParent = NULL);   // standard constructor
	virtual ~IPAddrDlg();

// Dialog Data
	enum { IDD = IDD_IPADDR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	Interface *m_interface;
	CIPAddressCtrl m_ipaddr;
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
