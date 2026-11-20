/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/toolbar.h
// Author:      Sean D'Epagnier, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_QT_TOOLBAR_H_
#define _WX_QT_TOOLBAR_H_

class QActionGroup;
class QToolBar;

class wxQtToolButton;

class WXDLLIMPEXP_CORE wxToolBar : public wxToolBarBase
{
public:

    wxToolBar() = default;

    wxToolBar(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTB_DEFAULT_STYLE | wxNO_BORDER,
              const wxString& name = wxASCII_STR(wxToolBarNameStr))
    {
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxToolBar();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTB_DEFAULT_STYLE | wxNO_BORDER,
                const wxString& name = wxASCII_STR(wxToolBarNameStr));

    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const override;

    virtual void SetWindowStyleFlag( long style ) override;

    virtual void SetToolShortHelp(int id, const wxString& helpString) override;
    virtual void SetToolNormalBitmap(int id, const wxBitmapBundle& bitmap) override;
    virtual void SetToolDisabledBitmap(int id, const wxBitmapBundle& bitmap) override;

    virtual bool Realize() override;

    virtual wxToolBarToolBase *CreateTool(int toolid,
                                          const wxString& label,
                                          const wxBitmapBundle& bmpNormal,
                                          const wxBitmapBundle& bmpDisabled = wxNullBitmap,
                                          wxItemKind kind = wxITEM_NORMAL,
                                          wxObject *clientData = nullptr,
                                          const wxString& shortHelp = wxEmptyString,
                                          const wxString& longHelp = wxEmptyString) override;

    virtual wxToolBarToolBase *CreateTool(wxControl *control,
                                          const wxString& label) override;

    QToolBar* GetQToolBar() const;

protected:
    QActionGroup* GetActionGroup(size_t pos);
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool) override;
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool) override;
    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable) override;
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle) override;
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle) override;

    virtual void DoSetToolBitmapSize(const wxSize& size) override;

private:
    long GetButtonStyle();

    wxDECLARE_DYNAMIC_CLASS(wxToolBar);
};

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar, int id, const wxString& label, const wxBitmapBundle& bitmap1,
                  const wxBitmapBundle& bitmap2, wxItemKind kind, wxObject *clientData,
                  const wxString& shortHelpString, const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, label, bitmap1, bitmap2, kind,
                            clientData, shortHelpString, longHelpString)
    {
        m_qtToolButton = nullptr;
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control, const wxString& label)
        : wxToolBarToolBase(tbar, control, label)
    {
        m_qtToolButton = nullptr;
    }

    virtual void SetLabel( const wxString &label ) override;
    virtual void SetDropdownMenu(wxMenu* menu) override;

private:
    void SetIcon();
    void ClearToolTip();
    void SetToolTip();

    wxQtToolButton* m_qtToolButton;

    friend class wxToolBar;
};

#endif // _WX_QT_TOOLBAR_H_
