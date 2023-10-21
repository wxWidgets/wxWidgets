///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/ownerdrw.h
// Purpose:     wxOwnerDrawn class
// Author:      Marcin Malich
// Created:     2009-09-22
// Copyright:   (c) 2009 Marcin Malich <me@malcom.pl>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OWNERDRW_H_
#define _WX_OWNERDRW_H_

#if wxUSE_OWNER_DRAWN

class WXDLLIMPEXP_CORE wxOwnerDrawn : public wxOwnerDrawnBase
{
public:
    wxOwnerDrawn() {}
    virtual ~wxOwnerDrawn() {}

    virtual bool OnDrawItem(wxDC& dc, const wxRect& rc,
                            wxODAction act, wxODStatus stat) override;

protected:
    // get the type of the text to draw in OnDrawItem(), by default is
    // DST_PREFIXTEXT but can be overridden to return DST_TEXT when not using
    // mnemonics
    virtual int MSWGetTextType() const;
};

#endif // wxUSE_OWNER_DRAWN

#endif // _WX_OWNERDRW_H_
