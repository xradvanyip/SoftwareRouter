
// RouterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Router.h"
#include "RouterDlg.h"
#include "afxdialogex.h"
#include "IPAddrDlg.h"
#include "StaticRouteDlg.h"
#include "RipTimersDlg.h"
#include "StaticNatDlg.h"
#include "AddressPoolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRouterDlg dialog



CRouterDlg::CRouterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRouterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRouterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INT1DESC, m_int1desc);
	DDX_Control(pDX, IDC_INT1MACADDR, m_int1macaddr);
	DDX_Control(pDX, IDC_INT1IPADDR, m_int1ipaddr);
	DDX_Control(pDX, IDC_INT1IPBUTTON, m_int1ipbutton);
	DDX_Control(pDX, IDC_INT1SWBUTTON, m_int1swbutton);
	DDX_Control(pDX, IDC_INT1NATCOMBO, m_int1natmode);
	DDX_Control(pDX, IDC_INT2DESC, m_int2desc);
	DDX_Control(pDX, IDC_INT2MACADDR, m_int2macaddr);
	DDX_Control(pDX, IDC_INT2IPADDR, m_int2ipaddr);
	DDX_Control(pDX, IDC_INT2IPBUTTON, m_int2ipbutton);
	DDX_Control(pDX, IDC_INT2SWBUTTON, m_int2swbutton);
	DDX_Control(pDX, IDC_INT2NATCOMBO, m_int2natmode);
	DDX_Control(pDX, IDC_RIBLIST, m_rib);
	DDX_Control(pDX, IDC_ARPLIST, m_arptable);
	DDX_Control(pDX, IDC_STATSLIST, m_stats);
	DDX_Control(pDX, IDC_STATSCHECK, m_statscheckbox);
	DDX_Control(pDX, IDC_RIPSWBUTTON, m_RipSWButton);
	DDX_Control(pDX, IDC_RIPUPDATEIN, m_ripupdatein);
	DDX_Control(pDX, IDC_NATLIST, m_nattable);
	DDX_Control(pDX, IDC_POOL, m_pool);
	DDX_Control(pDX, IDC_DYNNATSWBUTTON, m_DynNatSwButton);
	DDX_Control(pDX, IDC_PATSWBUTTON, m_PatSwButton);
}

