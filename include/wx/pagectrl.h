/////////////////////////////////////////////////////////////////////////////
// Name:        wx/pagectrl.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PAGECTRL_H_BASE_
#define _WX_PAGECTRL_H_BASE_

#include "wx/control.h"

/**
 Styles for wxPageCtrl.
 */

enum {
    wxPC_HIDE_FOR_SINGLE_PAGE=0x0800
};

/**
 @class wxPageCtrl
 
 A control representing available pages as a row of dots, with one
 dot selected to represent the current page.
 
 @category{wxbile}
 */

class WXDLLEXPORT wxPageCtrlBase : public wxControl
{
public:
    /// Default constructor.
    wxPageCtrlBase() { }
    
    /// Constructor.
    wxPageCtrlBase(wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxT("pagectrl"));
    
    virtual ~wxPageCtrlBase() { }
    
    /// Gets the current page count.
    virtual int GetPageCount() const { return m_pageCount; }
    
    /// Sets the page count.
    virtual void SetPageCount(int pageCount) { m_pageCount = pageCount; }
    
    /// Gets the current page.
    virtual int GetCurrentPage() const { return m_currentPage; }
    
    /// Sets the current page.
    virtual void SetCurrentPage(int page) { m_currentPage = page; }
    
    /// Returns true if it has the style wxPC_HIDE_FOR_SINGLE_PAGE.
    virtual bool GetHideForSinglePage() const { return (GetWindowStyle() & wxPC_HIDE_FOR_SINGLE_PAGE) != 0; }
    
    /// Gets the size of the control for the given page count.
    //virtual wxSize GetSizeForPageCount(int pageCount) const;
    
    // Hit testing (implementation only).
    int HitTest(const wxPoint& pos);
    
    /*
    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);
    */
        
private:
    
    int     m_pageCount;
    int     m_currentPage;
    int     m_dotSize;
    int     m_dotMargin;
    
    wxDECLARE_NO_COPY_CLASS(wxPageCtrlBase);
    
};


// ----------------------------------------------------------------------------
// wxPageCtrl class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/pagectrl.h"
#endif


#endif
    // _WX_PAGECTRL_H_BASE_
