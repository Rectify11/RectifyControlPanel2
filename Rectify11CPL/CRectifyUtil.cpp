#include "Rectify11CPL.h"
#include "CRectifyUtil.h"
#include "psapi.h"
#include <tlhelp32.h>
#include <taskschd.h>
#include <comutil.h>
#include "Guid.h"
#include <Shlwapi.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "comsuppw.lib")

CRectifyUtil::CRectifyUtil() : m_ref(1)
{

}

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


HRESULT deleteTask(std::wstring taskName)
{
	HRESULT hr = S_OK;

	ITaskService* pITS;
	hr = CoCreateInstance(CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pITS);
	if (FAILED(hr)) {
		return hr;
	}

	hr = pITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr)) {
		pITS->Release();
		return hr;
	}

	ITaskFolder* pITF;
	hr = pITS->GetFolder(_bstr_t(L"\\"), &pITF);
	if (FAILED(hr)) {
		pITS->Release();
		return hr;
	}

	pITS->Release();

	hr = pITF->DeleteTask(_bstr_t(taskName.c_str()), 0);
	if (FAILED(hr)) {
		pITF->Release();
		return hr;
	}

	pITF->Release();

	return hr;
}

HRESULT taskExists(wstring taskName, BOOL* taskExists)
{
	HRESULT hr = S_OK;
	*taskExists = FALSE;
	ITaskService* pITS;
	hr = CoCreateInstance(CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pITS);
	if (FAILED(hr)) {
		return hr;
	}

	hr = pITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr)) {
		pITS->Release();
		return hr;
	}

	ITaskFolder* pITF;
	hr = pITS->GetFolder(_bstr_t(L"\\"), &pITF);
	if (FAILED(hr)) {
		pITS->Release();
		return hr;
	}

	pITS->Release();

	IRegisteredTask* task = NULL;
	hr = pITF->GetTask(_bstr_t(taskName.c_str()), &task);
	if (FAILED(hr)) {
		pITF->Release();
		return hr;
	}
	else
	{
		*taskExists = TRUE;
	}

	pITF->Release();

	return hr;
}


HRESULT createTask(std::wstring taskName, std::wstring taskExe)
{
	HRESULT hr = S_OK;

	ITaskService* pITS;
	hr = CoCreateInstance(CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pITS);
	if (FAILED(hr)) {
		return hr;
	}

	hr = pITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr)) {
		pITS->Release();
		return hr;
	}

	ITaskFolder* pITF;
	hr = pITS->GetFolder(_bstr_t(L"\\"), &pITF);
	if (FAILED(hr)) {
		pITS->Release();
		return hr;
	}

	wstring taskxml = wstring(L"<?xml version=\"1.0\" encoding=\"UTF-16\"?><Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\"><RegistrationInfo><URI>\micafix</URI></RegistrationInfo><Triggers><LogonTrigger><Enabled>true</Enabled></LogonTrigger></Triggers><Principals><Principal id=\"Author\"><GroupId>S-1-5-32-545</GroupId><RunLevel>HighestAvailable</RunLevel></Principal></Principals><Settings><MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy><DisallowStartIfOnBatteries>false</DisallowStartIfOnBatteries><StopIfGoingOnBatteries>false</StopIfGoingOnBatteries><AllowHardTerminate>true</AllowHardTerminate><StartWhenAvailable>false</StartWhenAvailable><RunOnlyIfNetworkAvailable>false</RunOnlyIfNetworkAvailable><IdleSettings><StopOnIdleEnd>true</StopOnIdleEnd><RestartOnIdle>false</RestartOnIdle></IdleSettings><AllowStartOnDemand>true</AllowStartOnDemand><Enabled>true</Enabled><Hidden>false</Hidden><RunOnlyIfIdle>false</RunOnlyIfIdle><WakeToRun>false</WakeToRun><ExecutionTimeLimit>PT0S</ExecutionTimeLimit><Priority>5</Priority></Settings><Actions Context=\"Author\"><Exec><Command>");
	taskxml += taskExe;
	taskxml += L"</Command></Exec></Actions></Task>";

	pITS->Release();
	IRegisteredTask* task;
	hr = pITF->RegisterTask(_bstr_t(taskName.c_str()), _bstr_t(taskxml.c_str()), TASK_CREATE_OR_UPDATE, variant_t(), variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, variant_t(), &task);
	if (FAILED(hr)) {
		pITF->Release();
		return hr;
	}

	pITF->Release();

	return hr;
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
size_t GetSizeOfFile(const std::wstring& path)
{
	struct _stat fileinfo;
	_wstat(path.c_str(), &fileinfo);
	return fileinfo.st_size;
}

