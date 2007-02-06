/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/toolbar.h
// Purpose:     wxToolBar class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBAR_H_
#define _WX_TOOLBAR_H_

#if wxUSE_TOOLBAR

#include "wx/tbarbase.h"
#include "wx/dynarray.h"

WXDLLEXPORT_DATA(extern const wxChar) wxToolBarNameStr[];

class WXDLLEXPORT wxToolBar: public wxToolBarBase
{
  DECLARE_DYNAMIC_CLASS(wxToolBar)
 public:
  /*
   * Public interface
   */

   wxToolBar() { Init(); }

  inline wxToolBar(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                   long style = wxNO_BORDER|wxTB_HORIZONTAL,
                   const wxString& name = wxToolBarNameStr)
  {
    Init();
    Create(parent, id, pos, size, style, name);
  }
  virtual ~wxToolBar();

  bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER|wxTB_HORIZONTAL,
            const wxString& name = wxToolBarNameStr);

    virtual void SetWindowStyleFlag(long style);

    // override/implement base class virtuals
    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const;

    virtual bool Show(bool show = true);
    virtual bool IsShown() const;
    virtual bool Realize();

    virtual void SetToolBitmapSize(const wxSize& size);
    virtual wxSize GetToolSize() const;

    virtual void SetRows(int nRows);

    virtual void SetToolNormalBitmap(int id, const wxBitmap& bitmap);
    virtual void SetToolDisabledBitmap(int id, const wxBitmap& bitmap);

    // Add all the buttons

    virtual wxString MacGetToolTipString( wxPoint &where ) ;
    void OnPaint(wxPaintEvent& event) ;
    void OnMouse(wxMouseEvent& event) ;
    virtual void MacSuperChangedPosition() ;

#if wxMAC_USE_NATIVE_TOOLBAR
    bool MacInstallNativeToolbar(bool usesNative);
    bool MacWantsNativeToolbar();
    bool MacTopLevelHasNativeToolbar(bool *ownToolbarInstalled) const;
#endif
protected:
    // common part of all ctors
    void Init();

    virtual void DoGetSize(int *width, int *height) const;
    virtual wxSize DoGetBestSize() const;
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxString& label,
                                          const wxBitmap& bmpNormal,
                                          const wxBitmap& bmpDisabled,
                                          wxItemKind kind,
                                          wxObject *clientData,
                                          const wxString& shortHelp,
                                          const wxString& longHelp);
    virtual wxToolBarToolBase *CreateTool(wxControl *control);

    DECLARE_EVENT_TABLE()
#if wxMAC_USE_NATIVE_TOOLBAR
    bool m_macUsesNativeToolbar ;
    void* m_macHIToolbarRef ;
#endif
};

#endif // wxUSE_TOOLBAR

#endif
    // _WX_TOOLBAR_H_
