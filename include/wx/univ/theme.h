///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/theme.h
// Purpose:     wxTheme class manages all configurable aspects of the
//              application including the look (wxRenderer), feel
//              (wxInputHandler) and the colours (wxColourScheme)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_THEME_H_
#define _WX_UNIX_THEME_H_

// ----------------------------------------------------------------------------
// wxTheme
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRenderer;
class WXDLLEXPORT wxInputHandler;
class WXDLLEXPORT wxColourScheme;

class WXDLLEXPORT wxTheme
{
public:
    virtual wxRenderer *GetRenderer() = 0;
    virtual wxInputHandler *GetInputHandler() = 0;
    virtual wxColourScheme *GetColourScheme() = 0;
};

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

extern WXDLLEXPORT_DATA(wxTheme *) wxTheTheme;

#endif // _WX_UNIX_THEME_H_

