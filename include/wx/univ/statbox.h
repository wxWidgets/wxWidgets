//////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/statbox.h
// Purpose:     wxStaticBox declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_STATBOX_H_
#define _WX_UNIV_STATBOX_H_

#ifdef __GNUG__
    #pragma interface "univstatbox.h"
#endif

class WXDLLEXPORT wxStaticBox : public wxStaticBoxBase
{
public:
    wxStaticBox() { }

    wxStaticBox(wxWindow *parent,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize)
    {
        Create(parent, -1, label, pos, size);
    }

    wxStaticBox(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBoxNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBoxNameStr);

    // the origin of the static box is inside the border and under the label:
    // take account of this
    virtual wxPoint GetBoxAreaOrigin() const;

protected:
    // draw the control
    virtual void DoDraw(wxControlRenderer *renderer);

    // get the size of the border
    wxRect GetBorderGeometry() const;

private:
    DECLARE_DYNAMIC_CLASS(wxStaticBox)
};

#endif // _WX_UNIV_STATBOX_H_
