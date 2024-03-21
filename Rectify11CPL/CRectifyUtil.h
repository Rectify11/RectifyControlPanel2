#pragma once
#include <string.h>
#include "IRectifyUtil_h.h"
using namespace std;


class CRectifyUtil : IRectifyUtil
{
public:
	CRectifyUtil();
	~CRectifyUtil();
	virtual HRESULT QueryInterface(
		REFIID riid,
		_COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject);
	virtual ULONG AddRef(void);
	virtual ULONG Release(void);

	virtual HRESULT GetMicaSettings(BOOL* pEnabled, BOOL* pTabbed);
	virtual HRESULT SetMicaForEveryoneEnabled(BOOL micaEnabled, BOOL tabbed);
	
	virtual HRESULT GetCurrentMenuIndex(DWORD* menuIndex);
	virtual HRESULT SetCurrentMenuByIndex(DWORD pIndex);

	virtual HRESULT ApplyTheme(LPCWSTR pThemeName);
	virtual HRESULT InstallThemeTool();
	virtual HRESULT UninstallThemeTool();

	static BOOL KillTask(wstring proc);
	static void RestartExplorer();

private:
	LONG m_ref;
	HRESULT _EnableClassicTransparent();
	HRESULT _DeleteClassicTransparent();
	HRESULT _EnableClassicMenu();
	HRESULT _DeleteNilesoftIfExists();
};

BOOL IsDarkTheme();