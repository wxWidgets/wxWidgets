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
               const wxString& name = wxASCII_STR(wxFrameNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    virtual bool Show(bool show = true) override;
    virtual void Maximize(bool maximize = true) override;
    virtual void Restore() override;
    virtual void Iconize(bool iconize = true) override;
    virtual bool IsMaximized() const override;
    virtual bool IsIconized() const override;

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL) override;
    virtual bool IsFullScreen() const override;
    virtual void SetTitle(const wxString& title) override;
    virtual wxString GetTitle() const override;
    virtual void SetIcons(const wxIconBundle& icons) override;

    // Styles
    virtual void SetWindowStyleFlag( long style ) override;
    virtual long GetWindowStyleFlag() const override;
};

#endif // _WX_QT_TOPLEVEL_H_
