#pragma once

// NOTE: The following code is from https://github.com/namazso/SecureUxTheme/blob/923b51ed5a20412ede1b56827e078f281d3648f0/ThemeLib/theme.cpp
// which is licensed under the LGPL 2.1 license

static constexpr GUID CLSID_ThemeManager2 = { 0x9324da94, 0x50ec, 0x4a14, { 0xa7, 0x70, 0xe9, 0x0c, 0xa0, 0x3e, 0x7c, 0x8f } };
typedef VOID(NTAPI LDR_ENUM_CALLBACK)(
    _In_ PLDR_DATA_TABLE_ENTRY ModuleInformation,
    _In_ PVOID Parameter,
    _Out_ BOOLEAN* Stop
    );
typedef LDR_ENUM_CALLBACK* PLDR_ENUM_CALLBACK;

NTSTATUS
NTAPI
LdrEnumerateLoadedModules(
    _In_ BOOLEAN ReservedFlag,
    _In_ PLDR_ENUM_CALLBACK EnumProc,
    _In_opt_ PVOID Context
);


enum THEME_MANAGER_INITIALIZATION_FLAGS : unsigned
{
    ThemeInitNoFlags = 0,
    ThemeInitCurrentThemeOnly = 1 << 0,
    ThemeInitFlagUnk1 = 1 << 1,
    ThemeInitFlagUnk2 = 1 << 2,
};


enum tagTHEMECAT {};

struct ISlideshowSettings;

// const CThemeFile::`vftable'{for `ITheme'}
struct ITheme : IUnknown
{
private:
    virtual HRESULT WINAPI get_DisplayName(LPWSTR*) = 0;
    virtual HRESULT WINAPI put_DisplayName(LPWSTR) = 0;
    virtual HRESULT WINAPI get_VisualStyle(LPWSTR*) = 0;  // win8: get_ScreenSaver(LPWSTR*)
    virtual HRESULT WINAPI put_VisualStyle(LPWSTR) = 0;   // win8: set_ScreenSaver(LPWSTR)
    virtual HRESULT WINAPI get_VisualStyle2(LPWSTR*) = 0; // 1903: get_VisualStyleColor(LPWSTR*)
    virtual HRESULT WINAPI put_VisualStyle2(LPWSTR) = 0;  // 1903: put_VisualStyleColor(LPWSTR)

    // see "re" folder for full vtables

public:
    HRESULT GetDisplayName(std::wstring& name)
    {
        name.clear();
        LPWSTR lpwstr = nullptr;
        auto hr = get_DisplayName(&lpwstr);
        if (SUCCEEDED(hr) && lpwstr)
        {
            if (lpwstr)
            {
                name = lpwstr;
                SysFreeString(lpwstr);
            }
            else
            {
                hr = E_FAIL;
            }
        }
        return hr;
    }

