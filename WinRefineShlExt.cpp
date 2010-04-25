// WinRefineShlExt.cpp : Implementation of CWinRefineShlExt
#include "stdafx.h"
#include "WinRefine.h"
#include "WinRefineShlExt.h"
#include "Util.h"

#include "Player.cpp"

/////////////////////////////////////////////////////////////////////////////
// CWinRefineShlExt

#define MyKey CString(_T("Software\\CrystalSoft\\WinRefine"))

struct PassThread
{
	CWinRefineShlExt* pnt;
	CComBSTR* FileName;
};

CWinRefineShlExt::CWinRefineShlExt()
{
	Thumb = NULL;
}

CString CWinRefineShlExt::GetFileExt(CString m_sFilename)
{
	m_sFilename = m_sFilename.Right(m_sFilename.GetLength() - (m_sFilename.ReverseFind('.') + 1));
	m_sFilename.MakeLower();

	return m_sFilename;
}

CString CWinRefineShlExt::GetFileName(CString m_sFilename)
{
	return m_sFilename.Mid(m_sFilename.ReverseFind('\\') + 1);
}

typedef HRESULT (__stdcall* PsGetPropertyKeyFromName)(PCWSTR pszName, PROPERTYKEY* ppropkey);

HRESULT __stdcall GetPropertyKeyFromName(PCWSTR pszName, PROPERTYKEY* ppropkey)
{
	HRESULT Hr = E_NOTIMPL;

	HMODULE PropSys = LoadLibrary(_T("propsys.dll"));

	if (PropSys)
	{
		PsGetPropertyKeyFromName f = (PsGetPropertyKeyFromName)GetProcAddress(PropSys, "PSGetPropertyKeyFromName");

		if (f)
		{
			Hr = (f)(pszName, ppropkey);
		}

		FreeLibrary(PropSys);
	}

	return Hr;
}

CString* GetValueFromPropertyName(CString FilePath, CString PropertyName)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	LPMALLOC pMalloc;

	CString* Return = new CString[2];
	Return[0].Empty();
	Return[1].Empty();

	if (FAILED(SHGetMalloc(&pMalloc)))
	{
		return Return;
	}

	CString Path = FilePath.Left(FilePath.ReverseFind('\\') + 1);
	CString Filename = FilePath.Mid(FilePath.ReverseFind('\\') + 1);

	IPropertyUI* pID = NULL;
	HRESULT Hr = CoCreateInstance(CLSID_PropertiesUI, NULL, CLSCTX_INPROC, IID_IPropertyUI, (void**)&pID);

	if (SUCCEEDED(Hr))
	{
		IShellFolder* DesktopFolder = NULL;

		if (SUCCEEDED(SHGetDesktopFolder(&DesktopFolder)))
		{
			LPITEMIDLIST PIDL = NULL;

			if (SUCCEEDED(DesktopFolder->ParseDisplayName(NULL, NULL, Path.GetBuffer(), NULL, &PIDL, NULL)))
			{
				Path.ReleaseBuffer();

				IShellFolder2* FileFolder = NULL;

				if (SUCCEEDED(DesktopFolder->BindToObject(PIDL, NULL, IID_IShellFolder2, (void**)(&FileFolder))))
				{
					::CoTaskMemFree(PIDL);
					PIDL = NULL;

					if (SUCCEEDED(FileFolder->ParseDisplayName(NULL, NULL, Filename.GetBuffer(), NULL, &PIDL, NULL)))
					{
						Filename.ReleaseBuffer();

						PROPERTYKEY PropKey;

						ULONG Eated = 0;

						HRESULT Hr = pID->ParsePropertyName(PropertyName, &PropKey.fmtid, &PropKey.pid, &Eated);

						if (!SUCCEEDED(Hr))
						{
							Hr = GetPropertyKeyFromName(PropertyName, &PropKey);
						}

						if (SUCCEEDED(Hr))
						{
							pID->GetDisplayName(PropKey.fmtid, PropKey.pid, PUIFNF_DEFAULT, Return[0].GetBuffer(1024), 1024);
							Return[0].ReleaseBuffer();

							SHELLDETAILS ShellDetails;
							int Column = -1;

							TCHAR* temp = new TCHAR[1024];

							for (int i = 0; true; i++)
							{
								if (SUCCEEDED(FileFolder->GetDetailsOf(NULL, i, &ShellDetails)))
								{
									*temp = 0;

									::StrRetToStr(&ShellDetails.str, NULL, &temp);

									if (!wcscmp(temp, Return[0]))
									{
										Column = i;

										break;
									}
								}
							}

							if (Column >= 0)
							{
								if (SUCCEEDED(FileFolder->GetDetailsOf(PIDL, Column, &ShellDetails)))
								{
									*temp = 0;

									::StrRetToStr(&ShellDetails.str, NULL, &temp);

									Return[1] = temp;
								}
							}

							pMalloc->Free(temp);
						}

						FileFolder->Release();
					}
				}
			}

			DesktopFolder->Release();
		}
	}

	pMalloc->Release();

	CoUninitialize();

	return Return;
}

