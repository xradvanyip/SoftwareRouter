// StaticRouteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Router.h"
#include "StaticRouteDlg.h"
#include "afxdialogex.h"
#include "Frame.h"


// StaticRouteDlg dialog

IMPLEMENT_DYNAMIC(StaticRouteDlg, CDialog)

StaticRouteDlg::StaticRouteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(StaticRouteDlg::IDD, pParent)
{

}

StaticRouteDlg::~StaticRouteDlg()
{
}

void StaticRouteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREFIX, m_prefix);
	DDX_Control(pDX, IDC_MASK, m_mask);
	DDX_Control(pDX, IDC_CIDR, m_cidr);
	DDX_Control(pDX, IDC_CIDRSPIN, m_cidrspin);
	DDX_Control(pDX, IDC_NEXTHOP, m_nexthop);
	DDX_Control(pDX, IDC_INTCOMBO, m_OutIfCombo);
	DDX_Control(pDX, IDC_AD, m_ad);
	DDX_Control(pDX, IDC_ADSPIN, m_adspin);
}


BEGIN_MESSAGE_MAP(StaticRouteDlg, CDialog)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_MASK, &StaticRouteDlg::OnIpnFieldchangedMask)
	ON_EN_CHANGE(IDC_CIDR, &StaticRouteDlg::OnEnChangeCidr)
	ON_CBN_SELCHANGE(IDC_INTCOMBO, &StaticRouteDlg::OnCbnSelchangeIntcombo)
	ON_BN_CLICKED(IDOK, &StaticRouteDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// StaticRouteDlg message handlers

BOOL StaticRouteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cidrspin.SetBuddy(GetDlgItem(IDC_CIDR));
	m_cidrspin.SetRange32(0,32);
	m_OutIfCombo.SetCurSel(0);
	m_adspin.SetBuddy(GetDlgItem(IDC_AD));
	m_adspin.SetRange32(1,254);
	m_adspin.SetPos(1);
	
	return TRUE;
}


BYTE StaticRouteDlg::GetCIDR(void)
{
	DWORD dwMask;
	BYTE cidr = 0;

	m_mask.GetAddress(dwMask);
	while ((dwMask >> 31) != 0)
	{
		dwMask <<= 1;
		cidr++;
	}
	if (dwMask == 0) return cidr;
	else return INVALID_CIDR;
}


void StaticRouteDlg::SetMask(BYTE cidr)
{
	DWORD dwMask = 0;
	DWORD maxBit = 1 << 31;
	BYTE i;
	
	for (i=0;i < cidr;i++)
	{
		dwMask >>= 1;
		dwMask |= maxBit;
	}
	m_mask.SetAddress(dwMask);
}


void StaticRouteDlg::OnIpnFieldchangedMask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	BYTE cidr = GetCIDR();

	if (cidr != INVALID_CIDR) m_cidrspin.SetPos(cidr);

	*pResult = 0;
}


void StaticRouteDlg::OnEnChangeCidr()
{
	BYTE cidr = (BYTE) GetDlgItemInt(IDC_CIDR,NULL,0);

	if ((cidr >= 0) && (cidr <= 32)) SetMask(cidr);
}


void StaticRouteDlg::OnCbnSelchangeIntcombo()
{
	if (m_OutIfCombo.GetCurSel() != 0) m_nexthop.EnableWindow(FALSE);
	else m_nexthop.EnableWindow(TRUE);
}


void StaticRouteDlg::OnBnClickedOk()
{
	Route newRoute;
	DWORD dwAddr;
	BYTE cidr_field = (BYTE) GetDlgItemInt(IDC_CIDR,NULL,0);
	BYTE AD_field = (BYTE) GetDlgItemInt(IDC_AD,NULL,0);

	newRoute.prefix.SubnetMaskCIDR = GetCIDR();

	if ((newRoute.prefix.SubnetMaskCIDR == INVALID_CIDR) || (cidr_field < 0) || (cidr_field > 32))
	{
		AfxMessageBox(_T("The entered subnet mask or CIDR format is invalid!"));
		return;
	}

	m_prefix.GetAddress(dwAddr);
	dwAddr <<= newRoute.prefix.SubnetMaskCIDR;
	if ((newRoute.prefix.SubnetMaskCIDR < 32) && (dwAddr != 0))
	{
		AfxMessageBox(_T("Please enter a valid PREFIX!"));
		return;
	}

	if ((AD_field < 1) || (AD_field > 254))
	{
		AfxMessageBox(_T("The administrative distance have to be between 1 and 254!"));
		return;
	}

	m_prefix.GetAddress(newRoute.prefix.b[3],newRoute.prefix.b[2],newRoute.prefix.b[1],newRoute.prefix.b[0]);

	newRoute.type = STATIC;
	newRoute.AD = AD_field;
	newRoute.metric = 0;
	
	if (m_OutIfCombo.GetCurSel() == 0)
	{
		newRoute.NextHop.HasNextHop = TRUE;
		m_nexthop.GetAddress(newRoute.NextHop.b[3],newRoute.NextHop.b[2],newRoute.NextHop.b[1],newRoute.NextHop.b[0]);
		newRoute.i = NULL;
	}
	else
	{
		newRoute.NextHop.HasNextHop = FALSE;
		if (m_OutIfCombo.GetCurSel() == 1) newRoute.i = theApp.GetInt1();
		else newRoute.i = theApp.GetInt2();
	}

	theApp.GetRIB()->AddRoute(newRoute);

	CDialog::OnOK();
}
