
// Router.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Router.h"
#include "InitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRouterApp

BEGIN_MESSAGE_MAP(CRouterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CRouterApp construction

CRouterApp::CRouterApp()
	: rib(NULL)
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CRouterApp object

CRouterApp theApp;


// CRouterApp initialization

BOOL CRouterApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
		
	Int1 = new Interface(1);
	Int2 = new Interface(2);
	rib = new RoutingTable();
	RouterARPtab = new ArpTable();
	StatsTable = new Stats();
			
	CInitDlg init_dlg;
	INT_PTR nResponse = init_dlg.DoModal();
	if (nResponse == IDOK)
	{
		CRouterDlg router_dlg;
		m_pMainWnd = &router_dlg;
		router_dlg.DoModal();
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


Interface * CRouterApp::GetInt1(void)
{
	return Int1;
}


Interface * CRouterApp::GetInt2(void)
{
	return Int2;
}


CRouterDlg * CRouterApp::GetRouterDlg(void)
{
	return (CRouterDlg *)m_pMainWnd;
}


RoutingTable * CRouterApp::GetRIB(void)
{
	return rib;
}


ArpTable * CRouterApp::GetARPtable(void)
{
	return RouterARPtab;
}


Stats * CRouterApp::GetStatistics(void)
{
	return StatsTable;
}


UINT CRouterApp::RoutingProcess(void * pParam)
{
	Interface *iface = (Interface *) pParam;
	Interface *out_if;
	Frame *buffer = iface->GetBuffer();
	Frame TTLex;
	Stats *stats = theApp.GetStatistics();
	RoutingTable *rib = theApp.GetRIB();
	ArpTable *arp = theApp.GetARPtable();
	int retval;
	MACaddr src_mac, dest_mac, local_mac = iface->GetMACAddrStruct();
	IPaddr dest_ip, NextHop, *NextHop_ptr;

	while (TRUE)
	{
		buffer->GetFrame();

		src_mac = buffer->GetSrcMAC();
		// if source MAC address is the local MAC address, the frame is ignored
		if (theApp.CompareMAC(src_mac,local_mac) == 0) continue;
		dest_mac = buffer->GetDestMAC();
		// if source and destination MAC addresses are the same, the frame is ignored
		if (theApp.CompareMAC(src_mac,dest_mac) == 0) continue;
		// if source MAC address is broadcast address, the frame is ignored
		if (theApp.IsBroadcast(src_mac)) continue;

		// if destination MAC address is not the local or broadcast MAC address
		// or multicast for router, the frame is ignored
		if ((theApp.CompareMAC(dest_mac,local_mac) == 1)
			&& (theApp.IsBroadcast(dest_mac) == 0)
			&& (!buffer->IsMulticast9())) continue;

		// create statistics for the incoming frame
		stats->Add(iface->GetIndex(),In,buffer);

		// if it is ARP
		if (buffer->GetLay3Type() == 0x0806)
		{
			if (buffer->IsArpReply()) arp->ProcessArpReply(buffer,iface);
			if (buffer->IsArpRequest()) arp->ReplyToRequest(buffer,iface);
			continue;
		}

		// if not include IP packet, the frame is ignored
		if (buffer->GetLay3Type() != 0x0800) continue;

		// if the IP header checksum is not valid, the packet is ignored
		if (!buffer->IsIPChecksumValid()) continue;

		// check for RIPv2 messages
		retval = buffer->IsRipMessage();

		// if it is RIPv2 request message
		if (retval == 1)
		{
			rib->ProcessRipRequest(buffer,iface);
			continue;
		}

		// if it is RIPv2 response message
		if (retval == 2)
		{
			rib->ProcessRipResponse(buffer,iface);
			continue;
		}

		// decrease the TTL value of packet by 1
		buffer->DecTTL();
		if (buffer->GetTTL() == 0)
		{
			// traceroute
			TTLex.GenerateTTLExceeded(buffer,iface->GetIPAddrStruct(),iface->GenerateIpHeaderID());
			iface->SendFrame(&TTLex,NULL,FALSE);
			TTLex.Clear();
			continue;
		}
		
		dest_ip = buffer->GetDestIPaddr();
		// if destination of packet is this router
		if ((theApp.GetInt1()->IsLocalIP(dest_ip)) || (theApp.GetInt2()->IsLocalIP(dest_ip)))
		{
			// ECHO reply
			if ((buffer->IsIcmpEchoRequest()) && (buffer->IsICMPChecksumValid()))
			{
				buffer->GenerateIcmpEchoReply(dest_ip);
				iface->SendFrame(buffer,NULL,FALSE);
			}
			continue;
		}

		// NAT

		rib->m_cs_table.Lock();
		NextHop_ptr = NULL;
		out_if = rib->DoLookup(dest_ip,&NextHop_ptr);
		if (NextHop_ptr) {
			NextHop = *NextHop_ptr;
			NextHop_ptr = &NextHop;
		}
		rib->m_cs_table.Unlock();
		// if can not find prefix in the routing table, the packet is ignored
		if (!out_if) continue;

		// recalculate IP header checksum
		buffer->FillIPChecksum();

		// pack the packet to a new frame and send it out
		retval = out_if->SendFrame(buffer,NextHop_ptr);
		if (retval != 0) break;
	}

	return 0;
}


void CRouterApp::StartThreads(void)
{
	AfxBeginThread(CRouterApp::RoutingProcess,Int1);
	AfxBeginThread(CRouterApp::RoutingProcess,Int2);
}


int CRouterApp::CompareMAC(MACaddr& mac1, MACaddr& mac2)
{
	int i;

	for (i=0;i < 6;i++) if (mac1.b[i] != mac2.b[i]) return 1;
	
	return 0;
}


int CRouterApp::IsBroadcast(MACaddr& address)
{
	int i;

	for (i=0;i < 6;i++) if (address.b[i] != 0xFF) return 0;
	
	return 1;
}
