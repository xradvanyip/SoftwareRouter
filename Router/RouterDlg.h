
// RouterDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxbutton.h"
#include "fixlistctrl.h"
#include "Interface.h"
#include "RoutingTable.h"
#include "ArpTable.h"

#define WM_EDITIP_MESSAGE WM_APP+100
#define WM_INSERTROUTE_MESSAGE WM_APP+101
#define WM_REMOVEROUTE_MESSAGE WM_APP+102
#define WM_INSERTARP_MESSAGE WM_APP+103
#define WM_REMOVEARP_MESSAGE WM_APP+104

// CRouterDlg dialog
class CRouterDlg : public CDialog
{
// Construction
public:
	CRouterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ROUTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual void OnOK(void);
private:
	CStatic m_int1desc;
	CStatic m_int1macaddr;
	CStatic m_int1ipaddr;
	CButton m_int1ipbutton;
	CMFCButton m_int1swbutton;
	CComboBox m_int1natmode;

	CStatic m_int2desc;
	CStatic m_int2macaddr;
	CStatic m_int2ipaddr;
	CButton m_int2ipbutton;
	CMFCButton m_int2swbutton;
	CComboBox m_int2natmode;

	CFixListCtrl m_rib;

	CFixListCtrl m_arptable;

	void AutoResizeColumns(CListCtrl *control);
	void InitInterfacesInfo(void);
	void InitRoutingTable(void);
	void InitArpTable(void);
public:
	afx_msg void OnBnClickedInt1IPbutton();
	afx_msg void OnBnClickedInt2ipbutton();
	afx_msg void OnBnClickedInt1swbutton();
	afx_msg void OnBnClickedInt2swbutton();
	afx_msg void OnBnClickedAddStaticButton();
	afx_msg void OnBnClickedRemoveStaticButton();
	afx_msg void OnBnClickedArpClearButton();
	static UINT EditIPThread(void * pParam);
	void EditIP(Interface *i, IPaddr ip_addr);
	void EnableInterface(Interface *i, CMFCButton *swbutton);
	void DisableInterface(Interface *i, CMFCButton *swbutton);
	void InsertRoute(int index, Route& r);
	void RemoveRoute(int index);
	static UINT EditRouteThread(void * pParam);
	void InsertArp(int index, ArpEntry& entry);
	void RemoveArp(int index);
protected:
	afx_msg LRESULT OnEditIPMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInsertRouteMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRemoveRouteMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInsertArpMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRemoveArpMessage(WPARAM wParam, LPARAM lParam);
};
