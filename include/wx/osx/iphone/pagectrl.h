/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/pagectrl.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PAGECTRL_H_
#define _WX_PAGECTRL_H_

#include "wx/control.h"


/**
 @class wxPageCtrl
 
 @category{wxbile}
 */

class WXDLLEXPORT wxPageCtrl : public wxPageCtrlBase
{
public:
    /// Default constructor.
    wxPageCtrl() { Init(); }
    
    /// Constructor.
    wxPageCtrl(wxWindow *parent,
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
    
    virtual ~wxPageCtrl();
        
    /// Sets the page count.
    void SetPageCount(int pageCount);
    
    /// Sets the current page.
    void SetCurrentPage(int page);
    
    /// Returns true if it has the style wxPC_HIDE_FOR_SINGLE_PAGE.
    bool GetHideForSinglePage() const { return (GetWindowStyle() & wxPC_HIDE_FOR_SINGLE_PAGE) != 0; }
    
    /// Gets the size of the control for the given page count.
    wxSize GetSizeForPageCount(int pageCount) const;
        
protected:
    void Init();
    
private:
    
    int     m_pageCount;
    int     m_currentPage;
    int     m_dotSize;
    int     m_dotMargin;
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxPageCtrl)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_PAGECTRL_H_