std::wstring LoadUtf8FileToString(const std::wstring& filename)
{
	std::wstring buffer;            // stores file contents
	FILE* f = _wfopen(filename.c_str(), L"rtS, ccs=UTF-8");

	// Failed to open file
	if (f == NULL)
	{
		// ...handle some error...
		return buffer;
	}

	size_t filesize = GetSizeOfFile(filename);

	// Read entire file contents in to memory
	if (filesize > 0)
	{
		buffer.resize(filesize);
		size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, f);
		buffer.resize(wchars_read);
		buffer.shrink_to_fit();
	}

	fclose(f);

	return buffer;
}

/// <summary>
/// Check if mica for everyone is enabled
/// </summary>
/// <returns>Returns if Mica for everyone is enabled</returns>
HRESULT CRectifyUtil::GetMicaSettings(BOOL* pEnabled, BOOL* pTabbed)
{
	*pEnabled = FALSE;
	*pTabbed = FALSE;

	taskExists(L"mfe", pEnabled);

	// read mfe config file
	struct stat sb;
	wstring config_file = wstring(L"c:\\windows\\micaforeveryone\\MicaForEveryone.conf");
	if (stat("c:\\windows\\micaforeveryone\\MicaForEveryone.conf", &sb) == 0)
	{
		wstring config = LoadUtf8FileToString(config_file);
		if (config.compare(L"Tabbed"))
		{
			*pTabbed = TRUE;
		}
	}


	return S_OK;
}

HRESULT CRectifyUtil::_DeleteClassicTransparent()
{
	CHAR path[MAX_PATH];
	HRESULT hr = SHGetFolderPathA(NULL, CSIDL_COMMON_STARTMENU, NULL, 0, path);
	if (SUCCEEDED(hr))
	{
		string file = string(path);
		file += +"\\programs\\startup\\acrylmenu.lnk";
		return !DeleteFileA(file.c_str());
	}
	else
	{
		return hr;
	}
}

/// <summary>
/// Enable/disable micaforeveryone tool
/// </summary>
/// <param name="enabled"></param>
HRESULT CRectifyUtil::SetMicaForEveryoneEnabled(BOOL micaEnabled, BOOL tabbed)
{
	HRESULT hr = S_OK;
	WCHAR value[255] = { 0 };
	PVOID pvData = value;
	DWORD size = sizeof(value);
	RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager", L"DllName", RRF_RT_REG_SZ, 0, pvData, &size);
	std::wstring msstylePath = std::wstring((LPCWSTR)pvData);

	std::wstring currentThemeName = msstylePath;

	const size_t last_slash_idx = currentThemeName.find_last_of(L"\\/");
	if (std::string::npos != last_slash_idx)
	{
		currentThemeName.erase(0, last_slash_idx + 1);
	}

	// Remove extension if present.
	const size_t period_idx = currentThemeName.rfind('.');
	if (std::string::npos != period_idx)
	{
		currentThemeName.erase(period_idx);
	}

	WCHAR buffer[1024];
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
			HRESULT hr = createTask(TEXT("mfe"), TEXT("%systemroot%\\MicaForEveryone\\MicaForEveryone.exe"));
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
				createTask(L"mfefix", L"%systemroot%\\MicaForEveryone\\EFamd64\\ExplorerFrame.exe");
				startProc(L"%systemroot%\\MicaForEveryone\\EFamd64\\ExplorerFrame.exe");
			}
			else
			{
				deleteTask(L"mfefix");
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
		BOOL mfeExists;
		taskExists(L"mfe", &mfeExists);
		if (FAILED(deleteTask(L"mfe")) && mfeExists)
		{
			MessageBox(NULL, L"Failed to delete MFE task", L"Failed to delete MFE task", MB_ICONERROR);
		}
		deleteTask(L"mfefix");
		KillTask(L"MicaForEveryone.exe");
		KillTask(L"explorerframe.exe");
	}
	return hr;
}

