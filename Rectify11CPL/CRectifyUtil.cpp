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


bool createTask(std::wstring taskName, const char* taskxml)
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
	IRegisteredTask* task;
	if (FAILED(pITF->RegisterTask(_bstr_t(taskName.c_str()), _bstr_t(taskxml), TASK_CREATE_OR_UPDATE, variant_t(), variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, variant_t(), &task))) {
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
void CRectifyUtil::SetMicaForEveryoneEnabled(BOOL micaEnabled, BOOL tabbed)
{
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

			if (!createTask(wstring(L"mfe"), "<?xml version=\"1.0\" encoding=\"UTF-16\"?><Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\"><RegistrationInfo><URI>\micafix</URI></RegistrationInfo><Triggers><LogonTrigger><Enabled>true</Enabled></LogonTrigger></Triggers><Principals><Principal id=\"Author\"><GroupId>S-1-5-32-545</GroupId><RunLevel>HighestAvailable</RunLevel></Principal></Principals><Settings><MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy><DisallowStartIfOnBatteries>false</DisallowStartIfOnBatteries><StopIfGoingOnBatteries>false</StopIfGoingOnBatteries><AllowHardTerminate>true</AllowHardTerminate><StartWhenAvailable>false</StartWhenAvailable><RunOnlyIfNetworkAvailable>false</RunOnlyIfNetworkAvailable><IdleSettings><StopOnIdleEnd>true</StopOnIdleEnd><RestartOnIdle>false</RestartOnIdle></IdleSettings><AllowStartOnDemand>true</AllowStartOnDemand><Enabled>true</Enabled><Hidden>false</Hidden><RunOnlyIfIdle>false</RunOnlyIfIdle><WakeToRun>false</WakeToRun><ExecutionTimeLimit>PT0S</ExecutionTimeLimit><Priority>5</Priority></Settings><Actions Context=\"Author\"><Exec><Command>%systemroot%\MicaForEveryone\MicaForEveryone.exe</Command></Exec></Actions></Task>"))
			{
				MessageBox(NULL, L"Failed to create MFE task.", L"Failed to create MFE task", MB_ICONERROR);
			}

			STARTUPINFOW si;
			PROCESS_INFORMATION pi;

			// set the size of the structures
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			// start the program up
			CreateProcessW(L"%systemroot%\MicaForEveryone\MicaForEveryone.exe",
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