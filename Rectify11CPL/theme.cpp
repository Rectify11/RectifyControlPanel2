// NOTE: The following code is from https://github.com/namazso/SecureUxTheme/blob/923b51ed5a20412ede1b56827e078f281d3648f0/ThemeLib/theme.cpp
// which is licensed under the LGPL 2.1 license

#include "Rectify11CPL.h"

static CComPtr<IThemeManager2> g_pThemeManager2;

HRESULT themetool_init()
{
    if (g_pThemeManager2.p)
        return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);

    auto hr = CoInitialize(nullptr);
    if (FAILED(hr) && hr != CO_E_ALREADYINITIALIZED)
        return hr;

    hr = g_pThemeManager2.CoCreateInstance(CLSID_ThemeManager2);
    if (FAILED(hr))
        return hr;

    hr = g_pThemeManager2->Init(ThemeInitNoFlags);
    if (FAILED(hr))
        return hr;

    // win8
    LoadLibraryExW(L"advapi32", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    // win10
    LoadLibraryExW(L"cryptsp", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    const auto ntdll = GetModuleHandleW(L"ntdll");
    if (!ntdll)
        return HRESULT_FROM_WIN32(GetLastError());

    const auto pLdrEnumerateLoadedModules = (decltype(&LdrEnumerateLoadedModules))GetProcAddress(ntdll, "LdrEnumerateLoadedModules");
    if (!pLdrEnumerateLoadedModules)
        return HRESULT_FROM_WIN32(GetLastError());

    bool was_patched = false;

    // This one only supports local process, so antiviruses dont spazz out over it. Or they just don't know it exists
    pLdrEnumerateLoadedModules(
        0,
        [](
            _In_ PLDR_DATA_TABLE_ENTRY ModuleInformation,
            _In_ PVOID Parameter,
            _Out_ BOOLEAN* Stop
            )
        {
            *Stop = FALSE;

            constexpr BYTE bytes[] =
#if defined(_M_IX86)
            {
              0xB8, 0x01, 0x00, 0x00, 0x00,   // mov eax, 1
              0xC2, 0x18, 0x00                // ret 18
            }
#elif defined(_M_X64)
            {
                0xB8, 0x01, 0x00, 0x00, 0x00,   // mov eax, 1
                    0xC3                            // ret
            }
#elif defined(_M_ARM64)
            {
                0x20, 0x00, 0x80, 0x52,         // mov w0, #1
                    0xC0, 0x03, 0x5F, 0xD6          // ret
            }
#else
#error "Unsupported architecture"
#endif
                ;

            if (const auto pfn = GetProcAddress((HMODULE)ModuleInformation->DllBase, "CryptVerifySignatureW"))
            {
                DWORD old_protect = 0;
                const auto ret = VirtualProtect(
                    (PVOID)pfn,
                    sizeof(bytes),
                    PAGE_EXECUTE_READWRITE,
                    &old_protect
                );

                if (!ret)
                    return;

                memcpy((PVOID)pfn, bytes, sizeof(bytes));

                *(bool*)Parameter = true;

                // we don't care if this fails, the page will just stay RWX at most
                VirtualProtect(
                    (PVOID)pfn,
                    sizeof(bytes),
                    old_protect,
                    &old_protect
                );
            }
        },
        &was_patched
    );

    if (!was_patched)
        return E_UNEXPECTED;

    return S_OK;
}

IThemeManager2* themetool_get_manager()
{
    return g_pThemeManager2.p;
}

HRESULT themetool_get_theme_count(PULONG count)
{
    *count = 0;
    int icount{};
    const auto hr = g_pThemeManager2->GetThemeCount(&icount);
    *count = icount;
    return hr;
}

HRESULT themetool_get_theme(ULONG idx, ITheme** theme)
{
    return g_pThemeManager2->GetTheme((int)idx, theme);
}

HRESULT themetool_set_active(
	HWND parent,
	ULONG theme_idx,
	BOOLEAN apply_now_not_only_registry,
	ULONG apply_flags,
	ULONG pack_flags
)
{
	const auto idx = (int)theme_idx;
	return g_pThemeManager2->SetCurrentTheme(parent, idx, !!apply_now_not_only_registry, apply_flags, pack_flags);
}


HRESULT themetool_theme_get_display_name(ITheme* theme, LPWSTR out, SIZE_T cch)
{
    memset(out, 0, cch * sizeof(WCHAR));
    std::wstring str;
    const auto hr = theme->GetDisplayName(str);
    if (FAILED(hr))
        return hr;
    if (str.size() >= cch)
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    std::copy_n(str.begin(), str.size(), out);
    return hr;
}

HRESULT themetool_theme_get_vs_path(ITheme* theme, LPWSTR out, SIZE_T cch)
{
    memset(out, 0, cch * sizeof(WCHAR));
    std::wstring str;
    const auto hr = theme->GetVisualStyle(str);
    if (FAILED(hr))
        return hr;
    if (str.size() >= cch)
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    std::copy_n(str.begin(), str.size(), out);
    return hr;
}

void themetool_theme_release(ITheme* theme)
{
    theme->Release();
}