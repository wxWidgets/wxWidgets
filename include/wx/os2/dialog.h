/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.h
// Purpose:     wxDialog class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIALOG_H_
#define _WX_DIALOG_H_

#include "wx/panel.h"

WXDLLEXPORT_DATA(extern const char*) wxDialogNameStr;

//
// Dialog boxes
//
class WXDLLEXPORT wxDialog: public wxDialogBase
{
public:

  inline wxDialog() { Init(); }

  //
  // Constructor with a modal flag, but no window id - the old convention
  //
  inline wxDialog( wxWindow*       pParent
                  ,const wxString& rsTitle
                  ,bool            bModal
                  ,int             nX = -1
                  ,int             nY = -1
                  ,int             nWidth = 500
                  ,int             nHeight = 500
                  ,long            lStyle = wxDEFAULT_DIALOG_STYLE
                  ,const wxString& rsName = wxDialogNameStr
                 )
    {
        long                        lModalStyle = lStyle ? wxDIALOG_MODAL : wxDIALOG_MODELESS ;

        Create( pParent
               ,-1
               ,rsTitle
               ,wxPoint(nX, nY)
               ,wxSize(nWidth, nHeight)
               ,lStyle | lModalStyle
               ,rsName
              );
    }

    //
    // Constructor with no modal flag - the new convention.
    //
    inline wxDialog( wxWindow*       pParent
                    ,wxWindowID      vId
                    ,const wxString& rsTitle
                    ,const wxPoint&  rPos = wxDefaultPosition
                    ,const wxSize&   rSize = wxDefaultSize
                    ,long            lStyle = wxDEFAULT_DIALOG_STYLE
                    ,const wxString& rsName = wxDialogNameStr
                   )
    {
        Create( pParent
               ,vId
               ,rsTitle
               ,rPos
               ,rSize
               ,lStyle
               ,rsName
              );
    }

    bool Create( wxWindow*       pParent
                ,wxWindowID      vId
                ,const wxString& rsTitle
                ,const wxPoint&  rPos = wxDefaultPosition
                ,const wxSize&   rSize = wxDefaultSize
                ,long            lStyle = wxDEFAULT_DIALOG_STYLE
                ,const wxString& rsName = wxDialogNameStr
               );
    ~wxDialog();

    virtual bool Destroy(void);
    virtual bool Show(bool bShow);
    virtual void Iconize(bool bIconize);
    virtual bool IsIconized(void) const;

    virtual bool IsTopLevel(void) const { return TRUE; }

    void         SetModal(bool bFlag);
    virtual bool IsModal(void) const;

    //
    // For now, same as Show(TRUE) but returns return code
    //
    virtual int  ShowModal(void);
    virtual void EndModal(int nRetCode);

    //
    // Returns TRUE if we're in a modal loop
    //
    bool IsModalShowing() const;

#if WXWIN_COMPATIBILITY
    bool Iconized() const { return IsIconized(); };
#endif

    //
    // Implementation only from now on
    // -------------------------------
    //

    //
    // Event handlers
    //
    bool OnClose(void);
    void OnCharHook(wxKeyEvent& rEvent);
    void OnCloseWindow(wxCloseEvent& rEvent);

    //
    // May be called to terminate the dialog with the given return code
    //

    //
    // Standard buttons
    //
    void     OnOK(wxCommandEvent& rEvent);
    void     OnApply(wxCommandEvent& rEvent);
    void     OnCancel(wxCommandEvent& rEvent);

    //
    // Responds to colour changes
    //
    void     OnSysColourChanged(wxSysColourChangedEvent& rEvent);

    //
    // Callbacks
    //
    virtual MRESULT OS2WindowProc( WXUINT   uMessage
                                  ,WXWPARAM wParam
                                  ,WXLPARAM lParam
                                 );
protected:
    //
    // Override more base class virtuals
    //
    virtual void DoSetClientSize( int nWidth
                                 ,int nHeight
                                );
    virtual void DoGetPosition( int* pnX
                               ,int* pnY
                              ) const;
    //
    // Show modal dialog and enter modal loop
    //
    void DoShowModal(void);

    //
    // Common part of all ctors
    //
    void Init();

private:
    wxWindow*                       m_pOldFocus;

    //
    // While we are showing a modal dialog we disable the other windows using
    // this object
    //
    class wxWindowDisabler*         m_pWindowDisabler;

    DECLARE_DYNAMIC_CLASS(wxDialog)
    DECLARE_EVENT_TABLE()
}; // end of CLASS wxDialog

#endif // _WX_DIALOG_H_

