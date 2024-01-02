//Core APIS
#pragma once
#pragma comment(lib,"dui70.lib")
#include <windows.h>
#include <shlwapi.h>
#include <objbase.h>

#include <Shlobj.h>
#include <olectl.h>
#include <strsafe.h>
#include <atlcomcli.h>
#include <string>
#include "undoc.h"
#include "resource.h"
#include "dllmain.h"
#include <fstream>
#include <sstream>
#include <format>
#include <functional>
#include <filesystem>
#include "..\dui70\DirectUI\DirectUI.h"
using namespace DirectUI;
using namespace std;
namespace fs = std::filesystem;

#include "CControlPanelNavLinkCommand.h"
#include "CControlPanelNavLink.h"
#include "CControlPanelNavLinks.h"

#include "theme.h"

#define NOT_IMPLEMENTED MessageBox(NULL, TEXT(__FUNCTION__), TEXT("Non implementented function in some class"), MB_ICONERROR)
#define SHOW_ERROR(x) MessageBox(NULL, TEXT(x), TEXT("Error"), MB_ICONERROR)