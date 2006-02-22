/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dataview.h
// Purpose:     wxDataViewCtrl GTK+2 implementation header
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDATAVIEWCTRLH__
#define __GTKDATAVIEWCTRLH__

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

// --------------------------------------------------------- 
// classes
// --------------------------------------------------------- 

class WXDLLIMPEXP_CORE wxDataViewCtrl;

// --------------------------------------------------------- 
// wxDataViewColumn
// --------------------------------------------------------- 

class WXDLLIMPEXP_CORE wxDataViewColumn: public wxDataViewColumnBase
{
public:
    wxDataViewColumn( const wxString &title, wxDataViewCtrl *ctrl, 
            wxDataViewColumnType kind, int flags = 0 );
    virtual ~wxDataViewColumn();

    virtual void SetTitle( const wxString &title );

    // implementation
    void* GetGtkHandle() { return m_column; }

private:
    void*   m_column;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewColumn)
};

// --------------------------------------------------------- 
// wxDataViewCtrl
// --------------------------------------------------------- 

class WXDLLIMPEXP_CORE wxDataViewCtrl: public wxDataViewCtrlBase
{
public:
    wxDataViewCtrl() 
    {
        Init();
    }
    
    wxDataViewCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator )
    {
        Create(parent, id, pos, size, style, validator );
    }

    virtual ~wxDataViewCtrl();

    void Init();

    bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator );

    virtual bool AssociateModel( wxDataViewListModel *model );
    virtual bool AppendColumn( wxDataViewColumn *col );    
    
private:
    DECLARE_DYNAMIC_CLASS(wxDataViewCtrl)
    DECLARE_NO_COPY_CLASS(wxDataViewCtrl)
};


#endif // __GTKDATAVIEWCTRLH__
