/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.h
// Purpose:     wxRadioButton class
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_
#define _WX_RADIOBUT_H_

#include "wx/control.h"

class WXDLLEXPORT wxRadioButton: public wxControl
{
public:
    inline wxRadioButton() {}
    inline wxRadioButton( wxWindow*          pParent
                         ,wxWindowID         vId
                         ,const wxString&    rsLabel
                         ,const wxPoint&     rPos = wxDefaultPosition
                         ,const wxSize&      rSize = wxDefaultSize
                         ,long               lStyle = 0
#if wxUSE_VALIDATORS
                         ,const wxValidator& rValidator = wxDefaultValidator
#endif
                         ,const wxString&    rsName = wxRadioButtonNameStr
                         )
    {
        Create( pParent
               ,vId
               ,rsLabel
               ,rPos
               ,rSize
               ,lStyle
#if wxUSE_VALIDATORS
               ,rValidator
#endif
               ,rsName
              );
    }

    bool Create( wxWindow* pParent
                ,wxWindowID         vId
                ,const wxString&    rsLabel
                ,const wxPoint&     rPos = wxDefaultPosition
                ,const wxSize&      rSize = wxDefaultSize
                ,long               lStyle = 0
#if wxUSE_VALIDATORS
                ,const wxValidator& rValidator = wxDefaultValidator
#endif
                ,const wxString&    rsName = wxRadioButtonNameStr
               );
    virtual void SetLabel(const wxString& rsLabel);
    virtual void SetValue(bool bVal);
    virtual bool GetValue(void) const ;

    bool OS2Command( WXUINT wParam
                    ,WXWORD wId
                   );
    void Command(wxCommandEvent& rEvent);
private:
    DECLARE_DYNAMIC_CLASS(wxRadioButton)
}; // end of wxRadioButton

#endif
    // _WX_RADIOBUT_H_
