/////////////////////////////////////////////////////////////////////////////
// Name:        mac/statline.h
// Purpose:     a generic wxStaticLine class used for mac before adaptation
// Author:      Vadim Zeitlin
// Created:     28.06.99
// Version:     $Id$
// Copyright:   (c) 1998 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_STATLINE_H_
#define _WX_GENERIC_STATLINE_H_

#ifdef __GNUG__
    #pragma interface
#endif

class wxStaticBox;

// ----------------------------------------------------------------------------
// wxStaticLine
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStaticLine : public wxStaticLineBase
{
    DECLARE_DYNAMIC_CLASS(wxStaticLine)

public:
    // constructors and pseudo-constructors
    wxStaticLine() { }

    wxStaticLine( wxWindow *parent,
                  wxWindowID id,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const wxString &name = wxStaticTextNameStr )
    {
        Create(parent, id, pos, size, style, name);
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = wxLI_HORIZONTAL,
                 const wxString &name = wxStaticTextNameStr );

    // it's necessary to override this wxWindow function because we
    // will want to return the main widget for m_statbox
    //
    WXWidget GetMainWidget() const;
        
protected:
    // we implement the static line using a static box
    wxStaticBox *m_statbox;
};

#endif // _WX_GENERIC_STATLINE_H_