BEGIN_MESSAGE_MAP(CRouterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_INT1IPBUTTON, &CRouterDlg::OnBnClickedInt1IPbutton)
	ON_MESSAGE(WM_EDITIP_MESSAGE, &CRouterDlg::OnEditIPMessage)
	ON_BN_CLICKED(IDC_INT2IPBUTTON, &CRouterDlg::OnBnClickedInt2ipbutton)
	ON_BN_CLICKED(IDC_INT1SWBUTTON, &CRouterDlg::OnBnClickedInt1swbutton)
	ON_BN_CLICKED(IDC_INT2SWBUTTON, &CRouterDlg::OnBnClickedInt2swbutton)
	ON_MESSAGE(WM_INSERTROUTE_MESSAGE, &CRouterDlg::OnInsertRouteMessage)
	ON_MESSAGE(WM_REMOVEROUTE_MESSAGE, &CRouterDlg::OnRemoveRouteMessage)
	ON_BN_CLICKED(IDC_ADDSTATICBUTTON, &CRouterDlg::OnBnClickedAddStaticButton)
	ON_BN_CLICKED(IDC_REMOVESTATICBUTTON, &CRouterDlg::OnBnClickedRemoveStaticButton)
	ON_BN_CLICKED(IDC_ARPCLEARBUTTON, &CRouterDlg::OnBnClickedArpClearButton)
	ON_MESSAGE(WM_INSERTARP_MESSAGE, &CRouterDlg::OnInsertArpMessage)
	ON_MESSAGE(WM_REMOVEARP_MESSAGE, &CRouterDlg::OnRemoveArpMessage)
	ON_BN_CLICKED(IDC_STATSCHECK, &CRouterDlg::OnBnClickedStatscheck)
	ON_BN_CLICKED(IDC_STATSRESETBUTTON, &CRouterDlg::OnBnClickedStatsResetButton)
	ON_MESSAGE(WM_INSERTSTAT_MESSAGE, &CRouterDlg::OnInsertStatMessage)
	ON_MESSAGE(WM_UPDATESTAT_MESSAGE, &CRouterDlg::OnUpdateStatMessage)
	ON_BN_CLICKED(IDC_RIPSWBUTTON, &CRouterDlg::OnBnClickedRipSwButton)
	ON_BN_CLICKED(IDC_RIPTIMERSBUTTON, &CRouterDlg::OnBnClickedRipTimersButton)
	ON_MESSAGE(WM_RIPUPDATESEC_MESSAGE, &CRouterDlg::OnRipUpdateSecMessage)
	ON_BN_CLICKED(IDC_ADDNATBUTTON, &CRouterDlg::OnBnClickedAddNatButton)
	ON_BN_CLICKED(IDC_REMOVENATBUTTON, &CRouterDlg::OnBnClickedRemoveNatButton)
	ON_MESSAGE(WM_INSERTNAT_MESSAGE, &CRouterDlg::OnInsertNatMessage)
	ON_MESSAGE(WM_REMOVENAT_MESSAGE, &CRouterDlg::OnRemoveNatMessage)
	ON_CBN_SELCHANGE(IDC_INT1NATCOMBO, &CRouterDlg::OnCbnSelchangeInt1NatCombo)
	ON_CBN_SELCHANGE(IDC_INT2NATCOMBO, &CRouterDlg::OnCbnSelchangeInt2NatCombo)
	ON_BN_CLICKED(IDC_POOLBUTTON, &CRouterDlg::OnBnClickedPoolButton)
	ON_MESSAGE(WM_EDITPOOL_MESSAGE, &CRouterDlg::OnEditPoolMessage)
	ON_MESSAGE(WM_UPDATENAT_MESSAGE, &CRouterDlg::OnUpdateNatMessage)
	ON_BN_CLICKED(IDC_NATRESETBUTTON, &CRouterDlg::OnBnClickedNatResetButton)
	ON_BN_CLICKED(IDC_DYNNATSWBUTTON, &CRouterDlg::OnBnClickedDynNatSWbutton)
	ON_BN_CLICKED(IDC_PATSWBUTTON, &CRouterDlg::OnBnClickedPatSWbutton)
END_MESSAGE_MAP()


// CRouterDlg message handlers

BOOL CRouterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	InitInterfacesInfo();
	InitRoutingTable();
	InitArpTable();
	InitStatsTable();
	InitNatTable();
	m_ripupdatein.SetWindowTextW(_T("---"));
	theApp.StartThreads();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRouterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRouterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRouterDlg::OnOK(void)
{
}


void CRouterDlg::InitInterfacesInfo(void)
{
	m_int1desc.SetWindowTextW(theApp.GetInt1()->GetDescription());
	m_int2desc.SetWindowTextW(theApp.GetInt2()->GetDescription());

	m_int1macaddr.SetWindowTextW(theApp.GetInt1()->GetMACAddress());
	m_int2macaddr.SetWindowTextW(theApp.GetInt2()->GetMACAddress());

	m_int1swbutton.SetTextColor(RGB(0,255,0));
	m_int2swbutton.SetTextColor(RGB(0,255,0));

	m_int1natmode.SetCurSel(0);
	m_int2natmode.SetCurSel(0);
}


