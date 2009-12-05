/////////////////////////////////////////////////////////////////////////////
// Name:        my_dll.h
// Purpose:     Sample showing how to use wx from a DLL
// Author:      Vaclav Slavik
// Created:     2009-12-03
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _MY_DLL_H_
#define _MY_DLL_H_

#ifdef __cplusplus
extern "C" {
#endif

// launch wx UI from some application that may or may not be written in wx
WXIMPORT void run_wx_gui_from_dll(const char *title);

// run this to shutdown running threads etc.
WXIMPORT void wx_dll_cleanup(void);


#ifdef __cplusplus
}
#endif

#endif // _MY_DLL_H_