    // we guess which one is the correct function, since it's vtable index shifted across windows versions
    HRESULT GetVisualStyle(std::wstring& path)
    {
        path.clear();
        LPWSTR lpwstr = nullptr;
        auto hr = get_VisualStyle2(&lpwstr);
        if (SUCCEEDED(hr) && lpwstr)
        {
            const auto lower = SysAllocString(lpwstr);
            for (auto it = lower; *it; ++it)
                *it = towlower(*it);
            const auto is_style = wcsstr(lower, L"msstyles") != nullptr;
            SysFreeString(lower);
            if (is_style)
            {
                path = lpwstr;
                SysFreeString(lpwstr);
                return hr;
            }
            SysFreeString(lpwstr);
        }
        lpwstr = nullptr;
        hr = get_VisualStyle(&lpwstr);
        if (SUCCEEDED(hr) && lpwstr)
        {
            const auto lower = SysAllocString(lpwstr);
            for (auto it = lower; *it; ++it)
                *it = towlower(*it);
            const auto is_style = wcsstr(lower, L"msstyles") != nullptr;
            SysFreeString(lower);
            if (is_style)
            {
                path = lpwstr;
                SysFreeString(lpwstr);
                return hr;
            }
            SysFreeString(lpwstr);
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        return hr;
    }
};

// const CThemeManager2::`vftable'
MIDL_INTERFACE("{c1e8c83e-845d-4d95-81db-e283fdffc000}") IThemeManager2 : IUnknown
{
  virtual HRESULT WINAPI Init(THEME_MANAGER_INITIALIZATION_FLAGS) = 0;
  virtual HRESULT WINAPI InitAsync(HWND, int) = 0;
  virtual HRESULT WINAPI Refresh() = 0;
  virtual HRESULT WINAPI RefreshAsync(HWND, int) = 0;
  virtual HRESULT WINAPI RefreshComplete() = 0;
  virtual HRESULT WINAPI GetThemeCount(int*) = 0;
  virtual HRESULT WINAPI GetTheme(int, ITheme**) = 0;
  virtual HRESULT WINAPI IsThemeDisabled(int, int*) = 0;
  virtual HRESULT WINAPI GetCurrentTheme(int*) = 0;
  virtual HRESULT WINAPI SetCurrentTheme(
    HWND parent,
    int theme_idx,
    int apply_now_not_only_registry, // 1 when called in Windows
    ULONG apply_flags, // 0 when called in Windows
    ULONG pack_flags // 0 when called in Windows
  ) = 0;
  virtual HRESULT WINAPI GetCustomTheme(int*) = 0;
  virtual HRESULT WINAPI GetDefaultTheme(int*) = 0;
  virtual HRESULT WINAPI CreateThemePack(HWND, LPCWSTR, ULONG pack_flags) = 0;
  virtual HRESULT WINAPI CloneAndSetCurrentTheme(HWND, LPCWSTR, LPWSTR*) = 0;
  virtual HRESULT WINAPI InstallThemePack(HWND, LPCWSTR, int, ULONG pack_flags, LPWSTR*, ITheme**) = 0;
  virtual HRESULT WINAPI DeleteTheme(LPCWSTR) = 0;
  virtual HRESULT WINAPI OpenTheme(HWND, LPCWSTR, ULONG pack_flags) = 0;
  virtual HRESULT WINAPI AddAndSelectTheme(HWND, LPCWSTR, ULONG apply_flags, ULONG pack_flags) = 0;
  virtual HRESULT WINAPI SQMCurrentTheme() = 0;
  virtual HRESULT WINAPI ExportRoamingThemeToStream(IStream*, int) = 0;
  virtual HRESULT WINAPI ImportRoamingThemeFromStream(IStream*, int) = 0;
  virtual HRESULT WINAPI UpdateColorSettingsForLogonUI() = 0;
  virtual HRESULT WINAPI GetDefaultThemeId(GUID*) = 0;
  virtual HRESULT WINAPI UpdateCustomTheme() = 0;
};

HRESULT themetool_init();
IThemeManager2* themetool_get_manager();
HRESULT themetool_get_theme_count(PULONG count);
HRESULT themetool_get_theme(ULONG idx, ITheme** theme);

HRESULT themetool_set_active(
    HWND parent,
    ULONG theme_idx,
    BOOLEAN apply_now_not_only_registry,
    ULONG apply_flags,
    ULONG pack_flags
);

HRESULT themetool_theme_get_display_name(ITheme* theme, LPWSTR out, SIZE_T cch);

HRESULT themetool_theme_get_vs_path(ITheme* theme, LPWSTR out, SIZE_T cch);

void themetool_theme_release(ITheme* theme);



/**
 * @brief Ignore background.
 */
#define THEMETOOL_APPLY_FLAG_IGNORE_BACKGROUND    (ULONG)(1 << 0)
 /**
  * @brief Ignore cursor.
  */
#define THEMETOOL_APPLY_FLAG_IGNORE_CURSOR        (ULONG)(1 << 1)
  /**
   * @brief Ignore desktop icons.
   */
#define THEMETOOL_APPLY_FLAG_IGNORE_DESKTOP_ICONS (ULONG)(1 << 2)
   /**
    * @brief Ignore color accent.
    */
#define THEMETOOL_APPLY_FLAG_IGNORE_COLOR         (ULONG)(1 << 3)
    /**
     * @brief Ignore sounds.
     */
#define THEMETOOL_APPLY_FLAG_IGNORE_SOUND         (ULONG)(1 << 4)
     /**
      * @brief Ignore screensaver.
      */
#define THEMETOOL_APPLY_FLAG_IGNORE_SCREENSAVER   (ULONG)(1 << 5)
      /**
       * @brief Unknown, maybe ignore window metrics.
       */
#define THEMETOOL_APPLY_FLAG_UNKNOWN              (ULONG)(1 << 6)
       /**
        * @brief Unknown.
        */
#define THEMETOOL_APPLY_FLAG_UNKNOWN2             (ULONG)(1 << 7)
        /**
         * @brief Suppress hourglass.
         */
#define THEMETOOL_APPLY_FLAG_NO_HOURGLASS         (ULONG)(1 << 8)

         /**
          * @brief Unknown, seems to suppress hourglass.
          */
#define THEMETOOL_PACK_FLAG_UNKNOWN1              (ULONG)(1 << 0)
          /**
           * @brief Unknown, seems to suppress hourglass.
           */
#define THEMETOOL_PACK_FLAG_UNKNOWN2              (ULONG)(1 << 1)
           /**
            * @brief Hides all dialogs and prevents sound.
            */
#define THEMETOOL_PACK_FLAG_SILENT                (ULONG)(1 << 2)
            /**
             * @brief Roamed.
             */
#define THEMETOOL_PACK_FLAG_ROAMED                (ULONG)(1 << 3)