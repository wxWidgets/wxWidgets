#ifndef __WX_TOPLEVEL_H__
#define __WX_TOPLEVEL_H__

class WXDLLEXPORT wxMenuItem; //HACK for wx/frame.h, why isn't this line included there?

class wxTopLevelWindowWeb : public wxTopLevelWindowBase {
public:
    wxTopLevelWindowWeb();
    wxTopLevelWindowWeb(wxWindow *parent,
                            wxWindowID id,
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

    virtual ~wxTopLevelWindowWeb();

    virtual void Maximize(bool maximize = true);
    virtual bool IsMaximized() const;
    virtual void Iconize(bool iconize = true);
    virtual bool IsIconized() const;
    virtual void Restore();

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);
    virtual bool IsFullScreen() const;

    virtual bool Show(bool show = true);

    virtual void SetTitle(const wxString &title);
    virtual wxString GetTitle() const;
};

#endif // __WX_TOPLEVEL_H__