HRESULT CRectifyUtil::GetCurrentMenuIndex(DWORD* menuIndex)
{
	*menuIndex = Normal;
	struct stat sb;
	if (stat("c:\\windows\\nilesoft\\shell.nss", &sb) != 0)
	{
		HKEY key = HKEY_CURRENT_USER;
		HKEY result;
		HRESULT hr = RegOpenKey(key, L"Software\\Classes\\CLSID\\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}", &result);


		if (SUCCEEDED(hr))
		{
			*menuIndex = Classic;
			RegCloseKey(result);
		}

		CHAR path[MAX_PATH];

		hr = SHGetFolderPathA(NULL, CSIDL_COMMON_STARTMENU, NULL, 0, path);

		string file = string(path);
		file += +"\\programs\\startup\\acrylmenu.lnk";
		if (stat(file.c_str(), &sb) == 0)
		{
			*menuIndex = ClassicTransparent;
		}
	}
	else
	{
		wstring config = LoadUtf8FileToString(L"c:\\windows\\nilesoft\\shell.nss");
		if (config.compare(L"modify(where=this.title.length > 15 menu=title.more_options)"))
		{
			*menuIndex = NilesoftSmall;
		}
		else
		{
			*menuIndex = NilesoftFull;
		}
	}
	return S_OK;
}

HRESULT CRectifyUtil::SetCurrentMenuByIndex(DWORD menuIndex)
{
	if (menuIndex == Normal)
	{
		// Restore default Windows 11 menus
		RegDeleteKey(HKEY_CURRENT_USER, TEXT("Software\\Classes\\CLSID\\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}"));
		_DeleteClassicTransparent();
		KillTask(L"AcrylicMenusLoader");
		return S_OK;
	}
	else if (menuIndex == Classic)
	{
		_DeleteClassicTransparent();
		KillTask(L"AcrylicMenusLoader");

		HKEY result;
		HRESULT status = RegCreateKey(HKEY_CURRENT_USER, TEXT("Software\\Classes\\CLSID\\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}"), &result);
		if (SUCCEEDED(status))
		{
			RegCloseKey(result);
			return S_OK;
		}
		
		return status;
	}
	else
	{
		WCHAR buffer[200];

		swprintf(buffer, 199, L"Failed to update menu settings. Unknown enum index %d", menuIndex);
		MessageBox(NULL, buffer, TEXT("SetCurrentMenuByIndex"), MB_ICONERROR);
		return E_NOTIMPL;
	}
}

BOOL IsDarkTheme()
{
	WCHAR value[255] = { 0 };
	PVOID pvData = value;
	DWORD size = sizeof(value);
	RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager", L"DllName", RRF_RT_REG_SZ, 0, pvData, &size);
	std::wstring msstylePath = std::wstring((LPCWSTR)pvData);

	size_t result = msstylePath.find(L"Dark");
	return result > 0 ? TRUE : FALSE;
}

HRESULT CRectifyUtil::QueryInterface(
	REFIID riid,
	_COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppv)
{
	static const QITAB qit[] = {
		QITABENT(CRectifyUtil, IRectifyUtil),
		QITABENT(CRectifyUtil, IUnknown),
		{ 0 },
	};
	HRESULT x = QISearch(this, qit, riid, ppv);
	return x;
}

ULONG CRectifyUtil::AddRef(void)
{
	return m_ref++;
}

ULONG CRectifyUtil::Release(void)
{
	ULONG cRef = InterlockedDecrement(&m_ref);
	if (0 == cRef)
	{
		delete this;
	}
	return cRef;
}