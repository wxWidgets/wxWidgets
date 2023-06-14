/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/toolbar.h
// Purpose:     GTK toolbar
// Author:      Robert Roebling
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_TOOLBAR_H_
#define _WX_GTK_TOOLBAR_H_

typedef struct _GtkTooltips GtkTooltips;

// ----------------------------------------------------------------------------
// wxToolBar
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToolBar : public wxToolBarBase
{
public:
    // construction/destruction
    wxToolBar() { Init(); }
    wxToolBar( wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTB_DEFAULT_STYLE,
               const wxString& name = wxASCII_STR(wxToolBarNameStr) )
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxTB_DEFAULT_STYLE,
                 const wxString& name = wxASCII_STR(wxToolBarNameStr) );

    virtual ~wxToolBar();

    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const override;

    virtual void SetToolShortHelp(int id, const wxString& helpString) override;

    virtual void SetWindowStyleFlag( long style ) override;

    virtual void SetToolNormalBitmap(int id, const wxBitmapBundle& bitmap) override;
    virtual void SetToolDisabledBitmap(int id, const wxBitmapBundle& bitmap) override;

    virtual bool Realize() override;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxString& label,
                                          const wxBitmapBundle& bitmap1,
                                          const wxBitmapBundle& bitmap2 = wxNullBitmap,
                                          wxItemKind kind = wxITEM_NORMAL,
                                          wxObject *clientData = nullptr,
                                          const wxString& shortHelpString = wxEmptyString,
                                          const wxString& longHelpString = wxEmptyString) override;
    virtual wxToolBarToolBase *CreateTool(wxControl *control,
                                          const wxString& label) override;

    // implementation from now on
    // --------------------------

    GtkToolbar* GTKGetToolbar() const { return m_toolbar; }

protected:
    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_DEFAULT; }

    virtual wxSize DoGetBestSize() const override;
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const override;

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool) override;
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool) override;

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable) override;
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle) override;
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle) override;

private:
    void Init();
    void GtkSetStyle();
    GSList* GetRadioGroup(size_t pos);
    virtual void AddChildGTK(wxWindowGTK* child) override;

    GtkToolbar* m_toolbar;
    GtkTooltips* m_tooltips;

    wxDECLARE_DYNAMIC_CLASS(wxToolBar);
};

#endif
    // _WX_GTK_TOOLBAR_H_
