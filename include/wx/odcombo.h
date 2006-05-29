/////////////////////////////////////////////////////////////////////////////
// Name:        wx/odcombo.h
// Purpose:     wxOwnerDrawnComboBox and wxVListBoxPopup
// Author:      Jaakko Salli
// Modified by:
// Created:     Apr-30-2006
// RCS-ID:      $Id$
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ODCOMBO_H_
#define _WX_ODCOMBO_H_

#include "wx/defs.h"

#if wxUSE_ODCOMBOBOX

#include "wx/combo.h"
#include "wx/ctrlsub.h"
#include "wx/vlbox.h"


//
// New window styles for wxOwnerDrawnComboBox
//
enum
{
    // Double-clicking cycles item if wxCB_READONLY is also used.
    wxODCB_DCLICK_CYCLES            = wxCC_SPECIAL_DCLICK,

    // If used, control itself is not custom paint using callback.
    // Even if this is not used, writable combo is never custom paint
    // until SetCustomPaintWidth is called
    wxODCB_STD_CONTROL_PAINT        = 0x1000
};


//
// Callback flags
//
enum
{
    // when set, we are painting the selected item in control,
    // not in the popup
    wxCP_PAINTING_CONTROL           = 0x0001
};


// ----------------------------------------------------------------------------
// wxVListBoxComboPopup is a wxVListBox customized to act as a popup control.
//
// Notes:
//   wxOwnerDrawnComboBox uses this as its popup. However, it always derives
//   from native wxComboCtrl. If you need to use this popup with
//   wxGenericComboControl, then remember that vast majority of item manipulation
//   functionality is implemented in the wxVListBoxComboPopup class itself.
//
// ----------------------------------------------------------------------------


