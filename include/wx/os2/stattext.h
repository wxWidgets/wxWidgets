/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.h
// Purpose:     wxStaticText class
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATTEXT_H_
#define _WX_STATTEXT_H_

#ifdef __GNUG__
#pragma interface "stattext.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxStaticTextNameStr;

class WXDLLEXPORT wxStaticText : public wxControl
{
 DECLARE_DYNAMIC_CLASS(wxStaticText)

 public:
    inline wxStaticText() { }

    inline wxStaticText( wxWindow*       pParent
                        ,wxWindowID      vId
                        ,const wxString& rsLabel
                        ,const wxPoint&  rPos = wxDefaultPosition
                        ,const wxSize&   rSize = wxDefaultSize
                        ,long            lStyle = 0L
                        ,const wxString& rsName = wxStaticTextNameStr
                       )
    {
        Create(pParent, vId, rsLabel, rPos, rSize, lStyle, rsName);
    }

    bool Create( wxWindow*       pParent
                ,wxWindowID      vId
                ,const wxString& rsLabel
                ,const wxPoint&  rPos = wxDefaultPosition
                ,const wxSize&   rSize = wxDefaultSize
                ,long            lStyle = 0L
                ,const wxString& rsName = wxStaticTextNameStr
               );

    //
    // Accessors
    //
    void SetLabel(const wxString&);
    bool SetFont(const wxFont &rFont);

    //
    // Overriden base class virtuals
    //
    virtual bool AcceptsFocus() const { return FALSE; }

    //
    // Callbacks
    //
    virtual MRESULT OS2WindowProc( WXUINT   uMsg
                                  ,WXWPARAM wParam
                                  ,WXLPARAM lParam
                                 );

protected:
    virtual wxSize DoGetBestSize(void) const;
}; // end of CLASS wxStaticText

#endif
    // _WX_STATTEXT_H_
