/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/nonownedwnd.h
// Author:      Sean D'Epagnier
// Copyright:   (c) 2016 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_NONOWNEDWND_H_
#define _WX_QT_NONOWNEDWND_H_

// ----------------------------------------------------------------------------
// wxNonOwnedWindow contains code common to wx{Popup,TopLevel}Window in wxQT.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNonOwnedWindow : public wxNonOwnedWindowBase
{
public:
    wxNonOwnedWindow();

protected:
    virtual bool DoClearShape() wxOVERRIDE;
    virtual bool DoSetRegionShape(const wxRegion& region) wxOVERRIDE;
#if wxUSE_GRAPHICS_CONTEXT
    virtual bool DoSetPathShape(const wxGraphicsPath& path) wxOVERRIDE;
#endif // wxUSE_GRAPHICS_CONTEXT

    wxDECLARE_NO_COPY_CLASS(wxNonOwnedWindow);
};

#endif // _WX_QT_NONOWNEDWND_H_
