
// PredictEdDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "PredictEdCtrl.h"


// CPredictEdDlg dialog
class CPredictEdDlg : public CDialogEx
{
// Construction
public:
	CPredictEdDlg(CWnd* pParent = NULL);	// standard constructor
// Dialog Data

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PREDICTED_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CPredictEdCtrl m_Ed;






	void InitEd();
	void InsertText(CString text, COLORREF color = RGB(0, 0, 0), bool bold = false, bool italic = false);








};
