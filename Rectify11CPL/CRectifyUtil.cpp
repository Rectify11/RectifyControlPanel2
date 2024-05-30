// This class implements the IRectifyUtil interface. This class is registered as a COM object to allow the code
// to be ran as administrator, without elevating explorer.exe or starting another process

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

WCHAR Rectify11PrefsKey[] = L"SOFTWARE\\Rectify11Prefs";

CRectifyUtil::CRectifyUtil() : m_ref(1)
{
	DllAddRef();
}

CRectifyUtil::~CRectifyUtil()
{
	DllRelease();
}

DWORD FindProcessId(const WCHAR* procname)
{
	HANDLE hSnapshot;
	PROCESSENTRY32 pe;
	int pid = 0;
	BOOL hResult;

	memset(&pe, 0, sizeof(pe));

	// snapshot of all processes in the system
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

	// initializing size: needed for using Process32First
	pe.dwSize = sizeof(PROCESSENTRY32);

	// info about first process encountered in a system snapshot
	hResult = Process32First(hSnapshot, &pe);

	// retrieve information about the processes
	// and exit if unsuccessful
	while (hResult) {
		// if we find the process: return process ID
		if (wcscmp(procname, pe.szExeFile) == 0) {
			pid = pe.th32ProcessID;
			break;
		}
		hResult = Process32Next(hSnapshot, &pe);
	}

	// closes an open handle (CreateToolhelp32Snapshot)
	CloseHandle(hSnapshot);
	return pid;
}

