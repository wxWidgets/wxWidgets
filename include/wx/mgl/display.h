/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mgl/display.h
// Purpose:     wxDisplay class customization for WXMGL
// Author:      Wlodzimierz ABX Skiba
// Modified by:
// Created:     05/03/2006
// RCS-ID:      $Id$
// Copyright:   (c) Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MGL_DISPLAY_H_
#define _WX_MGL_DISPLAY_H_

class WXDLLEXPORT wxDisplay : public wxDisplayBase
{
public:

    // create the display object for the given physical display
    wxDisplay(size_t index = 0);

    virtual ~wxDisplay();

    // implement base class pure virtuals
    virtual bool IsOk() const;
    virtual wxRect GetGeometry() const;
    virtual wxString GetName() const;

    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode = wxDefaultVideoMode) const;
    virtual wxVideoMode GetCurrentMode() const;
    virtual bool ChangeMode(const wxVideoMode& mode = wxDefaultVideoMode);

    virtual bool IsPrimary() const;

private:

    DECLARE_NO_COPY_CLASS(wxDisplay)
};

#endif // _WX_MGL_DISPLAY_H_