HRESULT CWinRefineShlExt::GetInfoTipFromFile(CWinRefineShlExt* pnt, wchar_t* file, LPTSTR pszText, int cchTextMax)
{
	HRESULT fRet = S_FALSE;

	*pszText = 0;

	CString FileName = CString(file).Mid(CString(file).ReverseFind('\\') + 1);

	CString FinalTip;
	FinalTip.Empty();

	DWORD propLen = 1024;
	wchar_t* propToRead;

	LPMALLOC pMalloc;

	if (FAILED(SHGetMalloc(&pMalloc)))
	{
		return E_FAIL;
	}

	ULONG cbText = (propLen + 1) * sizeof(wchar_t);

	propToRead = (wchar_t*)pMalloc->Alloc(cbText);
	pMalloc->Release();

	if (propToRead == NULL)
	{
		return E_OUTOFMEMORY;
	}

	IQueryAssociations* pQA = NULL;

	if (SUCCEEDED(AssocCreate(CLSID_QueryAssociations, IID_IQueryAssociations, (void**)&pQA)) && SUCCEEDED(pQA->Init(NULL, file, NULL, NULL)))
	{
		*propToRead = 0;

		if (!SUCCEEDED(pQA->GetString(NULL, ASSOCSTR_INFOTIP, NULL, propToRead, &propLen)))
		{
			CRegKey Reg;

			if (Reg.Open(HKEY_CLASSES_ROOT, _T("*\0"), KEY_QUERY_VALUE) == ERROR_SUCCESS)
			{
				DWORD dwcbNeeded = 1024;

				Reg.QueryStringValue(_T("InfoTip\0"), propToRead, &dwcbNeeded);

				Reg.Close();
			}
		}

		if (*propToRead)
		{
			if (!wcsncmp(propToRead, _T("prop:"), 5))
			{
				propToRead = propToRead + 5;

				if (wcslen(FileName) > 15)
				{
					FinalTip.Format(_T("%s\r\n"), FileName);
				}

				int len = wcslen(propToRead);

				for (int i = 0, z = 0; i < len; i++)
				{
					if ((propToRead[i] == ';') || ((propToRead[i] != ';') && ((i) == (len-1)) && i++))
					{
						CString Prop = CString(propToRead+z).Left(i - z);
						z = i+1;

						CString* Value = GetValueFromPropertyName(file, Prop);

						if (!Value[1].IsEmpty())
						{
							FinalTip.Format(_T("%s%s: %s\r\n"), FinalTip, Value[0], Value[1]);
						}
					}
				}
			}
		}

		pQA->Release();
	}

	if (!FinalTip.IsEmpty())
	{
		FinalTip.Delete(FinalTip.GetLength()-2, 2);

		lstrcpyn(pszText, FinalTip, cchTextMax);

		return S_OK;
	}

	pMalloc->Free(propToRead);

	return E_FAIL;
}

void CWinRefineShlExt::ShowDialog(CPoint Point, CString FileName, HWND Parent)
{
	Thumb = new CThumbDialog(Point, FileName, Parent);
	Thumb->Create(IDD_THUMB, CWnd::FromHandle(Parent));
}

void CWinRefineShlExt::CloseDialog()
{
	Thumb->DestroyWindow();

	delete Thumb;
	Thumb = NULL;
}

bool PositionChanged(CPoint Point)
{
	CPoint Point2;
	GetCursorPos(&Point2);

	int diff = 20;

	if ((((Point2.x - Point.x) >= diff) || ((Point2.x - Point.x) <= -diff)) || (((Point2.y - Point.y) >= diff) || ((Point2.y - Point.y) <= -diff)))
	{
		return true;
	}

	return false;
}

//C:\project\Sourcecode\MyProg\Visual C++\Pubblici\WinRefine\ReleaseUMinSize

DWORD WINAPI CWinRefineShlExt::GeneralPreviewThread(LPVOID params)
{
	PassThread* Ps = (PassThread*)params;

	CRegKey Reg;
	DWORD Latency = 0, Volume = 1000;

	if (Reg.Open(HKEY_CURRENT_USER, MyKey, KEY_READ) == ERROR_SUCCESS)
	{
		Reg.QueryDWORDValue(_T("Latency\0"), Latency);
		Reg.QueryDWORDValue(_T("Volume\0"), Volume);

		Reg.Close();
	}

	Sleep(Latency);

	CComBSTR m_sFilename = *Ps->FileName;

	int Type = Ps->pnt->IsValidAudioVideo(m_sFilename) ? 0 : 1;

	CPoint Point;
	GetCursorPos(&Point);

	HWND ListCtrl = WindowFromPoint(Point);
	HWND Parent = GetMainWindow(ListCtrl);

	bool ok = false;

	MidiPlayer Pl(m_sFilename, _T("icon"));

	if (!Type)
	{
		Pl.open();
		Pl.setvolume(Volume);

		ok = Pl.play();
	}

	else
	{
		Ps->pnt->ShowDialog(Point, m_sFilename.m_str, Parent);

		ok = true;
	}

	if (ok)
	{
		while (!PositionChanged(Point))
		{
			Sleep(100);
		}

		if (!Type)
		{
			Pl.stop();
			Pl.close();
		}

		else
		{
			Ps->pnt->CloseDialog();
		}
	}

	Ps->pnt->Release();

	return TRUE;
}

