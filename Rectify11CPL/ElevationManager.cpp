#include <windows.h>
#include "undoc.h"
#include "ElevationManager.h"
#include "Guid.h"
#include <string>
using namespace std;
#include <system_error>
#include "CRectifyUtil.h"

IRectifyUtil* ElevationManager::Initialize(HWND window)
{
    GUID CLSID_MultiObjectElevationFactory = {0}, GUID_IMultiObjectElevationFactory = { 0 };
    if (FAILED(CLSIDFromString(L"{36f0bd14-d84d-468c-b79c-9990f3fa897f}", (LPCLSID)&CLSID_MultiObjectElevationFactory)))
    {
        return NULL;
    };
    if (FAILED(CLSIDFromString(L"{6fabda16-031e-47e3-b2a2-2339c05ccb9e}", (LPCLSID)&GUID_IMultiObjectElevationFactory)))
    {
        return NULL;
    };

    IMultiObjectElevationFactory* ppv = NULL;
    IRectifyUtil* ppv2 = NULL;

    // Create an instance of MultiObjectElevation factory which allows us to run COM objects as administrator
    HRESULT hr = CoCreateInstance(CLSID_MultiObjectElevationFactory, NULL, CLSCTX_INPROC_SERVER, GUID_IMultiObjectElevationFactory, (LPVOID*)&ppv);
    if (SUCCEEDED(hr))
    {
        // find explorer window
        HWND hwnd = FindWindow(TEXT("Progman"), TEXT("Program Manager"));

        // initalize
        hr = ppv->Initialize(window, CLSID_RectifyUtilServer);
        if (FAILED(hr))
        {
            CHAR buffer[1024];
            std::string message = std::system_category().message(hr);
            sprintf_s(buffer, 1024, "MultiObjectElevationFactory::Initialize() failed with 0x%x (%s)", hr, message.c_str());
            MessageBoxA(window, buffer, "Error", MB_ICONERROR);
            return NULL;
        }
        
        // create CRectifyUtil as administrator
        hr = ppv->CreateElevatedObject(CLSID_CRectifyUtil, IID_IRectifyUtil, (void**)&ppv2);
        if (hr != S_OK)
        {
            std::string message = std::system_category().message(hr);
            CHAR buffer[1024];
            sprintf_s(buffer, "CreateElevatedObject() failed with 0x%x (%s)", hr, message.c_str());
            MessageBoxA(window, buffer, "Error", MB_ICONERROR);
        }
        ppv->Release();
        if (SUCCEEDED(hr))
        {
            return ppv2;
        }
    }
    return NULL;
}