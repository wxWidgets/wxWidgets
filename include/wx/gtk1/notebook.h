/////////////////////////////////////////////////////////////////////////////
// Name:        tabctrl.h
// Purpose:     wxTabCtrl class
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TABCTRLH__
#define __TABCTRLH__

#ifdef __GNUG__
#pragma interface "notebook.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxImageList;
class wxNotebook;
class wxNotebookPage;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// wxNotebook
//-----------------------------------------------------------------------------

class wxNotebook: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxNotebook)
  
  public:

    wxNotebook(void);
    wxNotebook( wxWindow *parent, const wxWindowID id, 
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      const long style = 0, const wxString& name = "notebook" );
    ~wxNotebook(void);
    bool Create(wxWindow *parent, const wxWindowID id, 
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      const long style = 0, const wxString& name = "notebook" );
    int GetSelection(void) const;
    wxImageList* GetImageList(void) const;
    int GetPageCount(void) const;
    int GetRowCount(void) const;
    wxString GetPageText( const int page ) const;
    int GetPageImage( const int page ) const;
    void* GetPageData( const int page ) const;
    wxNotebookPage* GetNotebookPage(int page) const;
    int SetSelection( const int page );
    void SetImageList( wxImageList* imageList );
    bool SetPageText( const int page, const wxString& text );
    bool SetPageImage( const int oage, const int image );
    bool SetPageData( const int page, void* data );
    void SetPageSize( const wxSize& size );
    void SetPadding( const wxSize& padding );
    bool DeleteAllPages(void);
    bool DeletePage( const int page );
    bool AddPage(wxWindow* win, const wxString& text, const int imageId = -1, void* data = NULL );
    wxWindow *GetPageWindow( const int page ) const;
    virtual void AddChild( wxWindow *win );

  protected:
    // wxWin callbacks
    void OnSize(wxSizeEvent& event);

    wxImageList*    m_imageList;
    wxList          m_pages;

  DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxTabEvent
//-----------------------------------------------------------------------------

class wxTabEvent: public wxCommandEvent
{
  DECLARE_DYNAMIC_CLASS(wxTabEvent)

 public:
 
  wxTabEvent( WXTYPE commandType = 0, int id = 0 );
};

typedef void (wxEvtHandler::*wxTabEventFunction)(wxTabEvent&);

#define EVT_TAB_SEL_CHANGED(id, fn) { wxEVT_COMMAND_TAB_SEL_CHANGED, \
  id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTabEventFunction) & fn, NULL },
#define EVT_TAB_SEL_CHANGING(id, fn) { wxEVT_COMMAND_TAB_SEL_CHANGING, \
  id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTabEventFunction) & fn, NULL },

#endif
    // __TABCTRLH__
