// InitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Router.h"
#include "InitDlg.h"
#include "afxdialogex.h"


PIP_ADAPTER_ADDRESSES CInitDlg::AdaptersInfoBuffer = NULL;


// CInitDlg dialog

IMPLEMENT_DYNAMIC(CInitDlg, CDialog)

CInitDlg::CInitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInitDlg::IDD, pParent)
{

}

CInitDlg::~CInitDlg()
{
}

void CInitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INT1COMBO, m_int1box);
	DDX_Control(pDX, IDC_INT2COMBO, m_int2box);
}


BEGIN_MESSAGE_MAP(CInitDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CInitDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CInitDlg message handlers


BOOL CInitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	PIP_ADAPTER_ADDRESSES CurrentAdapter;
	size_t newsize;
	wchar_t * wcstring;
	CString error = ProtocolDB::GetInstance()->CheckTextFiles();
		
	if (error.IsEmpty() == FALSE) {
		AfxMessageBox(error,MB_ICONERROR);
		OnCancel();
	}

	if (FillAdaptersBuffer() != NO_ERROR)
	{
		MessageBox(CString("Error during searching for interfaces!"),_T("Error"),MB_ICONERROR);
		OnCancel();
	}

	CurrentAdapter = AdaptersInfoBuffer;
	while (CurrentAdapter)
	{
		if (CurrentAdapter->PhysicalAddressLength == 6)
		{
			IfNames.Add(CurrentAdapter->AdapterName);
			if (CurrentAdapter->Description)
			{
				IfDescs.Add(CurrentAdapter->Description);
				m_int1box.AddString(CString(CurrentAdapter->Description));
				m_int2box.AddString(CString(CurrentAdapter->Description));
			}
			else
			{
				newsize = strlen(CurrentAdapter->AdapterName) + 1;
				wcstring = new wchar_t[newsize];
				mbstowcs(wcstring,CurrentAdapter->AdapterName,newsize);
				IfDescs.Add(wcstring);
				m_int1box.AddString(CString(CurrentAdapter->AdapterName));
				m_int2box.AddString(CString(CurrentAdapter->AdapterName));
			}
			IfMACs.Add(CurrentAdapter->PhysicalAddress);
		}
		CurrentAdapter = CurrentAdapter->Next;
	}

	if (IfNames.GetCount() < 2)
	{
		MessageBox(_T("Application requires at least 2 network interfaces!"),_T("Error"),MB_ICONERROR);
		OnCancel();
	}

	m_int1box.SetCurSel(0);
	m_int2box.SetCurSel(1);
	return TRUE;
}


DWORD CInitDlg::FillAdaptersBuffer(void)
{
	DWORD dwRetVal = NO_ERROR;
	ULONG InfoBufLength = 10000;
	ULONG Iterations = 0;

	// Set the flags to pass to GetAdaptersAddresses
	ULONG flags = GAA_FLAG_SKIP_UNICAST | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
		
	if (!AdaptersInfoBuffer) do {
		AdaptersInfoBuffer = (IP_ADAPTER_ADDRESSES *) malloc(InfoBufLength);
		
		dwRetVal = GetAdaptersAddresses(AF_INET, flags, NULL, AdaptersInfoBuffer, &InfoBufLength);

		if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
			free(AdaptersInfoBuffer);
			AdaptersInfoBuffer = NULL;
		} else {
			break;
		}

		Iterations++;
	} while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < 3));

	return dwRetVal;
}


void CInitDlg::OnBnClickedOk()
{
	CStringA newname;

	if (m_int1box.GetCurSel() == m_int2box.GetCurSel())
	{
		MessageBox(_T("You cannot select the same device for each interfaces!"),_T("Warning"),MB_ICONWARNING);
		return;
	}
	newname.Format("rpcap://\\Device\\NPF_%s",IfNames[m_int1box.GetCurSel()]);
	theApp.GetInt1()->SetName(newname);
	theApp.GetInt1()->SetDescription(IfDescs[m_int1box.GetCurSel()]);
	theApp.GetInt1()->SetMACAddress(IfMACs[m_int1box.GetCurSel()]);
	newname.Format("rpcap://\\Device\\NPF_%s",IfNames[m_int2box.GetCurSel()]);
	theApp.GetInt2()->SetName(newname);
	theApp.GetInt2()->SetDescription(IfDescs[m_int2box.GetCurSel()]);
	theApp.GetInt2()->SetMACAddress(IfMACs[m_int2box.GetCurSel()]);
	IfNames.RemoveAll();
	IfDescs.RemoveAll();
	IfMACs.RemoveAll();
	free(AdaptersInfoBuffer);
	AdaptersInfoBuffer = NULL;

	CDialog::OnOK();
}
