/*
 * File:	ctl3d32.c
 * Purpose:	32bit interface to CTL3D functions for Watcom C/C++
 * Author:	Patrick Halke
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995
 */

#include <malloc.h>

#include <windows.h>

#include "ctl3d.h"

#if defined(__WINDOWS_386__)

#ifdef __cplusplus
extern "C" {
#endif

#undef FAR
#define FAR

#define INDIR_INT INDIR_WORD
#define INDIR_UINT INDIR_WORD
#define INDIR_WPARAM INDIR_UINT
#define INDIR_LPARAM INDIR_DWORD
#define INDIR_LONG INDIR_DWORD
#define INDIR_ULONG INDIR_DWORD

#ifdef STRICT
#define INDIR_HANDLE INDIR_PTR
#define INDIR_HWND INDIR_PTR
#define INDIR_HDC INDIR_PTR
#else
#define INDIR_HANDLE INDIR_UINT
#define INDIR_HWND INDIR_UINT
#define INDIR_HDC INDIR_UINT
#endif

typedef struct tagCTL3DFUNCTIONS {
    HINSTANCE	dll;
    /* Function Handles */
    HINDIR	_Ctl3dSubclassDlg;
    HINDIR	_Ctl3dSubclassDlgEx;
    HINDIR	_Ctl3dGetVer;
    HINDIR	_Ctl3dEnabled;
    HINDIR	_Ctl3dCtlColor;
    HINDIR	_Ctl3dCtlColorEx;
    HINDIR	_Ctl3dColorChange;
    HINDIR	_Ctl3dSubclassCtl;
    HINDIR	_Ctl3dDlgFramePaint;
    HINDIR	_Ctl3dAutoSubclass;
    HINDIR	_Ctl3dRegister;
    HINDIR	_Ctl3dUnregister;
    HINDIR	_Ctl3dWinIniChange;
} CTL3DFUNCTIONS;

static CTL3DFUNCTIONS	Ctl3dFunc = { 0 };

static BOOL load_functions( CTL3DFUNCTIONS* functions )
{
    FARPROC		proc;
    HINSTANCE		dll;

    dll = LoadLibrary( "CTL3D.DLL" );
    if( dll < HINSTANCE_ERROR ) {
	return( FALSE );
    }

    /* Function thunks */
    
    proc = GetProcAddress(dll, "Ctl3dSubclassDlg");
    functions->_Ctl3dSubclassDlg = GetIndirectFunctionHandle( proc,
	INDIR_HWND,
	INDIR_WORD,
	INDIR_ENDLIST );
    
    proc = GetProcAddress( dll, "Ctl3dSubclassDlgEx" );
    functions->_Ctl3dSubclassDlgEx = GetIndirectFunctionHandle( proc,
	INDIR_HWND,
	INDIR_DWORD,
	INDIR_ENDLIST );
    
    proc = GetProcAddress( dll, "Ctl3dGetVer" );
    functions->_Ctl3dGetVer = GetIndirectFunctionHandle( proc,
	INDIR_ENDLIST );
    
    proc = GetProcAddress( dll, "Ctl3dEnabled" );
    functions->_Ctl3dEnabled = GetIndirectFunctionHandle( proc,
	INDIR_ENDLIST );
    
    proc = GetProcAddress( dll, "Ctl3dCtlColor" );
    functions->_Ctl3dCtlColor = GetIndirectFunctionHandle( proc,
	INDIR_HDC,
	INDIR_LONG,
	INDIR_ENDLIST );
    
    proc = GetProcAddress( dll, "Ctl3dCtlColorEx" );
    functions->_Ctl3dCtlColorEx = GetIndirectFunctionHandle( proc,
	INDIR_UINT,
	INDIR_WPARAM,
	INDIR_LPARAM,
	INDIR_ENDLIST );

    proc = GetProcAddress( dll, "Ctl3dColorChange" );
    functions->_Ctl3dColorChange = GetIndirectFunctionHandle( proc,
	INDIR_ENDLIST );

    proc = GetProcAddress( dll, "Ctl3dSubclassCtl" );
    functions->_Ctl3dSubclassCtl = GetIndirectFunctionHandle( proc,
	INDIR_HWND,
	INDIR_ENDLIST );
  
    proc = GetProcAddress( dll, "Ctl3dDlgFramePaint" );
    functions->_Ctl3dDlgFramePaint = GetIndirectFunctionHandle( proc,
	INDIR_HWND,
	INDIR_UINT,
	INDIR_WPARAM,
	INDIR_LPARAM,
	INDIR_ENDLIST );
  
    proc = GetProcAddress( dll, "Ctl3dAutoSubclass" );
    functions->_Ctl3dAutoSubclass = GetIndirectFunctionHandle( proc,
	INDIR_HANDLE,
	INDIR_ENDLIST );
  
    proc = GetProcAddress( dll, "Ctl3dRegister" );
    functions->_Ctl3dRegister = GetIndirectFunctionHandle( proc,
	INDIR_HANDLE,
	INDIR_ENDLIST );
  
    proc = GetProcAddress( dll, "Ctl3dUnregister" );
    functions->_Ctl3dUnregister = GetIndirectFunctionHandle( proc,
	INDIR_HANDLE,
	INDIR_ENDLIST );
  
    proc = GetProcAddress( dll, "Ctl3dWinIniChange" );
    functions->_Ctl3dWinIniChange = GetIndirectFunctionHandle( proc,
	INDIR_ENDLIST );

    functions->dll = dll;
    return( TRUE );
}

static void unload_functions( CTL3DFUNCTIONS * functions )
{
    FreeLibrary( functions->dll );
    functions->dll = 0;
}

/* Function Definitions */

BOOL WINAPI Ctl3dSubclassDlg(HWND hwnd, WORD w)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
    return FALSE;
  
  return (BOOL)InvokeIndirectFunction(Ctl3dFunc._Ctl3dSubclassDlg,
				      hwnd, w);
}

