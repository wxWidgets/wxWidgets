/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/toplevel.h
// Purpose:     wxTopLevelWindow Motif implementation
// Author:      Mattia Barbon
// Modified by:
// Created:     12/10/2002
// RCS-ID:      $Id$
// Copyright:   (c) Mattia Barbon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __MOTIFTOPLEVELH__
#define __MOTIFTOPLEVELH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "toplevel.h"
#endif

class wxTopLevelWindowMotif : public wxTopLevelWindowBase
{
public:
    wxTopLevelWindowMotif() { Init(); }
    wxTopLevelWindowMotif( wxWindow* parent, wxWindowID id,
                           const wxString& title,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize,
                           long style = wxDEFAULT_FRAME_STYLE,
                           const wxString& name = wxFrameNameStr )
    {
        Init();

        Create( parent, id, title, pos, size, style, name );
    }

    bool Create( wxWindow* parent, wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_FRAME_STYLE,
                 const wxString& name = wxFrameNameStr );

    virtual ~wxTopLevelWindowMotif();

    virtual bool ShowFullScreen( bool show, long style = wxFULLSCREEN_ALL );
    virtual bool IsFullScreen() const;

    virtual void Maximize(bool maximize = TRUE);
    virtual void Restore();
    virtual void Iconize(bool iconize = TRUE);
    virtual bool IsMaximized() const;
    virtual bool IsIconized() const;

    virtual void Raise();
    virtual void Lower();

    virtual wxString GetTitle() const { return m_title; }
    virtual void SetTitle( const wxString& title ) { m_title = title; }

    virtual void DoSetSizeHints( int minW, int minH,
                               int maxW = -1, int maxH = -1,
                               int incW = -1, int incH = -1 );

    virtual bool SetShape( const wxRegion& region );

    WXWidget GetShellWidget() const;
protected:
    // common part of all constructors
    void Init();
    // common part of wxDialog/wxFrame destructors
    void PreDestroy();

    virtual void DoGetPosition(int* x, int* y) const;
private:
    // both these functions should be pure virtual
    virtual bool DoCreate( wxWindow* parent, wxWindowID id,
                           const wxString& title,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name )
    {
        return FALSE;
    }

    virtual void DoDestroy() { }

    wxString m_title;
};

#endif // __MOTIFTOPLEVELH__
