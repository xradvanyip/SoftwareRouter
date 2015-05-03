// AddressPoolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Router.h"
#include "AddressPoolDlg.h"
#include "afxdialogex.h"


// AddressPoolDlg dialog

IMPLEMENT_DYNAMIC(AddressPoolDlg, CDialog)

AddressPoolDlg::AddressPoolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AddressPoolDlg::IDD, pParent)
{

}

AddressPoolDlg::~AddressPoolDlg()
{
}

void AddressPoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STARTIP, m_firstIP);
	DDX_Control(pDX, IDC_ENDIP, m_lastIP);
	DDX_Control(pDX, IDC_MASK, m_mask);
	DDX_Control(pDX, IDC_CIDR, m_cidr);
	DDX_Control(pDX, IDC_CIDRSPIN, m_cidrspin);
}


BEGIN_MESSAGE_MAP(AddressPoolDlg, CDialog)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_MASK, &AddressPoolDlg::OnIpnFieldchangedMask)
	ON_EN_CHANGE(IDC_CIDR, &AddressPoolDlg::OnEnChangeCidr)
	ON_BN_CLICKED(IDOK, &AddressPoolDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// AddressPoolDlg message handlers


BOOL AddressPoolDlg::OnInitDialog()
{
	IPaddr first, last;
	CDialog::OnInitDialog();

	m_cidrspin.SetBuddy(GetDlgItem(IDC_CIDR));
	m_cidrspin.SetRange32(0,32);

	if (theApp.GetNAT()->IsPoolConfigured())
	{
		first = theApp.GetNAT()->GetFirstIPinPool();
		last = theApp.GetNAT()->GetLastIPinPool();
		m_firstIP.SetAddress(first.dw);
		m_lastIP.SetAddress(last.dw);
		SetMask(first.SubnetMaskCIDR);
		m_cidrspin.SetPos(first.SubnetMaskCIDR);
	}

	return TRUE;
}


BYTE AddressPoolDlg::GetCIDR(void)
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


void AddressPoolDlg::SetMask(BYTE cidr)
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


void AddressPoolDlg::OnIpnFieldchangedMask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	BYTE cidr = GetCIDR();

	if (cidr != INVALID_CIDR) m_cidrspin.SetPos(cidr);

	*pResult = 0;
}


void AddressPoolDlg::OnEnChangeCidr()
{
	BYTE cidr = (BYTE) GetDlgItemInt(IDC_CIDR,NULL,0);

	if ((cidr >= 0) && (cidr <= 32)) SetMask(cidr);
}


void AddressPoolDlg::OnBnClickedOk()
{
	IPaddr firstIP, lastIP;
	DWORD dwAddr;
	BYTE cidr_field = (BYTE) GetDlgItemInt(IDC_CIDR,NULL,0);
	BYTE cidr;

	cidr = GetCIDR();

	if ((cidr == INVALID_CIDR) || (cidr_field < 0) || (cidr_field > 32))
	{
		AfxMessageBox(_T("The entered subnet mask or CIDR format is invalid!"));
		return;
	}
	firstIP.SubnetMaskCIDR = cidr;
	lastIP.SubnetMaskCIDR = cidr;

	m_firstIP.GetAddress(dwAddr);
	dwAddr <<= cidr;
	if ((dwAddr == 0) || (((~dwAddr) >> cidr) == 0))
	{
		AfxMessageBox(_T("Invalid first IP address!"));
		return;
	}
	m_firstIP.GetAddress(firstIP.dw);

	m_lastIP.GetAddress(dwAddr);
	dwAddr <<= cidr;
	if ((dwAddr == 0) || (((~dwAddr) >> cidr) == 0))
	{
		AfxMessageBox(_T("Invalid last IP address!"));
		return;
	}
	m_lastIP.GetAddress(lastIP.dw);

	if ((firstIP.dw >> (32 - cidr)) != (lastIP.dw >> (32 - cidr)))
	{
		AfxMessageBox(_T("The IP addresses are not from the same subnet!"));
		return;
	}

	if (firstIP.dw > lastIP.dw)
	{
		AfxMessageBox(_T("The first IP address can not be higher than the last!"));
		return;
	}

	theApp.GetNAT()->SetPool(firstIP,lastIP);

	CDialog::OnOK();
}
