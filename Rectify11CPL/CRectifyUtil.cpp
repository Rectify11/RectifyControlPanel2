#include "Rectify11CPL.h"
#include "CRectifyUtil.h"
#include "psapi.h"
#include <tlhelp32.h>
#include <taskschd.h>
#include <comutil.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "comsuppw.lib")

DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

LONG createStartup(LPCWSTR app_name, LPCWSTR app_path)
{
	HKEY hKey;

	WCHAR proc_buffer[2066];
	ExpandEnvironmentStringsW(app_path, proc_buffer, 2066);

	LONG lnRes = RegOpenKeyEx(HKEY_CURRENT_USER,
		TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
		0, KEY_WRITE,
		&hKey);
	if (ERROR_SUCCESS == lnRes)
	{
		lnRes = RegSetValueExW(hKey,
			app_name,
			0,
			REG_SZ,
			(LPBYTE)proc_buffer,
			(lstrlenW(proc_buffer) + 1) * sizeof(WCHAR));
	}

	RegCloseKey(hKey);
	return lnRes;
}

LONG deleteStartup(LPCWSTR app_name)
{
	HKEY hKey;

	LONG lnRes = RegOpenKeyEx(HKEY_CURRENT_USER,
		TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
		0, KEY_WRITE,
		&hKey);
	if (ERROR_SUCCESS == lnRes)
	{
		lnRes = RegDeleteValue(hKey, app_name);
	}

	RegCloseKey(hKey);
	return lnRes;
}

