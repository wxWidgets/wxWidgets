/////////////////////////////////////////////////////////////////////////////
// Name:        palmos/statline.h
// Purpose:     Palm OS version of wxStaticLine class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_STATLINE_H_
#define _WX_MSW_STATLINE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface
#endif

// ----------------------------------------------------------------------------
// wxStaticLine
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStaticLine : public wxStaticLineBase
{
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

    // overriden base class virtuals
    virtual bool AcceptsFocus() const { return FALSE; }

protected:
    // usually overridden base class virtuals
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxStaticLine)
};

#endif // _WX_MSW_STATLINE_H_


