#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

class WXDLLEXPORT wxWindowWeb : public wxWindowBase {
public:
    wxWindowWeb();

    wxWindowWeb(wxWindow *parent,
                    wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxPanelNameStr);

    virtual ~wxWindowWeb();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr);

    virtual void SetLabel(const wxString &label);
    virtual wxString GetLabel() const;

    virtual void Raise();
    virtual void Lower();

    virtual bool Show(bool show = true);

    virtual void SetFocus();

    virtual bool Reparent(wxWindowBase *newParent);

    virtual void WarpPointer(int x, int y);

    virtual void Refresh(bool eraseBackground = true,
                         const wxRect *rect = (const wxRect *) NULL);
    virtual void Update();
    virtual void Clear();
    virtual void Freeze();
    virtual void Thaw();

    virtual bool SetCursor(const wxCursor &cursor);
    virtual bool SetFont(const wxFont &font);
    virtual void SetScrollbar(int orient, int pos, int thumbVisible,
                              int range, bool refresh = true);
    virtual void SetScrollPos(int orient, int pos, bool refresh = true);

    virtual int GetScrollPos(int orient) const;
    virtual int GetScrollThumb(int orient) const;
    virtual int GetScrollRange(int orient) const;
    virtual void ScrollWindow(int dx, int dy, const wxRect* rect = (wxRect *) NULL);

    virtual int GetCharHeight() const;
    virtual int GetCharWidth() const;
    virtual void GetTextExtent(const wxString& string,
                               int *x, int *y,
                               int *descent = (int *) NULL,
                               int *externalLeading = (int *) NULL,
                               const wxFont *theFont = (const wxFont *) NULL)
                               const;

    virtual WXWidget GetHandle() const;
    virtual void DoClientToScreen(int *x, int *y) const;
    virtual void DoScreenToClient(int *x, int *y) const;
    virtual void DoCaptureMouse();
    virtual void DoReleaseMouse();

    virtual void DoGetPosition(int *x, int *y) const;
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetClientSize(int *width, int * height) const;

    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags);
    virtual void DoSetClientSize(int width, int height);
    virtual void DoMoveWindow(int x, int y, int width, int height);

private:
    DECLARE_DYNAMIC_CLASS(wxWindowWeb)
    DECLARE_NO_COPY_CLASS(wxWindowWeb)
};


#endif // _WX_WINDOW_H_
