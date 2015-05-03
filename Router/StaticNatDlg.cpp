// StaticNatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Router.h"
#include "StaticNatDlg.h"
#include "afxdialogex.h"

#define TCP_ID 6
#define UDP_ID 17


// StaticNatDlg dialog

IMPLEMENT_DYNAMIC(StaticNatDlg, CDialog)

StaticNatDlg::StaticNatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(StaticNatDlg::IDD, pParent)
{

}

StaticNatDlg::~StaticNatDlg()
{
}

void StaticNatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODECOMBO, m_modecombo);
	DDX_Control(pDX, IDC_GLOBALIP, m_globalIP);
	DDX_Control(pDX, IDC_LOCALIP, m_localIP);
	DDX_Control(pDX, IDC_PORTFWCHECK, m_pfwdcheck);
	DDX_Control(pDX, IDC_GLOBALPORT, m_globalPort);
	DDX_Control(pDX, IDC_GLOBALPORTSPIN, m_globalPortSpin);
	DDX_Control(pDX, IDC_LOCALPORT, m_localPort);
	DDX_Control(pDX, IDC_LOCALPORTSPIN, m_localPortSpin);
	DDX_Control(pDX, IDC_PROTCOMBO, m_protcombo);
}


BEGIN_MESSAGE_MAP(StaticNatDlg, CDialog)
	ON_BN_CLICKED(IDC_PORTFWCHECK, &StaticNatDlg::OnBnClickedPortFWcheck)
	ON_BN_CLICKED(IDOK, &StaticNatDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// StaticNatDlg message handlers

BOOL StaticNatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_modecombo.SetCurSel(0);
	
	m_pfwdcheck.SetCheck(BST_UNCHECKED);
	m_protcombo.SetCurSel(0);
	m_protcombo.EnableWindow(FALSE);
	m_globalPortSpin.SetBuddy(GetDlgItem(IDC_GLOBALPORT));
	m_globalPortSpin.SetRange32(1,65535);
	m_globalPortSpin.EnableWindow(FALSE);
	m_globalPort.EnableWindow(FALSE);
	m_localPortSpin.SetBuddy(GetDlgItem(IDC_LOCALPORT));
	m_localPortSpin.SetRange32(1,65535);
	m_localPortSpin.EnableWindow(FALSE);
	m_localPort.EnableWindow(FALSE);

	return TRUE;
}

void StaticNatDlg::OnBnClickedPortFWcheck()
{
	if (m_pfwdcheck.GetCheck())
	{
		m_protcombo.EnableWindow(TRUE);
		m_globalPortSpin.EnableWindow(TRUE);
		m_globalPort.EnableWindow(TRUE);
		m_localPortSpin.EnableWindow(TRUE);
		m_localPort.EnableWindow(TRUE);
	}
	else
	{
		m_protcombo.EnableWindow(FALSE);
		m_globalPortSpin.EnableWindow(FALSE);
		m_globalPort.EnableWindow(FALSE);
		m_localPortSpin.EnableWindow(FALSE);
		m_localPort.EnableWindow(FALSE);
	}
}


void StaticNatDlg::OnBnClickedOk()
{
	Translation newTrans;
	WORD global_port = (WORD) GetDlgItemInt(IDC_GLOBALPORT,NULL,0);
	WORD local_port = (WORD) GetDlgItemInt(IDC_LOCALPORT,NULL,0);

	newTrans.type = STATICNAT;
	
	if (m_modecombo.GetCurSel() == 0) newTrans.mode = INSIDE;
	else newTrans.mode = OUTSIDE;

	m_globalIP.GetAddress(newTrans.global.b[3],newTrans.global.b[2],newTrans.global.b[1],newTrans.global.b[0]);
	m_localIP.GetAddress(newTrans.local.b[3],newTrans.local.b[2],newTrans.local.b[1],newTrans.local.b[0]);

	if (m_pfwdcheck.GetCheck())
	{
		if ((global_port < 1) || (global_port > 65535))
		{
			AfxMessageBox(_T("The global port must be between 1 and 65535!"));
			return;
		}
		if ((local_port < 1) || (local_port > 65535))
		{
			AfxMessageBox(_T("The local port must be between 1 and 65535!"));
			return;
		}
		newTrans.IsPortForward = TRUE;
		if (m_protcombo.GetCurSel() == 0) newTrans.Lay4Prot = TCP_ID;
		else newTrans.Lay4Prot = UDP_ID;
		newTrans.global_port = global_port;
		newTrans.local_port = local_port;
	}
	else newTrans.IsPortForward = FALSE;

	theApp.GetNAT()->AddRule(newTrans);

	CDialog::OnOK();
}