bool CWinRefineShlExt::CanGo(HWND ListCtrl, int Sel2)
{
	return ((GetListSelectedItem(ListCtrl) != -1) && GetListSelectedItem(ListCtrl) == Sel2) ? false : true;
}

bool CWinRefineShlExt::IsValidAudioVideo(CComBSTR m_sFilename)
{
	CString ext = GetFileExt(m_sFilename.m_str);

	wchar_t Extensions[1024] = _T("mp3|wav|avi|mpg|mpeg|wma|mpa|vob|wmv|asf");

	CRegKey Reg;

	if (Reg.Open(HKEY_CURRENT_USER, MyKey, KEY_READ) == ERROR_SUCCESS)
	{
		DWORD dwcbNeeded = 1024;
		Reg.QueryStringValue(_T("AudioVideo\0"), Extensions, &dwcbNeeded);
		Reg.Close();
	}

	if (wcsstr(Extensions, ext))
	{
		return true;
	}

	return false;
}

bool CWinRefineShlExt::IsValidImage(CComBSTR m_sFilename)
{
	CString ext = GetFileExt(m_sFilename.m_str);

	wchar_t Extensions[1024] = _T("png|gif|bmp|jpg|jpeg|tiff|tif|psd|tga");

	CRegKey Reg;

	if (Reg.Open(HKEY_CURRENT_USER, MyKey, KEY_READ) == ERROR_SUCCESS)
	{
		DWORD dwcbNeeded = 1024;
		Reg.QueryStringValue(_T("Image\0"), Extensions, &dwcbNeeded);
		Reg.Close();
	}

	if (wcsstr(Extensions, ext))
	{
		return true;
	}

	return false;
}

void CWinRefineShlExt::PerformPreview(CComBSTR FileName)
{
	PassThread* Ps = new PassThread;
	Ps->pnt = this;
	Ps->FileName = new CComBSTR(m_sFilename);

	if (IsValidAudioVideo(m_sFilename) || IsValidImage(m_sFilename))
	{
		AddRef();

		SHCreateThread(GeneralPreviewThread, Ps, CTF_COINIT | CTF_PROCESS_REF, NULL);
	}
}

STDMETHODIMP CWinRefineShlExt::GetClassID(LPCLSID csid)
{
	return E_NOTIMPL;
}

STDMETHODIMP CWinRefineShlExt::Load(LPCOLESTR wszFilename, DWORD dwMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //Init MFC
	UNREFERENCED_PARAMETER(dwMode);

	m_sFilename = wszFilename;

	PerformPreview(m_sFilename);

	return S_OK;
}

STDMETHODIMP CWinRefineShlExt::GetInfoTip(DWORD dwFlags, LPWSTR* ppwszTip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //Init MFC
	UNREFERENCED_PARAMETER(dwFlags);

	LPMALLOC pMalloc;

	if (FAILED(SHGetMalloc(&pMalloc)))
	{
		return E_FAIL;
	}

	ULONG cbText = (1025) * sizeof(LPWSTR);

	*ppwszTip = (LPWSTR)pMalloc->Alloc(cbText);
	pMalloc->Release();

    if (*ppwszTip == NULL)
	{
        return E_OUTOFMEMORY;
	}

	if (!SUCCEEDED(GetInfoTipFromFile(this, m_sFilename.m_str, *ppwszTip, 1024)))
	{
		wcscpy(*ppwszTip, m_sFilename.m_str);
	}

	else
	{
		if (IsValidImage(m_sFilename))
		{
			CRegKey Reg;
			DWORD MinWidth = 100, MinHeigth = 100, Tooltip = 0;

			if (Reg.Open(HKEY_CURRENT_USER, MyKey, KEY_READ) == ERROR_SUCCESS)
			{
				Reg.QueryDWORDValue(_T("MinWidth\0"), MinWidth);
				Reg.QueryDWORDValue(_T("MinHeight\0"), MinHeigth);
				Reg.QueryDWORDValue(_T("Tooltip\0"), Tooltip);

				Reg.Close();
			}

			FREE_IMAGE_FORMAT FIFormat = FreeImage_GetFileType(CStringA(m_sFilename));

			FIBITMAP* FBmp = FreeImage_Load(FIFormat, CStringA(m_sFilename), BMP_DEFAULT);

			if (FBmp)
			{
				int Width = FreeImage_GetWidth(FBmp);

				if (!((FreeImage_GetWidth(FBmp) < MinWidth) || (FreeImage_GetHeight(FBmp) < MinHeigth)))
				{
					if (!Tooltip)
					{
						*ppwszTip = 0;
					}
				}

				FreeImage_Unload(FBmp);
			}
		}
	}

	return S_OK;
}