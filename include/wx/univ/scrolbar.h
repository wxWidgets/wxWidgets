///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/scrolbar.h
// Purpose:     wxScrollBar for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_SCROLBAR_H_
#define _WX_UNIV_SCROLBAR_H_

#ifdef __GNUG__
    #pragma interface "univscrolbar.h"
#endif

#include "wx/univ/renderer.h"   // for wxHitTest

class WXDLLEXPORT wxInputHandler;

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

// scroll the bar
#define wxACTION_SCROLL_START       _T("start")     // to the beginning
#define wxACTION_SCROLL_END         _T("end")       // to the end
#define wxACTION_SCROLL_LINE_UP     _T("lineup")    // one line up/left
#define wxACTION_SCROLL_PAGE_UP     _T("pageup")    // one page up/left
#define wxACTION_SCROLL_LINE_DOWN   _T("linedown")  // one line down/right
#define wxACTION_SCROLL_PAGE_DOWN   _T("pagedown")  // one page down/right

// the scrollbar thumb may be dragged
#define wxACTION_SCROLL_THUMB_DRAG      _T("thumbdrag")
#define wxACTION_SCROLL_THUMB_MOVE      _T("thumbmove")
#define wxACTION_SCROLL_THUMB_RELEASE   _T("thumbrelease")

// ----------------------------------------------------------------------------
// wxScrollBar
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxScrollBar : public wxScrollBarBase
{
public:
    // scrollbar elements: they correspond to wxHT_SCROLLBAR_XXX constants but
    // start from 0 which allows to use them as array indices
    enum Element
    {
        Element_Arrow_Line_1,
        Element_Arrow_Line_2,
        Element_Arrow_Page_1,
        Element_Arrow_Page_2,
        Element_Thumb,
        Element_Bar_1,
        Element_Bar_2,
        Element_Max
    };

    wxScrollBar() { Init(); }
    wxScrollBar(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxScrollBarNameStr)
    {
        Init();

        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxScrollBarNameStr);

    virtual ~wxScrollBar();

    // implement base class pure virtuals
    virtual int GetThumbPosition() const;
    virtual int GetThumbSize() const;
    virtual int GetPageSize() const;
    virtual int GetRange() const;

    virtual void SetThumbPosition(int thumbPos);
    virtual void SetScrollbar(int position, int thumbSize,
                              int range, int pageSize,
                              bool refresh = TRUE);

    // wxScrollBar actions
    void ScrollToStart();
    void ScrollToEnd();
    void ScrollLines(int nLines);
    void ScrollPages(int nPages);

    virtual bool PerformAction(const wxControlAction& action,
                               const wxEvent& event);

    // wxScrollBar sub elements state (combination of wxCONTROL_XXX)
    void SetState(Element which, int flags) { m_elementsState[which] = flags; }
    int GetState(Element which) const { return m_elementsState[which]; }

protected:
    virtual wxSize DoGetBestSize() const;
    virtual void DoDraw(wxControlRenderer *renderer);

    // SetThumbPosition() helper
    void DoSetThumb(int thumbPos);

    // common part of all ctors
    void Init();

private:
    // get the mouse coordinates in the scrollbar direction from a
    // wxMouseEvent (the event *must* really be of this type!)
    wxCoord GetMouseCoord(const wxEvent& event) const;

    // total range of the scrollbar in logical units
    int m_range;

    // the size of the thumb in logical units (from 0 to m_range) and its
    // position (from 0 to m_range - m_thumbSize)
    int m_thumbSize,
        m_thumbPos;

    // the page size, i.e. the number of lines by which to scroll when page
    // up/down action is performed
    int m_pageSize;

    // the state of the sub elements
    int m_elementsState[Element_Max];

    // the offset of the top/left of the scrollbar relative to the mouse to
    // keep during the thumb drag
    int m_ofsMouse;

    DECLARE_DYNAMIC_CLASS(wxScrollBar)
};

#endif // _WX_UNIV_SCROLBAR_H_