HRESULT deleteTask(std::wstring taskName)
{
	HRESULT hr = S_OK;

	ITaskService* pITS = NULL;
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
	ITaskService* pITS = NULL;
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

	ITaskService* pITS = NULL;
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

	wstring taskxml = wstring(L"<?xml version=\"1.0\" encoding=\"UTF-16\"?><Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\"><RegistrationInfo><URI>");
	taskxml += L"\\";
	taskxml += taskName;
	taskxml += L"</URI></RegistrationInfo><Triggers><LogonTrigger><Enabled>true</Enabled></LogonTrigger></Triggers><Principals><Principal id=\"Author\"><GroupId>S-1-5-32-545</GroupId><RunLevel>HighestAvailable</RunLevel></Principal></Principals><Settings><MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy><DisallowStartIfOnBatteries>false</DisallowStartIfOnBatteries><StopIfGoingOnBatteries>false</StopIfGoingOnBatteries><AllowHardTerminate>true</AllowHardTerminate><StartWhenAvailable>false</StartWhenAvailable><RunOnlyIfNetworkAvailable>false</RunOnlyIfNetworkAvailable><IdleSettings><StopOnIdleEnd>true</StopOnIdleEnd><RestartOnIdle>false</RestartOnIdle></IdleSettings><AllowStartOnDemand>true</AllowStartOnDemand><Enabled>true</Enabled><Hidden>false</Hidden><RunOnlyIfIdle>false</RunOnlyIfIdle><WakeToRun>false</WakeToRun><ExecutionTimeLimit>PT0S</ExecutionTimeLimit><Priority>5</Priority></Settings><Actions Context=\"Author\"><Exec><Command>";
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

BOOL CRectifyUtil::KillTask(wstring proc)
{
	while (1)
	{
		DWORD pid = FindProcessId(proc.c_str());
		if (pid == 0)
		{
			return FALSE;
		}

		const auto proc = OpenProcess(PROCESS_TERMINATE, false, pid);
		BOOL result = TerminateProcess(proc, 1);
		CloseHandle(proc);
	}
	return TRUE;
}


HRESULT startProc(LPCWSTR proc, wstring args = L"", bool waitForExit = false)
{
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	WCHAR proc_buffer[1000];
	if (proc)
	{
		ExpandEnvironmentStringsW(proc, proc_buffer, 999);
	}

	WCHAR args_buffer[1000] = { 0 };
	if (!args.empty())
	{
		wcsncpy_s(args_buffer, 999, args.c_str(), args.size());
	}
	BOOL hr = CreateProcessW(proc ? proc_buffer : NULL,
		args_buffer,           // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure
	);

	if (!hr)
	{
		WCHAR error_buffer[1000];
		std::string message = std::system_category().message(hr);
		swprintf_s(error_buffer, L"Error while starting process %s with arguments %s: %s (%ld)", proc, args.c_str(), message.c_str(), GetLastError());
		MessageBox(NULL, error_buffer, TEXT("Starting process failed"), MB_ICONERROR);
	}
	else
	{
		if (waitForExit)
		{
			// Successfully created the process.  Wait for it to finish.
			WaitForSingleObject(pi.hProcess, INFINITE);
			DWORD exitCode;
			// Get the exit code.
			hr = GetExitCodeProcess(pi.hProcess, &exitCode);
		}
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return hr ? S_OK : S_FALSE;
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
	FILE* f = NULL;
	_wfopen_s(&f, filename.c_str(), L"rtS, ccs=UTF-8");

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

void CRectifyUtil::RestartExplorer()
{
	HWND hwnd = FindWindow(L"Shell_TrayWnd", NULL);
	DWORD pid = {};
	GetWindowThreadProcessId(hwnd, &pid);

	HANDLE h_explorer;
	h_explorer = OpenProcess(PROCESS_TERMINATE, false, pid);
	TerminateProcess(h_explorer, 2);
	CloseHandle(h_explorer);

	startProc(L"C:\\windows\\explorer.exe");
}

HRESULT CreateLink(LPCWSTR lpszTarget, LPCWSTR lpszDesc, LPCWSTR lpszWorkingDir, LPCSTR lpszShortcutPath)
{
	HRESULT hres;
	IShellLink* psl = NULL;

	// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
	// has already been called.
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf = NULL;

		// Set the path to the shortcut target and add the description. 
		psl->SetPath(lpszTarget);
		psl->SetDescription(lpszDesc);
		psl->SetWorkingDirectory(lpszWorkingDir);

		// Query IShellLink for the IPersistFile interface, used for saving the 
		// shortcut in persistent storage. 
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

		if (SUCCEEDED(hres))
		{
			WCHAR wsz[MAX_PATH];

			// Ensure that the string is Unicode. 
			MultiByteToWideChar(CP_ACP, 0, lpszShortcutPath, -1, wsz, MAX_PATH);

			// Save the link by calling IPersistFile::Save. 
			hres = ppf->Save(wsz, TRUE);
			ppf->Release();
		}
		psl->Release();
	}
	return hres;
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

HRESULT CRectifyUtil::_EnableClassicTransparent()
{
	CHAR path[MAX_PATH];
	HRESULT hr = SHGetFolderPathA(NULL, CSIDL_COMMON_STARTMENU, NULL, 0, path);
	if (SUCCEEDED(hr))
	{
		WCHAR workingdir_buffer[MAX_PATH];
		ExpandEnvironmentStringsW(L"%windir%\\nilesoft\\AcrylicMenus", workingdir_buffer, MAX_PATH);

		WCHAR target_buffer[MAX_PATH] = { 0 };
		ExpandEnvironmentStringsW(L"%windir%\\nilesoft\\AcrylicMenus\\AcrylicMenusLoader.exe", target_buffer, MAX_PATH);

		string shortcut_path = string(path);
		shortcut_path += +"\\programs\\startup\\acrylmenu.lnk";
		startProc(target_buffer);
		return CreateLink(target_buffer, L"Launch classic transparent menu hook", workingdir_buffer, shortcut_path.c_str());
	}
	else
	{
		return hr;
	}
}

HRESULT CRectifyUtil::_DeleteClassicTransparent()
{
	CHAR path[MAX_PATH];
	HRESULT hr = SHGetFolderPathA(NULL, CSIDL_COMMON_STARTMENU, NULL, 0, path);
	KillTask(L"AcrylicMenusLoader.exe");
	if (SUCCEEDED(hr))
	{
		string file = string(path);
		file += +"\\programs\\startup\\acrylmenu.lnk";
		return DeleteFileA(file.c_str()) ? S_OK : E_ACTIVATIONDENIED_SHELLNOTREADY;
	}
	else
	{
		return hr;
	}
}
HRESULT CRectifyUtil::_EnableClassicMenu()
{
	HKEY result;
	HRESULT status = RegCreateKey(HKEY_CURRENT_USER, TEXT("Software\\Classes\\CLSID\\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}"), &result);
	if (SUCCEEDED(status))
	{
		HKEY inprocServer;
		HRESULT status = RegCreateKey(result, TEXT("InprocServer32"), &inprocServer);
		if (SUCCEEDED(status))
		{
			RegSetValueExW(inprocServer, NULL, 0, REG_SZ, (const BYTE*)L"", 2);
			RegCloseKey(inprocServer);
			return S_OK;
		}
		RegCloseKey(result);
		return status;
	}
	return status;
}

HRESULT CRectifyUtil::_DeleteNilesoftIfExists()
{
	struct stat sb;
	if (stat("c:\\windows\\nilesoft\\shell.nss", &sb) == 0)
	{
		// delete configuration file
		BOOL h = DeleteFile(TEXT("c:\\windows\\nilesoft\\shell.nss"));
		if (!h)
		{
			WCHAR buffer[200];

			swprintf(buffer, 199, L"Failed to uninstall nilesoft. failed to delete config file. hresult is %d\n", GetLastError());
			MessageBox(NULL, buffer, TEXT("_DeleteNilesoftIfExists"), MB_ICONERROR);
		}

		startProc(NULL, L"c:\\windows\\nilesoft\\shell.exe -unregister", true);
		return S_OK;
	}
	return S_OK;
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
			// kill micaforeveryone and explorerframe if already running
			KillTask(L"MicaForEveryone.exe");
			KillTask(L"ExplorerFrame.exe");

			char* localappdata = nullptr;
			size_t sz = 0;
			if (_dupenv_s(&localappdata, &sz, "localappdata") == 0 && localappdata != nullptr)
			{
				string appdata = string(localappdata);
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
					swprintf(buffer, 1024, L"Warning: Micaforeveryone configuration file is missing! File name is %ws. Try reinstalling themes option in Rectify11 Installer", config_file_src.c_str());
					MessageBox(NULL, buffer, L"Rectify11 control panel applet", MB_ICONWARNING);
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
					KillTask(L"ExplorerFrame.exe");
				}

				// Start mica for everyone
				startProc(L"%systemroot%\\MicaForEveryone\\MicaForEveryone.exe");
			}
			else
			{
				MessageBox(NULL, L"Appdata env variable not found", L"Failed to create MFE task", MB_ICONERROR);
			}
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
		KillTask(L"ExplorerFrame.exe");
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
		LONG hr = RegOpenKeyEx(key, L"Software\\Classes\\CLSID\\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}", 0, KEY_READ, &result);


		if (hr == 0)
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
	HRESULT hr = S_OK;
	if (menuIndex == Normal)
	{
		// Restore default Windows 11 menus
		hr = RegDeleteTree(HKEY_CURRENT_USER, TEXT("Software\\Classes\\CLSID\\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}"));
		_DeleteClassicTransparent();
		_DeleteNilesoftIfExists();
	}
	else if (menuIndex == NilesoftSmall)
	{
		std::wofstream f(L"c:\\windows\\nilesoft\\shell.nss");
		f.clear();
		f << "settings\r\n{\r\n	priority=1\r\n	exclude.where = !process.is_explorer\r\n	showdelay = 100\r\n	// Options to allow modification of system items\r\n	modify.remove.duplicate=1\r\n	tip\r\n	{\r\n		enabled=1\r\n		opacity=100\r\n		width=400\r\n		radius=1\r\n		time=1.25\r\n		padding=[10,10]\r\n	}\r\n}\r\ntheme\r\n{\r\n	name=\"modern\"\r\n}\r\nimport 'imports/theme.nss'\r\nimport 'imports/images.nss'\r\n\r\nimport 'imports/modify.nss'\r\nmodify(where=this.title.length > 15 menu=title.more_options)\r\n\r\nmenu(mode=\"multiple\" title=\"Pin/Unpin\" image=icon.pin) {}\r\nmenu(mode=\"multiple\" title=title.more_options image=icon.more_options) {}\r\nimport 'imports/taskbar.nss'";
		f.close();

		startProc(NULL, L"c:\\windows\\nilesoft\\shell.exe -register", true);


	}
	else if (menuIndex == NilesoftFull)
	{
		std::wofstream f(L"c:\\windows\\nilesoft\\shell.nss");
		f.clear();
		f << "settings\r\n{\r\n	priority=1\r\n	exclude.where = !process.is_explorer\r\n	showdelay = 100\r\n	// Options to allow modification of system items\r\n	modify.remove.duplicate=1\r\n	tip\r\n	{\r\n		enabled=1\r\n		opacity=100\r\n		width=400\r\n		radius=1\r\n		time=1.25\r\n		padding=[10,10]\r\n	}\r\n}\r\ntheme\r\n{\r\n	name=\"modern\"\r\n}\r\nimport 'imports/theme.nss'\r\nimport 'imports/images.nss'\r\n\r\nimport 'imports/modify.nss'\r\nmenu(mode=\"multiple\" title=\"Pin/Unpin\" image=icon.pin) {}\r\nmenu(mode=\"multiple\" title=title.more_options image=icon.more_options) {}\r\nimport 'imports/taskbar.nss'";
		f.close();

		startProc(NULL, L"c:\\windows\\nilesoft\\shell.exe -register", true);
	}
	else if (menuIndex == Classic)
	{
		_DeleteClassicTransparent();
		_DeleteNilesoftIfExists();
		hr = _EnableClassicMenu();
	}
	else if (menuIndex == ClassicTransparent)
	{
		_DeleteNilesoftIfExists();
		hr = _EnableClassicMenu();
		if (SUCCEEDED(hr))
		{
			hr = _EnableClassicTransparent();
		}
	}
	else
	{
		WCHAR buffer[200];

		swprintf(buffer, 199, L"Failed to update menu settings. Unknown enum index %d", menuIndex);
		MessageBox(NULL, buffer, TEXT("SetCurrentMenuByIndex"), MB_ICONERROR);
		hr = E_NOTIMPL;
	}

	return hr;
}

static bool themetool_loaded = false;
HRESULT CRectifyUtil::ApplyTheme(LPCWSTR pThemeName)
{
	HRESULT hr = S_OK;
	if (!themetool_loaded)
	{
		hr = themetool_init();
	}

	if (FAILED(hr))
	{
		return hr;
	}
	else {
		themetool_loaded = true;
	}
	ULONG apply_flags = 0;

	// load appy flags
	HKEY Rectify11;
	if (RegCreateKey(HKEY_CURRENT_USER, Rectify11PrefsKey, &Rectify11))
	{
		SHOW_ERROR("Failed to create Rectify11Prefs key");
	}

	DWORD size = 4;

	DWORD IgnoreBgVal = 0;
	DWORD IgnoreCursorsVal = 0;
	DWORD IgnoreIconsVal = 0;
	DWORD IgnoreColorsVal = 0;
	DWORD IgnoreSoundsVal = 0;
	DWORD IgnoreScreensaversVal = 0;

	RegQueryValueExW(Rectify11, L"IgnoreBg", 0, NULL, (LPBYTE)&IgnoreBgVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreCursors", 0, NULL, (LPBYTE)&IgnoreCursorsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreIcons", 0, NULL, (LPBYTE)&IgnoreIconsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreColors", 0, NULL, (LPBYTE)&IgnoreColorsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreSounds", 0, NULL, (LPBYTE)&IgnoreSoundsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreScreensavers", 0, NULL, (LPBYTE)&IgnoreScreensaversVal, &size);
	RegCloseKey(Rectify11);

	if (IgnoreBgVal)
	{
		apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_BACKGROUND;
	}
	if (IgnoreCursorsVal)
	{
		apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_CURSOR;
	}
	if (IgnoreIconsVal)
	{
		apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_DESKTOP_ICONS;
	}
	if (IgnoreColorsVal)
	{
		apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_COLOR;
	}
	if (IgnoreSoundsVal)
	{
		apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_SOUND;
	}
	if (IgnoreSoundsVal)
	{
		apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_SCREENSAVER;
	}


	ULONG themeCount = 0;
	themetool_get_theme_count(&themeCount);
	if (FAILED(hr))
	{
		return hr;
	}
	std::wstring targetTheme(pThemeName);
	std::transform(targetTheme.begin(), targetTheme.end(), targetTheme.begin(), ::tolower);

	for (ULONG i = 0; i < themeCount; i++)
	{
		ITheme* theme = NULL;
		themetool_get_theme(i, &theme);

		WCHAR buffer[512];
		themetool_theme_get_display_name(theme, buffer, 256);

		std::wstring displayName(buffer);
		std::transform(displayName.begin(), displayName.end(), displayName.begin(), ::tolower);

		if (displayName == targetTheme)
		{
			hr = themetool_set_active(NULL, i, TRUE, 0, 0);

			themetool_theme_release(theme);
			break;
		}

		themetool_theme_release(theme);
	}
	return hr;
}
HRESULT CRectifyUtil::InstallThemeTool()
{
	HRESULT hr = S_OK;
	if (!themetool_loaded)
	{
		hr = themetool_init();
	}

	if (hr != S_OK)
	{
		return hr;
	}
	else {
		themetool_loaded = true;
	}

	hr = secureuxtheme_install(SECUREUXTHEME_INSTALL_HOOK_LOGONUI | SECUREUXTHEME_INSTALL_RENAME_DEFAULTCOLORS);
	return hr;
}
HRESULT CRectifyUtil::UninstallThemeTool()
{
	HRESULT hr = S_OK;
	if (!themetool_loaded)
	{
		hr = themetool_init();
	}

	if (FAILED(hr))
	{
		return hr;
	}
	else {
		themetool_loaded = true;
	}

	hr = secureuxtheme_uninstall();
	return hr;
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