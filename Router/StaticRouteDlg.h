#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#define INVALID_CIDR 33

// StaticRouteDlg dialog

class StaticRouteDlg : public CDialog
{
	DECLARE_DYNAMIC(StaticRouteDlg)

public:
	StaticRouteDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~StaticRouteDlg();

// Dialog Data
	enum { IDD = IDD_STATICROUTE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CIPAddressCtrl m_prefix;
	CIPAddressCtrl m_mask;
	CEdit m_cidr;
	CSpinButtonCtrl m_cidrspin;
	CIPAddressCtrl m_nexthop;
	CComboBox m_OutIfCombo;
	CEdit m_ad;
	CSpinButtonCtrl m_adspin;
	BYTE GetCIDR(void);
	void SetMask(BYTE cidr);
public:
	afx_msg void OnIpnFieldchangedMask(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeCidr();
	afx_msg void OnCbnSelchangeIntcombo();
	afx_msg void OnBnClickedOk();
};
