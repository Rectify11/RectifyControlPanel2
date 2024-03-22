#include "Rectify11CPL.h"

// This class invokes functions from themedll.dll, whose source code can be found at https://github.com/namazso/SecureUxTheme

static HMODULE themeDllModule = NULL;

typedef HRESULT(FAR WINAPI* ThemeDll_Init)();
typedef IThemeManager2* (FAR WINAPI* ThemeDll_themetool_get_manager)();
typedef HRESULT(FAR WINAPI* ThemeDll_themetool_get_theme_count)(PULONG count);
typedef HRESULT(FAR WINAPI* ThemeDll_themetool_get_theme)(ULONG idx, ITheme** theme);
typedef HRESULT(FAR WINAPI* ThemeDll_themetool_set_active)(HWND parent,
	ULONG theme_idx,
	BOOLEAN apply_now_not_only_registry,
	ULONG apply_flags,
	ULONG pack_flags);
typedef HRESULT(FAR WINAPI* ThemeDll_themetool_theme_get_display_name)(ITheme* theme, LPWSTR out, SIZE_T cch);
typedef HRESULT(FAR WINAPI* ThemeDll_themetool_theme_get_vs_path)(ITheme* theme, LPWSTR out, SIZE_T cch);
typedef void(FAR WINAPI* ThemeDll_themetool_theme_release)(ITheme* theme);
typedef HRESULT(FAR WINAPI* ThemeDll_themetool_install)(ULONG flags);
typedef HRESULT(FAR WINAPI* ThemeDll_themetool_uninstall)();

HRESULT loadThemeDll()
{
	if (themeDllModule)
	{
		return S_OK;
	}

	WCHAR filename[MAX_PATH];
	GetModuleFileName(g_hInst, filename, sizeof(filename) / sizeof(WCHAR));

	PathRemoveFileSpecW(filename);

	std::wstring path(filename);
	path += L"\\ThemeDll.dll";

	themeDllModule = LoadLibrary(path.c_str());

	if (themeDllModule)
	{
		return S_OK;
	}
	else {
		return HRESULT_FROM_WIN32(GetLastError());
	}
}

HRESULT themetool_init()
{
	HRESULT hr = loadThemeDll();
	if (FAILED(hr))
	{
		return hr;
	}

	ThemeDll_Init proc = (ThemeDll_Init)GetProcAddress(themeDllModule, "themetool_init");
	if (proc == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	hr = proc();

	return hr;
}

IThemeManager2* themetool_get_manager()
{
	HRESULT hr = loadThemeDll();
	if (FAILED(hr))
	{
		return NULL;
	}

	ThemeDll_themetool_get_manager proc = (ThemeDll_themetool_get_manager)GetProcAddress(themeDllModule, "themetool_get_manager");
	if (proc == NULL)
	{
		return NULL;
	}

	return proc();
}

HRESULT themetool_get_theme_count(PULONG count)
{
	HRESULT hr = loadThemeDll();
	if (FAILED(hr))
	{
		return hr;
	}

	ThemeDll_themetool_get_theme_count proc = (ThemeDll_themetool_get_theme_count)GetProcAddress(themeDllModule, "themetool_get_theme_count");
	if (proc == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	hr = proc(count);

	return hr;
}

HRESULT themetool_get_theme(ULONG idx, ITheme** theme)
{
	HRESULT hr = loadThemeDll();
	if (FAILED(hr))
	{
		return hr;
	}

	ThemeDll_themetool_get_theme proc = (ThemeDll_themetool_get_theme)GetProcAddress(themeDllModule, "themetool_get_theme");
	if (proc == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	hr = proc(idx, theme);

	return hr;
}

HRESULT themetool_set_active(
	HWND parent,
	ULONG theme_idx,
	BOOLEAN apply_now_not_only_registry,
	ULONG apply_flags,
	ULONG pack_flags
)
{
	HRESULT hr = loadThemeDll();
	if (FAILED(hr))
	{
		return hr;
	}

	ThemeDll_themetool_set_active proc = (ThemeDll_themetool_set_active)GetProcAddress(themeDllModule, "themetool_set_active");
	if (proc == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	hr = proc(parent, theme_idx, apply_now_not_only_registry, apply_flags, pack_flags);

	return hr;
}


HRESULT themetool_theme_get_display_name(ITheme* theme, LPWSTR out, SIZE_T cch)
{
	HRESULT hr = loadThemeDll();
	if (FAILED(hr))
	{
		return hr;
	}

	ThemeDll_themetool_theme_get_display_name proc = (ThemeDll_themetool_theme_get_display_name)GetProcAddress(themeDllModule, "themetool_theme_get_display_name");
	if (proc == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	hr = proc(theme, out, cch);

	return hr;
}

HRESULT themetool_theme_get_vs_path(ITheme* theme, LPWSTR out, SIZE_T cch)
{
	HRESULT hr = loadThemeDll();
	if (FAILED(hr))
	{
		return hr;
	}

	ThemeDll_themetool_theme_get_display_name proc = (ThemeDll_themetool_theme_get_display_name)GetProcAddress(themeDllModule, "themetool_theme_get_vs_path");
	if (proc == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	hr = proc(theme, out, cch);

	return hr;
}

void themetool_theme_release(ITheme* theme)
{
	HRESULT hr = loadThemeDll();
	if (FAILED(hr))
	{
		return;
	}

	ThemeDll_themetool_theme_release proc = (ThemeDll_themetool_theme_release)GetProcAddress(themeDllModule, "themetool_theme_release");
	if (proc == NULL)
	{
		return;
	}

	proc(theme);
}

HRESULT secureuxtheme_install(ULONG flags)
{
	HRESULT hr = loadThemeDll();
	if (FAILED(hr))
	{
		return hr;
	}

	ThemeDll_themetool_install proc = (ThemeDll_themetool_install)GetProcAddress(themeDllModule, "secureuxtheme_install");
	if (proc == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return proc(flags);
}

HRESULT secureuxtheme_uninstall()
{
	HRESULT hr = loadThemeDll();
	if (FAILED(hr))
	{
		return hr;
	}

	ThemeDll_themetool_uninstall proc = (ThemeDll_themetool_uninstall)GetProcAddress(themeDllModule, "secureuxtheme_uninstall");
	if (proc == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return proc();
}
