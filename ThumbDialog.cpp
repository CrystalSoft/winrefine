// ThumbDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ThumbDialog.h"

#define MyKey CString(_T("Software\\CrystalSoft\\WinRefine"))

// CThumbDialog dialog

IMPLEMENT_DYNAMIC(CThumbDialog, CDialog)

CThumbDialog::CThumbDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CThumbDialog::IDD, pParent)
{
	
}

CThumbDialog::CThumbDialog(CPoint Position, CString ImagePath, HWND Parent)
{
	CThumbDialog::Position = Position;
	CThumbDialog::ImagePath = ImagePath;
	CThumbDialog::Parent = Parent;
}

CThumbDialog::~CThumbDialog()
{
	
}

void CThumbDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREVIEW, Preview);
}


BEGIN_MESSAGE_MAP(CThumbDialog, CDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CThumbDialog message handlers

HBITMAP GetHBitmap(FIBITMAP* FBmp)
{
	HDC hDC = GetDC(NULL);

	HBITMAP hBmp = CreateDIBitmap(hDC, FreeImage_GetInfoHeader(FBmp), CBM_INIT, FreeImage_GetBits(FBmp), FreeImage_GetInfo(FBmp), DIB_RGB_COLORS);

	ReleaseDC(NULL, hDC);

	return hBmp;
}

BOOL CThumbDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRegKey Reg;
	DWORD Transparency = 255, Size = 300, MinWidth = 100, MinHeigth = 100, Tooltip = 0;

	if (Reg.Open(HKEY_CURRENT_USER, MyKey, KEY_READ) == ERROR_SUCCESS)
	{
		Reg.QueryDWORDValue(_T("Transparency\0"), Transparency);
		Reg.QueryDWORDValue(_T("MinWidth\0"), MinWidth);
		Reg.QueryDWORDValue(_T("MinHeight\0"), MinHeigth);
		Reg.QueryDWORDValue(_T("ThumbnailSize\0"), Size);
		Reg.QueryDWORDValue(_T("Tooltip\0"), Tooltip);

		Reg.Close();
	}

	if (Transparency < 255)
	{
		SetWindowLong(this->m_hWnd, GWL_EXSTYLE, WS_EX_LAYERED);
		SetLayeredWindowAttributes(0, Transparency, LWA_ALPHA);
	}

	int Move = 40;

	if (Tooltip)
	{
		Move = 70;
	}

	int ScreenWidth = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int ScreenHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);

	int PositionX = Position.x;
	int PositionY = Position.y + Move;

	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE);

	FREE_IMAGE_FORMAT FIFormat = FreeImage_GetFileType(CStringA(ImagePath));

	FIBITMAP* FBmp = FreeImage_Load(FIFormat, CStringA(ImagePath), BMP_DEFAULT); //CStringA(ImagePath)
	ImagePath.ReleaseBuffer();

	if (FBmp)
	{
		FBmp = FreeImage_ConvertTo24Bits(FBmp);

		if (FBmp)
		{
			int Width = FreeImage_GetWidth(FBmp);
			int Height = FreeImage_GetHeight(FBmp);

			if ((Width < MinWidth) || (Height < MinHeigth))
			{
				FreeImage_Unload(FBmp);

				ShowWindow(SW_HIDE);

				return FALSE;
			}
			
			FBmp = FreeImage_MakeThumbnail(FBmp, Size);

			if (FBmp)
			{
				Width = FreeImage_GetWidth(FBmp);
				Height = FreeImage_GetHeight(FBmp);

				if ((PositionX + Width) > ScreenWidth)
				{
					PositionX = (ScreenWidth - (Width + 10));
				}

				if ((PositionY + Height) > ScreenHeight)
				{
					PositionY = (Position.y - Height) - Move;
				}

				SetWindowPos(NULL, PositionX, PositionY, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

				HBITMAP sBmp = GetHBitmap(FBmp);

				if (sBmp)
				{
					Preview.ModifyStyle(SS_GRAYRECT, SS_BITMAP, 0);

					Preview.SetBitmap(sBmp);
					Preview.SetWindowPos(CWnd::FromHandle(HWND_TOPMOST), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOACTIVATE);
					Preview.RedrawWindow();

					SetWindowPos(NULL, 0, 0, Width, Height, SWP_NOMOVE | SWP_NOACTIVATE);
				}

				FreeImage_Unload(FBmp);
			}
		}
	}

	ShowWindow(SW_SHOWNOACTIVATE);
	RedrawWindow();

	return FALSE;
}
int CThumbDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (lpCreateStruct)
	{
		lpCreateStruct->dwExStyle &= WS_EX_NOACTIVATE;
	}

	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