BOOL WINAPI Ctl3dSubclassDlgEx(HWND hwnd, DWORD dw)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
    return FALSE;
  
  return (BOOL)InvokeIndirectFunction(Ctl3dFunc._Ctl3dSubclassDlgEx,
				      hwnd, dw);
}

WORD WINAPI Ctl3dGetVer(void)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
      return FALSE;
  
  return (WORD)InvokeIndirectFunction(Ctl3dFunc._Ctl3dGetVer);
}

BOOL WINAPI Ctl3dEnabled(void)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
      return FALSE;
  
  return (BOOL)InvokeIndirectFunction(Ctl3dFunc._Ctl3dEnabled);
}

HBRUSH WINAPI Ctl3dCtlColor(HDC hdc, LONG l)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
    return FALSE;
  
  return (HBRUSH)InvokeIndirectFunction(Ctl3dFunc._Ctl3dCtlColor,
				        hdc, l);
}

HBRUSH WINAPI Ctl3dCtlColorEx(UINT ui, WPARAM wp, LPARAM lp)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
    return FALSE;
  
  return (HBRUSH)InvokeIndirectFunction(Ctl3dFunc._Ctl3dCtlColorEx,
				        ui, wp, lp);
}

BOOL WINAPI Ctl3dColorChange(void)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
    return FALSE;
  
  return (BOOL)InvokeIndirectFunction(Ctl3dFunc._Ctl3dColorChange);
}

BOOL WINAPI Ctl3dSubclassCtl(HWND hwnd)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
    return FALSE;
  
  return (BOOL)InvokeIndirectFunction(Ctl3dFunc._Ctl3dSubclassCtl,
				      hwnd);
}

LONG WINAPI Ctl3dDlgFramePaint(HWND hwnd, UINT ui, WPARAM wp, LPARAM lp)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
    return FALSE;
  
  return (LONG)InvokeIndirectFunction(Ctl3dFunc._Ctl3dDlgFramePaint,
				      hwnd, ui, wp, lp);
}

BOOL WINAPI Ctl3dAutoSubclass(HANDLE hnd)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
    return FALSE;
  
  return (BOOL)InvokeIndirectFunction(Ctl3dFunc._Ctl3dAutoSubclass,
				      hnd);
}

BOOL WINAPI Ctl3dRegister(HANDLE hnd)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
    return FALSE;
  
  return (BOOL)InvokeIndirectFunction(Ctl3dFunc._Ctl3dRegister,
				      hnd);
}

BOOL WINAPI Ctl3dUnregister(HANDLE hnd)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
    return FALSE;
  
  return (BOOL)InvokeIndirectFunction(Ctl3dFunc._Ctl3dUnregister,
				      hnd);
}

VOID WINAPI Ctl3dWinIniChange(void)
{
  if (!Ctl3dFunc.dll)
    if (!load_functions(&Ctl3dFunc))
      return;
  
  InvokeIndirectFunction(Ctl3dFunc._Ctl3dWinIniChange);
}

#ifdef __cplusplus
}
#endif

#endif	// __WINDOWS_386__
