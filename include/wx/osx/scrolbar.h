/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/scrolbar.h
// Purpose:     wxScrollBar class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SCROLBAR_H_
#define _WX_SCROLBAR_H_

// Scrollbar item
class WXDLLIMPEXP_CORE wxScrollBar : public wxScrollBarBase
{
public:
    wxScrollBar() { m_pageSize = 0; m_viewSize = 0; m_objectSize = 0; }
    virtual ~wxScrollBar();

    wxScrollBar(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxScrollBarNameStr))
    {
        Create(parent, id, pos, size, style, validator, name);
    }
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxScrollBarNameStr));

    virtual int GetThumbPosition() const wxOVERRIDE;
    virtual int GetThumbSize() const wxOVERRIDE { return m_viewSize; }
    virtual int GetPageSize() const wxOVERRIDE { return m_pageSize; }
    virtual int GetRange() const wxOVERRIDE { return m_objectSize; }
    virtual int GetLineSize() const wxOVERRIDE { return m_lineSize; }

    virtual void SetThumbPosition(int viewStart) wxOVERRIDE;
    virtual void SetScrollbar(int position, int thumbSize, int range,
            int pageSize, bool refresh = true, int lineSize=1) wxOVERRIDE;

    // needed for RTTI
    void SetThumbSize( int s ) { SetScrollbar( GetThumbPosition() , s , GetRange() , GetPageSize() , true, GetLineSize() ) ; }
    void SetPageSize( int s ) { SetScrollbar( GetThumbPosition() , GetThumbSize() , GetRange() , s , true, GetLineSize() ) ; }
    void SetLineSize( int s ) { SetScrollbar( GetThumbPosition() , GetThumbSize() , GetRange() , GetPageSize() , true, s ) ; }
    void SetRange( int s ) { SetScrollbar( GetThumbPosition() , GetThumbSize() , s , GetPageSize() , true, GetLineSize() ) ; }

        // implementation only from now on
    void Command(wxCommandEvent& event) wxOVERRIDE;
    virtual void TriggerScrollEvent( wxEventType scrollEvent ) wxOVERRIDE;
    virtual bool OSXHandleClicked( double timestampsec ) wxOVERRIDE;
protected:
    virtual wxSize DoGetBestSize() const wxOVERRIDE;

    int m_pageSize;
    int m_lineSize;
    int m_viewSize;
    int m_objectSize;

    wxDECLARE_DYNAMIC_CLASS(wxScrollBar);
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_SCROLBAR_H_
