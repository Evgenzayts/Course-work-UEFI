/**
  DO NOT EDIT
  FILE auto-generated
  Module name:
    AutoGen.h
  Abstract:       Auto-generated AutoGen.h for building module or library.
**/

#ifndef _AUTOGENH_44B09E3D_5EDA_4673_ABCF_C8AE4560C8EC
#define _AUTOGENH_44B09E3D_5EDA_4673_ABCF_C8AE4560C8EC

#ifdef __cplusplus
extern "C" {
#endif

#include <PiMm.h>

extern GUID  gEfiCallerIdGuid;
extern GUID  gEdkiiDscPlatformGuid;
extern CHAR8 *gEfiCallerBaseName;


// Guids
extern EFI_GUID gVarCheckPolicyLibMmiHandlerGuid;

// Definition of SkuId Array
extern UINT64 _gPcd_SkuId_Array[];

EFI_STATUS
EFIAPI
VarCheckPolicyLibStandaloneConstructor (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_MM_SYSTEM_TABLE   *MmSystemTable
  );


#ifdef __cplusplus
}
#endif

#endif
