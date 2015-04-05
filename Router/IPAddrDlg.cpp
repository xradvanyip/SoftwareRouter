// IPAddrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Router.h"
#include "IPAddrDlg.h"
#include "afxdialogex.h"
#include "Frame.h"


// IPAddrDlg dialog

IMPLEMENT_DYNAMIC(IPAddrDlg, CDialog)

IPAddrDlg::IPAddrDlg(Interface *i, CWnd* pParent /*=NULL*/)
	: CDialog(IPAddrDlg::IDD, pParent)
{
	m_interface = i;
}

IPAddrDlg::~IPAddrDlg()
{
}

void IPAddrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, m_ipaddr);
	DDX_Control(pDX, IDC_MASK, m_mask);
	DDX_Control(pDX, IDC_CIDR, m_cidr);
	DDX_Control(pDX, IDC_CIDRSPIN, m_cidrspin);
}


BEGIN_MESSAGE_MAP(IPAddrDlg, CDialog)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_MASK, &IPAddrDlg::OnIpnFieldchangedMask)
	ON_EN_CHANGE(IDC_CIDR, &IPAddrDlg::OnEnChangeCidr)
	ON_BN_CLICKED(IDOK, &IPAddrDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// IPAddrDlg message handlers


BOOL IPAddrDlg::OnInitDialog()
{
	IPaddr IPstruct = m_interface->GetIPAddrStruct();
	CDialog::OnInitDialog();

	m_cidrspin.SetBuddy(GetDlgItem(IDC_CIDR));
	m_cidrspin.SetRange32(0,32);

	if (m_interface->IsIPaddrConfigured())
	{
		m_ipaddr.SetAddress(IPstruct.b[0],IPstruct.b[1],IPstruct.b[2],IPstruct.b[3]);
		SetMask(m_interface->GetMaskCIDR());
		m_cidrspin.SetPos(m_interface->GetMaskCIDR());
	}

	return TRUE;
}


BYTE IPAddrDlg::GetCIDR(void)
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


void IPAddrDlg::SetMask(BYTE cidr)
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


void IPAddrDlg::OnIpnFieldchangedMask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	BYTE cidr = GetCIDR();

	if (cidr != INVALID_CIDR) m_cidrspin.SetPos(cidr);

	*pResult = 0;
}


void IPAddrDlg::OnEnChangeCidr()
{
	BYTE cidr = (BYTE) GetDlgItemInt(IDC_CIDR,NULL,0);

	if ((cidr >= 0) && (cidr <= 32)) SetMask(cidr);
}


void IPAddrDlg::OnBnClickedOk()
{
	IPaddr ip_addr;
	DWORD dwAddr;
	BYTE cidr_field = (BYTE) GetDlgItemInt(IDC_CIDR,NULL,0);

	ip_addr.SubnetMaskCIDR = GetCIDR();

	if ((ip_addr.SubnetMaskCIDR == INVALID_CIDR) || (cidr_field < 0) || (cidr_field > 32))
	{
		AfxMessageBox(_T("The entered subnet mask or CIDR format is invalid!"));
		return;
	}

	m_ipaddr.GetAddress(dwAddr);
	dwAddr <<= ip_addr.SubnetMaskCIDR;
	if ((dwAddr == 0) || (((~dwAddr) >> ip_addr.SubnetMaskCIDR) == 0))
	{
		AfxMessageBox(_T("Invalid IP address!"));
		return;
	}

	m_ipaddr.GetAddress(ip_addr.b[0],ip_addr.b[1],ip_addr.b[2],ip_addr.b[3]);
	
	theApp.GetRouterDlg()->EditIP(m_interface,ip_addr);

	CDialog::OnOK();
}
