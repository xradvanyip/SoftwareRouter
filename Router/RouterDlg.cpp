
// RouterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Router.h"
#include "RouterDlg.h"
#include "afxdialogex.h"
#include "IPAddrDlg.h"

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
	m_rib.InsertColumn(4,_T("Next Hop"),LVCFMT_CENTER,112);
	m_rib.InsertColumn(5,_T("Interface"),LVCFMT_CENTER,57);
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
	theApp.GetRIB()->AddDirectlyConnected(i);
	swbutton->SetTextColor(RGB(255,0,0));
	swbutton->SetWindowTextW(_T("Disable interface"));
}


void CRouterDlg::DisableInterface(Interface *i, CMFCButton *swbutton)
{
	i->SetEnabled(FALSE);
	theApp.GetRIB()->RemoveDirectlyConnected(i);
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

	/*CString tmp;
	tmp.Format(_T("%d %d %d %d %d %d"),m_rib.GetColumnWidth(0),m_rib.GetColumnWidth(1),m_rib.GetColumnWidth(2)
		,m_rib.GetColumnWidth(3),m_rib.GetColumnWidth(4),m_rib.GetColumnWidth(5));
	AfxMessageBox(tmp);*/
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
		theApp.GetRIB()->RemoveDirectlyConnected(i);
		theApp.GetRIB()->AddDirectlyConnected(i);
	}
}


afx_msg LRESULT CRouterDlg::OnEditIPMessage(WPARAM wParam, LPARAM lParam)
{
	Interface *i = (Interface *)lParam;
	
	if (i->GetIndex() == 1) m_int1ipaddr.SetWindowTextW(i->GetIPAddrString());
	else m_int2ipaddr.SetWindowTextW(i->GetIPAddrString());

	return 0;
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
		m_rib.InsertItem(*index,_T("C"));
		break;

	case STATIC:
		m_rib.InsertItem(*index,_T("S"));
		break;

	case RIP:
		m_rib.InsertItem(*index,_T("R"));
	}

	tmp.Format(_T("%u.%u.%u.%u/%u"),r->prefix.b[3],r->prefix.b[2],r->prefix.b[1],r->prefix.b[0],r->prefix.SubnetMaskCIDR);
	m_rib.SetItemText(*index,1,tmp);

	tmp.Format(_T("%u"),r->AD);
	m_rib.SetItemText(*index,2,tmp);

	tmp.Format(_T("%u"),r->metric);
	m_rib.SetItemText(*index,3,tmp);

	if (r->NextHop.HasNextHop) tmp.Format(_T("%u.%u.%u.%u"),r->NextHop.b[3],r->NextHop.b[2],r->NextHop.b[1],r->NextHop.b[0]);
	else tmp.Format(_T("-"));
	m_rib.SetItemText(*index,4,tmp);

	tmp.Format(_T("Int %d"),r->i->GetIndex());
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
