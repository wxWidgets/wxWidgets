/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/toolbar.h
// Purpose:     wxToolBar class
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBAR_H_
#define _WX_TOOLBAR_H_

#if wxUSE_TOOLBAR

#include "wx/tbarbase.h"
#include "wx/dynarray.h"

class WXDLLIMPEXP_CORE wxToolBar: public wxToolBarBase
{
    wxDECLARE_DYNAMIC_CLASS(wxToolBar);
public:
  /*
   * Public interface
   */

   wxToolBar() { Init(); }

  wxToolBar(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                   long style = wxTB_DEFAULT_STYLE,
                   const wxString& name = wxASCII_STR(wxToolBarNameStr))
  {
    Init();
    Create(parent, id, pos, size, style, name);
  }
  virtual ~wxToolBar();

  bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxTB_DEFAULT_STYLE,
            const wxString& name = wxASCII_STR(wxToolBarNameStr));

    virtual void SetWindowStyleFlag(long style) override;

    virtual bool Destroy() override;

    // override/implement base class virtuals
    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const override;

#ifndef __WXOSX_IPHONE__
    virtual bool Show(bool show = true) override;
    virtual bool IsShown() const override;
#endif
    virtual bool Realize() override;

    virtual wxSize GetToolSize() const override;

    virtual void SetRows(int nRows) override;


    virtual void SetToolNormalBitmap(int id, const wxBitmapBundle& bitmap) override;
    virtual void SetToolDisabledBitmap(int id, const wxBitmapBundle& bitmap) override;

#ifndef __WXOSX_IPHONE__
    // Add all the buttons

    virtual wxString MacGetToolTipString( wxPoint &where ) override;
    void OnPaint(wxPaintEvent& event) ;
    void OnMouse(wxMouseEvent& event) ;
    virtual void MacSuperChangedPosition() override;
#endif

#if wxOSX_USE_NATIVE_TOOLBAR
    // make all tools selectable
    void OSXSetSelectableTools(bool set);
    void OSXSelectTool(int toolId);

    bool MacInstallNativeToolbar(bool usesNative);
    void MacUninstallNativeToolbar();
    bool MacWantsNativeToolbar();
    bool MacTopLevelHasNativeToolbar(bool *ownToolbarInstalled) const;
#endif

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxString& label,
                                          const wxBitmapBundle& bmpNormal,
                                          const wxBitmapBundle& bmpDisabled = wxNullBitmap,
                                          wxItemKind kind = wxITEM_NORMAL,
                                          wxObject *clientData = nullptr,
                                          const wxString& shortHelp = wxEmptyString,
                                          const wxString& longHelp = wxEmptyString) override;
    virtual wxToolBarToolBase *CreateTool(wxControl *control,
                                          const wxString& label) override;

protected:
    // common part of all ctors
    void Init();

    void DoLayout();

    void DoSetSize(int x, int y, int width, int height, int sizeFlags) override;

#ifndef __WXOSX_IPHONE__
    virtual void DoGetSize(int *width, int *height) const override;
    virtual wxSize DoGetBestSize() const override;
#endif
#ifdef __WXOSX_COCOA__
    virtual void DoGetPosition(int*x, int *y) const override;
#endif
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool) override;
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool) override;

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable) override;
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle) override;
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle) override;

    virtual void DoSetToolBitmapSize(const wxSize& size) override;

    wxDECLARE_EVENT_TABLE();
#if wxOSX_USE_NATIVE_TOOLBAR
    bool m_macUsesNativeToolbar ;
    void* m_macToolbar ;
#endif
#ifdef __WXOSX_IPHONE__
    WX_UIView m_macToolbar;
#endif

private:
#if wxOSX_USE_NATIVE_TOOLBAR
    wxString FormatToolId(const wxToolBarToolBase *tool) const;
#endif
};

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

#ifdef __WXOSX_COCOA__

#if wxOSX_USE_NATIVE_TOOLBAR
DECLARE_WXCOCOA_OBJC_CLASS(NSToolbarItem);
#endif

