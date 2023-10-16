
#define PROXY_CLSID_IS {0x53387088, 0xa8c4, 0x4ae3,{ 0x9c, 0xe7, 0x53, 0x6b, 0x90, 0x7, 0x7d, 0x13 }}
#include <rpcproxy.h>

#ifdef __cplusplus
extern "C" {
#endif

	extern const ProxyFileInfo IRectifyUtil_ProxyFileInfo;


	extern const ProxyFileInfo* const aProxyFileList[] = {
		/* Start of list */
				&IRectifyUtil_ProxyFileInfo,
				/* End of list */
				0 };


	CStdPSFactoryBuffer gPFactory = { 0,0,0,0 };
	HRESULT __stdcall DllCanUnloadNow2() {
		return NdrDllCanUnloadNow(&gPFactory);
	}
	void __stdcall GetProxyDllInfo(const ProxyFileInfo*** pInfo, const CLSID** pId) {
		*pInfo = (const ProxyFileInfo**)aProxyFileList; *pId = (aProxyFileList[0]->pStubVtblList[0] != 0 ? aProxyFileList[0]->pStubVtblList[0]->header.piid : 0);
	};
	
	HRESULT __stdcall DllGetClassObject2(const IID* rclsid, const IID* riid, void** ppv) {
		return NdrDllGetClassObject(rclsid, riid, ppv, (const ProxyFileInfo**)aProxyFileList, (aProxyFileList[0]->pStubVtblList[0] != 0 ? aProxyFileList[0]->pStubVtblList[0]->header.piid : 0), &gPFactory);
	}

	ULONG __stdcall CStdStubBuffer_Release(IRpcStubBuffer* This) {
		return NdrCStdStubBuffer_Release(This, (IPSFactoryBuffer*)&gPFactory);
	} 
	
	void __cdecl DllRpcDummyCall(void) { }

#ifdef __cplusplus
}  /*extern "C" */
#endif

/* end of generated dlldata file */
