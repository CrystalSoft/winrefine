#include "StdAfx.h"
#include "Util.h"

bool StringToInt(char* digit, int& result)
{
	result = 0;

	while (*digit >= '0' && *digit <='9')
	{
		result = (result * 10) + (*digit - '0');
		digit++;
	}

	if (*digit != 0)
	{
		return false;
	}

	return true;
}

HANDLE GetProcess(HWND hWnd, DWORD dwAccesses)
{
	DWORD PID;
	GetWindowThreadProcessId(hWnd, &PID);

	return OpenProcess(dwAccesses, FALSE, PID);
}

DWORD GetProcessName(HWND hWnd, wchar_t* buf, int bufsize)
{
	typedef DWORD (WINAPI *tGetModuleBaseNameA)(HANDLE, HMODULE, LPTSTR, DWORD); 

	tGetModuleBaseNameA GetModuleBaseName = 0;

	HINSTANCE hInstance = LoadLibrary(_T("Psapi.dll"));

	if (hInstance == NULL)
		return 0;

	GetModuleBaseName = (tGetModuleBaseNameA)::GetProcAddress(hInstance, "GetModuleBaseNameW");

	if (GetModuleBaseName)
	{
		HANDLE hProcess = GetProcess(hWnd, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);

		if (!GetModuleBaseName(hProcess, NULL, buf, bufsize/sizeof(TCHAR)))
		{
			wcscpy(buf, _T("Unknown"));
		}

		CloseHandle(hProcess);
	}

	::FreeLibrary(hInstance);

	return 1;
}

DWORD GetWindowModuleFileName(HWND hWnd, wchar_t* buf, int bufsize)
{
	typedef DWORD (WINAPI *tGetModuleFileNameExA)(HANDLE, HMODULE, LPTSTR, DWORD); 

	tGetModuleFileNameExA GetModuleFilenameEx = 0;

	HINSTANCE hInstance = LoadLibrary(_T("Psapi.dll"));

	if (hInstance == NULL)
		return 0;

	GetModuleFilenameEx = (tGetModuleFileNameExA)::GetProcAddress(hInstance, "GetModuleFileNameExW");

	if (GetModuleFilenameEx)
	{
		HANDLE hProcess = GetProcess(hWnd, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);

		if (!GetModuleFilenameEx(hProcess, NULL, buf, bufsize))
		{
			wcscpy(buf, _T("Unknown"));
		}

		CloseHandle(hProcess);
	}

	::FreeLibrary(hInstance);

	return 1;
}

int KillProcess(HWND hWnd)
{
	HANDLE hProcess = GetProcess(hWnd, SYNCHRONIZE | PROCESS_TERMINATE);

	if (WaitForSingleObject(hProcess, 200) != WAIT_OBJECT_0)
	{
		if (!TerminateProcess(hProcess, 0))
		{
			return 1;
		}
	}

	CloseHandle(hProcess);

	return 0;
}

HWND GetMainWindow(HWND hWnd)
{
	CWnd* Win = CWnd::FromHandle(hWnd)->GetTopLevelParent();

	return Win->m_hWnd;
}

void GetWindowInformations(CWnd* Win, CString &Title, CString &Class, CString &Process, CString &Exe, int &ID)
{
	wchar_t tmp[1000];

	Win->GetWindowText(Title);

	if (Title.GetLength() < 1)
	{
		Title = _T("No Title");
	}

	if (GetClassName(Win->m_hWnd, tmp, 1000))
	{
		Class = tmp;
	}

	else
	{
		Class = _T("Unknown");
	}

	GetProcessName(Win->m_hWnd, tmp, 1000);
	Process = tmp;

	GetWindowModuleFileName(Win->m_hWnd, tmp, 1000);
	Exe = tmp;

	ID = Win->GetDlgCtrlID();
}

bool IsListCtrl(HWND hWnd)
{
	wchar_t clas[1024];
	GetClassName(hWnd, clas, 1024);

	CString Class = clas;
	Class.MakeLower();

	return Class.Find(_T("listview")) >= 0 ? true : false;
}

