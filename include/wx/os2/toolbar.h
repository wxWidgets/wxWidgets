/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.h
// Purpose:     wxToolBar class
// Author:      David Webster
// Modified by:
// Created:     10/17/98
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBAR_H_
#define _WX_TOOLBAR_H_

#if wxUSE_TOOLBAR
#include "wx/tbarbase.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxToolBarNameStr;

class WXDLLEXPORT wxToolBar: public wxToolBarBase
{
public:
    /*
     * Public interface
     */

    wxToolBar() { Init(); }

    inline wxToolBar( wxWindow*       pParent
                     ,wxWindowID      vId
                     ,const wxPoint&  rPos = wxDefaultPosition
                     ,const wxSize&   rSize = wxDefaultSize
                     ,long            lStyle = wxNO_BORDER|wxTB_HORIZONTAL
                     ,const wxString& rName = wxToolBarNameStr
                    )
    {
        Create( pParent
               ,vId
               ,rPos
               ,rSize
               ,lStyle
               ,rName
              );
    }
    bool Create( wxWindow*       pParent
                ,wxWindowID      vId
                ,const wxPoint&  rPos = wxDefaultPosition
                ,const wxSize&   rSize = wxDefaultSize
                ,long            lStyle = wxNO_BORDER|wxTB_HORIZONTAL
                ,const wxString& rName = wxToolBarNameStr
               );

    virtual ~wxToolBar();

    // override/implement base class virtuals
    virtual wxToolBarToolBase* FindToolForPosition( wxCoord x
                                                   ,wxCoord y
                                                  ) const;

    // The post-tool-addition call
    virtual bool Realize(void);

    virtual void SetToolBitmapSize(const wxSize& rSize);
    virtual wxSize GetToolSize(void) const;

    virtual void SetRows(int nRows);

    // IMPLEMENTATION
    virtual bool OS2Command( WXUINT uParam
                            ,WXWORD wId
                           );
    virtual bool OS2OnNotify( int       nCtrl
                             ,WXLPARAM  lParam
                             ,WXLPARAM* pResult
                            );
    void OnMouseEvent(wxMouseEvent& rEvent);
    void OnSysColourChanged(wxSysColourChangedEvent& rEvent);

protected:
    void Init(void);
    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase* CreateTool( int             vId
                                          ,const wxBitmap& rBitmap1
                                          ,const wxBitmap& rBitmap2
                                          ,bool            bToggle
                                          ,wxObject*       pClientData
                                          ,const wxString& rShortHelpString
                                          ,const wxString& rLongHelpString
                                         );
    virtual wxToolBarToolBase* CreateTool(wxControl* pControl);

    // should be called whenever the toolbar size changes
    void UpdateSize(void);

    // override WndProc to process WM_SIZE
    virtual MRESULT OS2WindowProc( WXUINT   ulMsg
                                  ,WXWPARAM wParam
                                  ,WXLPARAM lParam
                                 );

    // the big bitmap containing all bitmaps of the toolbar buttons
    WXHBITMAP                       m_hBitmap;

    // the total number of toolbar elements
    size_t                          m_nButtons;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxToolBar)
};

#endif // wxUSE_TOOLBAR

#endif
    // _WX_TOOLBAR_H_
