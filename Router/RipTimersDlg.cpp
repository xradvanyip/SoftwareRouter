// RipTimersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Router.h"
#include "RipTimersDlg.h"
#include "afxdialogex.h"


// RipTimersDlg dialog

IMPLEMENT_DYNAMIC(RipTimersDlg, CDialog)

RipTimersDlg::RipTimersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(RipTimersDlg::IDD, pParent)
{

}

RipTimersDlg::~RipTimersDlg()
{
}

void RipTimersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UPDSPIN, m_updatespin);
	DDX_Control(pDX, IDC_INVSPIN, m_invalidspin);
	DDX_Control(pDX, IDC_FLUSHSPIN, m_flushspin);
	DDX_Control(pDX, IDC_HDSPIN, m_holddownspin);
}


BEGIN_MESSAGE_MAP(RipTimersDlg, CDialog)
	ON_BN_CLICKED(IDOK, &RipTimersDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// RipTimersDlg message handlers

BOOL RipTimersDlg::OnInitDialog()
{
	UDACCEL accel[3];
	UINT update, invalid, flush, holddown;
	
	CDialog::OnInitDialog();

	theApp.GetRIB()->GetRipTimeIntervals(update,invalid,flush,holddown);
	accel[0].nSec = 0;
	accel[1].nSec = 2;
	accel[2].nSec = 4;
	accel[0].nInc = 5;
	accel[1].nInc = 10;
	accel[2].nInc = 20;

	m_updatespin.SetBuddy(GetDlgItem(IDC_UPDATE));
	m_updatespin.SetRange32(5,1000);
	m_updatespin.SetAccel(3,accel);
	m_updatespin.SetPos(update);

	m_invalidspin.SetBuddy(GetDlgItem(IDC_INVALID));
	m_invalidspin.SetRange32(5,1000);
	m_invalidspin.SetAccel(3,accel);
	m_invalidspin.SetPos(invalid);

	m_flushspin.SetBuddy(GetDlgItem(IDC_FLUSH));
	m_flushspin.SetRange32(5,1000);
	m_flushspin.SetAccel(3,accel);
	m_flushspin.SetPos(flush);

	m_holddownspin.SetBuddy(GetDlgItem(IDC_HOLDDOWN));
	m_holddownspin.SetRange32(0,1000);
	m_holddownspin.SetAccel(3,accel);
	m_holddownspin.SetPos(holddown);

	return TRUE;
}


void RipTimersDlg::OnBnClickedOk()
{
	theApp.GetRIB()->SetRipTimeIntervals(m_updatespin.GetPos(),m_invalidspin.GetPos(),m_flushspin.GetPos(),m_holddownspin.GetPos());

	CDialog::OnOK();
}
