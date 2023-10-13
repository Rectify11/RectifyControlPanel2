#include "Rectify11CPL.h"
#include "CRectifyUtil.h"
#include "psapi.h"
#include <tlhelp32.h>
#include <taskschd.h>
#include <comutil.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")

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

bool deleteTask(std::wstring taskName)
{
	if (FAILED(CoInitialize(nullptr))) {
		return false;
	}

	ITaskService* pITS;
	if (FAILED(CoCreateInstance(CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pITS))) {
		CoUninitialize();
		return false;
	}

	if (FAILED(pITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t()))) {
		pITS->Release();
		CoUninitialize();
		return false;
	}

	ITaskFolder* pITF;
	if (FAILED(pITS->GetFolder(_bstr_t(L"\\"), &pITF))) {
		pITS->Release();
		CoUninitialize();
		return false;
	}

	pITS->Release();

	if (FAILED(pITF->DeleteTask(_bstr_t(taskName.c_str()), 0))) {
		pITF->Release();
		CoUninitialize();
		return false;
	}

	pITF->Release();

	CoUninitialize();

	return true;
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

/// <summary>
/// Check if mica for everyone is enabled
/// </summary>
/// <returns>Returns if Mica for everyone is enabled</returns>
BOOL CRectifyUtil::CheckIfMicaForEveryoneIsEnabled()
{
	return FindProcessId(L"micaforeveryone") != 0;
}
/// <summary>
/// Enable/disable micaforeveryone tool
/// </summary>
/// <param name="enabled"></param>
void CRectifyUtil::SetMicaForEveryoneEnabled(BOOL enabled)
{
	if (enabled)
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
		}
		else
		{
			MessageBox(NULL, L"Micaforeveryone is not correctly installed. Please rerun the rectify11 installed.", L"Failed to create MFE task", MB_ICONERROR);
		}
	}
	else
	{
		if (!deleteTask(L"mfe"))
		{
			MessageBox(NULL, L"Failed to delete MFE task", L"Failed to delete MFE task", MB_ICONERROR);
		}
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

/// <summary>
/// Set if mica tab is used instead of regular mica.
/// </summary>
/// <param name="enaled"></param>
void CRectifyUtil::SetTabbedEnabled(BOOL enabled)
{
	// TODO
}

BOOL CRectifyUtil::IsDarkTheme()
{
	WCHAR value[255] = {0};
	PVOID pvData = value;
	DWORD size = sizeof(value);
	RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager", L"DllName", RRF_RT_REG_SZ, 0, pvData, &size);
	std::wstring msstylePath = std::wstring((LPCWSTR)pvData);

	size_t result = msstylePath.find(L"Dark");
	return result > 0 ? TRUE : FALSE;
}