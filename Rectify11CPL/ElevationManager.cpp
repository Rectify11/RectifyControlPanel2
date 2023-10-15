#include <windows.h>
#include "undoc.h"
#include "ElevationManager.h"
#include "Guid.h"
#include <string>
using namespace std;
#include "CRectifyUtil.h"

IRectifyUtil* ElevationManager::Initialize(BOOL elevate)
{
    GUID CLSID_MultiObjectElevationFactory = {0}, GUID_IMultiObjectElevationFactory = { 0 };

    GUID CLSID_powercplthing = { 0 };
    GUID CLSID_powercplthingclass = { 0 };
    GUID CLSID_powercplthinginterface = { 0 };
    if (FAILED(CLSIDFromString(L"{36f0bd14-d84d-468c-b79c-9990f3fa897f}", (LPCLSID)&CLSID_MultiObjectElevationFactory)))
    {
        return NULL;
    };
    if (FAILED(CLSIDFromString(L"{6fabda16-031e-47e3-b2a2-2339c05ccb9e}", (LPCLSID)&GUID_IMultiObjectElevationFactory)))
    {
        return NULL;
    };
    if (FAILED(CLSIDFromString(L"{5bff4e02-d379-4050-a382-c6504a980d46}", (LPCLSID)&CLSID_powercplthingclass)))
    {
        return NULL;
    };

    if (FAILED(CLSIDFromString(L"{5BFF4E01-D379-4050-A382-C6504A980D46}", (LPCLSID)&CLSID_powercplthinginterface)))
    {
        return NULL;
    };


    IMultiObjectElevationFactory* ppv = NULL;
    IRectifyUtil* ppv2 = NULL;
    HRESULT hr = CoCreateInstance(CLSID_MultiObjectElevationFactory, NULL, CLSCTX_INPROC_SERVER, GUID_IMultiObjectElevationFactory, (LPVOID*)&ppv);
    //hr = CoCreateInstance(CLSID_CRectifyUtil, NULL, 3, INTERACE_IRectifyUtil, (LPVOID*)&ppv2);
    if (SUCCEEDED(hr))
    {
        HWND hwnd = FindWindow(TEXT("Progman"), TEXT("Program Manager"));
        hr = ppv->InitializeNoWaitCursor(hwnd, CLSID_RectifyUtilServer);
        if (FAILED(hr))
        {
            WCHAR buffer[1024];
            swprintf(buffer, 1024, L"InitializeNoWaitCursor() failed with %d", hr);
            MessageBox(NULL, buffer, L"work", MB_ICONERROR);
            return NULL;
        }
       
        hr = ppv->CreateElevatedObject(CLSID_CRectifyUtil, CLSID_powercplthinginterface, (void**)&ppv2);
        if (hr != S_OK)
        {
            WCHAR buffer[1024];
            swprintf(buffer, 1024, L"CreateElevatedObject() failed with %d", hr);
            MessageBox(NULL, buffer, L"work", MB_ICONERROR);
        }
        ppv->Release();
        if (SUCCEEDED(hr))
        {
            return ppv2;
        }
    }
    return NULL;
}