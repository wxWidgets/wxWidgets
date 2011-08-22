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
        
    void SetText(const wxString& text);
    void SetDetailText(const wxString& text);
    void SetTextFont(const wxFont& font);
    void SetDetailTextFont(const wxFont& font);
    void SetTextColour(const wxColour& colour);
    void SetDetailTextColour(const wxColour& colour);
    void SetSelectedTextColour(const wxColour& colour);
    void SetSelectedDetailTextColour(const wxColour& colour);
    void SetTextAlignment(wxTableCellTextAlignment alignment);
    void SetDetailTextAlignment(wxTableCellTextAlignment alignment);
    void SetTextLineBreakMode(wxTableCellLineBreakMode lineBreakMode);
    void SetDetailTextLineBreakMode(wxTableCellLineBreakMode lineBreakMode);
    void SetBitmap(const wxBitmap& bitmap);
    void SetSelectedBitmap(const wxBitmap& bitmap);
    void SetSelectionStyle(wxTableCellSelectionStyle style);
    void SetAccessoryWindow(wxWindow* win);
    void SetAccessoryType(wxTableCellAccessoryType accessoryType);
    void SetIndentationLevel(int indentationLevel);
    void SetIndentationWidth(int indentationWidth);
    void SetSelected(bool selected);
    bool GetSelected() const;
    bool SetEditingMode(bool editingMode, bool animated = true);
    bool IsEditing() const;
    void SetDetailWidth(float width);
    float GetDetailWidth() const;
    void SetEditingAccessoryType(wxTableCellAccessoryType accessoryType);
    void SetEditingAccessoryWindow(wxWindow* win);
    void SetIndentWhileEditing(bool indent);
    void SetShowReorderingControl(bool show);
    bool IsDeleteButtonShowing(wxTableCtrl* tableCtrl) const;
    wxTableCellEditStyle GetEditStyle() const;
    
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