CString GetListItemText(HWND hWnd, int Item, int SubItem)
{
	CString Txt;

	if (hWnd && IsListCtrl(hWnd))
	{
		LVITEM lvi, *_lvi;
		wchar_t item[512], subitem[512];
		wchar_t *_item, *_subitem;

		HANDLE hProcess = GetProcess(hWnd, PROCESS_ALL_ACCESS);

		_lvi = (LVITEM*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
		_item = (wchar_t*)VirtualAllocEx(hProcess, NULL, 512, MEM_COMMIT, PAGE_READWRITE);
		_subitem = (wchar_t*)VirtualAllocEx(hProcess, NULL, 512, MEM_COMMIT, PAGE_READWRITE);

		lvi.cchTextMax = 512;

		if (hProcess)
		{
			lvi.iSubItem = SubItem;
			lvi.pszText = _item;

			WriteProcessMemory(hProcess, _lvi, &lvi, sizeof(LVITEM), NULL);

			SendMessage(hWnd, LVM_GETITEMTEXT, (WPARAM)Item, (LPARAM)_lvi);

			lvi.iSubItem = 1;
			lvi.pszText = _subitem;

			WriteProcessMemory(hProcess, _lvi, &lvi, sizeof(LVITEM), NULL);

			SendMessage(hWnd, LVM_GETITEMTEXT, (WPARAM)Item, (LPARAM)_lvi);

			ReadProcessMemory(hProcess, _item, item, 512, NULL);
			ReadProcessMemory(hProcess, _subitem, subitem, 512, NULL);

			Txt = item;
		}

		VirtualFreeEx(hProcess, _lvi, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, _item, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, _subitem, 0, MEM_RELEASE);

		CloseHandle(hProcess);
	}

	return Txt;
}

int GetListSelectedItem(HWND hWnd)
{
	if (hWnd && IsListCtrl(hWnd))
	{
		CWnd* Window = CWnd::FromHandle(hWnd);

		int Sel = -1;

		POSITION p = ((CListCtrl*)Window)->GetFirstSelectedItemPosition();

		while (p)
		{
			Sel = ((CListCtrl*)Window)->GetNextSelectedItem(p);
		}

		if (((CListCtrl*)Window)->GetItemCount() > 0 && Sel <= ((CListCtrl*)Window)->GetItemCount())
		{
			return Sel;
		}
	}

	return -1;
}

int HitListTest(HWND hWnd, LVHITTESTINFO* pt)
{
	int sel = -1;

	if (hWnd && IsListCtrl(hWnd))
	{
		LVHITTESTINFO* _pt;

		HANDLE hProcess = GetProcess(hWnd, PROCESS_ALL_ACCESS);

		_pt = (LVHITTESTINFO*)VirtualAllocEx(hProcess, NULL, sizeof(LVHITTESTINFO), MEM_COMMIT, PAGE_READWRITE);

		if (hProcess)
		{
			WriteProcessMemory(hProcess, _pt, pt, sizeof(LVHITTESTINFO), NULL);

			sel = SendMessage(hWnd, LVM_HITTEST, (WPARAM)0, (LPARAM)_pt);
		}

		VirtualFreeEx(hProcess, _pt, 0, MEM_RELEASE);

		CloseHandle(hProcess);
	}

	return sel;
}

wchar_t* GetToolTipText(HWND hWnd)
{
	TOOLINFO lp, * _lp;
	wchar_t* _txt;

	HANDLE hProcess = GetProcess(hWnd, PROCESS_ALL_ACCESS);

	_lp = (TOOLINFO*)VirtualAllocEx(hProcess, NULL, sizeof(TOOLINFO), MEM_COMMIT, PAGE_READWRITE);
	_txt = (wchar_t*)VirtualAllocEx(hProcess, NULL, 512, MEM_COMMIT, PAGE_READWRITE);

	if (hProcess)
	{
		lp.hwnd = hWnd;
		lp.cbSize = sizeof(TOOLINFO);
		lp.lpszText = _txt;
		//GetWindowThreadProcessId(hWnd, &lp.uId);

		WriteProcessMemory(hProcess, _lp, &lp, sizeof(TOOLINFO), NULL);

		SendMessage(hWnd, TTM_GETTEXT, (WPARAM)0, (LPARAM)(LPTOOLINFO)_lp);

		ReadProcessMemory(hProcess, _lp->lpszText, _txt, 512, NULL);
	}

	VirtualFreeEx(hProcess, _lp, 0, MEM_RELEASE);

	CloseHandle(hProcess);

	return _txt;
}

/*
 * AnsiToUnicode converts the ANSI string pszA to a Unicode string
 * and returns the Unicode string through ppszW. Space for the
 * the converted string is allocated by AnsiToUnicode.
 */ 

HRESULT __fastcall AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW)
{

    ULONG cCharacters;
    DWORD dwError;

    // If input is null then just return the same.
    if (NULL == pszA)
    {
        *ppszW = NULL;
        return NOERROR;
    }

    // Determine number of wide characters to be allocated for the
    // Unicode string.
    cCharacters =  strlen(pszA)+1;

    // Use of the OLE allocator is required if the resultant Unicode
    // string will be passed to another COM component and if that
    // component will free it. Otherwise you can use your own allocator.
    *ppszW = (LPOLESTR) CoTaskMemAlloc(cCharacters*2);
    if (NULL == *ppszW)
        return E_OUTOFMEMORY;

    // Covert to Unicode.
    if (0 == MultiByteToWideChar(CP_ACP, 0, pszA, cCharacters,
                  *ppszW, cCharacters))
    {
        dwError = GetLastError();
        CoTaskMemFree(*ppszW);
        *ppszW = NULL;
        return HRESULT_FROM_WIN32(dwError);
    }

    return NOERROR;
}

/*
 * UnicodeToAnsi converts the Unicode string pszW to an ANSI string
 * and returns the ANSI string through ppszA. Space for the
 * the converted string is allocated by UnicodeToAnsi.
 */ 

HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA)
{

    ULONG cbAnsi, cCharacters;
    DWORD dwError;

    // If input is null then just return the same.
    if (pszW == NULL)
    {
        *ppszA = NULL;
        return NOERROR;
    }

    cCharacters = wcslen(pszW)+1;
    // Determine number of bytes to be allocated for ANSI string. An
    // ANSI string can have at most 2 bytes per character (for Double
    // Byte Character Strings.)
    cbAnsi = cCharacters*2;

    // Use of the OLE allocator is not required because the resultant
    // ANSI  string will never be passed to another COM component. You
    // can use your own allocator.
    *ppszA = (LPSTR) CoTaskMemAlloc(cbAnsi);
    if (NULL == *ppszA)
        return E_OUTOFMEMORY;

    // Convert to ANSI.
    if (0 == WideCharToMultiByte(CP_ACP, 0, pszW, cCharacters, *ppszA,
                  cbAnsi, NULL, NULL))
    {
        dwError = GetLastError();
        CoTaskMemFree(*ppszA);
        *ppszA = NULL;
        return HRESULT_FROM_WIN32(dwError);
    }
    return NOERROR;

}