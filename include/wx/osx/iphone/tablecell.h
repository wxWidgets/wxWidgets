/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/tablecell.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TABLECELL_H_
#define _WX_TABLECELL_H_

#include "wx/dynarray.h"


#pragma mark wxTableCellContentWindow

class WXDLLEXPORT wxTableCellContentWindow: public wxTableCellContentWindowBase
{
public:
    wxTableCellContentWindow() { Init(); }
    wxTableCellContentWindow(wxWindow* parent,
                               wxWindowID id = wxID_ANY,
                               const wxPoint& pos = wxDefaultPosition,
                               const wxSize& sz = wxDefaultSize,
                               long style = 0)
    {
        Create(parent, id, pos, sz, style);
    }
    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& sz = wxDefaultSize,
                long style = 0);
            
protected:
    void Init();
    
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvents(wxMouseEvent& event);
    
    wxTableCell*  m_cell;
    wxTableCtrl*  m_tableCtrl;
    
    DECLARE_DYNAMIC_CLASS(wxTableCellContentWindow)
    DECLARE_EVENT_TABLE()
};


#pragma mark wxTableCell

/**
 @class wxTableCell
 
 @category{wxbile}
 */

class WXDLLEXPORT wxTableCell: public wxTableCellBase
{
public:
    
    /// Constructor.
    wxTableCell(wxTableCtrl* ctrl,
                  const wxString& reuseName = wxEmptyString,
                  wxTableCellStyle cellStyle = CellStyleDefault);
    virtual ~wxTableCell();
        
    // Assignment operato
    void operator=(const wxTableCell& cell) { Copy(cell); }
    
    /// Copies the cell.
    void Copy(const wxTableCell& cell);
    
    /// Prepares the cell for reuse
    virtual void PrepareForReuse(wxTableCtrl* tableCtrl);
    
    /// Creates the content window.
    virtual bool CreateContentWindow(wxTableCtrl* ctrl);
    
    void SetAccessoryWindow(wxWindow* win);
    void SetAccessoryType(wxTableCellAccessoryType accessoryType);
    void SetIndentationLevel(int indentationLevel);
    void SetIndentationWidth(int indentationWidth);
    
    // Variants of SetPeer()/GetPeer()
    void                SetCellWidgetImpl(wxOSXWidgetImpl* widgetImpl) { m_widgetImpl = widgetImpl; }
    wxOSXWidgetImpl*    GetCellWidgetImpl() { return m_widgetImpl; }
    
protected:
    
    void Init();
    
    DECLARE_CLASS(wxTableCell)
    
private:
    wxOSXWidgetImpl*    m_widgetImpl;
    
};

#endif
    // _WX_TABLECELL_H_
