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
                     ,long            lStyle = wxNO_BORDER | wxTB_HORIZONTAL
                     ,const wxString& rName = wxToolBarNameStr
                    )
    {
        Init();
        Create( pParent
               ,vId
               ,rPos
               ,rSize
               ,lStyle
               ,rName
              );
    }
    virtual ~wxToolBar();

    bool Create( wxWindow*       pParent
                ,wxWindowID      vId
                ,const wxPoint&  rPos = wxDefaultPosition
                ,const wxSize&   rSize = wxDefaultSize
                ,long            lStyle = wxNO_BORDER | wxTB_HORIZONTAL
                ,const wxString& rName = wxToolBarNameStr
               );


    //
    // Override/implement base class virtuals
    //
    virtual wxToolBarToolBase* FindToolForPosition( wxCoord vX
                                                   ,wxCoord vY
                                                  ) const;
    virtual bool               Realize(void);
    virtual void               SetRows(int nRows);

    //
    // Event handlers
    //
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnKillFocus(wxFocusEvent& event);

protected:
    //
    // Common part of all ctors
    //
    void Init();

    //
    // Implement base class pure virtuals
    //
    virtual wxToolBarToolBase* DoAddTool( int id
                                         ,const wxString& label
                                         ,const wxBitmap& bitmap
                                         ,const wxBitmap& bmpDisabled
                                         ,wxItemKind kind
                                         ,const wxString& shortHelp = wxEmptyString
                                         ,const wxString& longHelp = wxEmptyString
                                         ,wxObject *clientData = NULL
                                         ,wxCoord xPos = -1
                                         ,wxCoord yPos = -1
                                        );

    virtual bool DoInsertTool( size_t             nPos
                              ,wxToolBarToolBase* pTool
                             );
    virtual bool DoDeleteTool( size_t              nPos
                              , wxToolBarToolBase* pTool
                             );

    virtual void DoEnableTool( wxToolBarToolBase* pTool
                              ,bool               bEnable
                             );
    virtual void DoToggleTool( wxToolBarToolBase* pTool
                              ,bool               bToggle
                             );
    virtual void DoSetToggle( wxToolBarToolBase* pTool
                             ,bool               bToggle
                            );

    virtual wxToolBarToolBase* CreateTool( int             vId
                                          ,const wxString& rsLabel
                                          ,const wxBitmap& rBmpNormal
                                          ,const wxBitmap& rBmpDisabled
                                          ,wxItemKind      eKind
                                          ,wxObject*       pClientData
                                          ,const wxString& rsShortHelp
                                          ,const wxString& rsLongHelp
                                         );
    virtual wxToolBarToolBase* CreateTool(wxControl* pControl);

    //
    // Helpers
    //
    void         DrawTool(wxToolBarToolBase *tool);
    virtual void DrawTool( wxDC&              rDC
                          ,wxToolBarToolBase* pTool
                         );
    virtual void SpringUpButton(int nIndex);

    int                             m_nCurrentRowsOrColumns;
    int                             m_nPressedTool;
    int                             m_nCurrentTool;
    wxCoord                         m_vLastX;
    wxCoord                         m_vLastY;
    wxCoord                         m_vMaxWidth;
    wxCoord                         m_vMaxHeight;
    wxCoord                         m_vXPos;
    wxCoord                         m_vYPos;
    wxCoord                         m_vTextX;
    wxCoord                         m_vTextY;

private:
    void LowerTool( wxToolBarToolBase* pTool
                   ,bool               bLower = TRUE
                  );
    void RaiseTool( wxToolBarToolBase* pTool
                   ,bool               bRaise = TRUE
                  );

    static bool                     m_bInitialized;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxToolBar)
};

#endif // wxUSE_TOOLBAR

#endif
    // _WX_TOOLBAR_H_
