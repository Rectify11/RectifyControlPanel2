//Core APIS
#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <objbase.h>
#include <Shlobj.h>
#include <olectl.h>
#include <strsafe.h>
#include <atlcomcli.h>
#include <string>
#include <winternl.h>
#include "undoc.h"

extern HINSTANCE g_hInst;

//DirectUI
#pragma comment(lib,"dui70.lib")
#include "..\dui70\DirectUI\DirectUI.h"
using namespace DirectUI;
#include <fstream>
#include <sstream>
#include <format>
#include <functional>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

#include "CControlPanelNavLinkCommand.h"
#include "CControlPanelNavLink.h"
#include "CControlPanelNavLinks.h"
#include "theme.h"

extern HINSTANCE g_hInst;

void DllAddRef();
void DllRelease();

