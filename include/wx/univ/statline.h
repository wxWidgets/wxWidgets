/////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/statline.h
// Purpose:     wxStaticLine class for wxUniversal
// Author:      Vadim Zeitlin
// Created:     28.06.99
// Version:     $Id$
// Copyright:   (c) 1999 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "univstatline.h"
#endif

#ifndef _WX_UNIV_STATLINE_H_
#define _WX_UNIV_STATLINE_H_

class WXDLLEXPORT wxStaticLine : public wxStaticLineBase
{
public:
    // constructors and pseudo-constructors
    wxStaticLine() { }

    wxStaticLine(wxWindow *parent,
                 const wxPoint &pos,
                 wxCoord length,
                 long style = wxLI_HORIZONTAL)
    {
        Create(parent, -1, pos,
               style & wxLI_VERTICAL ? wxSize(-1, length)
                                     : wxSize(length, -1),
               style);
    }

    wxStaticLine(wxWindow *parent,
                 wxWindowID id,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = wxLI_HORIZONTAL,
                 const wxString &name = wxStaticTextNameStr )
    {
        Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = wxLI_HORIZONTAL,
                const wxString &name = wxStaticTextNameStr );

protected:
    virtual void DoDraw(wxControlRenderer *renderer);

private:
    DECLARE_DYNAMIC_CLASS(wxStaticLine)
};

#endif // _WX_UNIV_STATLINE_H_

