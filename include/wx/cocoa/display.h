/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/cocoa/display.h
// Purpose:     wxDisplay class for wxCocoa
// Author:      Ryan Norton
// Modified by: 
// Created:     2004-10-03
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_DISPLAY_H_
#define _WX_COCOA_DISPLAY_H_

#include "wx/object.h"
#include "wx/display.h"

class wxRect;
class wxString;

class WXDLLEXPORT wxDisplay : public wxDisplayBase
{
public:
    wxDisplay ( size_t index = 0 );

    ~wxDisplay();

    virtual wxRect GetGeometry() const;
    virtual int GetDepth() const;
    virtual wxString GetName() const;

    virtual wxArrayVideoModes
        GetModes(const wxVideoMode& mode = wxDefaultVideoMode) const;

    virtual wxVideoMode GetCurrentMode() const;

    virtual bool ChangeMode(const wxVideoMode& mode = wxDefaultVideoMode);

private:
    struct _CGDirectDisplayID * m_id;

    DECLARE_NO_COPY_CLASS(wxDisplay)
};

#endif // _WX_COCOA_DISPLAY_H_
