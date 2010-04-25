// WinRefineShlExt.h : Declaration of the CWinRefineShlExt

#ifndef __WINREFINESHLEXT_H_
#define __WINREFINESHLEXT_H_

#include "resource.h"       // main symbols
#include "ThumbDialog.h"

#include <atlwin.h>
#include <comdef.h>
#include <shlobj.h>
#include <shlguid.h>

struct __declspec(uuid("00021500-0000-0000-c000-000000000046")) IQueryInfo;

/////////////////////////////////////////////////////////////////////////////
// CWinRefineShlExt
class ATL_NO_VTABLE CWinRefineShlExt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWinRefineShlExt, &CLSID_WinRefineShlExt>,
	public IDispatchImpl<IWinRefineShlExt, &IID_IWinRefineShlExt, &LIBID_WINREFINELib>,
	public IPersistFile,
	public IQueryInfo
{
	public:
		DECLARE_REGISTRY_RESOURCEID(IDR_WINREFINESHLEXT)
		DECLARE_PROTECT_FINAL_CONSTRUCT()

		BEGIN_COM_MAP(CWinRefineShlExt)
			COM_INTERFACE_ENTRY_IID(IID_IWinRefineShlExt, IWinRefineShlExt)
			COM_INTERFACE_ENTRY_IID(IID_IDispatch, IDispatch)
			COM_INTERFACE_ENTRY(IPersistFile)
			COM_INTERFACE_ENTRY_IID(IID_IQueryInfo, IQueryInfo)
		END_COM_MAP()

	// IWinRefineShlExt
	public:
		CWinRefineShlExt();

		CString GetFileName(CString m_sFilename);
		CString GetFileExt(CString m_sFilename);

		//IPersistFile
		STDMETHODIMP GetClassID(LPCLSID);
		STDMETHODIMP IsDirty()                { return E_NOTIMPL; }
		STDMETHODIMP Load(LPCOLESTR, DWORD);
		STDMETHODIMP Save(LPCOLESTR, BOOL)    { return E_NOTIMPL; }
		STDMETHODIMP SaveCompleted(LPCOLESTR) { return E_NOTIMPL; }
		STDMETHODIMP GetCurFile(LPOLESTR*)    { return E_NOTIMPL; }

		//IQueryInfo
		STDMETHODIMP GetInfoFlags(DWORD*)     { return E_NOTIMPL; }
		STDMETHODIMP GetInfoTip(DWORD dwFlags, LPWSTR* szStr);

	private:
		void ShowDialog(CPoint Point, CString FileName, HWND Parent);
		void CloseDialog();

		static DWORD WINAPI GeneralPreviewThread(LPVOID params);

		HRESULT GetInfoTipFromFile(CWinRefineShlExt* pnt, wchar_t* file, LPTSTR pszText, int cchTextMax);
		bool CanGo(HWND ListCtrl, int Sel2);
		bool IsValidAudioVideo(CComBSTR m_sFilename);
		bool IsValidImage(CComBSTR m_sFilename);
		void PerformPreview(CComBSTR FileName);

	protected:
		CComBSTR m_sFilename;
		CComBSTR sTooltip;

		CThumbDialog* Thumb;
};

#endif //__WINREFINESHLEXT_H_