void CRouterDlg::InitRoutingTable(void)
{
	m_rib.SetExtendedStyle(m_rib.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_rib.InsertColumn(0,_T("Type"),LVCFMT_CENTER,36);
	m_rib.InsertColumn(1,_T("Network"),LVCFMT_CENTER,112);
	m_rib.InsertColumn(2,_T("AD"),LVCFMT_CENTER,30);
	m_rib.InsertColumn(3,_T("Metric"),LVCFMT_CENTER,41);
	m_rib.InsertColumn(4,_T("Next Hop"),LVCFMT_CENTER,100);
	m_rib.InsertColumn(5,_T("Interface"),LVCFMT_CENTER,57);
}


void CRouterDlg::InitArpTable(void)
{
	m_arptable.SetExtendedStyle(m_arptable.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_arptable.InsertColumn(0,_T("IP address"),LVCFMT_CENTER,100);
	m_arptable.InsertColumn(1,_T("MAC address"),LVCFMT_CENTER,120);

	theApp.GetARPtable()->InsertDefaults();
}


void CRouterDlg::InitStatsTable(void)
{
	m_stats.SetExtendedStyle(m_stats.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_stats.InsertColumn(0,_T("NIC"),LVCFMT_CENTER,32);
	m_stats.InsertColumn(1,_T("In/Out"),LVCFMT_CENTER,44);
	m_stats.InsertColumn(2,_T("Frame type"),LVCFMT_CENTER,67);
	m_stats.InsertColumn(3,_T("EtherType"),LVCFMT_CENTER,62);
	m_stats.InsertColumn(4,_T("Protocol in IP"),LVCFMT_CENTER,75);
	m_stats.InsertColumn(5,_T("Count"),LVCFMT_CENTER,55);

	m_statscheckbox.SetCheck(BST_CHECKED);
}


void CRouterDlg::InitNatTable(void)
{
	m_nattable.SetExtendedStyle(m_stats.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_nattable.InsertColumn(0,_T("Protocol"),LVCFMT_CENTER,51);
	m_nattable.InsertColumn(1,_T("Mode"),LVCFMT_CENTER,56);
	m_nattable.InsertColumn(2,_T("Global address"),LVCFMT_CENTER,130);
	m_nattable.InsertColumn(3,_T("Local address"),LVCFMT_CENTER,130);
	m_nattable.InsertColumn(4,_T("Timeout"),LVCFMT_CENTER,50);
}


void CRouterDlg::AutoResizeColumns(CListCtrl *control)
{
	int i, ColumnWidth, HeaderWidth;
	int columns = control->GetHeaderCtrl()->GetItemCount();

	control->SetRedraw(FALSE);
	for (i=0;i < columns;i++)
	{
		control->SetColumnWidth(i,LVSCW_AUTOSIZE);
		ColumnWidth = control->GetColumnWidth(i);
		control->SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
		HeaderWidth = control->GetColumnWidth(i);
		control->SetColumnWidth(i,max(ColumnWidth,HeaderWidth));
	}
	control->SetRedraw(TRUE);
}


void CRouterDlg::EnableInterface(Interface *i, CMFCButton *swbutton)
{
	if (!i->IsIPaddrConfigured())
	{
		AfxMessageBox(_T("Please configure the IP address first!"));
		return;
	}
	
	i->SetEnabled(TRUE);
	swbutton->SetTextColor(RGB(255,0,0));
	swbutton->SetWindowTextW(_T("Disable interface"));
}


void CRouterDlg::DisableInterface(Interface *i, CMFCButton *swbutton)
{
	i->SetEnabled(FALSE);
	swbutton->SetTextColor(RGB(0,255,0));
	swbutton->SetWindowTextW(_T("Enable interface"));
}


void CRouterDlg::OnBnClickedInt1IPbutton()
{
	AfxBeginThread(CRouterDlg::EditIPThread,theApp.GetInt1());
}


void CRouterDlg::OnBnClickedInt2ipbutton()
{
	AfxBeginThread(CRouterDlg::EditIPThread,theApp.GetInt2());
}


void CRouterDlg::OnBnClickedInt1swbutton()
{
	Interface *i = theApp.GetInt1();

	if (i->IsEnabled()) DisableInterface(i,&m_int1swbutton);
	else EnableInterface(i,&m_int1swbutton);
}


void CRouterDlg::OnBnClickedInt2swbutton()
{
	Interface *i = theApp.GetInt2();

	if (i->IsEnabled()) DisableInterface(i,&m_int2swbutton);
	else EnableInterface(i,&m_int2swbutton);
}


UINT CRouterDlg::EditIPThread(void * pParam)
{
	IPAddrDlg ip_dlg((Interface *)pParam);
	ip_dlg.DoModal();
	
	return 0;
}


void CRouterDlg::EditIP(Interface *i, IPaddr ip_addr)
{
	i->SetIPAddress(ip_addr);
	SendMessage(WM_EDITIP_MESSAGE,0,(LPARAM)i);
	if (i->IsEnabled())
	{
		i->SetEnabled(FALSE);
		i->SetEnabled(TRUE);
	}
}


afx_msg LRESULT CRouterDlg::OnEditIPMessage(WPARAM wParam, LPARAM lParam)
{
	Interface *i = (Interface *)lParam;
	
	if (i->GetIndex() == 1) m_int1ipaddr.SetWindowTextW(i->GetIPAddrString());
	else m_int2ipaddr.SetWindowTextW(i->GetIPAddrString());

	return 0;
}


void CRouterDlg::OnCbnSelchangeInt1NatCombo()
{
	switch (m_int1natmode.GetCurSel())
	{
	case 0:
		theApp.GetInt1()->SetNATmode(DISABLED);
		break;
	case 1:
		theApp.GetInt1()->SetNATmode(INSIDE);
		break;
	case 2:
		theApp.GetInt1()->SetNATmode(OUTSIDE);
		break;
	}
}


void CRouterDlg::OnCbnSelchangeInt2NatCombo()
{
	switch (m_int2natmode.GetCurSel())
	{
	case 0:
		theApp.GetInt2()->SetNATmode(DISABLED);
		break;
	case 1:
		theApp.GetInt2()->SetNATmode(INSIDE);
		break;
	case 2:
		theApp.GetInt2()->SetNATmode(OUTSIDE);
		break;
	}
}


void CRouterDlg::InsertRoute(int index, Route& r)
{
	int *indexptr = (int *) malloc(sizeof(int));
	Route *rptr = (Route *) malloc(sizeof(Route));
	*indexptr = index;
	*rptr = r;
	SendMessage(WM_INSERTROUTE_MESSAGE,(WPARAM)indexptr,(LPARAM)rptr);
}


afx_msg LRESULT CRouterDlg::OnInsertRouteMessage(WPARAM wParam, LPARAM lParam)
{
	int *index = (int *)wParam;
	Route *r = (Route *)lParam;
	CString tmp;

	switch (r->type)
	{
	case CONNECTED:
		tmp.Format(_T("C"));
		break;

	case STATIC:
		tmp.Format(_T("S"));
		break;

	case RIP:
		tmp.Format(_T("R"));
		break;
	}
	if ((r->prefix.dw == 0) && (r->prefix.SubnetMaskCIDR == 0)) tmp.AppendFormat(_T("*"));
	m_rib.InsertItem(*index,tmp);

	tmp.Format(_T("%u.%u.%u.%u/%u"),r->prefix.b[3],r->prefix.b[2],r->prefix.b[1],r->prefix.b[0],r->prefix.SubnetMaskCIDR);
	m_rib.SetItemText(*index,1,tmp);

	tmp.Format(_T("%u"),r->AD);
	m_rib.SetItemText(*index,2,tmp);

	tmp.Format(_T("%u"),r->metric);
	m_rib.SetItemText(*index,3,tmp);

	if (r->NextHop.HasNextHop) tmp.Format(_T("%u.%u.%u.%u"),r->NextHop.b[3],r->NextHop.b[2],r->NextHop.b[1],r->NextHop.b[0]);
	else tmp.Format(_T("-"));
	m_rib.SetItemText(*index,4,tmp);

	if (!r->i) tmp.Format(_T("-"));
	else if (r->NextHop.HasNextHop) tmp.Format(_T("\"Int %d\""),r->i->GetIndex());
	else tmp.Format(_T("Int %d"),r->i->GetIndex());
	m_rib.SetItemText(*index,5,tmp);

	free(index);
	free(r);
	
	return 0;
}


void CRouterDlg::RemoveRoute(int index)
{
	int *indexptr = (int *) malloc(sizeof(int));

	*indexptr = index;
	SendMessage(WM_REMOVEROUTE_MESSAGE,0,(LPARAM)indexptr);
}


afx_msg LRESULT CRouterDlg::OnRemoveRouteMessage(WPARAM wParam, LPARAM lParam)
{
	int *index = (int *)lParam;
	
	m_rib.DeleteItem(*index);
	free(index);

	return 0;
}


void CRouterDlg::OnBnClickedAddStaticButton()
{
	AfxBeginThread(CRouterDlg::EditRouteThread,NULL);
}


void CRouterDlg::OnBnClickedRemoveStaticButton()
{
	int index = m_rib.GetSelectionMark();

	if (index == -1) AfxMessageBox(_T("No route was selected!"));
	else if (theApp.GetRIB()->RemoveStaticRoute(index)) AfxMessageBox(_T("Please select a STATIC route!"));
}


UINT CRouterDlg::EditRouteThread(void * pParam)
{
	StaticRouteDlg route_dlg;
	route_dlg.DoModal();

	return 0;
}


void CRouterDlg::OnBnClickedArpClearButton()
{
	theApp.GetARPtable()->RemoveAll();
	
	/*CString tmp;
	tmp.Format(_T("%d %d %d %d %d"),m_nattable.GetColumnWidth(0),m_nattable.GetColumnWidth(1),m_nattable.GetColumnWidth(2),
		m_nattable.GetColumnWidth(3),m_nattable.GetColumnWidth(4));
	AfxMessageBox(tmp);*/
}


void CRouterDlg::InsertArp(int index, ArpEntry& entry)
{
	int *indexptr = (int *) malloc(sizeof(int));
	ArpEntry *entryptr = (ArpEntry *) malloc(sizeof(ArpEntry));
	*indexptr = index;
	*entryptr = entry;
	SendMessage(WM_INSERTARP_MESSAGE,(WPARAM)indexptr,(LPARAM)entryptr);
}


afx_msg LRESULT CRouterDlg::OnInsertArpMessage(WPARAM wParam, LPARAM lParam)
{
	int *index = (int *)wParam;
	ArpEntry *entry = (ArpEntry *)lParam;
	CString tmp;
	
	tmp.Format(_T("%u.%u.%u.%u"),entry->ip.b[3],entry->ip.b[2],entry->ip.b[1],entry->ip.b[0]);
	m_arptable.InsertItem(*index,tmp);

	tmp.Format(_T("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X"),entry->mac.b[0],entry->mac.b[1],entry->mac.b[2],entry->mac.b[3],entry->mac.b[4],entry->mac.b[5]);
	m_arptable.SetItemText(*index,1,tmp);

	free(index);
	free(entry);

	return 0;
}


void CRouterDlg::RemoveArp(int index)
{
	int *indexptr = (int *) malloc(sizeof(int));

	*indexptr = index;
	SendMessage(WM_REMOVEARP_MESSAGE,0,(LPARAM)indexptr);
}


afx_msg LRESULT CRouterDlg::OnRemoveArpMessage(WPARAM wParam, LPARAM lParam)
{
	int *index = (int *)lParam;
	
	m_arptable.DeleteItem(*index);
	free(index);
	
	return 0;
}


void CRouterDlg::OnBnClickedStatscheck()
{
	if (m_statscheckbox.GetCheck()) theApp.GetStatistics()->SetEnabled(TRUE);
	else theApp.GetStatistics()->SetEnabled(FALSE);
}


void CRouterDlg::OnBnClickedStatsResetButton()
{
	Stats *s = theApp.GetStatistics();;
	
	s->m_cs_stats.Lock();
	s->Reset();
	m_stats.DeleteAllItems();
	s->m_cs_stats.Unlock();
}


void CRouterDlg::InsertStat(int index, Statistic& s)
{
	int *indexptr = (int *) malloc(sizeof(int));
	Statistic *sptr = (Statistic *) malloc(sizeof(Statistic));
	*indexptr = index;
	*sptr = s;
	SendMessage(WM_INSERTSTAT_MESSAGE,(WPARAM)indexptr,(LPARAM)sptr);
}


afx_msg LRESULT CRouterDlg::OnInsertStatMessage(WPARAM wParam, LPARAM lParam)
{
	int *index = (int *)wParam;
	Statistic *s = (Statistic *)lParam;
	ProtocolDB *db = ProtocolDB::GetInstance();
	CString tmp;

	tmp.Format(_T("Int %d"),s->IntId);
	m_stats.InsertItem(*index,tmp);

	if (s->d == In) m_stats.SetItemText(*index,1,_T("in"));
	else m_stats.SetItemText(*index,1,_T("out"));

	if (s->HasFrameType == 2) switch (s->FrameType)
	{
	case ETH2:
		m_stats.SetItemText(*index,2,_T("Eth II"));
		break;

	case RAW:
		m_stats.SetItemText(*index,2,_T("RAW"));
		break;

	case SNAP:
		m_stats.SetItemText(*index,2,_T("SNAP"));
		break;

	case LLC:
		m_stats.SetItemText(*index,2,_T("LLC"));
		break;
	}
	else m_stats.SetItemText(*index,2,_T("any"));

	if (s->HasLay3Type == 2) m_stats.SetItemText(*index,3,db->GetEth2ProtocolName(s->Lay3Type));
	else if (s->HasLay3Type == 1) m_stats.SetItemText(*index,3,_T("any"));
	else m_stats.SetItemText(*index,3,_T("-"));

	if (s->HasLay4Type == 2) m_stats.SetItemText(*index,4,db->GetIPProtocolName(s->Lay4Type));
	else if (s->HasLay4Type == 1) m_stats.SetItemText(*index,4,_T("any"));
	else m_stats.SetItemText(*index,4,_T("-"));

	tmp.Format(_T("%u"),s->count);
	m_stats.SetItemText(*index,5,tmp);

	free(index);
	free(s);
	
	return 0;
}


void CRouterDlg::UpdateStat(int index, UINT count)
{
	int *indexptr = (int *) malloc(sizeof(int));
	UINT *countptr = (UINT *) malloc(sizeof(UINT));
	*indexptr = index;
	*countptr = count;
	SendMessage(WM_UPDATESTAT_MESSAGE,(WPARAM)indexptr,(LPARAM)countptr);
}


afx_msg LRESULT CRouterDlg::OnUpdateStatMessage(WPARAM wParam, LPARAM lParam)
{
	int *index = (int *)wParam;
	UINT *count = (UINT *)lParam;
	CString tmp;

	tmp.Format(_T("%u"),*count);
	m_stats.SetItemText(*index,5,tmp);
	free(index);
	free(count);
	
	return 0;
}


void CRouterDlg::OnBnClickedRipSwButton()
{
	RoutingTable *rib = theApp.GetRIB();

	if (rib->IsRipEnabled())
	{
		AfxBeginThread(RoutingTable::StopRipProcess,NULL);
		m_RipSWButton.SetWindowTextW(_T("Start process"));
	}
	else
	{
		AfxBeginThread(RoutingTable::StartRipProcess,NULL);
		m_RipSWButton.SetWindowTextW(_T("Stop process"));
	}
}


void CRouterDlg::OnBnClickedRipTimersButton()
{
	AfxBeginThread(CRouterDlg::EditTimersThread,NULL);
}


UINT CRouterDlg::EditTimersThread(void * pParam)
{
	RipTimersDlg timers_dlg;
	timers_dlg.DoModal();

	return 0;
}


void CRouterDlg::SetRipUpdateStatus(int sec)
{
	int *secptr = (int *) malloc(sizeof(int));
	*secptr = sec;
	SendMessage(WM_RIPUPDATESEC_MESSAGE,0,(LPARAM)secptr);
}


afx_msg LRESULT CRouterDlg::OnRipUpdateSecMessage(WPARAM wParam, LPARAM lParam)
{
	int *sec = (int *)lParam;
	CString tmp;

	if (*sec == -1)
	{
		tmp.Format(_T("---"));
		m_ripupdatein.EnableWindow(FALSE);
	}
	else
	{
		tmp.Format(_T("%d"),*sec);
		if (!m_ripupdatein.IsWindowEnabled()) m_ripupdatein.EnableWindow(TRUE);
	}
	m_ripupdatein.SetWindowTextW(tmp);
	free(sec);
	
	return 0;
}


void CRouterDlg::OnBnClickedAddNatButton()
{
	AfxBeginThread(CRouterDlg::EditStaticNatThread,NULL);
}


void CRouterDlg::OnBnClickedRemoveNatButton()
{
	int index = m_nattable.GetSelectionMark();

	if (index == -1) AfxMessageBox(_T("No rule was selected!"));
	else if (theApp.GetNAT()->RemoveStaticRule(index)) AfxMessageBox(_T("Please select a STATIC rule!"));
}


void CRouterDlg::InsertTranslation(int index, Translation& nat)
{
	int *indexptr = (int *) malloc(sizeof(int));
	Translation *natptr = (Translation *) malloc(sizeof(Translation));
	*indexptr = index;
	*natptr = nat;
	SendMessage(WM_INSERTNAT_MESSAGE,(WPARAM)indexptr,(LPARAM)natptr);
}


afx_msg LRESULT CRouterDlg::OnInsertNatMessage(WPARAM wParam, LPARAM lParam)
{
	int *index = (int *)wParam;
	Translation *nat = (Translation *)lParam;
	ProtocolDB *db = ProtocolDB::GetInstance();
	CString tmp;

	if ((nat->type != PAT) && (!nat->IsPortForward)) m_nattable.InsertItem(*index,_T("-"));
	else m_nattable.InsertItem(*index,db->GetIPProtocolName(nat->Lay4Prot));

	if (nat->mode == INSIDE) m_nattable.SetItemText(*index,1,_T("INSIDE"));
	else m_nattable.SetItemText(*index,1,_T("OUTSIDE"));

	if ((nat->IsPortForward) || (nat->type == PAT)) tmp.Format(_T("%u.%u.%u.%u:%u"),nat->global.b[3],nat->global.b[2],nat->global.b[1],nat->global.b[0],nat->global_port);
	else tmp.Format(_T("%u.%u.%u.%u"),nat->global.b[3],nat->global.b[2],nat->global.b[1],nat->global.b[0]);
	m_nattable.SetItemText(*index,2,tmp);

	if ((nat->IsPortForward) || (nat->type == PAT)) tmp.Format(_T("%u.%u.%u.%u:%u"),nat->local.b[3],nat->local.b[2],nat->local.b[1],nat->local.b[0],nat->local_port);
	else tmp.Format(_T("%u.%u.%u.%u"),nat->local.b[3],nat->local.b[2],nat->local.b[1],nat->local.b[0]);
	m_nattable.SetItemText(*index,3,tmp);

	if (nat->type == STATICNAT) m_nattable.SetItemText(*index,4,_T("-"));
	else
	{
		tmp.Format(_T("%.2d:%.2d"),nat->timeout / 60, nat->timeout % 60);
		m_nattable.SetItemText(*index,4,tmp);
	}

	free(index);
	free(nat);
	
	return 0;
}


UINT CRouterDlg::EditStaticNatThread(void * pParam)
{
	StaticNatDlg nat_dlg;
	nat_dlg.DoModal();
	
	return 0;
}


void CRouterDlg::RemoveTranslation(int index)
{
	int *indexptr = (int *) malloc(sizeof(int));

	*indexptr = index;
	SendMessage(WM_REMOVENAT_MESSAGE,0,(LPARAM)indexptr);
}


afx_msg LRESULT CRouterDlg::OnRemoveNatMessage(WPARAM wParam, LPARAM lParam)
{
	int *index = (int *)lParam;
	
	m_nattable.DeleteItem(*index);
	free(index);
	
	return 0;
}


void CRouterDlg::OnBnClickedPoolButton()
{
	AfxBeginThread(CRouterDlg::EditPoolThread,NULL);
}


void CRouterDlg::EditPool(IPaddr first, IPaddr last)
{
	IPaddr *firstIPptr = (IPaddr *) malloc(sizeof(IPaddr));
	IPaddr *lastIPptr = (IPaddr *) malloc(sizeof(IPaddr));

	*firstIPptr = first;
	*lastIPptr = last;
	
	SendMessage(WM_EDITPOOL_MESSAGE,(WPARAM)firstIPptr,(LPARAM)lastIPptr);
}


afx_msg LRESULT CRouterDlg::OnEditPoolMessage(WPARAM wParam, LPARAM lParam)
{
	IPaddr *firstIPptr = (IPaddr *)wParam;
	IPaddr *lastIPptr = (IPaddr *)lParam;
	CString tmp;

	tmp.Format(_T("%u.%u.%u.%u/%u"),firstIPptr->b[3],firstIPptr->b[2],firstIPptr->b[1],firstIPptr->b[0],firstIPptr->SubnetMaskCIDR);
	tmp.AppendFormat(_T(" - "));
	tmp.AppendFormat(_T("%u.%u.%u.%u/%u"),lastIPptr->b[3],lastIPptr->b[2],lastIPptr->b[1],lastIPptr->b[0],lastIPptr->SubnetMaskCIDR);
	m_pool.SetWindowTextW(tmp);

	free(firstIPptr);
	free(lastIPptr);
	
	return 0;
}


UINT CRouterDlg::EditPoolThread(void * pParam)
{
	AddressPoolDlg pool_dlg;
	pool_dlg.DoModal();

	return 0;
}


void CRouterDlg::UpdateTranslation(int index, UINT timeout)
{
	int *indexptr = (int *) malloc(sizeof(int));
	UINT *timeoutptr = (UINT *) malloc(sizeof(UINT));
	*indexptr = index;
	*timeoutptr = timeout;
	SendMessage(WM_UPDATENAT_MESSAGE,(WPARAM)indexptr,(LPARAM)timeoutptr);
}


afx_msg LRESULT CRouterDlg::OnUpdateNatMessage(WPARAM wParam, LPARAM lParam)
{
	int *index = (int *)wParam;
	UINT *timeout = (UINT *)lParam;
	CString tmp;

	tmp.Format(_T("%.2d:%.2d"),*timeout / 60, *timeout % 60);
	m_nattable.SetItemText(*index,4,tmp);

	free(index);
	free(timeout);
	
	return 0;
}


void CRouterDlg::OnBnClickedNatResetButton()
{
	theApp.GetNAT()->RemoveAllDynamic();
}


void CRouterDlg::OnBnClickedDynNatSWbutton()
{
	NatTable *nat = theApp.GetNAT();

	if (nat->IsDynNatEnabled())
	{
		nat->SetDynNatEnabled(FALSE);
		m_DynNatSwButton.SetWindowTextW(_T("Start dynamic NAT"));
	}
	else
	{
		if (!nat->IsPoolConfigured())
		{
			AfxMessageBox(_T("Configure the IP address pool first!"));
			return;
		}
		
		if (nat->IsPatEnabled())
		{
			nat->SetPatEnabled(FALSE);
			m_PatSwButton.SetWindowTextW(_T("Start PAT"));
		}
		nat->SetDynNatEnabled(TRUE);
		m_DynNatSwButton.SetWindowTextW(_T("Stop dynamic NAT"));
	}
}


void CRouterDlg::OnBnClickedPatSWbutton()
{
	NatTable *nat = theApp.GetNAT();

	if (nat->IsPatEnabled())
	{
		nat->SetPatEnabled(FALSE);
		m_PatSwButton.SetWindowTextW(_T("Start PAT"));
	}
	else
	{
		if (nat->IsDynNatEnabled())
		{
			nat->SetDynNatEnabled(FALSE);
			m_DynNatSwButton.SetWindowTextW(_T("Start dynamic NAT"));
		}
		nat->SetPatEnabled(TRUE);
		m_PatSwButton.SetWindowTextW(_T("Stop PAT"));
	}
}