int DeleteDirectory(const std::string& refcstrRootDirectory,
	bool              bDeleteSubdirectories = true)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
	// subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	std::string     strFilePath;                 // Filepath
	std::string     strPattern;                  // Pattern
	WIN32_FIND_DATAA FileInformation;             // File information


	strPattern = refcstrRootDirectory + "\\*.*";
	hFile = ::FindFirstFileA(strPattern.c_str(), &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FileInformation.cFileName[0] != '.')
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if (iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if (::SetFileAttributesA(strFilePath.c_str(),
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();

					// Delete file
					if (::DeleteFileA(strFilePath.c_str()) == FALSE)
						return ::GetLastError();
				}
			}
		} while (::FindNextFileA(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			if (!bSubdirectory)
			{
				// Set directory attributes
				if (::SetFileAttributesA(refcstrRootDirectory.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

				// Delete directory
				if (::RemoveDirectoryA(refcstrRootDirectory.c_str()) == FALSE)
					return ::GetLastError();
			}
		}
	}

	return 0;
}

void KillTask(wstring proc)
{
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	wstring args = wstring(L"/f /im ");
	args += proc;
	WCHAR args_buffer[2066];
	wcsncpy_s(args_buffer, 1024, args.c_str(), args.size());

	WCHAR proc_buffer[2066];
	ExpandEnvironmentStringsW(L"%systemroot%\\system32\\taskkill.exe", proc_buffer, 2066);

	BOOL x = CreateProcessW(proc_buffer,
		args_buffer,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	HRESULT hr = GetLastError();
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

HRESULT startProc(wstring proc)
{
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	WCHAR proc_buffer[4096];
	ExpandEnvironmentStringsW(proc.c_str(), proc_buffer, 4095);

	HRESULT hr = CreateProcessW(proc_buffer,
		NULL,           // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return hr;
}

bool check_if_file_exists(std::wstring path)
{
	std::ifstream ff(path.c_str());
	return ff.is_open();
}

/// <summary>
/// Check if mica for everyone is enabled
/// </summary>
/// <returns>Returns if Mica for everyone is enabled</returns>
BOOL CRectifyUtil::CheckIfMicaForEveryoneIsEnabled()
{
	HKEY hKey = NULL;
	DWORD cbData = 2096;
	WCHAR buffer[2096];
	LONG lnRes = RegOpenKeyEx(HKEY_CURRENT_USER,
		TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
		0, KEY_READ,
		&hKey);

	if (ERROR_SUCCESS == lnRes)
	{
		lnRes = RegQueryValueExW(hKey, L"mfe", NULL, NULL, (LPBYTE)&buffer, &cbData);
		if (lnRes == ERROR_SUCCESS || lnRes == ERROR_MORE_DATA)
		{
			return TRUE;
		}
	}

	return FALSE;
}


/// <summary>
/// Enable/disable micaforeveryone tool
/// </summary>
/// <param name="enabled"></param>
void CRectifyUtil::SetMicaForEveryoneEnabled(wstring currentThemeName, BOOL micaEnabled, BOOL tabbed)
{
	WCHAR buffer[1024];
	swprintf(buffer, 1024, L"Setting mica enabled. theme: %ws, mica: %d, tabbed: %d", currentThemeName.c_str(), micaEnabled, tabbed);
	MessageBox(NULL, buffer, L"work", MB_ICONERROR);
	if (micaEnabled)
	{
		struct stat sb;
		if (stat("c:/windows/MicaForEveryone", &sb) == 0)
		{
			string appdata = string(getenv("localappdata"));
			string micaFolder = appdata + "/Mica For Everyone/";
			if (stat(micaFolder.c_str(), &sb) != 0)
			{
				if (DeleteDirectory(micaFolder.c_str()))
				{
					MessageBox(NULL, L"Failed to delete local micaforeveryone folder.", L"Failed to create MFE task", MB_ICONERROR);
				}
			}
			HRESULT hr = createStartup(TEXT("mfe"), TEXT("%systemroot%\\MicaForEveryone\\MicaForEveryone.exe"));
			if (FAILED(hr))
			{
				swprintf(buffer, 1024, L"Failed create MFE task: %x", hr);
				MessageBox(NULL, buffer, L"Failed to create MFE task", MB_ICONERROR);
			}

			wstring config_file_src = wstring(L"c:\\windows\\micaforeveryone\\CONF\\");
			if (tabbed)
				config_file_src += L"T";
			config_file_src += currentThemeName;
			config_file_src += L".conf";

			if (!check_if_file_exists(config_file_src))
			{
				swprintf(buffer, 1024, L"Warning: Micaforeveryone configuration file is missing! File name is %ws", config_file_src.c_str());
				MessageBox(NULL, buffer, L"Warning", MB_ICONWARNING);
			}
			else
			{
				if (!CopyFileExW(config_file_src.c_str(), L"c:\\windows\\micaforeveryone\\MicaForEveryone.conf", NULL, NULL, NULL, 0))
				{
					swprintf(buffer, 1024, L"Warning: Failed to copy micaforeveryone configuration file with result %x", GetLastError());
					MessageBox(NULL, buffer, L"Warning", MB_ICONWARNING);
				}
			}

			// Enable micafix if black theme
			if (currentThemeName.compare(L"black"))
			{
				createStartup(L"mfefix", L"%systemroot%\\MicaForEveryone\\EFamd64\\ExplorerFrame.exe");
				startProc(L"%systemroot%\\MicaForEveryone\\EFamd64\\ExplorerFrame.exe");
			}
			else
			{
				deleteStartup(L"mfefix");
				KillTask(L"explorerframe.exe");
			}

			// Start mica for everyone
			STARTUPINFOW si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));
			CreateProcessW(L"%systemroot%\\MicaForEveryone\\MicaForEveryone.exe",
				NULL,        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
			);

			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		else
		{
			MessageBox(NULL, L"Micaforeveryone is not correctly installed. Please rerun the rectify11 installed.", L"Failed to create MFE task", MB_ICONERROR);
		}
	}
	else
	{
		if (FAILED(deleteStartup(L"mfe")))
		{
			MessageBox(NULL, L"Failed to delete MFE task", L"Failed to delete MFE task", MB_ICONERROR);
		}
		deleteStartup(L"mfefix");
		KillTask(L"MicaForEveryone.exe");
		KillTask(L"explorerframe.exe");
	}
}

/// <summary>
/// Check if mica tab is used instead of mica
/// </summary>
/// <returns></returns>
BOOL CRectifyUtil::GetTabbedEnabled()
{
	// TODO
	return FALSE;
}


BOOL CRectifyUtil::IsDarkTheme()
{
	WCHAR value[255] = { 0 };
	PVOID pvData = value;
	DWORD size = sizeof(value);
	RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager", L"DllName", RRF_RT_REG_SZ, 0, pvData, &size);
	std::wstring msstylePath = std::wstring((LPCWSTR)pvData);

	size_t result = msstylePath.find(L"Dark");
	return result > 0 ? TRUE : FALSE;
}