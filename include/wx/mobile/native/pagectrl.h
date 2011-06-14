/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/pagectrl.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_PAGECTRL_H_
#define _WX_MOBILE_NATIVE_PAGECTRL_H_

#include "wx/control.h"

/**
 Styles for wxMoPageCtrl.
 */

enum { wxPC_HIDE_FOR_SINGLE_PAGE=0x0800 };

/**
 @class wxMoPageCtrl
 
 A control representing available pages as a row of dots, with one
 dot selected to represent the current page.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoPageCtrl : public wxControl
{
public:
    /// Default constructor.
    wxMoPageCtrl() { Init(); }
    
    /// Constructor.
    wxMoPageCtrl(wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxT("pagectrl"))
    {
        Init();
        Create(parent, id, pos, size, style, validator, name);
    }
    
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxT("pagectrl"));
    
    virtual ~wxMoPageCtrl();
    
    void Init();
    
    /// Gets the current page count.
    int GetPageCount() const { return m_pageCount; }
    
    /// Sets the page count.
    void SetPageCount(int pageCount);
    
    /// Gets the current page.
    int GetCurrentPage() const { return m_currentPage; }
    
    /// Sets the current page.
    void SetCurrentPage(int page);
    
    /// Returns true if it has the style wxPC_HIDE_FOR_SINGLE_PAGE.
    bool GetHideForSinglePage() const { return (GetWindowStyle() & wxPC_HIDE_FOR_SINGLE_PAGE) != 0; }
    
    /// Gets the size of the control for the given page count.
    wxSize GetSizeForPageCount(int pageCount) const;
    
    // Hit testing (implementation only).
    int HitTest(const wxPoint& pos);
    
    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);
    
protected:
    virtual wxSize DoGetBestSize() const;
    
    void SendClickEvent();
    
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    
private:
    
    int     m_pageCount;
    int     m_currentPage;
    int     m_dotSize;
    int     m_dotMargin;
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoPageCtrl)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MOBILE_NATIVE_PAGECTRL_H_
