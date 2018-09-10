﻿// AESEncryptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PredictEd.h"
#include "AESEncryptDlg.h"
#include "afxdialogex.h"


// CAESEncryptDlg dialog

IMPLEMENT_DYNAMIC(CAESEncryptDlg, CDialog)

CAESEncryptDlg::CAESEncryptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_ENCPASS, pParent)
	, m_DeleteOriginal(false)
{
	m_FileEnc = FALSE;
}

CAESEncryptDlg::~CAESEncryptDlg()
{
}

void CAESEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_PASS, m_PassStrengthProg);
	DDX_Control(pDX, IDC_CHECK_DELFILE, m_DelCheckBox);
}


BEGIN_MESSAGE_MAP(CAESEncryptDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &CAESEncryptDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_PASS1, &CAESEncryptDlg::OnEnChangeEditPass1)
	ON_BN_CLICKED(IDC_BUTTON_GENPASS, &CAESEncryptDlg::OnBnClickedButtonGenpass)
	ON_BN_CLICKED(IDC_BUTTON_SELFILE, &CAESEncryptDlg::OnBnClickedButtonSelfile)
	ON_BN_CLICKED(IDC_BUTTON_DEC, &CAESEncryptDlg::OnBnClickedButtonDec)
END_MESSAGE_MAP()


// CAESEncryptDlg message handlers


BOOL CAESEncryptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetDlgItemText(IDC_EDIT_WARN, _T("Store this password securely. It cannot be recovered if you forget it."));

	m_PassStrengthProg.SetRange(0, 100);

	srand(time(NULL));

	if (m_FileEnc)
	{
		GetDlgItem(IDC_CHECK_DELFILE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_SELFILE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_DEC)->ShowWindow(SW_SHOW);
		SetWindowText(_T("Encrypt/Decrypt Any File"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CAESEncryptDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_WARN)
	{
		pDC->SetTextColor(RGB(150, 0, 0));
	}


	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void CAESEncryptDlg::OnBnClickedOk()
{
	CString pass;
	GetDlgItemText(IDC_EDIT_PASS1, pass);

	if (pass.IsEmpty())
	{
		AfxMessageBox(_T("Password is required!"), MB_ICONERROR);
		return;
	}
	if (m_SourceFileName.IsEmpty())
	{
		AfxMessageBox(_T("File name is required!"), MB_ICONERROR);
		return;
	}

	CString cryptfilename = m_SourceFileName + _T(".enc");

	CCryptHelper m_CryptHelper; //local because needs to reinit while dialog is open

	bool cryptres = m_CryptHelper.CryptFile(true, m_SourceFileName, cryptfilename, pass);

	if (cryptres)
	{
		AfxMessageBox(_T("File was AES encrypted and saved as:\r\n\r\n") + cryptfilename, MB_ICONINFORMATION);
		if (m_DelCheckBox.GetCheck())
		{
			if (DeleteFile(m_SourceFileName) && DeleteFile(m_BackupFileName)) AfxMessageBox(_T("Original file and backup file deleted."), MB_ICONINFORMATION);
			else AfxMessageBox(_T("Failed to delete the original file and backup file."), MB_ICONERROR);
		}
	}
	else AfxMessageBox(_T("Failed to encrypt the file:\r\n\r\n") + m_CryptHelper.GetLastError(), MB_ICONERROR);

	if (!m_FileEnc) CDialog::OnOK();
}


void CAESEncryptDlg::OnEnChangeEditPass1()
{
	CString pass;
	GetDlgItemText(IDC_EDIT_PASS1, pass);
	DisplayPasswordStrength(pass);
}

void CAESEncryptDlg::DisplayPasswordStrength(CString pass)
{
	if (pass.IsEmpty()) return;

	TCHAR nums[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		L'०', L'१', L'२', L'३', L'४', L'५', L'६', L'७', L'८', L'९' }; //L'x' is necessary

	TCHAR spl[] = { '~', '`', '!', '@', '#', '$', '%', '^', '&', '*',
		'(', ')', '-', '_', '+', '=', '|', '\\', '{', '[', '}', ']',
		':', ';', '\"', '\'', '<', ',', '>', '.', '?', '/' }; //32

	BOOL hasnum = FALSE;
	BOOL hasspl = FALSE;

	for (int i = 0; i < 20; i++)
	{
		if (pass.Find(nums[i]) >= 0)
		{
			hasnum = TRUE;
			break;
		}
	}

	for (int i = 0; i < 32; i++)
	{
		if (pass.Find(spl[i]) >= 0)
		{
			hasspl = TRUE;
			break;
		}
	}

	int passstrength = 0;
	int passlen = pass.GetLength();

	//these are arbitrary criteria
	passstrength = passlen * 5 + hasnum * 25 + hasspl * 25;
	m_PassStrengthProg.SetPos(passstrength);


}

void CAESEncryptDlg::OnBnClickedButtonGenpass()
{
	CString pass;
	int len = 10;
	int max = 126;
	int min = 33;
	int range = max - min + 1;

	for (int i = 0; i < len; i++)
	{
		int num = rand() % range + min;
		pass.AppendChar((WCHAR)num);
	}

	SetDlgItemText(IDC_EDIT_WARN, _T("Generated Password: ") + pass);
}


void CAESEncryptDlg::OnBnClickedButtonSelfile()
{
	CFileDialog DataFileOpenDialog(true, _T(""), _T(""), OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"));
	DataFileOpenDialog.m_ofn.lpstrTitle = _T("Select the file to encrypt/decrypt ...");

	INT_PTR res = DataFileOpenDialog.DoModal();
	if (res != IDCANCEL)
	{
		m_SourceFileName = DataFileOpenDialog.GetPathName();
		SetDlgItemText(IDC_BUTTON_SELFILE, DataFileOpenDialog.GetFileName());

	}
}


void CAESEncryptDlg::OnBnClickedButtonDec()
{
	CString pass;
	GetDlgItemText(IDC_EDIT_PASS1, pass);

	//m_CryptHelper.Create_SHA512_Hash(pass);
	//return;


	if (pass.IsEmpty())
	{
		AfxMessageBox(_T("Password is required!"), MB_ICONERROR);
		return;
	}
	if (m_SourceFileName.IsEmpty())
	{
		AfxMessageBox(_T("File name is required!"), MB_ICONERROR);
		return;
	}

	CString cryptfilename = m_SourceFileName + _T(".dec");

	CCryptHelper m_CryptHelper;

	bool cryptres = m_CryptHelper.CryptFile(false, m_SourceFileName, cryptfilename, pass);

	if (cryptres)
	{
		AfxMessageBox(_T("File was AES decrypted and saved as:\r\n\r\n") + cryptfilename, MB_ICONINFORMATION);
	}
	else AfxMessageBox(_T("Failed to decrypt the file:\r\n\r\n") + m_CryptHelper.GetLastError(), MB_ICONERROR);
}
