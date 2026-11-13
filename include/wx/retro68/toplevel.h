///////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/toplevel.h
// Purpose:     wxTopLevelWindowRetro68 class for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_TOPLEVEL_H_
#define _WX_RETRO68_TOPLEVEL_H_

//-----------------------------------------------------------------------------
// wxTopLevelWindowRetro68
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTopLevelWindowRetro68 : public wxTopLevelWindowBase
{
public:
    // construction
    wxTopLevelWindowRetro68() { Init(); }
    wxTopLevelWindowRetro68(wxWindow *parent,
                            wxWindowID id,
                            const wxString& title,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxDEFAULT_FRAME_STYLE,
                            const wxString& name = wxASCII_STR(wxFrameNameStr))
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    virtual ~wxTopLevelWindowRetro68();

    // implement base class pure virtuals
    virtual void Maximize(bool maximize = true) override;
    virtual bool IsMaximized() const override;
    virtual void Iconize(bool iconize = true) override;
    virtual bool IsIconized() const override;
    virtual void Restore() override;

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL) override;
    virtual bool IsFullScreen() const override { return m_fsIsShowing; }

    virtual void SetTitle(const wxString &title) override;
    virtual wxString GetTitle() const override { return m_title; }

    virtual bool IsActive() override;

    // implementation
    virtual void HandleActivateEvent(bool activate) override;

protected:
    void Init();

    wxString      m_title;

    bool          m_fsIsShowing;    // full screen
    wxRect        m_fsSaveFrame;

    bool          m_isMaximized;
    wxRect        m_savedFrame;
};

#endif // _WX_RETRO68_TOPLEVEL_H_
