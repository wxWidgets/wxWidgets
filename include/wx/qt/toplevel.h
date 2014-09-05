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
    
    virtual void Maximize(bool maximize = true);
    virtual void Restore();
    virtual void Iconize(bool iconize = true);
    virtual bool IsMaximized() const;
    virtual bool IsIconized() const;

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);
    virtual bool IsFullScreen() const;
    virtual void SetTitle(const wxString& title);
    virtual wxString GetTitle() const;
    virtual void SetIcons(const wxIconBundle& icons);
    
    // Styles
    virtual void SetWindowStyleFlag( long style );
    virtual long GetWindowStyleFlag() const;
};
    
#endif // _WX_QT_TOPLEVEL_H_
