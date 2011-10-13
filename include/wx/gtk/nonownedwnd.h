///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/nonownedwnd.h
// Purpose:     wxGTK-specific wxNonOwnedWindow declaration.
// Author:      Vadim Zeitlin
// Created:     2011-10-12
// RCS-ID:      $Id: wxhead.h,v 1.12 2010-04-22 12:44:51 zeitlin Exp $
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_NONOWNEDWND_H_
#define _WX_GTK_NONOWNEDWND_H_

// ----------------------------------------------------------------------------
// wxNonOwnedWindow contains code common to wx{Popup,TopLevel}Window in wxGTK.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNonOwnedWindow : public wxNonOwnedWindowBase
{
public:
    wxNonOwnedWindow() { }

    virtual bool SetShape(const wxRegion& region);

    // Overridden to actually set the shape when the window becomes realized.
    virtual void GTKHandleRealized();

private:
    // If valid, defines the custom shape of the window.
    wxRegion m_shape;

    wxDECLARE_NO_COPY_CLASS(wxNonOwnedWindow);
};

#endif // _WX_GTK_NONOWNEDWND_H_
