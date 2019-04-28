///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/toplevel.h
// Purpose:     declares wxTopLevelWindowNative class
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TOPLEVEL_H_
#define _WX_QT_TOPLEVEL_H_

class WXDLLIMPEXP_CORE wxTopLevelWindowQt : public wxTopLevelWindowBase
{
public:
    wxTopLevelWindowQt();
    wxTopLevelWindowQt(wxWindow *parent,
               wxWindowID winid,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxFrameNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual bool Show(bool show = true) wxOVERRIDE;
    virtual void Maximize(bool maximize = true) wxOVERRIDE;
    virtual void Restore() wxOVERRIDE;
    virtual void Iconize(bool iconize = true) wxOVERRIDE;
    virtual bool IsMaximized() const wxOVERRIDE;
    virtual bool IsIconized() const wxOVERRIDE;

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL) wxOVERRIDE;
    virtual bool IsFullScreen() const wxOVERRIDE;
    virtual void SetTitle(const wxString& title) wxOVERRIDE;
    virtual wxString GetTitle() const wxOVERRIDE;
    virtual void SetIcons(const wxIconBundle& icons) wxOVERRIDE;

    // Styles
    virtual void SetWindowStyleFlag( long style ) wxOVERRIDE;
    virtual long GetWindowStyleFlag() const wxOVERRIDE;
};

#endif // _WX_QT_TOPLEVEL_H_
