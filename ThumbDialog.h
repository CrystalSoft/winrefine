#pragma once

#include "resource.h"
#include "afxwin.h"

// CThumbDialog dialog

class CThumbDialog : public CDialog
{
	DECLARE_DYNAMIC(CThumbDialog)

	public:
		CThumbDialog(CWnd* pParent = NULL);   // standard constructor
		CThumbDialog(CPoint Position, CString ImagePath, HWND Parent);   // standard constructor
		virtual ~CThumbDialog();

	// Dialog Data
		enum { IDD = IDD_THUMB };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		virtual BOOL OnInitDialog();

	private:
		CPoint Position;
		CString ImagePath;
		HWND Parent;
		CStatic Preview;
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