class WXDLLIMPEXP_ADV wxVListBoxComboPopup : public wxVListBox,
                                             public wxComboPopup
{
    friend class wxOwnerDrawnComboBox;
public:

    // init and dtor
    wxVListBoxComboPopup() : wxVListBox(), wxComboPopup() { }
    virtual ~wxVListBoxComboPopup();

    // required virtuals
    virtual void Init();
    virtual bool Create(wxWindow* parent);
    virtual wxWindow *GetControl() { return this; }
    virtual void SetStringValue( const wxString& value );
    virtual wxString GetStringValue() const;

    // more customization
    virtual void OnPopup();
    virtual wxSize GetAdjustedSize( int minWidth, int prefHeight, int maxHeight );
    virtual void PaintComboControl( wxDC& dc, const wxRect& rect );
    virtual void OnComboKeyEvent( wxKeyEvent& event );
    virtual void OnComboDoubleClick();
    virtual bool LazyCreate();

    // Callbacks for drawing and measuring items. Override in a derived class for
    // owner-drawnness.
    // item: item index to be drawn, may be wxNOT_FOUND when painting combo control itself
    //       and there is no valid selection
    // flags: wxCP_PAINTING_CONTROL is set if painting to combo control instead of list
    virtual void OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const;

    // Return item width, or -1 for calculating from text extent (default)
    virtual wxCoord OnMeasureItemWidth( size_t item ) const;


    // Item management
    void SetSelection( int item );
    void Insert( const wxString& item, int pos );
    int Append(const wxString& item);
    void Clear();
    void Delete( unsigned int item );
    void SetItemClientData(unsigned int n, void* clientData, wxClientDataType clientDataItemsType);
    void *GetItemClientData(unsigned int n) const;
    void SetString( int item, const wxString& str );
    wxString GetString( int item ) const;
    unsigned int GetCount() const;
    int FindString(const wxString& s, bool bCase = false) const;
    int GetSelection() const;

    //void Populate( int n, const wxString choices[] );
    void Populate( const wxArrayString& choices );
    void ClearClientDatas();

    // helpers
    int GetItemAtPosition( const wxPoint& pos ) { return HitTest(pos); }
    wxCoord GetTotalHeight() const { return EstimateTotalHeight(); }
    wxCoord GetLineHeight(int line) const { return OnGetLineHeight(line); }

protected:

    // Called by OnComboDoubleClick and OnComboKeyEvent
    bool HandleKey( int keycode, bool saturate );

    // sends combobox select event from the parent combo control
    void SendComboBoxEvent( int selection );

    // gets value, sends event and dismisses
    void DismissWithEvent();

    // Re-calculates width for given item
    void CheckWidth( int pos );

    // wxVListBox implementation
    virtual void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;
    //virtual wxCoord OnMeasureItem(size_t n) const;
    void OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const;

    // Return item height
    virtual wxCoord OnMeasureItem( size_t item ) const;

    // filter mouse move events happening outside the list box
    // move selection with cursor
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnLeftClick(wxMouseEvent& event);

    wxArrayString           m_strings;
    wxArrayPtrVoid          m_clientDatas;
    wxArrayInt              m_widths; // cached line widths

    wxFont                  m_useFont;

    //wxString                m_stringValue; // displayed text (may be different than m_strings[m_value])
    int                     m_value; // selection
    int                     m_itemHover; // on which item the cursor is
    int                     m_widestWidth; // width of widest item thus far
    int                     m_avgCharWidth;
    int                     m_baseImageWidth; // how much per item drawn in addition to text
    int                     m_itemHeight; // default item height (calculate from font size
                                          // and used in the absence of callback)
    wxClientDataType        m_clientDataItemsType;

private:

    // has the mouse been released on this control?
    bool m_clicked;

    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// wxOwnerDrawnComboBox: a generic wxComboBox that allows custom paint items
// in addition to many other types of customization already allowed by
// the wxComboCtrl.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxOwnerDrawnComboBox : public wxComboCtrl,
                                             public wxItemContainer
{
    friend class wxComboPopupWindow;
    friend class wxComboCtrlBase;
public:

    // ctors and such
    wxOwnerDrawnComboBox() : wxComboCtrl() { Init(); }

    wxOwnerDrawnComboBox(wxWindow *parent,
                         wxWindowID id,
                         const wxString& value,
                         const wxPoint& pos,
                         const wxSize& size,
                         int n,
                         const wxString choices[],
                         long style = 0,
                         const wxValidator& validator = wxDefaultValidator,
                         const wxString& name = wxComboBoxNameStr)
        : wxComboCtrl()
    {
        Init();

        (void)Create(parent, id, value, pos, size, n,
                     choices, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    wxOwnerDrawnComboBox(wxWindow *parent,
                         wxWindowID id,
                         const wxString& value,
                         const wxPoint& pos,
                         const wxSize& size,
                         const wxArrayString& choices,
                         long style,
                         const wxValidator& validator = wxDefaultValidator,
                         const wxString& name = wxComboBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                int n,
                const wxString choices[],
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    virtual ~wxOwnerDrawnComboBox();

    // NULL popup can be used to indicate default interface
    virtual void SetPopupControl( wxComboPopup* popup );

    // wxControlWithItems methods
    virtual void Clear();
    virtual void Delete(unsigned int n);
    virtual unsigned int GetCount() const;
    virtual wxString GetString(unsigned int n) const;
    virtual void SetString(unsigned int n, const wxString& s);
    virtual int FindString(const wxString& s, bool bCase = false) const;
    virtual void Select(int n);
    virtual int GetSelection() const;
    virtual void SetSelection(int n) { Select(n); }


    // Prevent a method from being hidden
    virtual void SetSelection(long from, long to)
    {
        wxComboCtrl::SetSelection(from,to);
    }

    wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST

protected:

    // clears all allocated client datas
    void ClearClientDatas();

    virtual int DoAppend(const wxString& item);
    virtual int DoInsert(const wxString& item, unsigned int pos);
    virtual void DoSetItemClientData(unsigned int n, void* clientData);
    virtual void* DoGetItemClientData(unsigned int n) const;
    virtual void DoSetItemClientObject(unsigned int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(unsigned int n) const;

    // overload m_popupInterface member so we can access specific popup interface easier
    wxVListBoxComboPopup*   m_popupInterface;

    // temporary storage for the initial choices
    //const wxString*         m_baseChoices;
    //int                     m_baseChoicesCount;
    wxArrayString           m_initChs;

private:
    void Init();

    DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(wxOwnerDrawnComboBox)
};


#endif // wxUSE_ODCOMBOBOX

#endif
    // _WX_ODCOMBO_H_
