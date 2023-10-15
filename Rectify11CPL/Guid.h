/**************************************************************************
    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 
   (c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/
#define INITGUID
#include <guiddef.h>
#include <propkeydef.h>

//shell:::{542EEE1B-A254-46F7-B980-35BECF6076A4}

// {542EEE1B-A254-46F7-B980-35BECF6076A4}
DEFINE_GUID(CLSID_FolderViewImpl, 0x542eee1b, 0xa254, 0x46f7, 0xb9, 0x80, 0x35, 0xbe, 0xcf, 0x60, 0x76, 0xa4);

//NOTE: the guid must also be changed in tasks.xml
// {EC8669E2-7F9F-42AC-A2D7-307E23CA9E20}
DEFINE_GUID(CLSID_FolderViewImplElement, 0xec8669e2, 0x7f9f, 0x42ac, 0xa2, 0xd7, 0x30, 0x7e, 0x23, 0xca, 0x9e, 0x20);

// NOTE: It is 100% intentonal that this interface CLSID already exists, otherwide some actxprxy thing fails... Do not change this CLSID
// {5BFF4E01-D379-4050-A382-C6504A980D46}
DEFINE_GUID(IID_IRectifyUtil, 0x5BFF4E01, 0xD379, 0x4050, 0xa3, 0x82, 0xc6, 0x50, 0x4A, 0x98, 0x0D, 0x46);

// {9CD66066-9784-4DA6-A27A-D322FC96D02E}
DEFINE_GUID(CLSID_CRectifyUtil, 0x9cd66066, 0x9784, 0x4da6, 0xa2, 0x7a, 0xd3, 0x22, 0xfc, 0x96, 0xd0, 0x2e);

// {CA4F4753-3B80-4713-A13A-5885FC56DD8D}
DEFINE_GUID(CLSID_RectifyUtilServer, 0xca4f4753, 0x3b80, 0x4713, 0xa1, 0x3a, 0x58, 0x85, 0xfc, 0x56, 0xdd, 0x8d);