class WXDLLIMPEXP_CORE wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(
        wxToolBar *tbar,
        int id,
        const wxString& label,
        const wxBitmapBundle& bmpNormal,
        const wxBitmapBundle& bmpDisabled,
        wxItemKind kind,
        wxObject *clientData,
        const wxString& shortHelp,
        const wxString& longHelp );

    wxToolBarTool(wxToolBar *tbar, wxControl *control, const wxString& label)
        : wxToolBarToolBase(tbar, control, label)
    {
        if (control != nullptr)
            SetControlHandle( (WXWidget) control->GetHandle() );
    }

    virtual ~wxToolBarTool()
    {
        ClearControl();
    }

    bool Enable( bool enable ) override;

#if wxOSX_USE_NATIVE_TOOLBAR
    virtual void SetLabel(const wxString& label) override
    {
        wxToolBarToolBase::SetLabel(label);
        UpdateLabel();
    }

    virtual bool SetShortHelp(const wxString& help) override
    {
        if ( !wxToolBarToolBase::SetShortHelp(help) )
            return false;

        UpdateLabel();

        return true;
    }

    WX_NSToolbarItem GetToolbarItemRef() const
    {
        return m_toolbarItem;
    }
#endif // wxOSX_USE_NATIVE_TOOLBAR

private:
    WXWidget GetControlHandle()
    {
        return (WXWidget) m_controlHandle;
    }

    void SetControlHandle( WXWidget handle )
    {
        m_controlHandle = handle;
    }

    void SetPosition( const wxPoint& position );

    void ClearControl();

    wxSize GetSize() const;

    wxPoint GetPosition() const
    {
        return wxPoint( m_x, m_y );
    }

    void UpdateImages();

    void UpdateToggleImage( bool toggle );

    void UpdateLabel();

#if wxOSX_USE_NATIVE_TOOLBAR
    void SetToolbarItemRef( WX_NSToolbarItem ref );

    void SetIndex( long idx )
    {
        m_index = idx;
    }

    long GetIndex() const
    {
        return m_index;
    }
#endif // wxOSX_USE_NATIVE_TOOLBAR

    WXWidget m_controlHandle = nullptr;
    wxCoord     m_x;
    wxCoord     m_y;
    wxBitmap    m_alternateBitmap;

#if wxOSX_USE_NATIVE_TOOLBAR
    WX_NSToolbarItem m_toolbarItem = nullptr;
    // position in its toolbar, -1 means not inserted
    long m_index = -1;
#endif

    friend class wxToolBar;

private:
    wxDECLARE_CLASS(wxToolBarTool);
};

#elif defined(__WXOSX_IPHONE__)

DECLARE_WXCOCOA_OBJC_CLASS(UIBarButtonItem);

class WXDLLIMPEXP_CORE wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(
                  wxToolBar *tbar,
                  int id,
                  const wxString& label,
                  const wxBitmapBundle& bmpNormal,
                  const wxBitmapBundle& bmpDisabled,
                  wxItemKind kind,
                  wxObject *clientData,
                  const wxString& shortHelp,
                  const wxString& longHelp );

    wxToolBarTool(wxToolBar *tbar, wxControl *control, const wxString& label);

    virtual ~wxToolBarTool();

    void MakeStretchable() override;

private:
    WX_UIBarButtonItem GetUIBarButtonItem() const {return m_toolbarItem;}

    void Init()
    {
        m_toolbarItem = nullptr;
        m_index = -1;
    }

    void RemoveAssociation();

    WX_UIBarButtonItem m_toolbarItem;
    // position in its toolbar, -1 means not inserted
    long m_index;

    friend class wxToolBar;

private:
    wxDECLARE_CLASS(wxToolBarTool);
};

#endif // __WXOSX_COCOA__/__WXOSX_IPHONE__

#endif // wxUSE_TOOLBAR

#endif
    // _WX_TOOLBAR_H_
