#pragma once
#include <string.h>
#include "IRectifyUtil_h.h"
using namespace std;


class CRectifyUtil : IRectifyUtil
{
public:
	CRectifyUtil();
	virtual HRESULT QueryInterface(
		REFIID riid,
		_COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject);
	virtual ULONG AddRef(void);
	virtual ULONG Release(void);

	virtual HRESULT GetMicaSettings(BOOL* pEnabled, BOOL* pTabbed);
	virtual HRESULT SetMicaForEveryoneEnabled(BOOL micaEnabled, BOOL tabbed);
	
	virtual HRESULT GetCurrentMenuIndex(DWORD* menuIndex);
	virtual HRESULT SetCurrentMenuByIndex(DWORD pIndex);

private:
	LONG m_ref;
	HRESULT _DeleteClassicTransparent();
};

BOOL IsDarkTheme();