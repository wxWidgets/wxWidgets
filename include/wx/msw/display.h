/////////////////////////////////////////////////////////////////////////////
// Name:        display.h
// Purpose:     wxDisplay class customization for WXMSW
// Author:      Royce Mitchell III
// Modified by: 
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "display.h"
#endif

#ifndef _WX_MSW_DISPLAY_H_
#define _WX_MSW_DISPLAY_H_

class WXDLLEXPORT wxDisplay : public wxDisplayBase
{
public:
    wxDisplay(size_t index = 0);

    // implement base class pure virtuals
    virtual wxRect GetGeometry() const;
    virtual wxString GetName() const;

    virtual wxArrayVideoModes
        GetModes(const wxVideoMode& mode = wxVideoMode()) const;
    virtual wxVideoMode GetCurrentMode() const;
    virtual bool ChangeMode(const wxVideoMode& mode = wxVideoMode());

private:
    // get the display name to use with EnumDisplaySettings()
    wxString GetNameForEnumSettings() const;

    DECLARE_NO_COPY_CLASS(wxDisplay);
};

#endif // _WX_MSW_DISPLAY_H_
