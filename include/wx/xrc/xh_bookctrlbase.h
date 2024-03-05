///////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_bookctrlbase.h
// Purpose:     Base class for wxBookCtrl-derived classes XRC handlers
// Author:      Vadim Zeitlin
// Created:     2022-02-23
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XRC_XH_BOOKCTRLBASE_H_
#define _WX_XRC_XH_BOOKCTRLBASE_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_BOOKCTRL

class WXDLLIMPEXP_FWD_CORE wxBookCtrlBase;

// ----------------------------------------------------------------------------
// wxBookCtrlXmlHandlerBase: base class of handlers for wxBookCtrl subclasses
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_XRC wxBookCtrlXmlHandlerBase : public wxXmlResourceHandler
{
protected:
    wxBookCtrlXmlHandlerBase();
    virtual ~wxBookCtrlXmlHandlerBase();

    // Create all pages under the current node and add them to the book control.
    //
    // This should be called instead of calling CreateChildren() directly by
    // the derived class to deal with the contents of the book control node.
    void DoCreatePages(wxBookCtrlBase* book);

    // Create a new page using the contents of the current node.
    //
    // This should be called to handle the book control page node.
    wxObject* DoCreatePage(wxBookCtrlBase* book);

    // Return true if we're parsing the book control node itself.
    bool IsInside() const { return m_isInside; }

    // This struct contains the actual page, created by DoCreatePage(), and all
    // its attributes read from wxXmlNode.
    struct PageWithAttrs
    {
        PageWithAttrs();

        // Returns bmpId if it's valid or imgId (which can still be invalid)
        // otherwise.
        int GetImageId() const;

        wxWindow* wnd;
        wxString label;
        bool selected;
        int imgId; // index in the image list
        int bmpId; // index in m_bookImages vector
    };

private:
    // This function is implemented by just calling AddPage() in the base
    // class, but can be overridden if something else is needed, as is e.g. the
    // case for wxTreebookXmlHandler.
    virtual void
    DoAddPage(wxBookCtrlBase* book, size_t n, const PageWithAttrs& page);


    // And this vector contains all the pages created so far.
    wxVector<PageWithAttrs> m_bookPages;

    // All bitmaps defined for the pages, may be empty.
    wxVector<wxBitmapBundle> m_bookImages;

    // True if we're used for parsing the contents of the book control node.
    bool m_isInside;

    wxDECLARE_NO_COPY_CLASS(wxBookCtrlXmlHandlerBase);
};

#endif // wxUSE_XRC && wxUSE_BOOKCTRL

#endif // _WX_XRC_XH_BOOKCTRLBASE_H_
