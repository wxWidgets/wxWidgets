///////////////////////////////////////////////////////////////////////////////
// Name:        wx/htmllbox.h
// Purpose:     wxHtmlListBox is a listbox whose items are wxHtmlCells
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.05.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HTMLLBOX_H_
#define _WX_HTMLLBOX_H_

#include "wx/vlbox.h"               // base class

class WXDLLIMPEXP_HTML wxHtmlCell;
class WXDLLIMPEXP_HTML wxHtmlWinParser;
class WXDLLIMPEXP_HTML wxHtmlListBoxCache;
class WXDLLIMPEXP_HTML wxHtmlListBoxStyle;

// ----------------------------------------------------------------------------
// wxHtmlListBox
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_HTML wxHtmlListBox : public wxVListBox
{
public:
    // constructors and such
    // ---------------------

    // default constructor, you must call Create() later
    wxHtmlListBox() { Init(); }

    // normal constructor which calls Create() internally
    wxHtmlListBox(wxWindow *parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxString& name = wxVListBoxNameStr)
    {
        Init();

        (void)Create(parent, id, pos, size, style, name);
    }

    // really creates the control and sets the initial number of items in it
    // (which may be changed later with SetItemCount())
    //
    // the only special style which may be specified here is wxLB_MULTIPLE
    //
    // returns true on success or false if the control couldn't be created
    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxVListBoxNameStr);

    // destructor cleans up whatever resources we use
    virtual ~wxHtmlListBox();

    // override some base class virtuals
    virtual void RefreshAll();
    virtual void SetItemCount(size_t count);

protected:
    // this method must be implemented in the derived class and should return
    // the body (i.e. without <html>) of the HTML for the given item
    virtual wxString OnGetItem(size_t n) const = 0;

    // this function may be overridden to decorate HTML returned by OnGetItem()
    virtual wxString OnGetItemMarkup(size_t n) const;


    // this method allows to customize the selection appearance: it may be used
    // to specify the colour of the text which normally has the given colour
    // colFg when it is inside the selection
    //
    // by default, the original colour is not used at all and all text has the
    // same (default for this system) colour inside selection
    virtual wxColour GetSelectedTextColour(const wxColour& colFg) const;

    // this is the same as GetSelectedTextColour() but allows to customize the
    // background colour -- this is even more rarely used as you can change it
    // globally using SetSelectionBackground()
    virtual wxColour GetSelectedTextBgColour(const wxColour& colBg) const;


    // we implement both of these functions in terms of OnGetItem(), they are
    // not supposed to be overridden by our descendants
    virtual void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;
    virtual wxCoord OnMeasureItem(size_t n) const;


    // event handlers
    void OnSize(wxSizeEvent& event);


    // common part of all ctors
    void Init();

    // ensure that the given item is cached
    void CacheItem(size_t n) const;

private:
    // this class caches the pre-parsed HTML to speed up display
    wxHtmlListBoxCache *m_cache;

    // HTML parser we use
    wxHtmlWinParser *m_htmlParser;

    // rendering style for the parser which allows us to customize our colours
    wxHtmlListBoxStyle *m_htmlRendStyle;


    // it calls our GetSelectedTextColour() and GetSelectedTextBgColour()
    friend class wxHtmlListBoxStyle;


    DECLARE_EVENT_TABLE()
};

#endif // _WX_HTMLLBOX_H_

