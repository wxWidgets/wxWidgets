/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/scrolbar.h
// Purpose:     wxScrollBar class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SCROLBAR_H_
#define _WX_SCROLBAR_H_

// Scrollbar item
class WXDLLIMPEXP_CORE wxScrollBar: public wxScrollBarBase
{
public:
    wxScrollBar() { m_pageSize = 0; m_viewSize = 0; m_objectSize = 0; }
    virtual ~wxScrollBar();

    wxScrollBar(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxSB_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxScrollBarNameStr))
    {
        Create(parent, id, pos, size, style, validator, name);
    }
    bool Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxSB_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxScrollBarNameStr));

    int GetThumbPosition() const wxOVERRIDE;
    int GetThumbSize() const wxOVERRIDE { return m_pageSize; }
    int GetPageSize() const wxOVERRIDE { return m_viewSize; }
    int GetRange() const wxOVERRIDE { return m_objectSize; }
    int GetLineSize() const { return m_lineSize; }

    virtual void SetThumbPosition(int viewStart) wxOVERRIDE;
    virtual void SetScrollbar(int position, int thumbSize, int range, int pageSize,
            bool refresh = true, int lineSize = 1) wxOVERRIDE ;

    // needed for RTTI
    void SetThumbSize( int s ) { SetScrollbar( GetThumbPosition() , s , GetRange() , GetPageSize() , true , GetLineSize() ) ; }
    void SetPageSize( int s ) { SetScrollbar( GetThumbPosition() , GetThumbSize() , GetRange() , s , true , GetLineSize()) ; }
    void SetRange( int s ) { SetScrollbar( GetThumbPosition() , GetThumbSize() , s , GetPageSize() , true , GetLineSize()) ; }
    void SetLineSize( int s ) { SetScrollbar( GetThumbPosition() , GetThumbSize() , GetRange() , GetPageSize() , true , s) ; }

    void Command(wxCommandEvent& event) wxOVERRIDE;
    virtual bool MSWOnScroll(int orientation, WXWORD wParam,
                             WXWORD pos, WXHWND control) wxOVERRIDE;

    // override wxControl version to not use solid background here
    virtual WXHBRUSH MSWControlColor(WXHDC pDC, WXHWND hWnd) wxOVERRIDE;

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const wxOVERRIDE;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const wxOVERRIDE { return false; }

protected:
    virtual wxSize DoGetBestSize() const wxOVERRIDE;

    int m_pageSize;
    int m_viewSize;
    int m_objectSize;
    int m_lineSize;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxScrollBar);
};

#endif
    // _WX_SCROLBAR_H_
