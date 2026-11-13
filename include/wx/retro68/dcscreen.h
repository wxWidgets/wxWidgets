/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/dcscreen.h
// Purpose:     wxScreenDCImpl declaration
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_DCSCREEN_H_
#define _WX_RETRO68_DCSCREEN_H_

#include "wx/retro68/dc.h"

class WXDLLIMPEXP_CORE wxScreenDCImpl : public wxRetro68DCImpl
{
public:
    explicit wxScreenDCImpl(wxScreenDC *owner);

    wxDECLARE_DYNAMIC_CLASS(wxScreenDCImpl);
};

#endif // _WX_RETRO68_DCSCREEN_H_
