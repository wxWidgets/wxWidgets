/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     wxControl class
// Author:      David Webster
// Modified by:
// Created:     09/17/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

#include "wx/dynarray.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxControlNameStr;

// General item class
class WXDLLEXPORT wxControl : public wxControlBase
{
    DECLARE_ABSTRACT_CLASS(wxControl)

public:
   wxControl();
   wxControl( wxWindow*          pParent
             ,wxWindowID         vId
             ,const wxPoint&     rPos = wxDefaultPosition
             ,const wxSize&      rSize = wxDefaultSize
             ,long               lStyle = 0
#if wxUSE_VALIDATORS
             ,const wxValidator& rValidator = wxDefaultValidator
#endif
             ,const wxString&    rsName = wxControlNameStr
            )
    {
        Create( pParent
               ,vId
               ,rPos
               ,rSize
               ,lStyle
               ,rValidator
               ,rsName
              );
    }
    virtual ~wxControl();

    bool Create( wxWindow*          pParent
                ,wxWindowID         vId
                ,const wxPoint&     rPos = wxDefaultPosition
                ,const wxSize&      rSize = wxDefaultSize
                ,long               lStyle = 0
#if wxUSE_VALIDATORS
                ,const wxValidator& rValidator = wxDefaultValidator
#endif
                ,const wxString&    rsName = wxControlNameStr
               );

    //
    // Simulates an event
    //
    virtual void Command(wxCommandEvent& rEvent) { ProcessCommand(rEvent); }

    //
    // Implementation from now on
    // --------------------------
    //

    //
    // Calls the callback and appropriate event handlers
    //
    bool ProcessCommand(wxCommandEvent& rEvent);

    //
    // For ownerdraw items
    //
    virtual bool OS2OnDraw(WXDRAWITEMSTRUCT* WXUNUSED(pItem)) { return FALSE; };
    virtual bool OS2OnMeasure(WXMEASUREITEMSTRUCT* WXUNUSED(pItem)) { return FALSE; };

    wxArrayLong&     GetSubcontrols() { return m_aSubControls; }
    void             OnEraseBackground(wxEraseEvent& rEvent);
    virtual WXHBRUSH OnCtlColor( WXHDC    hDC
                                ,WXHWND   pWnd
                                ,WXUINT   nCtlColor
                                ,WXUINT   uMessage
                                ,WXWPARAM wParam
                                ,WXLPARAM lParam
                               );

#if WXWIN_COMPATIBILITY
    virtual void SetButtonColour(const wxColour& WXUNUSED(rCol)) { }
    wxColour*    GetButtonColour(void) const { return NULL; }

    inline virtual void SetLabelFont(const wxFont& rFont);
    inline virtual void SetButtonFont(const wxFont& rFont);
    inline wxFont&      GetLabelFont(void) const;
    inline wxFont&      GetButtonFont(void) const;

    //
    // Adds callback
    //
    inline void Callback(const wxFunction function);
    wxFunction  GetCallback(void) { return m_callback; }

protected:
    wxFunction                      m_callback;     // Callback associated with the window
#endif // WXWIN_COMPATIBILITY

protected:
    //
    // For controls like radiobuttons which are really composite
    //
    wxArrayLong m_aSubControls;

    virtual wxSize DoGetBestSize(void) const;

    //
    // Create the control of the given class with the given style, returns FALSE
    // if creation failed.
    //
    bool OS2CreateControl( const wxChar*   zClassname
                          ,WXDWORD         dwStyle
                          ,const wxPoint&  rPos = wxDefaultPosition
                          ,const wxSize&   rSize = wxDefaultSize
                          ,const wxString& rsLabel = wxEmptyString
                          ,WXDWORD         dwExstyle = (WXDWORD)-1
                         );

    //
    // Determine the extended styles combination for this window (may slightly
    // modify styl parameter)
    //
    WXDWORD GetExStyle(WXDWORD& rStyle) const;

private:
   DECLARE_EVENT_TABLE()
}; // end of wxControl

#if WXWIN_COMPATIBILITY
    inline void wxControl::Callback(const wxFunction f) { m_callback = f; };
    inline wxFont& wxControl::GetLabelFont(void) const { return GetFont(); }
    inline wxFont& wxControl::GetButtonFont(void) const { return GetFont(); }
    inline void wxControl::SetLabelFont(const wxFont& rFont) { SetFont(rFont); }
    inline void wxControl::SetButtonFont(const wxFont& rFont) { SetFont(rFont); }
#endif // WXWIN_COMPATIBILITY

#endif // _WX_CONTROL_H_

