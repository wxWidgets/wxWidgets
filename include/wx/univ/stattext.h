/////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/stattext.h
// Purpose:     wxStaticText
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_STATTEXT_H_
#define _WX_UNIV_STATTEXT_H_

#ifdef __GNUG__
    #pragma interface "univstattext.h"
#endif

class WXDLLEXPORT wxStaticText : public wxStaticTextBase
{
public:
    // usual ctor
    wxStaticText(wxWindow *parent,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize)
    {
        Create(parent, -1, label, pos, size, 0, wxStaticTextNameStr);
    }

    // full form
    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize, 
                 long style = 0,
                 const wxString &name = wxStaticTextNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }

    // function ctor
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &label,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize, 
                long style = 0,
                const wxString &name = wxStaticTextNameStr);

    // implementation only from now on

    virtual void SetLabel(const wxString& label);

protected:
    // calculate the optimal size for the label
    virtual wxSize DoGetBestClientSize() const;

    // draw the control
    virtual void DoDraw(wxControlRenderer *renderer);
};

#endif // _WX_UNIV_STATTEXT_H_
