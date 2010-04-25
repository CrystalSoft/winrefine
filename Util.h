#define AW_HOR_POSITIVE 0x00000001
#define AW_HOR_NEGATIVE 0x00000002
#define AW_VER_POSITIVE 0x00000004
#define AW_VER_NEGATIVE 0x00000008
#define AW_CENTER 0x00000010
#define AW_HIDE 0x00010000
#define AW_ACTIVATE 0x00020000
#define AW_SLIDE 0x00040000
#define AW_BLEND 0x00080000

bool StringToInt(wchar_t* digit, int& result);
HANDLE GetProcess(HWND hWnd, DWORD dwAccesses);
DWORD GetProcessName(HWND hWnd, wchar_t* buf, int bufsize);
DWORD GetWindowModuleFileName(HWND hWnd, wchar_t* buf, int bufsize);
int KillProcess(HWND hWnd);
void GetWindowInformations(CWnd* Win, CString &Title, CString &Class, CString &Process, CString &Exe, int &ID);
HWND GetMainWindow(HWND hWnd);
//BOOL AnimateWindow(HWND hWnd, DWORD dwTime, DWORD dwFlags);
bool IsListCtrl(HWND hWnd);
CString GetListItemText(HWND hWnd, int Item, int SubItem);
int GetListSelectedItem(HWND hWnd);
int HitListTest(HWND hWnd, LVHITTESTINFO* pt);
wchar_t* GetToolTipText(HWND hWnd);
HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA);
HRESULT __fastcall AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW);

/*
val InfoTip = s 'prop:Type;Author;Title;Comment;
						{AC146E80-3679-4BCA-9BE4-E36512573E6C},0;
						{AC146E80-3679-4BCA-9BE4-E36512573E6C},1;
						{AC146E80-3679-4BCA-9BE4-E36512573E6C},2;Size'

		
*/