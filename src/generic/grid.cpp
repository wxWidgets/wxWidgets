///////////////////////////////////////////////////////////////////////////
// Name:        generic/grid.cpp
// Purpose:     wxGrid and related classes
// Author:      Michael Bedward (based on code by Julian Smart, Robin Dunn)
// Modified by:
// Created:     1/08/1999
// RCS-ID:      $Id$
// Copyright:   (c) Michael Bedward (mbedward@ozemail.com.au)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "grid.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !defined(wxUSE_NEW_GRID) || !(wxUSE_NEW_GRID)
#include "gridg.cpp"
#else

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/log.h"
    #include "wx/textctrl.h"
    #include "wx/checkbox.h"
    #include "wx/combobox.h"
    #include "wx/valtext.h"
#endif

#include "wx/textfile.h"
#include "wx/spinctrl.h"

#include "wx/grid.h"

// ----------------------------------------------------------------------------
// array classes
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY(wxGridCellAttr *, wxArrayAttrs);

struct wxGridCellWithAttr
{
    wxGridCellWithAttr(int row, int col, wxGridCellAttr *attr_)
        : coords(row, col), attr(attr_)
    {
    }

    ~wxGridCellWithAttr()
    {
        attr->DecRef();
    }

    wxGridCellCoords coords;
    wxGridCellAttr  *attr;
};

WX_DECLARE_OBJARRAY(wxGridCellWithAttr, wxGridCellWithAttrArray);

#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(wxGridCellCoordsArray)
WX_DEFINE_OBJARRAY(wxGridCellWithAttrArray)

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGridRowLabelWindow : public wxWindow
{
public:
    wxGridRowLabelWindow() { m_owner = (wxGrid *)NULL; }
    wxGridRowLabelWindow( wxGrid *parent, wxWindowID id,
                          const wxPoint &pos, const wxSize &size );

private:
    wxGrid   *m_owner;

    void OnPaint( wxPaintEvent& event );
    void OnMouseEvent( wxMouseEvent& event );
    void OnKeyDown( wxKeyEvent& event );

    DECLARE_DYNAMIC_CLASS(wxGridRowLabelWindow)
    DECLARE_EVENT_TABLE()
};


class WXDLLEXPORT wxGridColLabelWindow : public wxWindow
{
public:
    wxGridColLabelWindow() { m_owner = (wxGrid *)NULL; }
    wxGridColLabelWindow( wxGrid *parent, wxWindowID id,
                          const wxPoint &pos, const wxSize &size );

private:
    wxGrid   *m_owner;

    void OnPaint( wxPaintEvent &event );
    void OnMouseEvent( wxMouseEvent& event );
    void OnKeyDown( wxKeyEvent& event );

    DECLARE_DYNAMIC_CLASS(wxGridColLabelWindow)
    DECLARE_EVENT_TABLE()
};


class WXDLLEXPORT wxGridCornerLabelWindow : public wxWindow
{
public:
    wxGridCornerLabelWindow() { m_owner = (wxGrid *)NULL; }
    wxGridCornerLabelWindow( wxGrid *parent, wxWindowID id,
                             const wxPoint &pos, const wxSize &size );

private:
    wxGrid *m_owner;

    void OnMouseEvent( wxMouseEvent& event );
    void OnKeyDown( wxKeyEvent& event );
    void OnPaint( wxPaintEvent& event );

    DECLARE_DYNAMIC_CLASS(wxGridCornerLabelWindow)
    DECLARE_EVENT_TABLE()
};

class WXDLLEXPORT wxGridWindow : public wxPanel
{
public:
    wxGridWindow()
    {
        m_owner = (wxGrid *)NULL;
        m_rowLabelWin = (wxGridRowLabelWindow *)NULL;
        m_colLabelWin = (wxGridColLabelWindow *)NULL;
    }

    wxGridWindow( wxGrid *parent,
                  wxGridRowLabelWindow *rowLblWin,
                  wxGridColLabelWindow *colLblWin,
                  wxWindowID id, const wxPoint &pos, const wxSize &size );
    ~wxGridWindow();

    void ScrollWindow( int dx, int dy, const wxRect *rect );

private:
    wxGrid                   *m_owner;
    wxGridRowLabelWindow     *m_rowLabelWin;
    wxGridColLabelWindow     *m_colLabelWin;

    void OnPaint( wxPaintEvent &event );
    void OnMouseEvent( wxMouseEvent& event );
    void OnKeyDown( wxKeyEvent& );
    void OnEraseBackground( wxEraseEvent& );


    DECLARE_DYNAMIC_CLASS(wxGridWindow)
    DECLARE_EVENT_TABLE()
};



class wxGridCellEditorEvtHandler : public wxEvtHandler
{
public:
    wxGridCellEditorEvtHandler()
        : m_grid(0), m_editor(0)
        { }
    wxGridCellEditorEvtHandler(wxGrid* grid, wxGridCellEditor* editor)
        : m_grid(grid), m_editor(editor)
        { }

    void OnKeyDown(wxKeyEvent& event);
    void OnChar(wxKeyEvent& event);

private:
    wxGrid*             m_grid;
    wxGridCellEditor*   m_editor;
    DECLARE_DYNAMIC_CLASS(wxGridCellEditorEvtHandler)
    DECLARE_EVENT_TABLE()
};


IMPLEMENT_DYNAMIC_CLASS( wxGridCellEditorEvtHandler, wxEvtHandler )
BEGIN_EVENT_TABLE( wxGridCellEditorEvtHandler, wxEvtHandler )
    EVT_KEY_DOWN( wxGridCellEditorEvtHandler::OnKeyDown )
    EVT_CHAR( wxGridCellEditorEvtHandler::OnChar )
END_EVENT_TABLE()



// ----------------------------------------------------------------------------
// the internal data representation used by wxGridCellAttrProvider
// ----------------------------------------------------------------------------

// this class stores attributes set for cells
class WXDLLEXPORT wxGridCellAttrData
{
public:
    void SetAttr(wxGridCellAttr *attr, int row, int col);
    wxGridCellAttr *GetAttr(int row, int col) const;
    void UpdateAttrRows( size_t pos, int numRows );
    void UpdateAttrCols( size_t pos, int numCols );

private:
    // searches for the attr for given cell, returns wxNOT_FOUND if not found
    int FindIndex(int row, int col) const;

    wxGridCellWithAttrArray m_attrs;
};

// this class stores attributes set for rows or columns
class WXDLLEXPORT wxGridRowOrColAttrData
{
public:
    // empty ctor to suppress warnings
    wxGridRowOrColAttrData() { }
    ~wxGridRowOrColAttrData();

    void SetAttr(wxGridCellAttr *attr, int rowOrCol);
    wxGridCellAttr *GetAttr(int rowOrCol) const;
    void UpdateAttrRowsOrCols( size_t pos, int numRowsOrCols );

private:
    wxArrayInt m_rowsOrCols;
    wxArrayAttrs m_attrs;
};

// NB: this is just a wrapper around 3 objects: one which stores cell
//     attributes, and 2 others for row/col ones
class WXDLLEXPORT wxGridCellAttrProviderData
{
public:
    wxGridCellAttrData m_cellAttrs;
    wxGridRowOrColAttrData m_rowAttrs,
                           m_colAttrs;
};


// ----------------------------------------------------------------------------
// data structures used for the data type registry
// ----------------------------------------------------------------------------

struct wxGridDataTypeInfo
{
    wxGridDataTypeInfo(const wxString& typeName,
                       wxGridCellRenderer* renderer,
                       wxGridCellEditor* editor)
        : m_typeName(typeName), m_renderer(renderer), m_editor(editor)
        { }

    ~wxGridDataTypeInfo() { delete m_renderer; delete m_editor; }

    wxString            m_typeName;
    wxGridCellRenderer* m_renderer;
    wxGridCellEditor*   m_editor;
};


WX_DEFINE_ARRAY(wxGridDataTypeInfo*, wxGridDataTypeInfoArray);


class WXDLLEXPORT wxGridTypeRegistry
{
public:
    ~wxGridTypeRegistry();

    void RegisterDataType(const wxString& typeName,
                     wxGridCellRenderer* renderer,
                     wxGridCellEditor* editor);
    int FindDataType(const wxString& typeName);
    wxGridCellRenderer* GetRenderer(int index);
    wxGridCellEditor*   GetEditor(int index);

private:
    wxGridDataTypeInfoArray m_typeinfo;
};

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

#ifndef WXGRID_DRAW_LINES
#define WXGRID_DRAW_LINES 1
#endif

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

//#define DEBUG_ATTR_CACHE
#ifdef DEBUG_ATTR_CACHE
    static size_t gs_nAttrCacheHits = 0;
    static size_t gs_nAttrCacheMisses = 0;
#endif // DEBUG_ATTR_CACHE

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

wxGridCellCoords wxGridNoCellCoords( -1, -1 );
wxRect           wxGridNoCellRect( -1, -1, -1, -1 );

// scroll line size
// TODO: fixed so far - make configurable later (and also different for x/y)
static const size_t GRID_SCROLL_LINE = 10;

// the size of hash tables used a bit everywhere (the max number of elements
// in these hash tables is the number of rows/columns)
static const int GRID_HASH_SIZE = 100;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxGridCellEditor
// ----------------------------------------------------------------------------

wxGridCellEditor::wxGridCellEditor()
{
    m_control = NULL;
}


wxGridCellEditor::~wxGridCellEditor()
{
    Destroy();
}

void wxGridCellEditor::Create(wxWindow* WXUNUSED(parent),
                              wxWindowID WXUNUSED(id),
                              wxEvtHandler* evtHandler)
{
    if ( evtHandler )
        m_control->PushEventHandler(evtHandler);
}

void wxGridCellEditor::PaintBackground(const wxRect& rectCell,
                                       wxGridCellAttr *attr)
{
    // erase the background because we might not fill the cell
    wxClientDC dc(m_control->GetParent());
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(attr->GetBackgroundColour(), wxSOLID));
    dc.DrawRectangle(rectCell);

    // redraw the control we just painted over
    m_control->Refresh();
}

void wxGridCellEditor::Destroy()
{
    if (m_control)
    {
        m_control->PopEventHandler(TRUE /* delete it*/);

        m_control->Destroy();
        m_control = NULL;
    }
}

void wxGridCellEditor::Show(bool show, wxGridCellAttr *attr)
{
    wxASSERT_MSG(m_control,
                 wxT("The wxGridCellEditor must be Created first!"));
    m_control->Show(show);

    if ( show )
    {
        // set the colours/fonts if we have any
        if ( attr )
        {
            m_colFgOld = m_control->GetForegroundColour();
            m_control->SetForegroundColour(attr->GetTextColour());

            m_colBgOld = m_control->GetBackgroundColour();
            m_control->SetBackgroundColour(attr->GetBackgroundColour());

            m_fontOld = m_control->GetFont();
            m_control->SetFont(attr->GetFont());

            // can't do anything more in the base class version, the other
            // attributes may only be used by the derived classes
        }
    }
    else
    {
        // restore the standard colours fonts
        if ( m_colFgOld.Ok() )
        {
            m_control->SetForegroundColour(m_colFgOld);
            m_colFgOld = wxNullColour;
        }

        if ( m_colBgOld.Ok() )
        {
            m_control->SetBackgroundColour(m_colBgOld);
            m_colBgOld = wxNullColour;
        }

        if ( m_fontOld.Ok() )
        {
            m_control->SetFont(m_fontOld);
            m_fontOld = wxNullFont;
        }
    }
}

void wxGridCellEditor::SetSize(const wxRect& rect)
{
    wxASSERT_MSG(m_control,
                 wxT("The wxGridCellEditor must be Created first!"));
    m_control->SetSize(rect, wxSIZE_ALLOW_MINUS_ONE);
}

void wxGridCellEditor::HandleReturn(wxKeyEvent& event)
{
    event.Skip();
}


void wxGridCellEditor::StartingKey(wxKeyEvent& event)
{
    event.Skip();
}

void wxGridCellEditor::StartingClick()
{
}

// ----------------------------------------------------------------------------
// wxGridCellTextEditor
// ----------------------------------------------------------------------------

wxGridCellTextEditor::wxGridCellTextEditor()
{
}

void wxGridCellTextEditor::Create(wxWindow* parent,
                                  wxWindowID id,
                                  wxEvtHandler* evtHandler)
{
    m_control = new wxTextCtrl(parent, id, wxEmptyString,
                               wxDefaultPosition, wxDefaultSize
#if defined(__WXMSW__)
                               , wxTE_MULTILINE | wxTE_NO_VSCROLL // necessary ???
#endif
                              );

    wxGridCellEditor::Create(parent, id, evtHandler);
}

void wxGridCellTextEditor::PaintBackground(const wxRect& WXUNUSED(rectCell),
                                           wxGridCellAttr * WXUNUSED(attr))
{
    // as we fill the entire client area, don't do anything here to minimize
    // flicker
}

void wxGridCellTextEditor::SetSize(const wxRect& rectOrig)
{
    wxRect rect(rectOrig);

    // Make the edit control large enough to allow for internal
    // margins
    //
    // TODO: remove this if the text ctrl sizing is improved esp. for
    // unix
    //
#if defined(__WXGTK__)
    rect.Inflate(rect.x ? 1 : 0, rect.y ? 1 : 0);
#else // !GTK
    int extra_x = ( rect.x > 2 )? 2 : 1;
    int extra_y = ( rect.y > 2 )? 2 : 1;
#if defined(__WXMOTIF__)
    extra_x *= 2;
    extra_y *= 2;
#endif
    rect.SetLeft( wxMax(0, rect.x - extra_x) );
    rect.SetTop( wxMax(0, rect.y - extra_y) );
    rect.SetRight( rect.GetRight() + 2*extra_x );
    rect.SetBottom( rect.GetBottom() + 2*extra_y );
#endif // GTK/!GTK

    wxGridCellEditor::SetSize(rect);
}

void wxGridCellTextEditor::BeginEdit(int row, int col, wxGrid* grid)
{
    wxASSERT_MSG(m_control,
                 wxT("The wxGridCellEditor must be Created first!"));

    m_startValue = grid->GetTable()->GetValue(row, col);

    DoBeginEdit(m_startValue);
}

void wxGridCellTextEditor::DoBeginEdit(const wxString& startValue)
{
    Text()->SetValue(startValue);
    Text()->SetInsertionPointEnd();
    Text()->SetFocus();
}

bool wxGridCellTextEditor::EndEdit(int row, int col,
                                   wxGrid* grid)
{
    wxASSERT_MSG(m_control,
                 wxT("The wxGridCellEditor must be Created first!"));

    bool changed = FALSE;
    wxString value = Text()->GetValue();
    if (value != m_startValue)
        changed = TRUE;

    if (changed)
        grid->GetTable()->SetValue(row, col, value);

    m_startValue = wxEmptyString;
    Text()->SetValue(m_startValue);

    return changed;
}


void wxGridCellTextEditor::Reset()
{
    wxASSERT_MSG(m_control,
                 wxT("The wxGridCellEditor must be Created first!"));

    DoReset(m_startValue);
}

void wxGridCellTextEditor::DoReset(const wxString& startValue)
{
    Text()->SetValue(startValue);
    Text()->SetInsertionPointEnd();
}

void wxGridCellTextEditor::StartingKey(wxKeyEvent& event)
{
    if ( !event.AltDown() && !event.MetaDown() && !event.ControlDown() )
    {
        // insert the key in the control
        long keycode = event.KeyCode();
        if ( isprint(keycode) )
        {
            // FIXME this is not going to work for non letters...
            if ( !event.ShiftDown() )
            {
                keycode = tolower(keycode);
            }

            Text()->AppendText((wxChar)keycode);

            return;
        }

    }

    event.Skip();
}

void wxGridCellTextEditor::HandleReturn(wxKeyEvent& event)
{
#if defined(__WXMOTIF__) || defined(__WXGTK__)
    // wxMotif needs a little extra help...
    int pos = Text()->GetInsertionPoint();
    wxString s( Text()->GetValue() );
    s = s.Left(pos) + "\n" + s.Mid(pos);
    Text()->SetValue(s);
    Text()->SetInsertionPoint( pos );
#else
    // the other ports can handle a Return key press
    //
    event.Skip();
#endif
}

// ----------------------------------------------------------------------------
// wxGridCellNumberEditor
// ----------------------------------------------------------------------------

wxGridCellNumberEditor::wxGridCellNumberEditor(int min, int max)
{
    m_min = min;
    m_max = max;
}

void wxGridCellNumberEditor::Create(wxWindow* parent,
                                    wxWindowID id,
                                    wxEvtHandler* evtHandler)
{
    if ( HasRange() )
    {
        // create a spin ctrl
        m_control = new wxSpinCtrl(parent, -1, wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   wxSP_ARROW_KEYS,
                                   m_min, m_max);

        wxGridCellEditor::Create(parent, id, evtHandler);
    }
    else
    {
        // just a text control
        wxGridCellTextEditor::Create(parent, id, evtHandler);

#if wxUSE_VALIDATORS
        Text()->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
#endif // wxUSE_VALIDATORS
    }
}

void wxGridCellNumberEditor::BeginEdit(int row, int col, wxGrid* grid)
{
    // first get the value
    wxGridTableBase *table = grid->GetTable();
    if ( table->CanGetValueAs(row, col, wxGRID_VALUE_NUMBER) )
    {
        m_valueOld = table->GetValueAsLong(row, col);
    }
    else
    {
        wxString sValue = table->GetValue(row, col);
        if (! sValue.ToLong(&m_valueOld))
        {
            wxFAIL_MSG( _T("this cell doesn't have numeric value") );
            return;
        }
    }

    if ( HasRange() )
    {
        Spin()->SetValue(m_valueOld);
    }
    else
    {
        DoBeginEdit(GetString());
    }
}

bool wxGridCellNumberEditor::EndEdit(int row, int col,
                                     wxGrid* grid)
{
    bool changed;
    long value;

    if ( HasRange() )
    {
        value = Spin()->GetValue();
        changed = value != m_valueOld;
    }
    else
    {
        changed = Text()->GetValue().ToLong(&value) && (value != m_valueOld);
    }

    if ( changed )
    {
        if (grid->GetTable()->CanSetValueAs(row, col, wxGRID_VALUE_NUMBER))
            grid->GetTable()->SetValueAsLong(row, col, value);
        else
            grid->GetTable()->SetValue(row, col, wxString::Format("%ld", value));
    }

    return changed;
}

void wxGridCellNumberEditor::Reset()
{
    if ( HasRange() )
    {
        Spin()->SetValue(m_valueOld);
    }
    else
    {
        DoReset(GetString());
    }
}

void wxGridCellNumberEditor::StartingKey(wxKeyEvent& event)
{
    if ( !HasRange() )
    {
        long keycode = event.KeyCode();
        if ( isdigit(keycode) || keycode == '+' || keycode == '-' )
        {
            wxGridCellTextEditor::StartingKey(event);

            // skip Skip() below
            return;
        }
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// wxGridCellFloatEditor
// ----------------------------------------------------------------------------

void wxGridCellFloatEditor::Create(wxWindow* parent,
                                   wxWindowID id,
                                   wxEvtHandler* evtHandler)
{
    wxGridCellTextEditor::Create(parent, id, evtHandler);

#if wxUSE_VALIDATORS
    Text()->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
#endif // wxUSE_VALIDATORS
}

void wxGridCellFloatEditor::BeginEdit(int row, int col, wxGrid* grid)
{
    // first get the value
    wxGridTableBase *table = grid->GetTable();
    if ( table->CanGetValueAs(row, col, wxGRID_VALUE_FLOAT) )
    {
        m_valueOld = table->GetValueAsDouble(row, col);
    }
    else
    {
        wxString sValue = table->GetValue(row, col);
        if (! sValue.ToDouble(&m_valueOld))
        {
            wxFAIL_MSG( _T("this cell doesn't have float value") );
            return;
        }
    }

    DoBeginEdit(GetString());
}

bool wxGridCellFloatEditor::EndEdit(int row, int col,
                                     wxGrid* grid)
{
    double value;
    if ( Text()->GetValue().ToDouble(&value) && (value != m_valueOld) )
    {
        if (grid->GetTable()->CanSetValueAs(row, col, wxGRID_VALUE_FLOAT))
            grid->GetTable()->SetValueAsDouble(row, col, value);
        else
            grid->GetTable()->SetValue(row, col, wxString::Format("%f", value));

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void wxGridCellFloatEditor::Reset()
{
    DoReset(GetString());
}

void wxGridCellFloatEditor::StartingKey(wxKeyEvent& event)
{
    long keycode = event.KeyCode();
    if ( isdigit(keycode) ||
         keycode == '+' || keycode == '-' || keycode == '.' )
    {
        wxGridCellTextEditor::StartingKey(event);

        // skip Skip() below
        return;
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// wxGridCellBoolEditor
// ----------------------------------------------------------------------------

void wxGridCellBoolEditor::Create(wxWindow* parent,
                                  wxWindowID id,
                                  wxEvtHandler* evtHandler)
{
    m_control = new wxCheckBox(parent, id, wxEmptyString,
                               wxDefaultPosition, wxDefaultSize,
                               wxNO_BORDER);

    wxGridCellEditor::Create(parent, id, evtHandler);
}

void wxGridCellBoolEditor::SetSize(const wxRect& r)
{
    bool resize = FALSE;
    wxSize size = m_control->GetSize();
    wxCoord minSize = wxMin(r.width, r.height);

    // check if the checkbox is not too big/small for this cell
    wxSize sizeBest = m_control->GetBestSize();
    if ( !(size == sizeBest) )
    {
        // reset to default size if it had been made smaller
        size = sizeBest;

        resize = TRUE;
    }

    if ( size.x >= minSize || size.y >= minSize )
    {
        // leave 1 pixel margin
        size.x = size.y = minSize - 2;

        resize = TRUE;
    }

    if ( resize )
    {
        m_control->SetSize(size);
    }

    // position it in the centre of the rectangle (TODO: support alignment?)

#if defined(__WXGTK__) || defined (__WXMOTIF__)
    // the checkbox without label still has some space to the right in wxGTK,
    // so shift it to the right
    size.x -= 8;
#elif defined(__WXMSW__)
    // here too...
    size.x -= 6;
    size.y -= 2;
#endif

    m_control->Move(r.x + r.width/2 - size.x/2, r.y + r.height/2 - size.y/2);
}

void wxGridCellBoolEditor::Show(bool show, wxGridCellAttr *attr)
{
    m_control->Show(show);

    if ( show )
    {
        wxColour colBg = attr ? attr->GetBackgroundColour() : *wxLIGHT_GREY;
        CBox()->SetBackgroundColour(colBg);
    }
}

void wxGridCellBoolEditor::BeginEdit(int row, int col, wxGrid* grid)
{
    wxASSERT_MSG(m_control,
                 wxT("The wxGridCellEditor must be Created first!"));

    if (grid->GetTable()->CanGetValueAs(row, col, wxGRID_VALUE_BOOL))
        m_startValue = grid->GetTable()->GetValueAsBool(row, col);
    else
        m_startValue = !!grid->GetTable()->GetValue(row, col);
    CBox()->SetValue(m_startValue);
    CBox()->SetFocus();
}

bool wxGridCellBoolEditor::EndEdit(int row, int col,
                                   wxGrid* grid)
{
    wxASSERT_MSG(m_control,
                 wxT("The wxGridCellEditor must be Created first!"));

    bool changed = FALSE;
    bool value = CBox()->GetValue();
    if ( value != m_startValue )
        changed = TRUE;

    if ( changed )
    {
        if (grid->GetTable()->CanGetValueAs(row, col, wxGRID_VALUE_BOOL))
            grid->GetTable()->SetValueAsBool(row, col, value);
        else
            grid->GetTable()->SetValue(row, col, value ? _T("1") : wxEmptyString);
    }

    return changed;
}

void wxGridCellBoolEditor::Reset()
{
    wxASSERT_MSG(m_control,
                 wxT("The wxGridCellEditor must be Created first!"));

    CBox()->SetValue(m_startValue);
}

void wxGridCellBoolEditor::StartingClick()
{
    CBox()->SetValue(!CBox()->GetValue());
}

// ----------------------------------------------------------------------------
// wxGridCellChoiceEditor
// ----------------------------------------------------------------------------

wxGridCellChoiceEditor::wxGridCellChoiceEditor(size_t count,
                                               const wxChar* choices[],
                                               bool allowOthers)
                      : m_allowOthers(allowOthers)
{
    m_choices.Alloc(count);
    for ( size_t n = 0; n < count; n++ )
    {
        m_choices.Add(choices[n]);
    }
}

void wxGridCellChoiceEditor::Create(wxWindow* parent,
                                    wxWindowID id,
                                    wxEvtHandler* evtHandler)
{
    size_t count = m_choices.GetCount();
    wxString *choices = new wxString[count];
    for ( size_t n = 0; n < count; n++ )
    {
        choices[n] = m_choices[n];
    }

    m_control = new wxComboBox(parent, id, wxEmptyString,
                               wxDefaultPosition, wxDefaultSize,
                               count, choices,
                               m_allowOthers ? 0 : wxCB_READONLY);

    delete [] choices;

    wxGridCellEditor::Create(parent, id, evtHandler);
}

void wxGridCellChoiceEditor::PaintBackground(const wxRect& rectCell,
                                             wxGridCellAttr * attr)
{
    // as we fill the entire client area, don't do anything here to minimize
    // flicker

    // TODO: It doesn't actually fill the client area since the height of a
    // combo always defaults to the standard...  Until someone has time to
    // figure out the right rectangle to paint, just do it the normal way...
    wxGridCellEditor::PaintBackground(rectCell, attr);
}

void wxGridCellChoiceEditor::BeginEdit(int row, int col, wxGrid* grid)
{
    wxASSERT_MSG(m_control,
                 wxT("The wxGridCellEditor must be Created first!"));

    m_startValue = grid->GetTable()->GetValue(row, col);

    Combo()->SetValue(m_startValue);
    size_t count = m_choices.GetCount();
    for (size_t i=0; i<count; i++)
    {
        if (m_startValue == m_choices[i])
        {
            Combo()->SetSelection(i);
            break;
        }
    }
    Combo()->SetInsertionPointEnd();
    Combo()->SetFocus();
}

bool wxGridCellChoiceEditor::EndEdit(int row, int col,
                                     wxGrid* grid)
{
    wxString value = Combo()->GetValue();
    bool changed = value != m_startValue;

    if ( changed )
        grid->GetTable()->SetValue(row, col, value);

    m_startValue = wxEmptyString;
    Combo()->SetValue(m_startValue);

    return changed;
}

void wxGridCellChoiceEditor::Reset()
{
    Combo()->SetValue(m_startValue);
    Combo()->SetInsertionPointEnd();
}

// ----------------------------------------------------------------------------
// wxGridCellEditorEvtHandler
// ----------------------------------------------------------------------------

void wxGridCellEditorEvtHandler::OnKeyDown(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
        case WXK_ESCAPE:
            m_editor->Reset();
            m_grid->DisableCellEditControl();
            break;

        case WXK_TAB:
            event.Skip( m_grid->ProcessEvent( event ) );
            break;

        case WXK_RETURN:
            if (!m_grid->ProcessEvent(event))
                m_editor->HandleReturn(event);
            break;


        default:
            event.Skip();
    }
}

void wxGridCellEditorEvtHandler::OnChar(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
        case WXK_ESCAPE:
        case WXK_TAB:
        case WXK_RETURN:
            break;

        default:
            event.Skip();
    }
}

// ============================================================================
// renderer classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxGridCellRenderer
// ----------------------------------------------------------------------------

void wxGridCellRenderer::Draw(wxGrid& grid,
                              wxGridCellAttr& attr,
                              wxDC& dc,
                              const wxRect& rect,
                              int row, int col,
                              bool isSelected)
{
    dc.SetBackgroundMode( wxSOLID );

    if ( isSelected )
    {
        dc.SetBrush( wxBrush(grid.GetSelectionBackground(), wxSOLID) );
    }
    else
    {
        dc.SetBrush( wxBrush(attr.GetBackgroundColour(), wxSOLID) );
    }

    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.DrawRectangle(rect);
}

wxGridCellRenderer::~wxGridCellRenderer()
{
}

// ----------------------------------------------------------------------------
// wxGridCellStringRenderer
// ----------------------------------------------------------------------------

void wxGridCellStringRenderer::SetTextColoursAndFont(wxGrid& grid,
                                                     wxGridCellAttr& attr,
                                                     wxDC& dc,
                                                     bool isSelected)
{
    dc.SetBackgroundMode( wxTRANSPARENT );

    // TODO some special colours for attr.IsReadOnly() case?

    if ( isSelected )
    {
        dc.SetTextBackground( grid.GetSelectionBackground() );
        dc.SetTextForeground( grid.GetSelectionForeground() );
    }
    else
    {
        dc.SetTextBackground( attr.GetBackgroundColour() );
        dc.SetTextForeground( attr.GetTextColour() );
    }

    dc.SetFont( attr.GetFont() );
}

wxSize wxGridCellStringRenderer::DoGetBestSize(wxGridCellAttr& attr,
                                               wxDC& dc,
                                               const wxString& text)
{
    wxCoord x, y;
    dc.SetFont(attr.GetFont());
    dc.GetTextExtent(text, &x, &y);

    return wxSize(x, y);
}

wxSize wxGridCellStringRenderer::GetBestSize(wxGrid& grid,
                                             wxGridCellAttr& attr,
                                             wxDC& dc,
                                             int row, int col)
{
    return DoGetBestSize(attr, dc, grid.GetCellValue(row, col));
}

void wxGridCellStringRenderer::Draw(wxGrid& grid,
                                    wxGridCellAttr& attr,
                                    wxDC& dc,
                                    const wxRect& rectCell,
                                    int row, int col,
                                    bool isSelected)
{
    wxGridCellRenderer::Draw(grid, attr, dc, rectCell, row, col, isSelected);

    // now we only have to draw the text
    SetTextColoursAndFont(grid, attr, dc, isSelected);

    int hAlign, vAlign;
    attr.GetAlignment(&hAlign, &vAlign);

    wxRect rect = rectCell;
    rect.Inflate(-1);

    grid.DrawTextRectangle(dc, grid.GetCellValue(row, col),
                           rect, hAlign, vAlign);
}

// ----------------------------------------------------------------------------
// wxGridCellNumberRenderer
// ----------------------------------------------------------------------------

wxString wxGridCellNumberRenderer::GetString(wxGrid& grid, int row, int col)
{
    wxGridTableBase *table = grid.GetTable();
    wxString text;
    if ( table->CanGetValueAs(row, col, wxGRID_VALUE_NUMBER) )
    {
        text.Printf(_T("%ld"), table->GetValueAsLong(row, col));
    }
    else
    {
        text = table->GetValue(row, col);
    }

    return text;
}

void wxGridCellNumberRenderer::Draw(wxGrid& grid,
                                    wxGridCellAttr& attr,
                                    wxDC& dc,
                                    const wxRect& rectCell,
                                    int row, int col,
                                    bool isSelected)
{
    wxGridCellRenderer::Draw(grid, attr, dc, rectCell, row, col, isSelected);

    SetTextColoursAndFont(grid, attr, dc, isSelected);

    // draw the text right aligned by default
    int hAlign, vAlign;
    attr.GetAlignment(&hAlign, &vAlign);
    hAlign = wxRIGHT;

    wxRect rect = rectCell;
    rect.Inflate(-1);

    grid.DrawTextRectangle(dc, GetString(grid, row, col), rect, hAlign, vAlign);
}

wxSize wxGridCellNumberRenderer::GetBestSize(wxGrid& grid,
                                             wxGridCellAttr& attr,
                                             wxDC& dc,
                                             int row, int col)
{
    return DoGetBestSize(attr, dc, GetString(grid, row, col));
}

// ----------------------------------------------------------------------------
// wxGridCellFloatRenderer
// ----------------------------------------------------------------------------

wxGridCellFloatRenderer::wxGridCellFloatRenderer(int width, int precision)
{
    SetWidth(width);
    SetPrecision(precision);
}

wxString wxGridCellFloatRenderer::GetString(wxGrid& grid, int row, int col)
{
    wxGridTableBase *table = grid.GetTable();
    wxString text;
    if ( table->CanGetValueAs(row, col, wxGRID_VALUE_FLOAT) )
    {
        if ( !m_format )
        {
            m_format.Printf(_T("%%%d.%d%%f"), m_width, m_precision);
        }

        text.Printf(m_format, table->GetValueAsDouble(row, col));
    }
    else
    {
        text = table->GetValue(row, col);
    }

    return text;
}

void wxGridCellFloatRenderer::Draw(wxGrid& grid,
                                   wxGridCellAttr& attr,
                                   wxDC& dc,
                                   const wxRect& rectCell,
                                   int row, int col,
                                   bool isSelected)
{
    wxGridCellRenderer::Draw(grid, attr, dc, rectCell, row, col, isSelected);

    SetTextColoursAndFont(grid, attr, dc, isSelected);

    // draw the text right aligned by default
    int hAlign, vAlign;
    attr.GetAlignment(&hAlign, &vAlign);
    hAlign = wxRIGHT;

    wxRect rect = rectCell;
    rect.Inflate(-1);

    grid.DrawTextRectangle(dc, GetString(grid, row, col), rect, hAlign, vAlign);
}

wxSize wxGridCellFloatRenderer::GetBestSize(wxGrid& grid,
                                            wxGridCellAttr& attr,
                                            wxDC& dc,
                                            int row, int col)
{
    return DoGetBestSize(attr, dc, GetString(grid, row, col));
}

// ----------------------------------------------------------------------------
// wxGridCellBoolRenderer
// ----------------------------------------------------------------------------

wxSize wxGridCellBoolRenderer::ms_sizeCheckMark;

// FIXME these checkbox size calculations are really ugly...

// between checkmark and box
#ifdef __WXGTK__
    static const wxCoord wxGRID_CHECKMARK_MARGIN = 4;
#else
    static const wxCoord wxGRID_CHECKMARK_MARGIN = 2;
#endif

wxSize wxGridCellBoolRenderer::GetBestSize(wxGrid& grid,
                                           wxGridCellAttr& WXUNUSED(attr),
                                           wxDC& WXUNUSED(dc),
                                           int WXUNUSED(row),
                                           int WXUNUSED(col))
{
    // compute it only once (no locks for MT safeness in GUI thread...)
    if ( !ms_sizeCheckMark.x )
    {
        // get checkbox size
        wxCoord checkSize = 0;
        wxCheckBox *checkbox = new wxCheckBox(&grid, -1, wxEmptyString);
        wxSize size = checkbox->GetBestSize();
        checkSize = size.y + wxGRID_CHECKMARK_MARGIN;

        // FIXME wxGTK::wxCheckBox::GetBestSize() gives "wrong" result
#if defined(__WXGTK__) || defined(__WXMOTIF__)
        checkSize -= size.y / 2;
#endif

        delete checkbox;

        ms_sizeCheckMark.x = ms_sizeCheckMark.y = checkSize;
    }

    return ms_sizeCheckMark;
}

void wxGridCellBoolRenderer::Draw(wxGrid& grid,
                                  wxGridCellAttr& attr,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int row, int col,
                                  bool isSelected)
{
    wxGridCellRenderer::Draw(grid, attr, dc, rect, row, col, isSelected);

    // draw a check mark in the centre (ignoring alignment - TODO)
    wxSize size = GetBestSize(grid, attr, dc, row, col);

    // don't draw outside the cell
    wxCoord minSize = wxMin(rect.width, rect.height);
    if ( size.x >= minSize || size.y >= minSize )
    {
        // and even leave (at least) 1 pixel margin
        size.x = size.y = minSize - 2;
    }

    // draw a border around checkmark
    wxRect rectMark;
    rectMark.x = rect.x + rect.width/2 - size.x/2;
    rectMark.y = rect.y + rect.height/2 - size.y/2;
    rectMark.width = size.x;
    rectMark.height = size.y;

    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(wxPen(attr.GetTextColour(), 1, wxSOLID));
    dc.DrawRectangle(rectMark);

    rectMark.Inflate(-wxGRID_CHECKMARK_MARGIN);

#ifdef __WXMSW__
    // looks nicer under MSW
    rectMark.x++;
#endif // MSW

    bool value;
    if ( grid.GetTable()->CanGetValueAs(row, col, wxGRID_VALUE_BOOL) )
        value = grid.GetTable()->GetValueAsBool(row, col);
    else
        value = !!grid.GetTable()->GetValue(row, col);

    if ( value )
    {
        dc.SetTextForeground(attr.GetTextColour());
        dc.DrawCheckMark(rectMark);
    }
}

// ----------------------------------------------------------------------------
// wxGridCellAttr
// ----------------------------------------------------------------------------

const wxColour& wxGridCellAttr::GetTextColour() const
{
    if (HasTextColour())
    {
        return m_colText;
    }
    else if (m_defGridAttr != this)
    {
        return m_defGridAttr->GetTextColour();
    }
    else
    {
        wxFAIL_MSG(wxT("Missing default cell attribute"));
        return wxNullColour;
    }
}


const wxColour& wxGridCellAttr::GetBackgroundColour() const
{
    if (HasBackgroundColour())
        return m_colBack;
    else if (m_defGridAttr != this)
        return m_defGridAttr->GetBackgroundColour();
    else
    {
        wxFAIL_MSG(wxT("Missing default cell attribute"));
        return wxNullColour;
    }
}


const wxFont& wxGridCellAttr::GetFont() const
{
    if (HasFont())
        return m_font;
    else if (m_defGridAttr != this)
        return m_defGridAttr->GetFont();
    else
    {
        wxFAIL_MSG(wxT("Missing default cell attribute"));
        return wxNullFont;
    }
}


void wxGridCellAttr::GetAlignment(int *hAlign, int *vAlign) const
{
    if (HasAlignment())
    {
        if ( hAlign ) *hAlign = m_hAlign;
        if ( vAlign ) *vAlign = m_vAlign;
    }
    else if (m_defGridAttr != this)
        m_defGridAttr->GetAlignment(hAlign, vAlign);
    else
    {
        wxFAIL_MSG(wxT("Missing default cell attribute"));
    }
}


// GetRenderer and GetEditor use a slightly different decision path about
// which attribute to use.  If a non-default attr object has one then it is
// used, otherwise the default editor or renderer is fetched from the grid and
// used.  It should be the default for the data type of the cell.  If it is
// NULL (because the table has a type that the grid does not have in its
// registry,) then the grid's default editor or renderer is used.

wxGridCellRenderer* wxGridCellAttr::GetRenderer(wxGrid* grid, int row, int col) const
{
    if ((m_defGridAttr != this || grid == NULL) && HasRenderer())
        return m_renderer;      // use local attribute

    wxGridCellRenderer* renderer = NULL;
    if (grid)                   // get renderer for the data type
        renderer =  grid->GetDefaultRendererForCell(row, col);

    if (! renderer)
        // if we still don't have one then use the grid default
        renderer = m_defGridAttr->GetRenderer(NULL,0,0);

    if (! renderer)
        wxFAIL_MSG(wxT("Missing default cell attribute"));

    return renderer;
}

wxGridCellEditor* wxGridCellAttr::GetEditor(wxGrid* grid, int row, int col) const
{
    if ((m_defGridAttr != this || grid == NULL) && HasEditor())
        return m_editor;      // use local attribute

    wxGridCellEditor* editor = NULL;
    if (grid)                   // get renderer for the data type
        editor =  grid->GetDefaultEditorForCell(row, col);

    if (! editor)
        // if we still don't have one then use the grid default
        editor = m_defGridAttr->GetEditor(NULL,0,0);

    if (! editor)
        wxFAIL_MSG(wxT("Missing default cell attribute"));

    return editor;
}

// ----------------------------------------------------------------------------
// wxGridCellAttrData
// ----------------------------------------------------------------------------

void wxGridCellAttrData::SetAttr(wxGridCellAttr *attr, int row, int col)
{
    int n = FindIndex(row, col);
    if ( n == wxNOT_FOUND )
    {
        // add the attribute
        m_attrs.Add(new wxGridCellWithAttr(row, col, attr));
    }
    else
    {
        if ( attr )
        {
            // change the attribute
            m_attrs[(size_t)n].attr = attr;
        }
        else
        {
            // remove this attribute
            m_attrs.RemoveAt((size_t)n);
        }
    }
}

wxGridCellAttr *wxGridCellAttrData::GetAttr(int row, int col) const
{
    wxGridCellAttr *attr = (wxGridCellAttr *)NULL;

    int n = FindIndex(row, col);
    if ( n != wxNOT_FOUND )
    {
        attr = m_attrs[(size_t)n].attr;
        attr->IncRef();
    }

    return attr;
}

void wxGridCellAttrData::UpdateAttrRows( size_t pos, int numRows )
{
    size_t count = m_attrs.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxGridCellCoords& coords = m_attrs[n].coords;
        wxCoord row = coords.GetRow();
        if ((size_t)row >= pos)
        {
            if (numRows > 0)
            {
                // If rows inserted, include row counter where necessary
                coords.SetRow(row + numRows);
            }
            else if (numRows < 0)
            {
                // If rows deleted ...
                if ((size_t)row >= pos - numRows)
                {
                    // ...either decrement row counter (if row still exists)...
                    coords.SetRow(row + numRows);
                }
                else
                {
                    // ...or remove the attribute
                    m_attrs.RemoveAt((size_t)n);
                    n--; count--;
                }
            }
        }
    }
}

void wxGridCellAttrData::UpdateAttrCols( size_t pos, int numCols )
{
    size_t count = m_attrs.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxGridCellCoords& coords = m_attrs[n].coords;
        wxCoord col = coords.GetCol();
        if ( (size_t)col >= pos )
        {
            if ( numCols > 0 )
            {
                // If rows inserted, include row counter where necessary
                coords.SetCol(col + numCols);
            }
            else if (numCols < 0)
            {
                // If rows deleted ...
                if ((size_t)col >= pos - numCols)
                {
                    // ...either decrement row counter (if row still exists)...
                    coords.SetCol(col + numCols);
                }
                else
                {
                    // ...or remove the attribute
                    m_attrs.RemoveAt((size_t)n);
                    n--; count--;
                }
            }
        }
    }
}

int wxGridCellAttrData::FindIndex(int row, int col) const
{
    size_t count = m_attrs.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        const wxGridCellCoords& coords = m_attrs[n].coords;
        if ( (coords.GetRow() == row) && (coords.GetCol() == col) )
        {
            return n;
        }
    }

    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// wxGridRowOrColAttrData
// ----------------------------------------------------------------------------

wxGridRowOrColAttrData::~wxGridRowOrColAttrData()
{
    size_t count = m_attrs.Count();
    for ( size_t n = 0; n < count; n++ )
    {
        m_attrs[n]->DecRef();
    }
}

wxGridCellAttr *wxGridRowOrColAttrData::GetAttr(int rowOrCol) const
{
    wxGridCellAttr *attr = (wxGridCellAttr *)NULL;

    int n = m_rowsOrCols.Index(rowOrCol);
    if ( n != wxNOT_FOUND )
    {
        attr = m_attrs[(size_t)n];
        attr->IncRef();
    }

    return attr;
}

void wxGridRowOrColAttrData::SetAttr(wxGridCellAttr *attr, int rowOrCol)
{
    int n = m_rowsOrCols.Index(rowOrCol);
    if ( n == wxNOT_FOUND )
    {
        // add the attribute
        m_rowsOrCols.Add(rowOrCol);
        m_attrs.Add(attr);
    }
    else
    {
        if ( attr )
        {
            // change the attribute
            m_attrs[(size_t)n] = attr;
        }
        else
        {
            // remove this attribute
            m_attrs[(size_t)n]->DecRef();
            m_rowsOrCols.RemoveAt((size_t)n);
            m_attrs.RemoveAt((size_t)n);
        }
    }
}

void wxGridRowOrColAttrData::UpdateAttrRowsOrCols( size_t pos, int numRowsOrCols )
{
    size_t count = m_attrs.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        int & rowOrCol = m_rowsOrCols[n];
        if ( (size_t)rowOrCol >= pos )
        {
            if ( numRowsOrCols > 0 )
            {
                // If rows inserted, include row counter where necessary
                rowOrCol += numRowsOrCols;
            }
            else if ( numRowsOrCols < 0)
            {
                // If rows deleted, either decrement row counter (if row still exists)
                if ((size_t)rowOrCol >= pos - numRowsOrCols)
                    rowOrCol += numRowsOrCols;
                else
                {
                    m_rowsOrCols.RemoveAt((size_t)n);
                    m_attrs.RemoveAt((size_t)n);
                    n--; count--;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// wxGridCellAttrProvider
// ----------------------------------------------------------------------------

wxGridCellAttrProvider::wxGridCellAttrProvider()
{
    m_data = (wxGridCellAttrProviderData *)NULL;
}

wxGridCellAttrProvider::~wxGridCellAttrProvider()
{
    delete m_data;
}

void wxGridCellAttrProvider::InitData()
{
    m_data = new wxGridCellAttrProviderData;
}

wxGridCellAttr *wxGridCellAttrProvider::GetAttr(int row, int col) const
{
    wxGridCellAttr *attr = (wxGridCellAttr *)NULL;
    if ( m_data )
    {
        // first look for the attribute of this specific cell
        attr = m_data->m_cellAttrs.GetAttr(row, col);

        if ( !attr )
        {
            // then look for the col attr (col attributes are more common than
            // the row ones, hence they have priority)
            attr = m_data->m_colAttrs.GetAttr(col);
        }

        if ( !attr )
        {
            // finally try the row attributes
            attr = m_data->m_rowAttrs.GetAttr(row);
        }
    }

    return attr;
}

void wxGridCellAttrProvider::SetAttr(wxGridCellAttr *attr,
                                     int row, int col)
{
    if ( !m_data )
        InitData();

    m_data->m_cellAttrs.SetAttr(attr, row, col);
}

void wxGridCellAttrProvider::SetRowAttr(wxGridCellAttr *attr, int row)
{
    if ( !m_data )
        InitData();

    m_data->m_rowAttrs.SetAttr(attr, row);
}

void wxGridCellAttrProvider::SetColAttr(wxGridCellAttr *attr, int col)
{
    if ( !m_data )
        InitData();

    m_data->m_colAttrs.SetAttr(attr, col);
}

void wxGridCellAttrProvider::UpdateAttrRows( size_t pos, int numRows )
{
    if ( m_data )
    {
        m_data->m_cellAttrs.UpdateAttrRows( pos, numRows );

        m_data->m_rowAttrs.UpdateAttrRowsOrCols( pos, numRows );
    }
}

void wxGridCellAttrProvider::UpdateAttrCols( size_t pos, int numCols )
{
    if ( m_data )
    {
        m_data->m_cellAttrs.UpdateAttrCols( pos, numCols );

        m_data->m_colAttrs.UpdateAttrRowsOrCols( pos, numCols );
    }
}

// ----------------------------------------------------------------------------
// wxGridTypeRegistry
// ----------------------------------------------------------------------------

wxGridTypeRegistry::~wxGridTypeRegistry()
{
    size_t count = m_typeinfo.Count();
    for ( size_t i = 0; i < count; i++ )
        delete m_typeinfo[i];
}


void wxGridTypeRegistry::RegisterDataType(const wxString& typeName,
                                          wxGridCellRenderer* renderer,
                                          wxGridCellEditor* editor)
{
    int loc;
    wxGridDataTypeInfo* info = new wxGridDataTypeInfo(typeName, renderer, editor);

    // is it already registered?
    if ((loc = FindDataType(typeName)) != -1) {
        delete m_typeinfo[loc];
        m_typeinfo[loc] = info;
    }
    else {
        m_typeinfo.Add(info);
    }
}

int wxGridTypeRegistry::FindDataType(const wxString& typeName)
{
    int found = -1;

    for (size_t i=0; i<m_typeinfo.Count(); i++) {
        if (typeName == m_typeinfo[i]->m_typeName) {
            found = i;
            break;
        }
    }

    return found;
}

wxGridCellRenderer* wxGridTypeRegistry::GetRenderer(int index)
{
    wxGridCellRenderer* renderer = m_typeinfo[index]->m_renderer;
    return renderer;
}

wxGridCellEditor*   wxGridTypeRegistry::GetEditor(int index)
{
    wxGridCellEditor* editor = m_typeinfo[index]->m_editor;
    return editor;
}

// ----------------------------------------------------------------------------
// wxGridTableBase
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS( wxGridTableBase, wxObject )


wxGridTableBase::wxGridTableBase()
{
    m_view = (wxGrid *) NULL;
    m_attrProvider = (wxGridCellAttrProvider *) NULL;
}

wxGridTableBase::~wxGridTableBase()
{
    delete m_attrProvider;
}

void wxGridTableBase::SetAttrProvider(wxGridCellAttrProvider *attrProvider)
{
    delete m_attrProvider;
    m_attrProvider = attrProvider;
}

bool wxGridTableBase::CanHaveAttributes()
{
    if ( ! GetAttrProvider() )
    {
        // use the default attr provider by default
        SetAttrProvider(new wxGridCellAttrProvider);
    }
    return TRUE;
}

wxGridCellAttr *wxGridTableBase::GetAttr(int row, int col)
{
    if ( m_attrProvider )
        return m_attrProvider->GetAttr(row, col);
    else
        return (wxGridCellAttr *)NULL;
}

void wxGridTableBase::SetAttr(wxGridCellAttr* attr, int row, int col)
{
    if ( m_attrProvider )
    {
        m_attrProvider->SetAttr(attr, row, col);
    }
    else
    {
        // as we take ownership of the pointer and don't store it, we must
        // free it now
        attr->SafeDecRef();
    }
}

void wxGridTableBase::SetRowAttr(wxGridCellAttr *attr, int row)
{
    if ( m_attrProvider )
    {
        m_attrProvider->SetRowAttr(attr, row);
    }
    else
    {
        // as we take ownership of the pointer and don't store it, we must
        // free it now
        attr->SafeDecRef();
    }
}

void wxGridTableBase::SetColAttr(wxGridCellAttr *attr, int col)
{
    if ( m_attrProvider )
    {
        m_attrProvider->SetColAttr(attr, col);
    }
    else
    {
        // as we take ownership of the pointer and don't store it, we must
        // free it now
        attr->SafeDecRef();
    }
}

void wxGridTableBase::UpdateAttrRows( size_t pos, int numRows )
{
    if ( m_attrProvider )
    {
        m_attrProvider->UpdateAttrRows( pos, numRows );
    }
}

void wxGridTableBase::UpdateAttrCols( size_t pos, int numCols )
{
    if ( m_attrProvider )
    {
        m_attrProvider->UpdateAttrCols( pos, numCols );
    }
}

bool wxGridTableBase::InsertRows( size_t pos, size_t numRows )
{
    wxFAIL_MSG( wxT("Called grid table class function InsertRows\n"
                    "but your derived table class does not override this function") );

    return FALSE;
}

bool wxGridTableBase::AppendRows( size_t numRows )
{
    wxFAIL_MSG( wxT("Called grid table class function AppendRows\n"
                    "but your derived table class does not override this function"));

    return FALSE;
}

bool wxGridTableBase::DeleteRows( size_t pos, size_t numRows )
{
    wxFAIL_MSG( wxT("Called grid table class function DeleteRows\n"
                    "but your derived table class does not override this function"));

    return FALSE;
}

bool wxGridTableBase::InsertCols( size_t pos, size_t numCols )
{
    wxFAIL_MSG( wxT("Called grid table class function InsertCols\n"
                  "but your derived table class does not override this function"));

    return FALSE;
}

bool wxGridTableBase::AppendCols( size_t numCols )
{
    wxFAIL_MSG(wxT("Called grid table class function AppendCols\n"
                   "but your derived table class does not override this function"));

    return FALSE;
}

bool wxGridTableBase::DeleteCols( size_t pos, size_t numCols )
{
    wxFAIL_MSG( wxT("Called grid table class function DeleteCols\n"
                    "but your derived table class does not override this function"));

    return FALSE;
}


wxString wxGridTableBase::GetRowLabelValue( int row )
{
    wxString s;
    s << row + 1; // RD: Starting the rows at zero confuses users, no matter
                  //     how much it makes sense to us geeks.
    return s;
}

wxString wxGridTableBase::GetColLabelValue( int col )
{
    // default col labels are:
    //   cols 0 to 25   : A-Z
    //   cols 26 to 675 : AA-ZZ
    //   etc.

    wxString s;
    unsigned int i, n;
    for ( n = 1; ; n++ )
    {
        s += (_T('A') + (wxChar)( col%26 ));
        col = col/26 - 1;
        if ( col < 0 ) break;
    }

    // reverse the string...
    wxString s2;
    for ( i = 0;  i < n;  i++ )
    {
        s2 += s[n-i-1];
    }

    return s2;
}


wxString wxGridTableBase::GetTypeName( int WXUNUSED(row), int WXUNUSED(col) )
{
    return wxGRID_VALUE_STRING;
}

bool wxGridTableBase::CanGetValueAs( int WXUNUSED(row), int WXUNUSED(col),
                                     const wxString& typeName )
{
    return typeName == wxGRID_VALUE_STRING;
}

bool wxGridTableBase::CanSetValueAs( int row, int col, const wxString& typeName )
{
    return CanGetValueAs(row, col, typeName);
}

long wxGridTableBase::GetValueAsLong( int WXUNUSED(row), int WXUNUSED(col) )
{
    return 0;
}

double wxGridTableBase::GetValueAsDouble( int WXUNUSED(row), int WXUNUSED(col) )
{
    return 0.0;
}

bool wxGridTableBase::GetValueAsBool( int WXUNUSED(row), int WXUNUSED(col) )
{
    return FALSE;
}

void wxGridTableBase::SetValueAsLong( int WXUNUSED(row), int WXUNUSED(col),
                                      long WXUNUSED(value) )
{
}

void wxGridTableBase::SetValueAsDouble( int WXUNUSED(row), int WXUNUSED(col),
                                        double WXUNUSED(value) )
{
}

void wxGridTableBase::SetValueAsBool( int WXUNUSED(row), int WXUNUSED(col),
                                      bool WXUNUSED(value) )
{
}


void* wxGridTableBase::GetValueAsCustom( int WXUNUSED(row), int WXUNUSED(col),
                                         const wxString& WXUNUSED(typeName) )
{
    return NULL;
}

void  wxGridTableBase::SetValueAsCustom( int WXUNUSED(row), int WXUNUSED(col),
                                         const wxString& WXUNUSED(typeName),
                                         void* WXUNUSED(value) )
{
}


//////////////////////////////////////////////////////////////////////
//
// Message class for the grid table to send requests and notifications
// to the grid view
//

wxGridTableMessage::wxGridTableMessage()
{
    m_table = (wxGridTableBase *) NULL;
    m_id = -1;
    m_comInt1 = -1;
    m_comInt2 = -1;
}

wxGridTableMessage::wxGridTableMessage( wxGridTableBase *table, int id,
                                        int commandInt1, int commandInt2 )
{
    m_table = table;
    m_id = id;
    m_comInt1 = commandInt1;
    m_comInt2 = commandInt2;
}



//////////////////////////////////////////////////////////////////////
//
// A basic grid table for string data. An object of this class will
// created by wxGrid if you don't specify an alternative table class.
//

WX_DEFINE_OBJARRAY(wxGridStringArray)

IMPLEMENT_DYNAMIC_CLASS( wxGridStringTable, wxGridTableBase )

wxGridStringTable::wxGridStringTable()
        : wxGridTableBase()
{
}

wxGridStringTable::wxGridStringTable( int numRows, int numCols )
        : wxGridTableBase()
{
    int row, col;

    m_data.Alloc( numRows );

    wxArrayString sa;
    sa.Alloc( numCols );
    for ( col = 0;  col < numCols;  col++ )
    {
        sa.Add( wxEmptyString );
    }

    for ( row = 0;  row < numRows;  row++ )
    {
        m_data.Add( sa );
    }
}

wxGridStringTable::~wxGridStringTable()
{
}

long wxGridStringTable::GetNumberRows()
{
    return m_data.GetCount();
}

long wxGridStringTable::GetNumberCols()
{
    if ( m_data.GetCount() > 0 )
        return m_data[0].GetCount();
    else
        return 0;
}

wxString wxGridStringTable::GetValue( int row, int col )
{
    // TODO: bounds checking
    //
    return m_data[row][col];
}

void wxGridStringTable::SetValue( int row, int col, const wxString& value )
{
    // TODO: bounds checking
    //
    m_data[row][col] = value;
}

bool wxGridStringTable::IsEmptyCell( int row, int col )
{
    // TODO: bounds checking
    //
    return (m_data[row][col] == wxEmptyString);
}


void wxGridStringTable::Clear()
{
    int row, col;
    int numRows, numCols;

    numRows = m_data.GetCount();
    if ( numRows > 0 )
    {
        numCols = m_data[0].GetCount();

        for ( row = 0;  row < numRows;  row++ )
        {
            for ( col = 0;  col < numCols;  col++ )
            {
                m_data[row][col] = wxEmptyString;
            }
        }
    }
}


bool wxGridStringTable::InsertRows( size_t pos, size_t numRows )
{
    size_t row, col;

    size_t curNumRows = m_data.GetCount();
    size_t curNumCols = ( curNumRows > 0 ? m_data[0].GetCount() : 0 );

    if ( pos >= curNumRows )
    {
        return AppendRows( numRows );
    }

    wxArrayString sa;
    sa.Alloc( curNumCols );
    for ( col = 0;  col < curNumCols;  col++ )
    {
        sa.Add( wxEmptyString );
    }

    for ( row = pos;  row < pos + numRows;  row++ )
    {
        m_data.Insert( sa, row );
    }
    UpdateAttrRows( pos, numRows );
    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
                                pos,
                                numRows );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

bool wxGridStringTable::AppendRows( size_t numRows )
{
    size_t row, col;

    size_t curNumRows = m_data.GetCount();
    size_t curNumCols = ( curNumRows > 0 ? m_data[0].GetCount() : 0 );

    wxArrayString sa;
    if ( curNumCols > 0 )
    {
        sa.Alloc( curNumCols );
        for ( col = 0;  col < curNumCols;  col++ )
        {
            sa.Add( wxEmptyString );
        }
    }

    for ( row = 0;  row < numRows;  row++ )
    {
        m_data.Add( sa );
    }

    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_ROWS_APPENDED,
                                numRows );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

bool wxGridStringTable::DeleteRows( size_t pos, size_t numRows )
{
    size_t n;

    size_t curNumRows = m_data.GetCount();

    if ( pos >= curNumRows )
    {
        wxString errmsg;
        errmsg.Printf("Called wxGridStringTable::DeleteRows(pos=%d, N=%d)\n"
                      "Pos value is invalid for present table with %d rows",
                      pos, numRows, curNumRows );
        wxFAIL_MSG( wxT(errmsg) );
        return FALSE;
    }

    if ( numRows > curNumRows - pos )
    {
        numRows = curNumRows - pos;
    }

    if ( numRows >= curNumRows )
    {
        m_data.Empty();  // don't release memory just yet
    }
    else
    {
        for ( n = 0;  n < numRows;  n++ )
        {
            m_data.Remove( pos );
        }
    }
    UpdateAttrRows( pos, -((int)numRows) );
    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_ROWS_DELETED,
                                pos,
                                numRows );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

bool wxGridStringTable::InsertCols( size_t pos, size_t numCols )
{
    size_t row, col;

    size_t curNumRows = m_data.GetCount();
    size_t curNumCols = ( curNumRows > 0 ? m_data[0].GetCount() : 0 );

    if ( pos >= curNumCols )
    {
        return AppendCols( numCols );
    }

    for ( row = 0;  row < curNumRows;  row++ )
    {
        for ( col = pos;  col < pos + numCols;  col++ )
        {
            m_data[row].Insert( wxEmptyString, col );
        }
    }
    UpdateAttrCols( pos, numCols );
    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_COLS_INSERTED,
                                pos,
                                numCols );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

bool wxGridStringTable::AppendCols( size_t numCols )
{
    size_t row, n;

    size_t curNumRows = m_data.GetCount();
    if ( !curNumRows )
    {
        // TODO: something better than this ?
        //
        wxFAIL_MSG( wxT("Unable to append cols to a grid table with no rows.\n"
                        "Call AppendRows() first") );
        return FALSE;
    }

    for ( row = 0;  row < curNumRows;  row++ )
    {
        for ( n = 0;  n < numCols;  n++ )
        {
            m_data[row].Add( wxEmptyString );
        }
    }

    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_COLS_APPENDED,
                                numCols );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

bool wxGridStringTable::DeleteCols( size_t pos, size_t numCols )
{
    size_t row, n;

    size_t curNumRows = m_data.GetCount();
    size_t curNumCols = ( curNumRows > 0 ? m_data[0].GetCount() : 0 );

    if ( pos >= curNumCols )
    {
        wxString errmsg;
        errmsg.Printf( "Called wxGridStringTable::DeleteCols(pos=%d, N=%d)...\n"
                        "Pos value is invalid for present table with %d cols",
                        pos, numCols, curNumCols );
        wxFAIL_MSG( wxT( errmsg ) );
        return FALSE;
    }

    if ( numCols > curNumCols - pos )
    {
        numCols = curNumCols - pos;
    }

    for ( row = 0;  row < curNumRows;  row++ )
    {
        if ( numCols >= curNumCols )
        {
            m_data[row].Clear();
        }
        else
        {
            for ( n = 0;  n < numCols;  n++ )
            {
                m_data[row].Remove( pos );
            }
        }
    }
    UpdateAttrCols( pos, -((int)numCols) );
    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_COLS_DELETED,
                                pos,
                                numCols );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

wxString wxGridStringTable::GetRowLabelValue( int row )
{
    if ( row > (int)(m_rowLabels.GetCount()) - 1 )
    {
        // using default label
        //
        return wxGridTableBase::GetRowLabelValue( row );
    }
    else
    {
        return m_rowLabels[ row ];
    }
}

wxString wxGridStringTable::GetColLabelValue( int col )
{
    if ( col > (int)(m_colLabels.GetCount()) - 1 )
    {
        // using default label
        //
        return wxGridTableBase::GetColLabelValue( col );
    }
    else
    {
        return m_colLabels[ col ];
    }
}

void wxGridStringTable::SetRowLabelValue( int row, const wxString& value )
{
    if ( row > (int)(m_rowLabels.GetCount()) - 1 )
    {
        int n = m_rowLabels.GetCount();
        int i;
        for ( i = n;  i <= row;  i++ )
        {
            m_rowLabels.Add( wxGridTableBase::GetRowLabelValue(i) );
        }
    }

    m_rowLabels[row] = value;
}

void wxGridStringTable::SetColLabelValue( int col, const wxString& value )
{
    if ( col > (int)(m_colLabels.GetCount()) - 1 )
    {
        int n = m_colLabels.GetCount();
        int i;
        for ( i = n;  i <= col;  i++ )
        {
            m_colLabels.Add( wxGridTableBase::GetColLabelValue(i) );
        }
    }

    m_colLabels[col] = value;
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGridRowLabelWindow, wxWindow )

BEGIN_EVENT_TABLE( wxGridRowLabelWindow, wxWindow )
    EVT_PAINT( wxGridRowLabelWindow::OnPaint )
    EVT_MOUSE_EVENTS( wxGridRowLabelWindow::OnMouseEvent )
    EVT_KEY_DOWN( wxGridRowLabelWindow::OnKeyDown )
END_EVENT_TABLE()

wxGridRowLabelWindow::wxGridRowLabelWindow( wxGrid *parent,
                                            wxWindowID id,
                                            const wxPoint &pos, const wxSize &size )
  : wxWindow( parent, id, pos, size )
{
    m_owner = parent;
}

void wxGridRowLabelWindow::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc(this);

    // NO - don't do this because it will set both the x and y origin
    // coords to match the parent scrolled window and we just want to
    // set the y coord  - MB
    //
    // m_owner->PrepareDC( dc );

    int x, y;
    m_owner->CalcUnscrolledPosition( 0, 0, &x, &y );
    dc.SetDeviceOrigin( 0, -y );

    m_owner->CalcRowLabelsExposed( GetUpdateRegion() );
    m_owner->DrawRowLabels( dc );
}


void wxGridRowLabelWindow::OnMouseEvent( wxMouseEvent& event )
{
    m_owner->ProcessRowLabelMouseEvent( event );
}


// This seems to be required for wxMotif otherwise the mouse
// cursor must be in the cell edit control to get key events
//
void wxGridRowLabelWindow::OnKeyDown( wxKeyEvent& event )
{
    if ( !m_owner->ProcessEvent( event ) ) event.Skip();
}



//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGridColLabelWindow, wxWindow )

BEGIN_EVENT_TABLE( wxGridColLabelWindow, wxWindow )
    EVT_PAINT( wxGridColLabelWindow::OnPaint )
    EVT_MOUSE_EVENTS( wxGridColLabelWindow::OnMouseEvent )
    EVT_KEY_DOWN( wxGridColLabelWindow::OnKeyDown )
END_EVENT_TABLE()

wxGridColLabelWindow::wxGridColLabelWindow( wxGrid *parent,
                                            wxWindowID id,
                                            const wxPoint &pos, const wxSize &size )
  : wxWindow( parent, id, pos, size )
{
    m_owner = parent;
}

void wxGridColLabelWindow::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc(this);

    // NO - don't do this because it will set both the x and y origin
    // coords to match the parent scrolled window and we just want to
    // set the x coord  - MB
    //
    // m_owner->PrepareDC( dc );

    int x, y;
    m_owner->CalcUnscrolledPosition( 0, 0, &x, &y );
    dc.SetDeviceOrigin( -x, 0 );

    m_owner->CalcColLabelsExposed( GetUpdateRegion() );
    m_owner->DrawColLabels( dc );
}


void wxGridColLabelWindow::OnMouseEvent( wxMouseEvent& event )
{
    m_owner->ProcessColLabelMouseEvent( event );
}


// This seems to be required for wxMotif otherwise the mouse
// cursor must be in the cell edit control to get key events
//
void wxGridColLabelWindow::OnKeyDown( wxKeyEvent& event )
{
    if ( !m_owner->ProcessEvent( event ) ) event.Skip();
}



//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGridCornerLabelWindow, wxWindow )

BEGIN_EVENT_TABLE( wxGridCornerLabelWindow, wxWindow )
    EVT_MOUSE_EVENTS( wxGridCornerLabelWindow::OnMouseEvent )
    EVT_PAINT( wxGridCornerLabelWindow::OnPaint)
    EVT_KEY_DOWN( wxGridCornerLabelWindow::OnKeyDown )
END_EVENT_TABLE()

wxGridCornerLabelWindow::wxGridCornerLabelWindow( wxGrid *parent,
                                                  wxWindowID id,
                                                  const wxPoint &pos, const wxSize &size )
  : wxWindow( parent, id, pos, size )
{
    m_owner = parent;
}

void wxGridCornerLabelWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);

    int client_height = 0;
    int client_width = 0;
    GetClientSize( &client_width, &client_height );

    dc.SetPen( *wxBLACK_PEN );
    dc.DrawLine( client_width-1, client_height-1, client_width-1, 0 );
    dc.DrawLine( client_width-1, client_height-1, 0, client_height-1 );

    dc.SetPen( *wxWHITE_PEN );
    dc.DrawLine( 0, 0, client_width, 0 );
    dc.DrawLine( 0, 0, 0, client_height );
}


void wxGridCornerLabelWindow::OnMouseEvent( wxMouseEvent& event )
{
    m_owner->ProcessCornerLabelMouseEvent( event );
}


// This seems to be required for wxMotif otherwise the mouse
// cursor must be in the cell edit control to get key events
//
void wxGridCornerLabelWindow::OnKeyDown( wxKeyEvent& event )
{
    if ( !m_owner->ProcessEvent( event ) ) event.Skip();
}



//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGridWindow, wxPanel )

BEGIN_EVENT_TABLE( wxGridWindow, wxPanel )
    EVT_PAINT( wxGridWindow::OnPaint )
    EVT_MOUSE_EVENTS( wxGridWindow::OnMouseEvent )
    EVT_KEY_DOWN( wxGridWindow::OnKeyDown )
    EVT_ERASE_BACKGROUND( wxGridWindow::OnEraseBackground )
END_EVENT_TABLE()

wxGridWindow::wxGridWindow( wxGrid *parent,
                            wxGridRowLabelWindow *rowLblWin,
                            wxGridColLabelWindow *colLblWin,
                            wxWindowID id, const wxPoint &pos, const wxSize &size )
        : wxPanel( parent, id, pos, size, 0, "grid window" )
{
    m_owner = parent;
    m_rowLabelWin = rowLblWin;
    m_colLabelWin = colLblWin;
    SetBackgroundColour( "WHITE" );
}


wxGridWindow::~wxGridWindow()
{
}


void wxGridWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    m_owner->PrepareDC( dc );
    wxRegion reg = GetUpdateRegion();
    m_owner->CalcCellsExposed( reg );
    m_owner->DrawGridCellArea( dc );
#if WXGRID_DRAW_LINES
    m_owner->DrawAllGridLines( dc, reg );
#endif
    m_owner->DrawGridSpace( dc );
    m_owner->DrawHighlight( dc );
}


void wxGridWindow::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    wxPanel::ScrollWindow( dx, dy, rect );
    m_rowLabelWin->ScrollWindow( 0, dy, rect );
    m_colLabelWin->ScrollWindow( dx, 0, rect );
}


void wxGridWindow::OnMouseEvent( wxMouseEvent& event )
{
    m_owner->ProcessGridCellMouseEvent( event );
}


// This seems to be required for wxMotif otherwise the mouse
// cursor must be in the cell edit control to get key events
//
void wxGridWindow::OnKeyDown( wxKeyEvent& event )
{
    if ( !m_owner->ProcessEvent( event ) ) event.Skip();
}


void wxGridWindow::OnEraseBackground(wxEraseEvent& event)
{
}


//////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC_CLASS( wxGrid, wxScrolledWindow )

BEGIN_EVENT_TABLE( wxGrid, wxScrolledWindow )
    EVT_PAINT( wxGrid::OnPaint )
    EVT_SIZE( wxGrid::OnSize )
    EVT_KEY_DOWN( wxGrid::OnKeyDown )
    EVT_ERASE_BACKGROUND( wxGrid::OnEraseBackground )
END_EVENT_TABLE()

wxGrid::wxGrid( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxString& name )
  : wxScrolledWindow( parent, id, pos, size, style, name ),
    m_colMinWidths(wxKEY_INTEGER, GRID_HASH_SIZE)
{
    Create();
}


wxGrid::~wxGrid()
{
    ClearAttrCache();
    m_defaultCellAttr->SafeDecRef();

#ifdef DEBUG_ATTR_CACHE
    size_t total = gs_nAttrCacheHits + gs_nAttrCacheMisses;
    wxPrintf(_T("wxGrid attribute cache statistics: "
                "total: %u, hits: %u (%u%%)\n"),
             total, gs_nAttrCacheHits,
             total ? (gs_nAttrCacheHits*100) / total : 0);
#endif

    if (m_ownTable)
        delete m_table;

    delete m_typeRegistry;
}


//
// ----- internal init and update functions
//

void wxGrid::Create()
{
    m_created = FALSE;    // set to TRUE by CreateGrid
    m_displayed = TRUE; // FALSE;  // set to TRUE by OnPaint

    m_table        = (wxGridTableBase *) NULL;
    m_ownTable     = FALSE;

    m_cellEditCtrlEnabled = FALSE;

    m_defaultCellAttr = new wxGridCellAttr;
    m_defaultCellAttr->SetDefAttr(m_defaultCellAttr);

    // Set default cell attributes
    m_defaultCellAttr->SetFont(GetFont());
    m_defaultCellAttr->SetAlignment(wxLEFT, wxTOP);
    m_defaultCellAttr->SetTextColour(
        wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOWTEXT));
    m_defaultCellAttr->SetBackgroundColour(
        wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));
    m_defaultCellAttr->SetRenderer(new wxGridCellStringRenderer);
    m_defaultCellAttr->SetEditor(new wxGridCellTextEditor);


    m_numRows = 0;
    m_numCols = 0;
    m_currentCellCoords = wxGridNoCellCoords;

    m_rowLabelWidth = WXGRID_DEFAULT_ROW_LABEL_WIDTH;
    m_colLabelHeight = WXGRID_DEFAULT_COL_LABEL_HEIGHT;

    // data type registration: register all standard data types
    // TODO: may be allow the app to selectively disable some of them?
    m_typeRegistry = new wxGridTypeRegistry;
    RegisterDataType(wxGRID_VALUE_STRING,
                     new wxGridCellStringRenderer,
                     new wxGridCellTextEditor);
    RegisterDataType(wxGRID_VALUE_BOOL,
                     new wxGridCellBoolRenderer,
                     new wxGridCellBoolEditor);
    RegisterDataType(wxGRID_VALUE_NUMBER,
                     new wxGridCellNumberRenderer,
                     new wxGridCellNumberEditor);

    // subwindow components that make up the wxGrid
    m_cornerLabelWin = new wxGridCornerLabelWindow( this,
                                                    -1,
                                                    wxDefaultPosition,
                                                    wxDefaultSize );

    m_rowLabelWin = new wxGridRowLabelWindow( this,
                                              -1,
                                              wxDefaultPosition,
                                              wxDefaultSize );

    m_colLabelWin = new wxGridColLabelWindow( this,
                                              -1,
                                              wxDefaultPosition,
                                              wxDefaultSize );

    m_gridWin = new wxGridWindow( this,
                                  m_rowLabelWin,
                                  m_colLabelWin,
                                  -1,
                                  wxDefaultPosition,
                                  wxDefaultSize );

    SetTargetWindow( m_gridWin );
}


bool wxGrid::CreateGrid( int numRows, int numCols )
{
    if ( m_created )
    {
        wxFAIL_MSG( wxT("wxGrid::CreateGrid or wxGrid::SetTable called more than once") );
        return FALSE;
    }
    else
    {
        m_numRows = numRows;
        m_numCols = numCols;

        m_table = new wxGridStringTable( m_numRows, m_numCols );
        m_table->SetView( this );
        m_ownTable = TRUE;
        Init();
        m_created = TRUE;
    }

    return m_created;
}

bool wxGrid::SetTable( wxGridTableBase *table, bool takeOwnership )
{
    if ( m_created )
    {
        // RD: Actually, this should probably be allowed.  I think it would be
        // nice to be able to switch multiple Tables in and out of a single
        // View at runtime.  Is there anything in the implmentation that would
        // prevent this?

        wxFAIL_MSG( wxT("wxGrid::CreateGrid or wxGrid::SetTable called more than once") );
        return FALSE;
    }
    else
    {
        m_numRows = table->GetNumberRows();
        m_numCols = table->GetNumberCols();

        m_table = table;
        m_table->SetView( this );
        if (takeOwnership)
            m_ownTable = TRUE;
        Init();
        m_created = TRUE;
    }

    return m_created;
}


void wxGrid::Init()
{
    if ( m_numRows <= 0 )
        m_numRows = WXGRID_DEFAULT_NUMBER_ROWS;

    if ( m_numCols <= 0 )
        m_numCols = WXGRID_DEFAULT_NUMBER_COLS;

    m_rowLabelWidth  = WXGRID_DEFAULT_ROW_LABEL_WIDTH;
    m_colLabelHeight = WXGRID_DEFAULT_COL_LABEL_HEIGHT;

    if ( m_rowLabelWin )
    {
        m_labelBackgroundColour = m_rowLabelWin->GetBackgroundColour();
    }
    else
    {
        m_labelBackgroundColour = wxColour( _T("WHITE") );
    }

    m_labelTextColour = wxColour( _T("BLACK") );

    // init attr cache
    m_attrCache.row = -1;

    // TODO: something better than this ?
    //
    m_labelFont = this->GetFont();
    m_labelFont.SetWeight( m_labelFont.GetWeight() + 2 );

    m_rowLabelHorizAlign = wxLEFT;
    m_rowLabelVertAlign  = wxCENTRE;

    m_colLabelHorizAlign = wxCENTRE;
    m_colLabelVertAlign  = wxTOP;

    m_defaultColWidth  = WXGRID_DEFAULT_COL_WIDTH;
    m_defaultRowHeight = m_gridWin->GetCharHeight();

#if defined(__WXMOTIF__) || defined(__WXGTK__)  // see also text ctrl sizing in ShowCellEditControl()
    m_defaultRowHeight += 8;
#else
    m_defaultRowHeight += 4;
#endif

    m_gridLineColour = wxColour( 128, 128, 255 );
    m_gridLinesEnabled = TRUE;

    m_cursorMode  = WXGRID_CURSOR_SELECT_CELL;
    m_winCapture = (wxWindow *)NULL;
    m_canDragRowSize = TRUE;
    m_canDragColSize = TRUE;
    m_canDragGridSize = TRUE;
    m_dragLastPos  = -1;
    m_dragRowOrCol = -1;
    m_isDragging = FALSE;
    m_startDragPos = wxDefaultPosition;

    m_waitForSlowClick = FALSE;

    m_rowResizeCursor = wxCursor( wxCURSOR_SIZENS );
    m_colResizeCursor = wxCursor( wxCURSOR_SIZEWE );

    m_currentCellCoords = wxGridNoCellCoords;

    m_selectedTopLeft = wxGridNoCellCoords;
    m_selectedBottomRight = wxGridNoCellCoords;
    m_selectionBackground = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHT);
    m_selectionForeground = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHTTEXT);

    m_editable = TRUE;  // default for whole grid

    m_inOnKeyDown = FALSE;
    m_batchCount = 0;
}

// ----------------------------------------------------------------------------
// the idea is to call these functions only when necessary because they create
// quite big arrays which eat memory mostly unnecessary - in particular, if
// default widths/heights are used for all rows/columns, we may not use these
// arrays at all
//
// with some extra code, it should be possible to only store the
// widths/heights different from default ones but this will be done later...
// ----------------------------------------------------------------------------

void wxGrid::InitRowHeights()
{
    m_rowHeights.Empty();
    m_rowBottoms.Empty();

    m_rowHeights.Alloc( m_numRows );
    m_rowBottoms.Alloc( m_numRows );

    int rowBottom = 0;
    for ( int i = 0;  i < m_numRows;  i++ )
    {
        m_rowHeights.Add( m_defaultRowHeight );
        rowBottom += m_defaultRowHeight;
        m_rowBottoms.Add( rowBottom );
    }
}

void wxGrid::InitColWidths()
{
    m_colWidths.Empty();
    m_colRights.Empty();

    m_colWidths.Alloc( m_numCols );
    m_colRights.Alloc( m_numCols );
    int colRight = 0;
    for ( int i = 0;  i < m_numCols;  i++ )
    {
        m_colWidths.Add( m_defaultColWidth );
        colRight += m_defaultColWidth;
        m_colRights.Add( colRight );
    }
}

int wxGrid::GetColWidth(int col) const
{
    return m_colWidths.IsEmpty() ? m_defaultColWidth : m_colWidths[col];
}

int wxGrid::GetColLeft(int col) const
{
    return m_colRights.IsEmpty() ? col * m_defaultColWidth
                                 : m_colRights[col] - m_colWidths[col];
}

int wxGrid::GetColRight(int col) const
{
    return m_colRights.IsEmpty() ? (col + 1) * m_defaultColWidth
                                 : m_colRights[col];
}

int wxGrid::GetRowHeight(int row) const
{
    return m_rowHeights.IsEmpty() ? m_defaultRowHeight : m_rowHeights[row];
}

int wxGrid::GetRowTop(int row) const
{
    return m_rowBottoms.IsEmpty() ? row * m_defaultRowHeight
                                  : m_rowBottoms[row] - m_rowHeights[row];
}

int wxGrid::GetRowBottom(int row) const
{
    return m_rowBottoms.IsEmpty() ? (row + 1) * m_defaultRowHeight
                                  : m_rowBottoms[row];
}

void wxGrid::CalcDimensions()
{
    int cw, ch;
    GetClientSize( &cw, &ch );

    if ( m_numRows > 0  &&  m_numCols > 0 )
    {
        int right = GetColRight( m_numCols-1 ) + 50;
        int bottom = GetRowBottom( m_numRows-1 ) + 50;

        // TODO: restore the scroll position that we had before sizing
        //
        int x, y;
        GetViewStart( &x, &y );
        SetScrollbars( GRID_SCROLL_LINE, GRID_SCROLL_LINE,
                       right/GRID_SCROLL_LINE, bottom/GRID_SCROLL_LINE,
                       x, y );
    }
}


void wxGrid::CalcWindowSizes()
{
    int cw, ch;
    GetClientSize( &cw, &ch );

    if ( m_cornerLabelWin->IsShown() )
        m_cornerLabelWin->SetSize( 0, 0, m_rowLabelWidth, m_colLabelHeight );

    if ( m_colLabelWin->IsShown() )
        m_colLabelWin->SetSize( m_rowLabelWidth, 0, cw-m_rowLabelWidth, m_colLabelHeight);

    if ( m_rowLabelWin->IsShown() )
        m_rowLabelWin->SetSize( 0, m_colLabelHeight, m_rowLabelWidth, ch-m_colLabelHeight);

    if ( m_gridWin->IsShown() )
        m_gridWin->SetSize( m_rowLabelWidth, m_colLabelHeight, cw-m_rowLabelWidth, ch-m_colLabelHeight);
}


// this is called when the grid table sends a message to say that it
// has been redimensioned
//
bool wxGrid::Redimension( wxGridTableMessage& msg )
{
    int i;

    // if we were using the default widths/heights so far, we must change them
    // now
    if ( m_colWidths.IsEmpty() )
    {
        InitColWidths();
    }

    if ( m_rowHeights.IsEmpty() )
    {
        InitRowHeights();
    }

    switch ( msg.GetId() )
    {
        case wxGRIDTABLE_NOTIFY_ROWS_INSERTED:
        {
            size_t pos = msg.GetCommandInt();
            int numRows = msg.GetCommandInt2();
            for ( i = 0;  i < numRows;  i++ )
            {
                m_rowHeights.Insert( m_defaultRowHeight, pos );
                m_rowBottoms.Insert( 0, pos );
            }
            m_numRows += numRows;

            int bottom = 0;
            if ( pos > 0 ) bottom = m_rowBottoms[pos-1];

            for ( i = pos;  i < m_numRows;  i++ )
            {
                bottom += m_rowHeights[i];
                m_rowBottoms[i] = bottom;
            }
            CalcDimensions();
        }
        return TRUE;

        case wxGRIDTABLE_NOTIFY_ROWS_APPENDED:
        {
            int numRows = msg.GetCommandInt();
            for ( i = 0;  i < numRows;  i++ )
            {
                m_rowHeights.Add( m_defaultRowHeight );
                m_rowBottoms.Add( 0 );
            }

            int oldNumRows = m_numRows;
            m_numRows += numRows;

            int bottom = 0;
            if ( oldNumRows > 0 ) bottom = m_rowBottoms[oldNumRows-1];

            for ( i = oldNumRows;  i < m_numRows;  i++ )
            {
                bottom += m_rowHeights[i];
                m_rowBottoms[i] = bottom;
            }
            CalcDimensions();
        }
        return TRUE;

        case wxGRIDTABLE_NOTIFY_ROWS_DELETED:
        {
            size_t pos = msg.GetCommandInt();
            int numRows = msg.GetCommandInt2();
            for ( i = 0;  i < numRows;  i++ )
            {
                m_rowHeights.Remove( pos );
                m_rowBottoms.Remove( pos );
            }
            m_numRows -= numRows;

            if ( !m_numRows )
            {
                m_numCols = 0;
                m_colWidths.Clear();
                m_colRights.Clear();
                m_currentCellCoords = wxGridNoCellCoords;
            }
            else
            {
                if ( m_currentCellCoords.GetRow() >= m_numRows )
                    m_currentCellCoords.Set( 0, 0 );

                int h = 0;
                for ( i = 0;  i < m_numRows;  i++ )
                {
                    h += m_rowHeights[i];
                    m_rowBottoms[i] = h;
                }
            }

            CalcDimensions();
        }
        return TRUE;

        case wxGRIDTABLE_NOTIFY_COLS_INSERTED:
        {
            size_t pos = msg.GetCommandInt();
            int numCols = msg.GetCommandInt2();
            for ( i = 0;  i < numCols;  i++ )
            {
                m_colWidths.Insert( m_defaultColWidth, pos );
                m_colRights.Insert( 0, pos );
            }
            m_numCols += numCols;

            int right = 0;
            if ( pos > 0 ) right = m_colRights[pos-1];

            for ( i = pos;  i < m_numCols;  i++ )
            {
                right += m_colWidths[i];
                m_colRights[i] = right;
            }
            CalcDimensions();
        }
        return TRUE;

        case wxGRIDTABLE_NOTIFY_COLS_APPENDED:
        {
            int numCols = msg.GetCommandInt();
            for ( i = 0;  i < numCols;  i++ )
            {
                m_colWidths.Add( m_defaultColWidth );
                m_colRights.Add( 0 );
            }

            int oldNumCols = m_numCols;
            m_numCols += numCols;

            int right = 0;
            if ( oldNumCols > 0 ) right = m_colRights[oldNumCols-1];

            for ( i = oldNumCols;  i < m_numCols;  i++ )
            {
                right += m_colWidths[i];
                m_colRights[i] = right;
            }
            CalcDimensions();
        }
        return TRUE;

        case wxGRIDTABLE_NOTIFY_COLS_DELETED:
        {
            size_t pos = msg.GetCommandInt();
            int numCols = msg.GetCommandInt2();
            for ( i = 0;  i < numCols;  i++ )
            {
                m_colWidths.Remove( pos );
                m_colRights.Remove( pos );
            }
            m_numCols -= numCols;

            if ( !m_numCols )
            {
#if 0  // leave the row alone here so that AppendCols will work subsequently
                m_numRows = 0;
                m_rowHeights.Clear();
                m_rowBottoms.Clear();
#endif
                m_currentCellCoords = wxGridNoCellCoords;
            }
            else
            {
                if ( m_currentCellCoords.GetCol() >= m_numCols )
                    m_currentCellCoords.Set( 0, 0 );

                int w = 0;
                for ( i = 0;  i < m_numCols;  i++ )
                {
                    w += m_colWidths[i];
                    m_colRights[i] = w;
                }
            }
            CalcDimensions();
        }
        return TRUE;
    }

    return FALSE;
}


void wxGrid::CalcRowLabelsExposed( wxRegion& reg )
{
    wxRegionIterator iter( reg );
    wxRect r;

    m_rowLabelsExposed.Empty();

    int top, bottom;
    while ( iter )
    {
        r = iter.GetRect();

        // TODO: remove this when we can...
        // There is a bug in wxMotif that gives garbage update
        // rectangles if you jump-scroll a long way by clicking the
        // scrollbar with middle button.  This is a work-around
        //
#if defined(__WXMOTIF__)
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );
        if ( r.GetTop() > ch ) r.SetTop( 0 );
        r.SetBottom( wxMin( r.GetBottom(), ch ) );
#endif

        // logical bounds of update region
        //
        int dummy;
        CalcUnscrolledPosition( 0, r.GetTop(), &dummy, &top );
        CalcUnscrolledPosition( 0, r.GetBottom(), &dummy, &bottom );

        // find the row labels within these bounds
        //
        int row;
        for ( row = 0;  row < m_numRows;  row++ )
        {
            if ( GetRowBottom(row) < top )
                continue;

            if ( GetRowTop(row) > bottom )
                break;

            m_rowLabelsExposed.Add( row );
        }

        iter++ ;
    }
}


void wxGrid::CalcColLabelsExposed( wxRegion& reg )
{
    wxRegionIterator iter( reg );
    wxRect r;

    m_colLabelsExposed.Empty();

    int left, right;
    while ( iter )
    {
        r = iter.GetRect();

        // TODO: remove this when we can...
        // There is a bug in wxMotif that gives garbage update
        // rectangles if you jump-scroll a long way by clicking the
        // scrollbar with middle button.  This is a work-around
        //
#if defined(__WXMOTIF__)
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );
        if ( r.GetLeft() > cw ) r.SetLeft( 0 );
        r.SetRight( wxMin( r.GetRight(), cw ) );
#endif

        // logical bounds of update region
        //
        int dummy;
        CalcUnscrolledPosition( r.GetLeft(), 0, &left, &dummy );
        CalcUnscrolledPosition( r.GetRight(), 0, &right, &dummy );

        // find the cells within these bounds
        //
        int col;
        for ( col = 0;  col < m_numCols;  col++ )
        {
            if ( GetColRight(col) < left )
                continue;

            if ( GetColLeft(col) > right )
                break;

            m_colLabelsExposed.Add( col );
        }

        iter++ ;
    }
}


void wxGrid::CalcCellsExposed( wxRegion& reg )
{
    wxRegionIterator iter( reg );
    wxRect r;

    m_cellsExposed.Empty();
    m_rowsExposed.Empty();
    m_colsExposed.Empty();

    int left, top, right, bottom;
    while ( iter )
    {
        r = iter.GetRect();

        // TODO: remove this when we can...
        // There is a bug in wxMotif that gives garbage update
        // rectangles if you jump-scroll a long way by clicking the
        // scrollbar with middle button.  This is a work-around
        //
#if defined(__WXMOTIF__)
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );
        if ( r.GetTop() > ch ) r.SetTop( 0 );
        if ( r.GetLeft() > cw ) r.SetLeft( 0 );
        r.SetRight( wxMin( r.GetRight(), cw ) );
        r.SetBottom( wxMin( r.GetBottom(), ch ) );
#endif

        // logical bounds of update region
        //
        CalcUnscrolledPosition( r.GetLeft(), r.GetTop(), &left, &top );
        CalcUnscrolledPosition( r.GetRight(), r.GetBottom(), &right, &bottom );

        // find the cells within these bounds
        //
        int row, col;
        for ( row = 0;  row < m_numRows;  row++ )
        {
            if ( GetRowBottom(row) <= top )
                continue;

            if ( GetRowTop(row) > bottom )
                break;

            m_rowsExposed.Add( row );

            for ( col = 0;  col < m_numCols;  col++ )
            {
                if ( GetColRight(col) <= left )
                    continue;

                if ( GetColLeft(col) > right )
                    break;

                if ( m_colsExposed.Index( col ) == wxNOT_FOUND )
                    m_colsExposed.Add( col );
                m_cellsExposed.Add( wxGridCellCoords( row, col ) );
            }
        }

        iter++;
    }
}


void wxGrid::ProcessRowLabelMouseEvent( wxMouseEvent& event )
{
    int x, y, row;
    wxPoint pos( event.GetPosition() );
    CalcUnscrolledPosition( pos.x, pos.y, &x, &y );

    if ( event.Dragging() )
    {
        m_isDragging = TRUE;

        if ( event.LeftIsDown() )
        {
            switch( m_cursorMode )
            {
                case WXGRID_CURSOR_RESIZE_ROW:
                {
                    int cw, ch, left, dummy;
                    m_gridWin->GetClientSize( &cw, &ch );
                    CalcUnscrolledPosition( 0, 0, &left, &dummy );

                    wxClientDC dc( m_gridWin );
                    PrepareDC( dc );
                    y = wxMax( y, GetRowTop(m_dragRowOrCol) + WXGRID_MIN_ROW_HEIGHT );
                    dc.SetLogicalFunction(wxINVERT);
                    if ( m_dragLastPos >= 0 )
                    {
                        dc.DrawLine( left, m_dragLastPos, left+cw, m_dragLastPos );
                    }
                    dc.DrawLine( left, y, left+cw, y );
                    m_dragLastPos = y;
                }
                break;

                case WXGRID_CURSOR_SELECT_ROW:
                    if ( (row = YToRow( y )) >= 0  &&
                         !IsInSelection( row, 0 ) )
                    {
                        SelectRow( row, TRUE );
                    }

                // default label to suppress warnings about "enumeration value
                // 'xxx' not handled in switch
                default:
                    break;
            }
        }
        return;
    }

    m_isDragging = FALSE;


    // ------------ Entering or leaving the window
    //
    if ( event.Entering() || event.Leaving() )
    {
        ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL, m_rowLabelWin);
    }


    // ------------ Left button pressed
    //
    else if ( event.LeftDown() )
    {
        // don't send a label click event for a hit on the
        // edge of the row label - this is probably the user
        // wanting to resize the row
        //
        if ( YToEdgeOfRow(y) < 0 )
        {
            row = YToRow(y);
            if ( row >= 0  &&
                 !SendEvent( wxEVT_GRID_LABEL_LEFT_CLICK, row, -1, event ) )
            {
                SelectRow( row, event.ShiftDown() );
                ChangeCursorMode(WXGRID_CURSOR_SELECT_ROW, m_rowLabelWin);
            }
        }
        else
        {
            // starting to drag-resize a row
            //
            if ( CanDragRowSize() )
                ChangeCursorMode(WXGRID_CURSOR_RESIZE_ROW, m_rowLabelWin);
        }
    }


    // ------------ Left double click
    //
    else if (event.LeftDClick() )
    {
        if ( YToEdgeOfRow(y) < 0 )
        {
            row = YToRow(y);
            SendEvent( wxEVT_GRID_LABEL_LEFT_DCLICK, row, -1, event );
        }
    }


    // ------------ Left button released
    //
    else if ( event.LeftUp() )
    {
        if ( m_cursorMode == WXGRID_CURSOR_RESIZE_ROW )
        {
            DoEndDragResizeRow();

            // Note: we are ending the event *after* doing
            // default processing in this case
            //
            SendEvent( wxEVT_GRID_ROW_SIZE, m_dragRowOrCol, -1, event );
        }

        ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL, m_rowLabelWin);
        m_dragLastPos = -1;
    }


    // ------------ Right button down
    //
    else if ( event.RightDown() )
    {
        row = YToRow(y);
        if ( !SendEvent( wxEVT_GRID_LABEL_RIGHT_CLICK, row, -1, event ) )
        {
            // no default action at the moment
        }
    }


    // ------------ Right double click
    //
    else if ( event.RightDClick() )
    {
        row = YToRow(y);
        if ( !SendEvent( wxEVT_GRID_LABEL_RIGHT_DCLICK, row, -1, event ) )
        {
            // no default action at the moment
        }
    }


    // ------------ No buttons down and mouse moving
    //
    else if ( event.Moving() )
    {
        m_dragRowOrCol = YToEdgeOfRow( y );
        if ( m_dragRowOrCol >= 0 )
        {
            if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
            {
                // don't capture the mouse yet
                if ( CanDragRowSize() )
                    ChangeCursorMode(WXGRID_CURSOR_RESIZE_ROW, m_rowLabelWin, FALSE);
            }
        }
        else if ( m_cursorMode != WXGRID_CURSOR_SELECT_CELL )
        {
            ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL, m_rowLabelWin, FALSE);
        }
    }
}


void wxGrid::ProcessColLabelMouseEvent( wxMouseEvent& event )
{
    int x, y, col;
    wxPoint pos( event.GetPosition() );
    CalcUnscrolledPosition( pos.x, pos.y, &x, &y );

    if ( event.Dragging() )
    {
        m_isDragging = TRUE;

        if ( event.LeftIsDown() )
        {
            switch( m_cursorMode )
            {
                case WXGRID_CURSOR_RESIZE_COL:
                {
                    int cw, ch, dummy, top;
                    m_gridWin->GetClientSize( &cw, &ch );
                    CalcUnscrolledPosition( 0, 0, &dummy, &top );

                    wxClientDC dc( m_gridWin );
                    PrepareDC( dc );

                    x = wxMax( x, GetColLeft(m_dragRowOrCol) +
                                  GetColMinimalWidth(m_dragRowOrCol));
                    dc.SetLogicalFunction(wxINVERT);
                    if ( m_dragLastPos >= 0 )
                    {
                        dc.DrawLine( m_dragLastPos, top, m_dragLastPos, top+ch );
                    }
                    dc.DrawLine( x, top, x, top+ch );
                    m_dragLastPos = x;
                }
                break;

                case WXGRID_CURSOR_SELECT_COL:
                    if ( (col = XToCol( x )) >= 0  &&
                         !IsInSelection( 0, col ) )
                    {
                        SelectCol( col, TRUE );
                    }

                // default label to suppress warnings about "enumeration value
                // 'xxx' not handled in switch
                default:
                    break;
            }
        }
        return;
    }

    m_isDragging = FALSE;


    // ------------ Entering or leaving the window
    //
    if ( event.Entering() || event.Leaving() )
    {
        ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL, m_colLabelWin);
    }


    // ------------ Left button pressed
    //
    else if ( event.LeftDown() )
    {
        // don't send a label click event for a hit on the
        // edge of the col label - this is probably the user
        // wanting to resize the col
        //
        if ( XToEdgeOfCol(x) < 0 )
        {
            col = XToCol(x);
            if ( col >= 0  &&
                 !SendEvent( wxEVT_GRID_LABEL_LEFT_CLICK, -1, col, event ) )
            {
                SelectCol( col, event.ShiftDown() );
                ChangeCursorMode(WXGRID_CURSOR_SELECT_COL, m_colLabelWin);
            }
        }
        else
        {
            // starting to drag-resize a col
            //
            if ( CanDragColSize() )
                ChangeCursorMode(WXGRID_CURSOR_RESIZE_COL, m_colLabelWin);
        }
    }


    // ------------ Left double click
    //
    if ( event.LeftDClick() )
    {
        if ( XToEdgeOfCol(x) < 0 )
        {
            col = XToCol(x);
            SendEvent( wxEVT_GRID_LABEL_LEFT_DCLICK, -1, col, event );
        }
    }


    // ------------ Left button released
    //
    else if ( event.LeftUp() )
    {
        if ( m_cursorMode == WXGRID_CURSOR_RESIZE_COL )
        {
            DoEndDragResizeCol();

            // Note: we are ending the event *after* doing
            // default processing in this case
            //
            SendEvent( wxEVT_GRID_COL_SIZE, -1, m_dragRowOrCol, event );
        }

        ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL, m_colLabelWin);
        m_dragLastPos  = -1;
    }


    // ------------ Right button down
    //
    else if ( event.RightDown() )
    {
        col = XToCol(x);
        if ( !SendEvent( wxEVT_GRID_LABEL_RIGHT_CLICK, -1, col, event ) )
        {
            // no default action at the moment
        }
    }


    // ------------ Right double click
    //
    else if ( event.RightDClick() )
    {
        col = XToCol(x);
        if ( !SendEvent( wxEVT_GRID_LABEL_RIGHT_DCLICK, -1, col, event ) )
        {
            // no default action at the moment
        }
    }


    // ------------ No buttons down and mouse moving
    //
    else if ( event.Moving() )
    {
        m_dragRowOrCol = XToEdgeOfCol( x );
        if ( m_dragRowOrCol >= 0 )
        {
            if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
            {
                // don't capture the cursor yet
                if ( CanDragColSize() )
                    ChangeCursorMode(WXGRID_CURSOR_RESIZE_COL, m_colLabelWin, FALSE);
            }
        }
        else if ( m_cursorMode != WXGRID_CURSOR_SELECT_CELL )
        {
            ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL, m_colLabelWin, FALSE);
        }
    }
}


void wxGrid::ProcessCornerLabelMouseEvent( wxMouseEvent& event )
{
    if ( event.LeftDown() )
    {
        // indicate corner label by having both row and
        // col args == -1
        //
        if ( !SendEvent( wxEVT_GRID_LABEL_LEFT_CLICK, -1, -1, event ) )
        {
            SelectAll();
        }
    }

    else if ( event.LeftDClick() )
    {
        SendEvent( wxEVT_GRID_LABEL_LEFT_DCLICK, -1, -1, event );
    }

    else if ( event.RightDown() )
    {
        if ( !SendEvent( wxEVT_GRID_LABEL_RIGHT_CLICK, -1, -1, event ) )
        {
            // no default action at the moment
        }
    }

    else if ( event.RightDClick() )
    {
        if ( !SendEvent( wxEVT_GRID_LABEL_RIGHT_DCLICK, -1, -1, event ) )
        {
            // no default action at the moment
        }
    }
}

void wxGrid::ChangeCursorMode(CursorMode mode,
                              wxWindow *win,
                              bool captureMouse)
{
#ifdef __WXDEBUG__
    static const wxChar *cursorModes[] =
    {
        _T("SELECT_CELL"),
        _T("RESIZE_ROW"),
        _T("RESIZE_COL"),
        _T("SELECT_ROW"),
        _T("SELECT_COL")
    };

    wxLogTrace(_T("grid"),
               _T("wxGrid cursor mode (mouse capture for %s): %s -> %s"),
               win == m_colLabelWin ? _T("colLabelWin")
                                    : win ? _T("rowLabelWin")
                                          : _T("gridWin"),
               cursorModes[m_cursorMode], cursorModes[mode]);
#endif // __WXDEBUG__

    if ( mode == m_cursorMode )
        return;

    if ( !win )
    {
        // by default use the grid itself
        win = m_gridWin;
    }

    if ( m_winCapture )
    {
        m_winCapture->ReleaseMouse();
        m_winCapture = (wxWindow *)NULL;
    }

    m_cursorMode = mode;

    switch ( m_cursorMode )
    {
        case WXGRID_CURSOR_RESIZE_ROW:
            win->SetCursor( m_rowResizeCursor );
            break;

        case WXGRID_CURSOR_RESIZE_COL:
            win->SetCursor( m_colResizeCursor );
            break;

        default:
            win->SetCursor( *wxSTANDARD_CURSOR );
    }

    // we need to capture mouse when resizing
    bool resize = m_cursorMode == WXGRID_CURSOR_RESIZE_ROW ||
                  m_cursorMode == WXGRID_CURSOR_RESIZE_COL;

    if ( captureMouse && resize )
    {
        win->CaptureMouse();
        m_winCapture = win;
    }
}

void wxGrid::ProcessGridCellMouseEvent( wxMouseEvent& event )
{
    int x, y;
    wxPoint pos( event.GetPosition() );
    CalcUnscrolledPosition( pos.x, pos.y, &x, &y );

    wxGridCellCoords coords;
    XYToCell( x, y, coords );

    if ( event.Dragging() )
    {
        //wxLogDebug("pos(%d, %d) coords(%d, %d)", pos.x, pos.y, coords.GetRow(), coords.GetCol());

        // Don't start doing anything until the mouse has been drug at
        // least 3 pixels in any direction...
        if (! m_isDragging)
        {
            if (m_startDragPos == wxDefaultPosition)
            {
                m_startDragPos = pos;
                return;
            }
            if (abs(m_startDragPos.x - pos.x) < 4 && abs(m_startDragPos.y - pos.y) < 4)
                return;
        }

        m_isDragging = TRUE;
        if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
        {
            // Hide the edit control, so it
            // won't interfer with drag-shrinking.
            if ( IsCellEditControlEnabled() )
            {
                HideCellEditControl();
                SaveEditControlValue();
            }

            // Have we captured the mouse yet?
            if (! m_winCapture)
            {
                m_winCapture = m_gridWin;
                m_winCapture->CaptureMouse();
            }

            if ( coords != wxGridNoCellCoords )
            {
                if ( !IsSelection() )
                {
                    SelectBlock( coords, coords );
                }
                else
                {
                    SelectBlock( m_currentCellCoords, coords );
                }

                if (! IsVisible(coords))
                {
                    MakeCellVisible(coords);
                    // TODO: need to introduce a delay or something here.  The
                    // scrolling is way to fast, at least on MSW.
                }
            }
        }
        else if ( m_cursorMode == WXGRID_CURSOR_RESIZE_ROW )
        {
            int cw, ch, left, dummy;
            m_gridWin->GetClientSize( &cw, &ch );
            CalcUnscrolledPosition( 0, 0, &left, &dummy );

            wxClientDC dc( m_gridWin );
            PrepareDC( dc );
            y = wxMax( y, GetRowTop(m_dragRowOrCol) + WXGRID_MIN_ROW_HEIGHT );
            dc.SetLogicalFunction(wxINVERT);
            if ( m_dragLastPos >= 0 )
            {
                dc.DrawLine( left, m_dragLastPos, left+cw, m_dragLastPos );
            }
            dc.DrawLine( left, y, left+cw, y );
            m_dragLastPos = y;
        }
        else if ( m_cursorMode == WXGRID_CURSOR_RESIZE_COL )
        {
            int cw, ch, dummy, top;
            m_gridWin->GetClientSize( &cw, &ch );
            CalcUnscrolledPosition( 0, 0, &dummy, &top );

            wxClientDC dc( m_gridWin );
            PrepareDC( dc );
            x = wxMax( x, GetColLeft(m_dragRowOrCol) +
                          GetColMinimalWidth(m_dragRowOrCol) );
            dc.SetLogicalFunction(wxINVERT);
            if ( m_dragLastPos >= 0 )
            {
                dc.DrawLine( m_dragLastPos, top, m_dragLastPos, top+ch );
            }
            dc.DrawLine( x, top, x, top+ch );
            m_dragLastPos = x;
        }

        return;
    }

    m_isDragging = FALSE;
    m_startDragPos = wxDefaultPosition;

    // VZ: if we do this, the mode is reset to WXGRID_CURSOR_SELECT_CELL
    //     immediately after it becomes WXGRID_CURSOR_RESIZE_ROW/COL under
    //     wxGTK
#if 0
    if ( event.Entering() || event.Leaving() )
    {
        ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL);
        m_gridWin->SetCursor( *wxSTANDARD_CURSOR );
    }
    else
#endif // 0

    // ------------ Left button pressed
    //
    if ( event.LeftDown() && coords != wxGridNoCellCoords )
    {
        if ( event.ShiftDown() )
        {
            SelectBlock( m_currentCellCoords, coords );
        }
        else if ( XToEdgeOfCol(x) < 0  &&
                  YToEdgeOfRow(y) < 0 )
        {
            if ( !SendEvent( wxEVT_GRID_CELL_LEFT_CLICK,
                             coords.GetRow(),
                             coords.GetCol(),
                             event ) )
            {
                DisableCellEditControl();
                MakeCellVisible( coords );

                // if this is the second click on this cell then start
                // the edit control
                if ( m_waitForSlowClick &&
                     (coords == m_currentCellCoords) &&
                     CanEnableCellControl())
                {
                    EnableCellEditControl();

                    wxGridCellAttr* attr = GetCellAttr(m_currentCellCoords);
                    attr->GetEditor(this, coords.GetRow(), coords.GetCol())->StartingClick();
                    attr->DecRef();

                    m_waitForSlowClick = FALSE;
                }
                else
                {
                    SetCurrentCell( coords );
                    m_waitForSlowClick = TRUE;
                }
            }
        }
    }


    // ------------ Left double click
    //
    else if ( event.LeftDClick() && coords != wxGridNoCellCoords )
    {
        DisableCellEditControl();

        if ( XToEdgeOfCol(x) < 0  &&  YToEdgeOfRow(y) < 0 )
        {
            SendEvent( wxEVT_GRID_CELL_LEFT_DCLICK,
                       coords.GetRow(),
                       coords.GetCol(),
                       event );
        }
    }


    // ------------ Left button released
    //
    else if ( event.LeftUp() )
    {
        if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
        {
            if ( IsSelection() )
            {
                if (m_winCapture)
                {
                    m_winCapture->ReleaseMouse();
                    m_winCapture = NULL;
                }
                SendEvent( wxEVT_GRID_RANGE_SELECT, -1, -1, event );
            }

            // Show the edit control, if it has been hidden for
            // drag-shrinking.
            ShowCellEditControl();
        }
        else if ( m_cursorMode == WXGRID_CURSOR_RESIZE_ROW )
        {
            ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL);
            DoEndDragResizeRow();

            // Note: we are ending the event *after* doing
            // default processing in this case
            //
            SendEvent( wxEVT_GRID_ROW_SIZE, m_dragRowOrCol, -1, event );
        }
        else if ( m_cursorMode == WXGRID_CURSOR_RESIZE_COL )
        {
            ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL);
            DoEndDragResizeCol();

            // Note: we are ending the event *after* doing
            // default processing in this case
            //
            SendEvent( wxEVT_GRID_COL_SIZE, -1, m_dragRowOrCol, event );
        }

        m_dragLastPos = -1;
    }


    // ------------ Right button down
    //
    else if ( event.RightDown() && coords != wxGridNoCellCoords )
    {
        DisableCellEditControl();
        if ( !SendEvent( wxEVT_GRID_CELL_RIGHT_CLICK,
                         coords.GetRow(),
                         coords.GetCol(),
                         event ) )
        {
            // no default action at the moment
        }
    }


    // ------------ Right double click
    //
    else if ( event.RightDClick() && coords != wxGridNoCellCoords )
    {
        DisableCellEditControl();
        if ( !SendEvent( wxEVT_GRID_CELL_RIGHT_DCLICK,
                         coords.GetRow(),
                         coords.GetCol(),
                         event ) )
        {
            // no default action at the moment
        }
    }

    // ------------ Moving and no button action
    //
    else if ( event.Moving() && !event.IsButton() )
    {
        int dragRow = YToEdgeOfRow( y );
        int dragCol = XToEdgeOfCol( x );

        // Dragging on the corner of a cell to resize in both
        // directions is not implemented yet...
        //
        if ( dragRow >= 0  &&  dragCol >= 0 )
        {
            ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL);
            return;
        }

        if ( dragRow >= 0 )
        {
            m_dragRowOrCol = dragRow;

            if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
            {
                if ( CanDragRowSize() && CanDragGridSize() )
                    ChangeCursorMode(WXGRID_CURSOR_RESIZE_ROW);
            }

            if ( dragCol >= 0 )
            {
                m_dragRowOrCol = dragCol;
            }

            return;
        }

        if ( dragCol >= 0 )
        {
            m_dragRowOrCol = dragCol;

            if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
            {
                if ( CanDragColSize() && CanDragGridSize() )
                    ChangeCursorMode(WXGRID_CURSOR_RESIZE_COL);
            }

            return;
        }

        // Neither on a row or col edge
        //
        if ( m_cursorMode != WXGRID_CURSOR_SELECT_CELL )
        {
            ChangeCursorMode(WXGRID_CURSOR_SELECT_CELL);
        }
    }
}


void wxGrid::DoEndDragResizeRow()
{
    if ( m_dragLastPos >= 0 )
    {
        // erase the last line and resize the row
        //
        int cw, ch, left, dummy;
        m_gridWin->GetClientSize( &cw, &ch );
        CalcUnscrolledPosition( 0, 0, &left, &dummy );

        wxClientDC dc( m_gridWin );
        PrepareDC( dc );
        dc.SetLogicalFunction( wxINVERT );
        dc.DrawLine( left, m_dragLastPos, left+cw, m_dragLastPos );
        HideCellEditControl();
        SaveEditControlValue();

        int rowTop = GetRowTop(m_dragRowOrCol);
        SetRowSize( m_dragRowOrCol,
                    wxMax( m_dragLastPos - rowTop, WXGRID_MIN_ROW_HEIGHT ) );

        if ( !GetBatchCount() )
        {
            // Only needed to get the correct rect.y:
            wxRect rect ( CellToRect( m_dragRowOrCol, 0 ) );
            rect.x = 0;
            CalcScrolledPosition(0, rect.y, &dummy, &rect.y);
            rect.width = m_rowLabelWidth;
            rect.height = ch - rect.y;
            m_rowLabelWin->Refresh( TRUE, &rect );
            rect.width = cw;
            m_gridWin->Refresh( FALSE, &rect );
        }

        ShowCellEditControl();
    }
}


void wxGrid::DoEndDragResizeCol()
{
    if ( m_dragLastPos >= 0 )
    {
        // erase the last line and resize the col
        //
        int cw, ch, dummy, top;
        m_gridWin->GetClientSize( &cw, &ch );
        CalcUnscrolledPosition( 0, 0, &dummy, &top );

        wxClientDC dc( m_gridWin );
        PrepareDC( dc );
        dc.SetLogicalFunction( wxINVERT );
        dc.DrawLine( m_dragLastPos, top, m_dragLastPos, top+ch );
        HideCellEditControl();
        SaveEditControlValue();

        int colLeft = GetColLeft(m_dragRowOrCol);
        SetColSize( m_dragRowOrCol,
                    wxMax( m_dragLastPos - colLeft,
                           GetColMinimalWidth(m_dragRowOrCol) ) );

        if ( !GetBatchCount() )
        {
            // Only needed to get the correct rect.x:
            wxRect rect ( CellToRect( 0, m_dragRowOrCol ) );
            rect.y = 0;
            CalcScrolledPosition(rect.x, 0, &rect.x, &dummy);
            rect.width = cw - rect.x;
            rect.height = m_colLabelHeight;
            m_colLabelWin->Refresh( TRUE, &rect );
            rect.height = ch;
            m_gridWin->Refresh( FALSE, &rect );
        }

        ShowCellEditControl();
    }
}



//
// ------ interaction with data model
//
bool wxGrid::ProcessTableMessage( wxGridTableMessage& msg )
{
    switch ( msg.GetId() )
    {
        case wxGRIDTABLE_REQUEST_VIEW_GET_VALUES:
            return GetModelValues();

        case wxGRIDTABLE_REQUEST_VIEW_SEND_VALUES:
            return SetModelValues();

        case wxGRIDTABLE_NOTIFY_ROWS_INSERTED:
        case wxGRIDTABLE_NOTIFY_ROWS_APPENDED:
        case wxGRIDTABLE_NOTIFY_ROWS_DELETED:
        case wxGRIDTABLE_NOTIFY_COLS_INSERTED:
        case wxGRIDTABLE_NOTIFY_COLS_APPENDED:
        case wxGRIDTABLE_NOTIFY_COLS_DELETED:
            return Redimension( msg );

        default:
            return FALSE;
    }
}



// The behaviour of this function depends on the grid table class
// Clear() function.  For the default wxGridStringTable class the
// behavious is to replace all cell contents with wxEmptyString but
// not to change the number of rows or cols.
//
void wxGrid::ClearGrid()
{
    if ( m_table )
    {
        if (IsCellEditControlEnabled())
            DisableCellEditControl();

        m_table->Clear();
        if ( !GetBatchCount() ) m_gridWin->Refresh();
    }
}


bool wxGrid::InsertRows( int pos, int numRows, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxFAIL_MSG( wxT("Called wxGrid::InsertRows() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table )
    {
        if (IsCellEditControlEnabled())
            DisableCellEditControl();

        bool ok = m_table->InsertRows( pos, numRows );

        // the table will have sent the results of the insert row
        // operation to this view object as a grid table message
        //
        if ( ok )
        {
            if ( m_numCols == 0 )
            {
                m_table->AppendCols( WXGRID_DEFAULT_NUMBER_COLS );
                //
                // TODO: perhaps instead of appending the default number of cols
                // we should remember what the last non-zero number of cols was ?
                //
            }

            if ( m_currentCellCoords == wxGridNoCellCoords )
            {
                // if we have just inserted cols into an empty grid the current
                // cell will be undefined...
                //
                SetCurrentCell( 0, 0 );
            }

            ClearSelection();
            if ( !GetBatchCount() ) Refresh();
        }

        return ok;
    }
    else
    {
        return FALSE;
    }
}


bool wxGrid::AppendRows( int numRows, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxFAIL_MSG( wxT("Called wxGrid::AppendRows() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table && m_table->AppendRows( numRows ) )
    {
        if ( m_currentCellCoords == wxGridNoCellCoords )
        {
            // if we have just inserted cols into an empty grid the current
            // cell will be undefined...
            //
            SetCurrentCell( 0, 0 );
        }

        // the table will have sent the results of the append row
        // operation to this view object as a grid table message
        //
        ClearSelection();
        if ( !GetBatchCount() ) Refresh();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool wxGrid::DeleteRows( int pos, int numRows, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxFAIL_MSG( wxT("Called wxGrid::DeleteRows() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table )
    {
        if (IsCellEditControlEnabled())
            DisableCellEditControl();

        if (m_table->DeleteRows( pos, numRows ))
        {

            // the table will have sent the results of the delete row
            // operation to this view object as a grid table message
            //
            ClearSelection();
            if ( !GetBatchCount() ) Refresh();
            return TRUE;
        }
    }
    return FALSE;
}


bool wxGrid::InsertCols( int pos, int numCols, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxFAIL_MSG( wxT("Called wxGrid::InsertCols() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table )
    {
        if (IsCellEditControlEnabled())
            DisableCellEditControl();

        bool ok = m_table->InsertCols( pos, numCols );

        // the table will have sent the results of the insert col
        // operation to this view object as a grid table message
        //
        if ( ok )
        {
            if ( m_currentCellCoords == wxGridNoCellCoords )
            {
                // if we have just inserted cols into an empty grid the current
                // cell will be undefined...
                //
                SetCurrentCell( 0, 0 );
            }

            ClearSelection();
            if ( !GetBatchCount() ) Refresh();
        }

        return ok;
    }
    else
    {
        return FALSE;
    }
}


bool wxGrid::AppendCols( int numCols, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxFAIL_MSG( wxT("Called wxGrid::AppendCols() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table && m_table->AppendCols( numCols ) )
    {
        // the table will have sent the results of the append col
        // operation to this view object as a grid table message
        //
        if ( m_currentCellCoords == wxGridNoCellCoords )
        {
            // if we have just inserted cols into an empty grid the current
            // cell will be undefined...
            //
            SetCurrentCell( 0, 0 );
        }

        ClearSelection();
        if ( !GetBatchCount() ) Refresh();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool wxGrid::DeleteCols( int pos, int numCols, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxFAIL_MSG( wxT("Called wxGrid::DeleteCols() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table )
    {
        if (IsCellEditControlEnabled())
            DisableCellEditControl();

        if ( m_table->DeleteCols( pos, numCols ) )
        {
            // the table will have sent the results of the delete col
            // operation to this view object as a grid table message
            //
            ClearSelection();
            if ( !GetBatchCount() ) Refresh();
            return TRUE;
        }
    }
    return FALSE;
}



//
// ----- event handlers
//

// Generate a grid event based on a mouse event and
// return the result of ProcessEvent()
//
bool wxGrid::SendEvent( const wxEventType type,
                        int row, int col,
                        wxMouseEvent& mouseEv )
{
    if ( type == wxEVT_GRID_ROW_SIZE || type == wxEVT_GRID_COL_SIZE )
    {
        int rowOrCol = (row == -1 ? col : row);

        wxGridSizeEvent gridEvt( GetId(),
                                 type,
                                 this,
                                 rowOrCol,
                                 mouseEv.GetX(), mouseEv.GetY(),
                                 mouseEv.ControlDown(),
                                 mouseEv.ShiftDown(),
                                 mouseEv.AltDown(),
                                 mouseEv.MetaDown() );

        return GetEventHandler()->ProcessEvent(gridEvt);
    }
    else if ( type == wxEVT_GRID_RANGE_SELECT )
    {
        wxGridRangeSelectEvent gridEvt( GetId(),
                                        type,
                                        this,
                                        m_selectedTopLeft,
                                        m_selectedBottomRight,
                                        mouseEv.ControlDown(),
                                        mouseEv.ShiftDown(),
                                        mouseEv.AltDown(),
                                        mouseEv.MetaDown() );

        return GetEventHandler()->ProcessEvent(gridEvt);
    }
    else
    {
        wxGridEvent gridEvt( GetId(),
                             type,
                             this,
                             row, col,
                             mouseEv.GetX(), mouseEv.GetY(),
                             mouseEv.ControlDown(),
                             mouseEv.ShiftDown(),
                             mouseEv.AltDown(),
                             mouseEv.MetaDown() );

        return GetEventHandler()->ProcessEvent(gridEvt);
    }
}


// Generate a grid event of specified type and return the result
// of ProcessEvent().
//
bool wxGrid::SendEvent( const wxEventType type,
                        int row, int col )
{
    if ( type == wxEVT_GRID_ROW_SIZE || type == wxEVT_GRID_COL_SIZE )
    {
        int rowOrCol = (row == -1 ? col : row);

        wxGridSizeEvent gridEvt( GetId(),
                                 type,
                                 this,
                                 rowOrCol );

        return GetEventHandler()->ProcessEvent(gridEvt);
    }
    else
    {
        wxGridEvent gridEvt( GetId(),
                             type,
                             this,
                             row, col );

        return GetEventHandler()->ProcessEvent(gridEvt);
    }
}


void wxGrid::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc( this );

    if ( m_currentCellCoords == wxGridNoCellCoords  &&
         m_numRows && m_numCols )
    {
        m_currentCellCoords.Set(0, 0);
        ShowCellEditControl();
    }

    m_displayed = TRUE;
}


// This is just here to make sure that CalcDimensions gets called when
// the grid view is resized... then the size event is skipped to allow
// the box sizers to handle everything
//
void wxGrid::OnSize( wxSizeEvent& event )
{
    CalcWindowSizes();
    CalcDimensions();
}


void wxGrid::OnKeyDown( wxKeyEvent& event )
{
    if ( m_inOnKeyDown )
    {
        // shouldn't be here - we are going round in circles...
        //
        wxFAIL_MSG( wxT("wxGrid::OnKeyDown called while already active") );
    }

    m_inOnKeyDown = TRUE;

    // propagate the event up and see if it gets processed
    //
    wxWindow *parent = GetParent();
    wxKeyEvent keyEvt( event );
    keyEvt.SetEventObject( parent );

    if ( !parent->GetEventHandler()->ProcessEvent( keyEvt ) )
    {

        // TODO:  Should also support Shift-cursor keys for
        //        extending the selection.  Maybe add a flag to
        //        MoveCursorXXX() and MoveCursorXXXBlock() and
        //        just send event.ShiftDown().

        // try local handlers
        //
        switch ( event.KeyCode() )
        {
            case WXK_UP:
                if ( event.ControlDown() )
                {
                    MoveCursorUpBlock();
                }
                else
                {
                    MoveCursorUp();
                }
                break;

            case WXK_DOWN:
                if ( event.ControlDown() )
                {
                    MoveCursorDownBlock();
                }
                else
                {
                    MoveCursorDown();
                }
                break;

            case WXK_LEFT:
                if ( event.ControlDown() )
                {
                    MoveCursorLeftBlock();
                }
                else
                {
                    MoveCursorLeft();
                }
                break;

            case WXK_RIGHT:
                if ( event.ControlDown() )
                {
                    MoveCursorRightBlock();
                }
                else
                {
                    MoveCursorRight();
                }
                break;

            case WXK_RETURN:
                if ( event.ControlDown() )
                {
                    event.Skip();  // to let the edit control have the return
                }
                else
                {
                    MoveCursorDown();
                }
                break;

            case WXK_TAB:
                if (event.ShiftDown())
                    MoveCursorLeft();
                else
                    MoveCursorRight();
                break;

            case WXK_HOME:
                if ( event.ControlDown() )
                {
                    MakeCellVisible( 0, 0 );
                    SetCurrentCell( 0, 0 );
                }
                else
                {
                    event.Skip();
                }
                break;

            case WXK_END:
                if ( event.ControlDown() )
                {
                    MakeCellVisible( m_numRows-1, m_numCols-1 );
                    SetCurrentCell( m_numRows-1, m_numCols-1 );
                }
                else
                {
                    event.Skip();
                }
                break;

            case WXK_PRIOR:
                MovePageUp();
                break;

            case WXK_NEXT:
                MovePageDown();
                break;

            case WXK_SPACE:
                if ( !IsEditable() )
                {
                    MoveCursorRight();
                    break;
                }
                // Otherwise fall through to default

            default:
                // alphanumeric keys enable the cell edit control
                if ( !(event.AltDown() ||
                       event.MetaDown() ||
                       event.ControlDown()) &&
                     isalnum(event.KeyCode()) &&
                     !IsCellEditControlEnabled() &&
                     CanEnableCellControl() )
                {
                    EnableCellEditControl();
                    int row = m_currentCellCoords.GetRow();
                    int col = m_currentCellCoords.GetCol();
                    wxGridCellAttr* attr = GetCellAttr(row, col);
                    attr->GetEditor(this, row, col)->StartingKey(event);
                    attr->DecRef();
                }
                else
                {
                    // let others process char events with modifiers or all
                    // char events for readonly cells
                    event.Skip();
                }
                break;
        }
    }

    m_inOnKeyDown = FALSE;
}


void wxGrid::OnEraseBackground(wxEraseEvent&)
{
}

void wxGrid::SetCurrentCell( const wxGridCellCoords& coords )
{
    if ( SendEvent( wxEVT_GRID_SELECT_CELL, coords.GetRow(), coords.GetCol() ) )
    {
        // the event has been intercepted - do nothing
        return;
    }

    if ( m_displayed  &&
         m_currentCellCoords != wxGridNoCellCoords )
    {
        HideCellEditControl();
        DisableCellEditControl();

        // Clear the old current cell highlight
        wxRect r = BlockToDeviceRect(m_currentCellCoords, m_currentCellCoords);

        // Otherwise refresh redraws the highlight!
        m_currentCellCoords = coords;

        m_gridWin->Refresh( FALSE, &r );
    }

    m_currentCellCoords = coords;

    if ( m_displayed )
    {
        wxClientDC dc(m_gridWin);
        PrepareDC(dc);

        wxGridCellAttr* attr = GetCellAttr(coords);
        DrawCellHighlight(dc, attr);
        attr->DecRef();

        if ( IsSelection() )
        {
            wxRect r( SelectionToDeviceRect() );
            ClearSelection();
            if ( !GetBatchCount() ) m_gridWin->Refresh( FALSE, &r );
        }
    }
}


//
// ------ functions to get/send data (see also public functions)
//

bool wxGrid::GetModelValues()
{
    if ( m_table )
    {
        // all we need to do is repaint the grid
        //
        m_gridWin->Refresh();
        return TRUE;
    }

    return FALSE;
}


bool wxGrid::SetModelValues()
{
    int row, col;

    if ( m_table )
    {
        for ( row = 0;  row < m_numRows;  row++ )
        {
            for ( col = 0;  col < m_numCols;  col++ )
            {
                m_table->SetValue( row, col, GetCellValue(row, col) );
            }
        }

        return TRUE;
    }

    return FALSE;
}



// Note - this function only draws cells that are in the list of
// exposed cells (usually set from the update region by
// CalcExposedCells)
//
void wxGrid::DrawGridCellArea( wxDC& dc )
{
    if ( !m_numRows || !m_numCols ) return;

    size_t i;
    size_t numCells = m_cellsExposed.GetCount();

    for ( i = 0;  i < numCells;  i++ )
    {
        DrawCell( dc, m_cellsExposed[i] );
    }
}


void wxGrid::DrawGridSpace( wxDC& dc )
{
    if ( m_numRows  &&  m_numCols )
    {
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );

        int right, bottom;
        CalcUnscrolledPosition( cw, ch, &right, &bottom );

        if ( right > GetColRight(m_numCols-1)  ||
             bottom > GetRowBottom(m_numRows-1) )
        {
            int left, top;
            CalcUnscrolledPosition( 0, 0, &left, &top );

            dc.SetBrush( wxBrush(GetDefaultCellBackgroundColour(), wxSOLID) );
            dc.SetPen( *wxTRANSPARENT_PEN );

            if ( right > GetColRight(m_numCols-1) )
                dc.DrawRectangle( GetColRight(m_numCols-1), top,
                                  right - GetColRight(m_numCols-1), ch );

            if ( bottom > GetRowBottom(m_numRows-1) )
                dc.DrawRectangle( left, GetRowBottom(m_numRows-1),
                                  cw, bottom - GetRowBottom(m_numRows-1) );
        }
    }
}


void wxGrid::DrawCell( wxDC& dc, const wxGridCellCoords& coords )
{
    int row = coords.GetRow();
    int col = coords.GetCol();

    if ( GetColWidth(col) <= 0 || GetRowHeight(row) <= 0 )
        return;

    // we draw the cell border ourselves
#if !WXGRID_DRAW_LINES
    if ( m_gridLinesEnabled )
        DrawCellBorder( dc, coords );
#endif

    wxGridCellAttr* attr = GetCellAttr(row, col);

    bool isCurrent = coords == m_currentCellCoords;

    wxRect rect;
    rect.x = GetColLeft(col);
    rect.y = GetRowTop(row);
    rect.width = GetColWidth(col) - 1;
    rect.height = GetRowHeight(row) - 1;

    // if the editor is shown, we should use it and not the renderer
    if ( isCurrent && IsCellEditControlEnabled() )
    {
        attr->GetEditor(this, row, col)->PaintBackground(rect, attr);
    }
    else
    {
        // but all the rest is drawn by the cell renderer and hence may be
        // customized
        attr->GetRenderer(this, row, col)->
            Draw(*this, *attr, dc, rect, row, col, IsInSelection(coords));

    }

    attr->DecRef();
}

void wxGrid::DrawCellHighlight( wxDC& dc, const wxGridCellAttr *attr )
{
    int row = m_currentCellCoords.GetRow();
    int col = m_currentCellCoords.GetCol();

    if ( GetColWidth(col) <= 0 || GetRowHeight(row) <= 0 )
        return;

    wxRect rect;
    rect.x = GetColLeft(col);
    rect.y = GetRowTop(row);
    rect.width = GetColWidth(col) - 1;
    rect.height = GetRowHeight(row) - 1;

    // hmmm... what could we do here to show that the cell is disabled?
    // for now, I just draw a thinner border than for the other ones, but
    // it doesn't look really good
    dc.SetPen(wxPen(m_gridLineColour, attr->IsReadOnly() ? 1 : 3, wxSOLID));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(rect);

#if 0
        // VZ: my experiments with 3d borders...

        // how to properly set colours for arbitrary bg?
        wxCoord x1 = rect.x,
                y1 = rect.y,
                x2 = rect.x + rect.width -1,
                y2 = rect.y + rect.height -1;

        dc.SetPen(*wxWHITE_PEN);
        dc.DrawLine(x1, y1, x2, y1);
        dc.DrawLine(x1, y1, x1, y2);

        dc.DrawLine(x1 + 1, y2 - 1, x2 - 1, y2 - 1);
        dc.DrawLine(x2 - 1, y1 + 1, x2 - 1, y2 );

        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine(x1, y2, x2, y2);
        dc.DrawLine(x2, y1, x2, y2+1);
#endif // 0
}


void wxGrid::DrawCellBorder( wxDC& dc, const wxGridCellCoords& coords )
{
    int row = coords.GetRow();
    int col = coords.GetCol();
    if ( GetColWidth(col) <= 0 || GetRowHeight(row) <= 0 )
        return;

    dc.SetPen( wxPen(GetGridLineColour(), 1, wxSOLID) );

    // right hand border
    //
    dc.DrawLine( GetColRight(col), GetRowTop(row),
                 GetColRight(col), GetRowBottom(row) );

    // bottom border
    //
    dc.DrawLine( GetColLeft(col), GetRowBottom(row),
                 GetColRight(col), GetRowBottom(row) );
}

void wxGrid::DrawHighlight(wxDC& dc)
{
    if ( IsCellEditControlEnabled() )
    {
        // don't show highlight when the edit control is shown
        return;
    }

    // if the active cell was repainted, repaint its highlight too because it
    // might have been damaged by the grid lines
    size_t count = m_cellsExposed.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( m_cellsExposed[n] == m_currentCellCoords )
        {
            wxGridCellAttr* attr = GetCellAttr(m_currentCellCoords);
            DrawCellHighlight(dc, attr);
            attr->DecRef();

            break;
        }
    }
}

// TODO: remove this ???
// This is used to redraw all grid lines e.g. when the grid line colour
// has been changed
//
void wxGrid::DrawAllGridLines( wxDC& dc, const wxRegion & reg )
{
    if ( !m_gridLinesEnabled ||
         !m_numRows ||
         !m_numCols ) return;

    int top, bottom, left, right;

#ifndef __WXGTK__
    if (reg.IsEmpty())
    {
      int cw, ch;
      m_gridWin->GetClientSize(&cw, &ch);

      // virtual coords of visible area
      //
      CalcUnscrolledPosition( 0, 0, &left, &top );
      CalcUnscrolledPosition( cw, ch, &right, &bottom );
    }
    else
    {
      wxCoord x, y, w, h;
      reg.GetBox(x, y, w, h);
      CalcUnscrolledPosition( x, y, &left, &top );
      CalcUnscrolledPosition( x + w, y + h, &right, &bottom );
    }
#else
      int cw, ch;
      m_gridWin->GetClientSize(&cw, &ch);
      CalcUnscrolledPosition( 0, 0, &left, &top );
      CalcUnscrolledPosition( cw, ch, &right, &bottom );
#endif

    // avoid drawing grid lines past the last row and col
    //
    right = wxMin( right, GetColRight(m_numCols - 1) );
    bottom = wxMin( bottom, GetRowBottom(m_numRows - 1) );

    dc.SetPen( wxPen(GetGridLineColour(), 1, wxSOLID) );

    // horizontal grid lines
    //
    int i;
    for ( i = 0; i < m_numRows; i++ )
    {
        int bot = GetRowBottom(i) - 1;

        if ( bot > bottom )
        {
            break;
        }

        if ( bot >= top )
        {
            dc.DrawLine( left, bot, right, bot );
        }
    }


    // vertical grid lines
    //
    for ( i = 0; i < m_numCols; i++ )
    {
        int colRight = GetColRight(i) - 1;
        if ( colRight > right )
        {
            break;
        }

        if ( colRight >= left )
        {
            dc.DrawLine( colRight, top, colRight, bottom );
        }
    }
}


void wxGrid::DrawRowLabels( wxDC& dc )
{
    if ( !m_numRows || !m_numCols ) return;

    size_t i;
    size_t numLabels = m_rowLabelsExposed.GetCount();

    for ( i = 0;  i < numLabels;  i++ )
    {
        DrawRowLabel( dc, m_rowLabelsExposed[i] );
    }
}


void wxGrid::DrawRowLabel( wxDC& dc, int row )
{
    if ( GetRowHeight(row) <= 0 )
        return;

    int rowTop = GetRowTop(row),
        rowBottom = GetRowBottom(row) - 1;

    dc.SetPen( *wxBLACK_PEN );
    dc.DrawLine( m_rowLabelWidth-1, rowTop,
                 m_rowLabelWidth-1, rowBottom );

    dc.DrawLine( 0, rowBottom, m_rowLabelWidth-1, rowBottom );

    dc.SetPen( *wxWHITE_PEN );
    dc.DrawLine( 0, rowTop, 0, rowBottom );
    dc.DrawLine( 0, rowTop, m_rowLabelWidth-1, rowTop );

    dc.SetBackgroundMode( wxTRANSPARENT );
    dc.SetTextForeground( GetLabelTextColour() );
    dc.SetFont( GetLabelFont() );

    int hAlign, vAlign;
    GetRowLabelAlignment( &hAlign, &vAlign );

    wxRect rect;
    rect.SetX( 2 );
    rect.SetY( GetRowTop(row) + 2 );
    rect.SetWidth( m_rowLabelWidth - 4 );
    rect.SetHeight( GetRowHeight(row) - 4 );
    DrawTextRectangle( dc, GetRowLabelValue( row ), rect, hAlign, vAlign );
}


void wxGrid::DrawColLabels( wxDC& dc )
{
    if ( !m_numRows || !m_numCols ) return;

    size_t i;
    size_t numLabels = m_colLabelsExposed.GetCount();

    for ( i = 0;  i < numLabels;  i++ )
    {
        DrawColLabel( dc, m_colLabelsExposed[i] );
    }
}


void wxGrid::DrawColLabel( wxDC& dc, int col )
{
    if ( GetColWidth(col) <= 0 )
        return;

    int colLeft = GetColLeft(col),
        colRight = GetColRight(col) - 1;

    dc.SetPen( *wxBLACK_PEN );
    dc.DrawLine( colRight, 0,
                 colRight, m_colLabelHeight-1 );

    dc.DrawLine( colLeft, m_colLabelHeight-1,
                 colRight, m_colLabelHeight-1 );

    dc.SetPen( *wxWHITE_PEN );
    dc.DrawLine( colLeft, 0, colLeft, m_colLabelHeight-1 );
    dc.DrawLine( colLeft, 0, colRight, 0 );

    dc.SetBackgroundMode( wxTRANSPARENT );
    dc.SetTextForeground( GetLabelTextColour() );
    dc.SetFont( GetLabelFont() );

    dc.SetBackgroundMode( wxTRANSPARENT );
    dc.SetTextForeground( GetLabelTextColour() );
    dc.SetFont( GetLabelFont() );

    int hAlign, vAlign;
    GetColLabelAlignment( &hAlign, &vAlign );

    wxRect rect;
    rect.SetX( colLeft + 2 );
    rect.SetY( 2 );
    rect.SetWidth( GetColWidth(col) - 4 );
    rect.SetHeight( m_colLabelHeight - 4 );
    DrawTextRectangle( dc, GetColLabelValue( col ), rect, hAlign, vAlign );
}


void wxGrid::DrawTextRectangle( wxDC& dc,
                                const wxString& value,
                                const wxRect& rect,
                                int horizAlign,
                                int vertAlign )
{
    long textWidth, textHeight;
    long lineWidth, lineHeight;
    wxArrayString lines;

    dc.SetClippingRegion( rect );
    StringToLines( value, lines );
    if ( lines.GetCount() )
    {
        GetTextBoxSize( dc, lines, &textWidth, &textHeight );
        dc.GetTextExtent( lines[0], &lineWidth, &lineHeight );

        float x, y;
        switch ( horizAlign )
        {
            case wxRIGHT:
                x = rect.x + (rect.width - textWidth - 1);
                break;

            case wxCENTRE:
                x = rect.x + ((rect.width - textWidth)/2);
                break;

            case wxLEFT:
            default:
                x = rect.x + 1;
                break;
        }

        switch ( vertAlign )
        {
            case wxBOTTOM:
                y = rect.y + (rect.height - textHeight - 1);
                break;

            case wxCENTRE:
                y = rect.y + ((rect.height - textHeight)/2);
                break;

            case wxTOP:
            default:
                y = rect.y + 1;
                break;
        }

        for ( size_t i = 0;  i < lines.GetCount();  i++ )
        {
            dc.DrawText( lines[i], (long)x, (long)y );
            y += lineHeight;
        }
    }

    dc.DestroyClippingRegion();
}


// Split multi line text up into an array of strings.  Any existing
// contents of the string array are preserved.
//
void wxGrid::StringToLines( const wxString& value, wxArrayString& lines )
{
    int startPos = 0;
    int pos;
    wxString eol = wxTextFile::GetEOL( wxTextFileType_Unix );
    wxString tVal = wxTextFile::Translate( value, wxTextFileType_Unix );

    while ( startPos < (int)tVal.Length() )
    {
        pos = tVal.Mid(startPos).Find( eol );
        if ( pos < 0 )
        {
            break;
        }
        else if ( pos == 0 )
        {
            lines.Add( wxEmptyString );
        }
        else
        {
            lines.Add( value.Mid(startPos, pos) );
        }
        startPos += pos+1;
    }
    if ( startPos < (int)value.Length() )
    {
        lines.Add( value.Mid( startPos ) );
    }
}


void wxGrid::GetTextBoxSize( wxDC& dc,
                             wxArrayString& lines,
                             long *width, long *height )
{
    long w = 0;
    long h = 0;
    long lineW, lineH;

    size_t i;
    for ( i = 0;  i < lines.GetCount();  i++ )
    {
        dc.GetTextExtent( lines[i], &lineW, &lineH );
        w = wxMax( w, lineW );
        h += lineH;
    }

    *width = w;
    *height = h;
}


//
// ------ Edit control functions
//


void wxGrid::EnableEditing( bool edit )
{
    // TODO: improve this ?
    //
    if ( edit != m_editable )
    {
        m_editable = edit;

        // FIXME IMHO this won't disable the edit control if edit == FALSE
        //       because of the check in the beginning of
        //       EnableCellEditControl() just below (VZ)
        EnableCellEditControl(m_editable);
    }
}


void wxGrid::EnableCellEditControl( bool enable )
{
    if (! m_editable)
        return;

    if ( m_currentCellCoords == wxGridNoCellCoords )
        SetCurrentCell( 0, 0 );

    if ( enable != m_cellEditCtrlEnabled )
    {
        // TODO allow the app to Veto() this event?
        SendEvent(enable ? wxEVT_GRID_EDITOR_SHOWN : wxEVT_GRID_EDITOR_HIDDEN);

        if ( enable )
        {
            // this should be checked by the caller!
            wxASSERT_MSG( CanEnableCellControl(),
                          _T("can't enable editing for this cell!") );

            // do it before ShowCellEditControl()
            m_cellEditCtrlEnabled = enable;

            ShowCellEditControl();
        }
        else
        {
            HideCellEditControl();
            SaveEditControlValue();

            // do it after HideCellEditControl()
            m_cellEditCtrlEnabled = enable;
        }
    }
}

bool wxGrid::IsCurrentCellReadOnly() const
{
    // const_cast
    wxGridCellAttr* attr = ((wxGrid *)this)->GetCellAttr(m_currentCellCoords);
    bool readonly = attr->IsReadOnly();
    attr->DecRef();

    return readonly;
}

bool wxGrid::CanEnableCellControl() const
{
    return m_editable && !IsCurrentCellReadOnly();
}

bool wxGrid::IsCellEditControlEnabled() const
{
    // the cell edit control might be disable for all cells or just for the
    // current one if it's read only
    return m_cellEditCtrlEnabled ? !IsCurrentCellReadOnly() : FALSE;
}

void wxGrid::ShowCellEditControl()
{
    if ( IsCellEditControlEnabled() )
    {
        if ( !IsVisible( m_currentCellCoords ) )
        {
            return;
        }
        else
        {
            wxRect rect = CellToRect( m_currentCellCoords );
            int row = m_currentCellCoords.GetRow();
            int col = m_currentCellCoords.GetCol();

            // convert to scrolled coords
            //
            CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

            // done in PaintBackground()
#if 0
            // erase the highlight and the cell contents because the editor
            // might not cover the entire cell
            wxClientDC dc( m_gridWin );
            PrepareDC( dc );
            dc.SetBrush(*wxLIGHT_GREY_BRUSH); //wxBrush(attr->GetBackgroundColour(), wxSOLID));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(rect);
#endif // 0

            // cell is shifted by one pixel
            rect.x--;
            rect.y--;

            wxGridCellAttr* attr = GetCellAttr(row, col);
            wxGridCellEditor* editor = attr->GetEditor(this, row, col);
            if ( !editor->IsCreated() )
            {
                editor->Create(m_gridWin, -1,
                               new wxGridCellEditorEvtHandler(this, editor));
            }

            editor->SetSize( rect );

            editor->Show( TRUE, attr );
            editor->BeginEdit(row, col, this);
            attr->DecRef();
         }
    }
}


void wxGrid::HideCellEditControl()
{
    if ( IsCellEditControlEnabled() )
    {
        int row = m_currentCellCoords.GetRow();
        int col = m_currentCellCoords.GetCol();

        wxGridCellAttr* attr = GetCellAttr(row, col);
        attr->GetEditor(this, row, col)->Show( FALSE );
        attr->DecRef();
        m_gridWin->SetFocus();
    }
}


void wxGrid::SaveEditControlValue()
{
    if ( IsCellEditControlEnabled() )
    {
        int row = m_currentCellCoords.GetRow();
        int col = m_currentCellCoords.GetCol();

        wxGridCellAttr* attr = GetCellAttr(row, col);
        wxGridCellEditor* editor = attr->GetEditor(this, row, col);
        bool changed = editor->EndEdit(row, col, this);

        attr->DecRef();

        if (changed)
        {
            SendEvent( wxEVT_GRID_CELL_CHANGE,
                       m_currentCellCoords.GetRow(),
                       m_currentCellCoords.GetCol() );
        }
    }
}


//
// ------ Grid location functions
//  Note that all of these functions work with the logical coordinates of
//  grid cells and labels so you will need to convert from device
//  coordinates for mouse events etc.
//

void wxGrid::XYToCell( int x, int y, wxGridCellCoords& coords )
{
    int row = YToRow(y);
    int col = XToCol(x);

    if ( row == -1  ||  col == -1 )
    {
        coords = wxGridNoCellCoords;
    }
    else
    {
        coords.Set( row, col );
    }
}


int wxGrid::YToRow( int y )
{
    int i;

    for ( i = 0;  i < m_numRows;  i++ )
    {
        if ( y < GetRowBottom(i) )
            return i;
    }

    return -1;
}


int wxGrid::XToCol( int x )
{
    int i;

    for ( i = 0;  i < m_numCols;  i++ )
    {
        if ( x < GetColRight(i) )
            return i;
    }

    return -1;
}


// return the row number that that the y coord is near the edge of, or
// -1 if not near an edge
//
int wxGrid::YToEdgeOfRow( int y )
{
    int i, d;

    for ( i = 0;  i < m_numRows;  i++ )
    {
        if ( GetRowHeight(i) > WXGRID_LABEL_EDGE_ZONE )
        {
            d = abs( y - GetRowBottom(i) );
            if ( d < WXGRID_LABEL_EDGE_ZONE )
                return i;
        }
    }

    return -1;
}


// return the col number that that the x coord is near the edge of, or
// -1 if not near an edge
//
int wxGrid::XToEdgeOfCol( int x )
{
    int i, d;

    for ( i = 0;  i < m_numCols;  i++ )
    {
        if ( GetColWidth(i) > WXGRID_LABEL_EDGE_ZONE )
        {
            d = abs( x - GetColRight(i) );
            if ( d < WXGRID_LABEL_EDGE_ZONE )
                return i;
        }
    }

    return -1;
}


wxRect wxGrid::CellToRect( int row, int col )
{
    wxRect rect( -1, -1, -1, -1 );

    if ( row >= 0  &&  row < m_numRows  &&
         col >= 0  &&  col < m_numCols )
    {
        rect.x = GetColLeft(col);
        rect.y = GetRowTop(row);
        rect.width = GetColWidth(col);
        rect.height = GetRowHeight(row);
    }

    return rect;
}


bool wxGrid::IsVisible( int row, int col, bool wholeCellVisible )
{
    // get the cell rectangle in logical coords
    //
    wxRect r( CellToRect( row, col ) );

    // convert to device coords
    //
    int left, top, right, bottom;
    CalcScrolledPosition( r.GetLeft(), r.GetTop(), &left, &top );
    CalcScrolledPosition( r.GetRight(), r.GetBottom(), &right, &bottom );

    // check against the client area of the grid window
    //
    int cw, ch;
    m_gridWin->GetClientSize( &cw, &ch );

    if ( wholeCellVisible )
    {
        // is the cell wholly visible ?
        //
        return ( left >= 0  &&  right <= cw  &&
                 top >= 0  &&  bottom <= ch );
    }
    else
    {
        // is the cell partly visible ?
        //
        return ( ((left >=0 && left < cw) || (right > 0 && right <= cw))  &&
                 ((top >=0 && top < ch) || (bottom > 0 && bottom <= ch)) );
    }
}


// make the specified cell location visible by doing a minimal amount
// of scrolling
//
void wxGrid::MakeCellVisible( int row, int col )
{
    int i;
    int xpos = -1, ypos = -1;

    if ( row >= 0  &&  row < m_numRows  &&
         col >= 0  &&  col < m_numCols )
    {
        // get the cell rectangle in logical coords
        //
        wxRect r( CellToRect( row, col ) );

        // convert to device coords
        //
        int left, top, right, bottom;
        CalcScrolledPosition( r.GetLeft(), r.GetTop(), &left, &top );
        CalcScrolledPosition( r.GetRight(), r.GetBottom(), &right, &bottom );

        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );

        if ( top < 0 )
        {
            ypos = r.GetTop();
        }
        else if ( bottom > ch )
        {
            int h = r.GetHeight();
            ypos = r.GetTop();
            for ( i = row-1;  i >= 0;  i-- )
            {
                int rowHeight = GetRowHeight(i);
                if ( h + rowHeight > ch )
                    break;

                h += rowHeight;
                ypos -= rowHeight;
            }

            // we divide it later by GRID_SCROLL_LINE, make sure that we don't
            // have rounding errors (this is important, because if we do, we
            // might not scroll at all and some cells won't be redrawn)
            ypos += GRID_SCROLL_LINE / 2;
        }

        if ( left < 0 )
        {
            xpos = r.GetLeft();
        }
        else if ( right > cw )
        {
            int w = r.GetWidth();
            xpos = r.GetLeft();
            for ( i = col-1;  i >= 0;  i-- )
            {
                int colWidth = GetColWidth(i);
                if ( w + colWidth > cw )
                    break;

                w += colWidth;
                xpos -= colWidth;
            }

            // see comment for ypos above
            xpos += GRID_SCROLL_LINE / 2;
        }

        if ( xpos != -1  ||  ypos != -1 )
        {
            if ( xpos != -1 ) xpos /= GRID_SCROLL_LINE;
            if ( ypos != -1 ) ypos /= GRID_SCROLL_LINE;
            Scroll( xpos, ypos );
            AdjustScrollbars();
        }
    }
}


//
// ------ Grid cursor movement functions
//

bool wxGrid::MoveCursorUp()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetRow() > 0 )
    {
        MakeCellVisible( m_currentCellCoords.GetRow() - 1,
                        m_currentCellCoords.GetCol() );

        SetCurrentCell( m_currentCellCoords.GetRow() - 1,
                        m_currentCellCoords.GetCol() );

        return TRUE;
    }

    return FALSE;
}


bool wxGrid::MoveCursorDown()
{
    // TODO: allow for scrolling
    //
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetRow() < m_numRows-1 )
    {
        MakeCellVisible( m_currentCellCoords.GetRow() + 1,
                        m_currentCellCoords.GetCol() );

        SetCurrentCell( m_currentCellCoords.GetRow() + 1,
                        m_currentCellCoords.GetCol() );

        return TRUE;
    }

    return FALSE;
}


bool wxGrid::MoveCursorLeft()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetCol() > 0 )
    {
        MakeCellVisible( m_currentCellCoords.GetRow(),
                        m_currentCellCoords.GetCol() - 1 );

        SetCurrentCell( m_currentCellCoords.GetRow(),
                        m_currentCellCoords.GetCol() - 1 );

        return TRUE;
    }

    return FALSE;
}


bool wxGrid::MoveCursorRight()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetCol() < m_numCols - 1 )
    {
        MakeCellVisible( m_currentCellCoords.GetRow(),
                        m_currentCellCoords.GetCol() + 1 );

        SetCurrentCell( m_currentCellCoords.GetRow(),
                        m_currentCellCoords.GetCol() + 1 );

        return TRUE;
    }

    return FALSE;
}


bool wxGrid::MovePageUp()
{
    if ( m_currentCellCoords == wxGridNoCellCoords ) return FALSE;

    int row = m_currentCellCoords.GetRow();
    if ( row > 0 )
    {
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );

        int y = GetRowTop(row);
        int newRow = YToRow( y - ch + 1 );
        if ( newRow == -1 )
        {
            newRow = 0;
        }
        else if ( newRow == row )
        {
            newRow = row - 1;
        }

        MakeCellVisible( newRow, m_currentCellCoords.GetCol() );
        SetCurrentCell( newRow, m_currentCellCoords.GetCol() );

        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MovePageDown()
{
    if ( m_currentCellCoords == wxGridNoCellCoords ) return FALSE;

    int row = m_currentCellCoords.GetRow();
    if ( row < m_numRows )
    {
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );

        int y = GetRowTop(row);
        int newRow = YToRow( y + ch );
        if ( newRow == -1 )
        {
            newRow = m_numRows - 1;
        }
        else if ( newRow == row )
        {
            newRow = row + 1;
        }

        MakeCellVisible( newRow, m_currentCellCoords.GetCol() );
        SetCurrentCell( newRow, m_currentCellCoords.GetCol() );

        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MoveCursorUpBlock()
{
    if ( m_table &&
         m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetRow() > 0 )
    {
        int row = m_currentCellCoords.GetRow();
        int col = m_currentCellCoords.GetCol();

        if ( m_table->IsEmptyCell(row, col) )
        {
            // starting in an empty cell: find the next block of
            // non-empty cells
            //
            while ( row > 0 )
            {
                row-- ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else if ( m_table->IsEmptyCell(row-1, col) )
        {
            // starting at the top of a block: find the next block
            //
            row--;
            while ( row > 0 )
            {
                row-- ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else
        {
            // starting within a block: find the top of the block
            //
            while ( row > 0 )
            {
                row-- ;
                if ( m_table->IsEmptyCell(row, col) )
                {
                    row++ ;
                    break;
                }
            }
        }

        MakeCellVisible( row, col );
        SetCurrentCell( row, col );

        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MoveCursorDownBlock()
{
    if ( m_table &&
         m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetRow() < m_numRows-1 )
    {
        int row = m_currentCellCoords.GetRow();
        int col = m_currentCellCoords.GetCol();

        if ( m_table->IsEmptyCell(row, col) )
        {
            // starting in an empty cell: find the next block of
            // non-empty cells
            //
            while ( row < m_numRows-1 )
            {
                row++ ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else if ( m_table->IsEmptyCell(row+1, col) )
        {
            // starting at the bottom of a block: find the next block
            //
            row++;
            while ( row < m_numRows-1 )
            {
                row++ ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else
        {
            // starting within a block: find the bottom of the block
            //
            while ( row < m_numRows-1 )
            {
                row++ ;
                if ( m_table->IsEmptyCell(row, col) )
                {
                    row-- ;
                    break;
                }
            }
        }

        MakeCellVisible( row, col );
        SetCurrentCell( row, col );

        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MoveCursorLeftBlock()
{
    if ( m_table &&
         m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetCol() > 0 )
    {
        int row = m_currentCellCoords.GetRow();
        int col = m_currentCellCoords.GetCol();

        if ( m_table->IsEmptyCell(row, col) )
        {
            // starting in an empty cell: find the next block of
            // non-empty cells
            //
            while ( col > 0 )
            {
                col-- ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else if ( m_table->IsEmptyCell(row, col-1) )
        {
            // starting at the left of a block: find the next block
            //
            col--;
            while ( col > 0 )
            {
                col-- ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else
        {
            // starting within a block: find the left of the block
            //
            while ( col > 0 )
            {
                col-- ;
                if ( m_table->IsEmptyCell(row, col) )
                {
                    col++ ;
                    break;
                }
            }
        }

        MakeCellVisible( row, col );
        SetCurrentCell( row, col );

        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MoveCursorRightBlock()
{
    if ( m_table &&
         m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetCol() < m_numCols-1 )
    {
        int row = m_currentCellCoords.GetRow();
        int col = m_currentCellCoords.GetCol();

        if ( m_table->IsEmptyCell(row, col) )
        {
            // starting in an empty cell: find the next block of
            // non-empty cells
            //
            while ( col < m_numCols-1 )
            {
                col++ ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else if ( m_table->IsEmptyCell(row, col+1) )
        {
            // starting at the right of a block: find the next block
            //
            col++;
            while ( col < m_numCols-1 )
            {
                col++ ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else
        {
            // starting within a block: find the right of the block
            //
            while ( col < m_numCols-1 )
            {
                col++ ;
                if ( m_table->IsEmptyCell(row, col) )
                {
                    col-- ;
                    break;
                }
            }
        }

        MakeCellVisible( row, col );
        SetCurrentCell( row, col );

        return TRUE;
    }

    return FALSE;
}



//
// ------ Label values and formatting
//

void wxGrid::GetRowLabelAlignment( int *horiz, int *vert )
{
    *horiz = m_rowLabelHorizAlign;
    *vert  = m_rowLabelVertAlign;
}

void wxGrid::GetColLabelAlignment( int *horiz, int *vert )
{
    *horiz = m_colLabelHorizAlign;
    *vert  = m_colLabelVertAlign;
}

wxString wxGrid::GetRowLabelValue( int row )
{
    if ( m_table )
    {
        return m_table->GetRowLabelValue( row );
    }
    else
    {
        wxString s;
        s << row;
        return s;
    }
}

wxString wxGrid::GetColLabelValue( int col )
{
    if ( m_table )
    {
        return m_table->GetColLabelValue( col );
    }
    else
    {
        wxString s;
        s << col;
        return s;
    }
}


void wxGrid::SetRowLabelSize( int width )
{
    width = wxMax( width, 0 );
    if ( width != m_rowLabelWidth )
    {
        if ( width == 0 )
        {
            m_rowLabelWin->Show( FALSE );
            m_cornerLabelWin->Show( FALSE );
        }
        else if ( m_rowLabelWidth == 0 )
        {
            m_rowLabelWin->Show( TRUE );
            if ( m_colLabelHeight > 0 ) m_cornerLabelWin->Show( TRUE );
        }

        m_rowLabelWidth = width;
        CalcWindowSizes();
        Refresh( TRUE );
    }
}


void wxGrid::SetColLabelSize( int height )
{
    height = wxMax( height, 0 );
    if ( height != m_colLabelHeight )
    {
        if ( height == 0 )
        {
            m_colLabelWin->Show( FALSE );
            m_cornerLabelWin->Show( FALSE );
        }
        else if ( m_colLabelHeight == 0 )
        {
            m_colLabelWin->Show( TRUE );
            if ( m_rowLabelWidth > 0 ) m_cornerLabelWin->Show( TRUE );
        }

        m_colLabelHeight = height;
        CalcWindowSizes();
        Refresh( TRUE );
    }
}


void wxGrid::SetLabelBackgroundColour( const wxColour& colour )
{
    if ( m_labelBackgroundColour != colour )
    {
        m_labelBackgroundColour = colour;
        m_rowLabelWin->SetBackgroundColour( colour );
        m_colLabelWin->SetBackgroundColour( colour );
        m_cornerLabelWin->SetBackgroundColour( colour );

        if ( !GetBatchCount() )
        {
            m_rowLabelWin->Refresh();
            m_colLabelWin->Refresh();
            m_cornerLabelWin->Refresh();
        }
    }
}

void wxGrid::SetLabelTextColour( const wxColour& colour )
{
    if ( m_labelTextColour != colour )
    {
        m_labelTextColour = colour;
        if ( !GetBatchCount() )
        {
            m_rowLabelWin->Refresh();
            m_colLabelWin->Refresh();
        }
    }
}

void wxGrid::SetLabelFont( const wxFont& font )
{
    m_labelFont = font;
    if ( !GetBatchCount() )
    {
        m_rowLabelWin->Refresh();
        m_colLabelWin->Refresh();
    }
}

void wxGrid::SetRowLabelAlignment( int horiz, int vert )
{
    if ( horiz == wxLEFT || horiz == wxCENTRE || horiz == wxRIGHT )
    {
        m_rowLabelHorizAlign = horiz;
    }

    if ( vert == wxTOP || vert == wxCENTRE || vert == wxBOTTOM )
    {
        m_rowLabelVertAlign = vert;
    }

    if ( !GetBatchCount() )
    {
        m_rowLabelWin->Refresh();
    }
}

void wxGrid::SetColLabelAlignment( int horiz, int vert )
{
    if ( horiz == wxLEFT || horiz == wxCENTRE || horiz == wxRIGHT )
    {
        m_colLabelHorizAlign = horiz;
    }

    if ( vert == wxTOP || vert == wxCENTRE || vert == wxBOTTOM )
    {
        m_colLabelVertAlign = vert;
    }

    if ( !GetBatchCount() )
    {
        m_colLabelWin->Refresh();
    }
}

void wxGrid::SetRowLabelValue( int row, const wxString& s )
{
    if ( m_table )
    {
        m_table->SetRowLabelValue( row, s );
        if ( !GetBatchCount() )
        {
            wxRect rect = CellToRect( row, 0);
            if ( rect.height > 0 )
            {
                CalcScrolledPosition(0, rect.y, &rect.x, &rect.y);
                rect.x = m_left;
                rect.width = m_rowLabelWidth;
                m_rowLabelWin->Refresh( TRUE, &rect );
            }
        }
    }
}

void wxGrid::SetColLabelValue( int col, const wxString& s )
{
    if ( m_table )
    {
        m_table->SetColLabelValue( col, s );
        if ( !GetBatchCount() )
        {
            wxRect rect = CellToRect( 0, col );
            if ( rect.width > 0 )
            {
                CalcScrolledPosition(rect.x, 0, &rect.x, &rect.y);
                rect.y = m_top;
                rect.height = m_colLabelHeight;
                m_colLabelWin->Refresh( TRUE, &rect );
            }
        }
    }
}

void wxGrid::SetGridLineColour( const wxColour& colour )
{
    if ( m_gridLineColour != colour )
    {
        m_gridLineColour = colour;

        wxClientDC dc( m_gridWin );
        PrepareDC( dc );
        DrawAllGridLines( dc, wxRegion() );
    }
}

void wxGrid::EnableGridLines( bool enable )
{
    if ( enable != m_gridLinesEnabled )
    {
        m_gridLinesEnabled = enable;

        if ( !GetBatchCount() )
        {
            if ( enable )
            {
                wxClientDC dc( m_gridWin );
                PrepareDC( dc );
                DrawAllGridLines( dc, wxRegion() );
            }
            else
            {
                m_gridWin->Refresh();
            }
        }
    }
}


int wxGrid::GetDefaultRowSize()
{
    return m_defaultRowHeight;
}

int wxGrid::GetRowSize( int row )
{
    wxCHECK_MSG( row >= 0 && row < m_numRows, 0, _T("invalid row index") );

    return GetRowHeight(row);
}

int wxGrid::GetDefaultColSize()
{
    return m_defaultColWidth;
}

int wxGrid::GetColSize( int col )
{
    wxCHECK_MSG( col >= 0 && col < m_numCols, 0, _T("invalid column index") );

    return GetColWidth(col);
}

// ============================================================================
// access to the grid attributes: each of them has a default value in the grid
// itself and may be overidden on a per-cell basis
// ============================================================================

// ----------------------------------------------------------------------------
// setting default attributes
// ----------------------------------------------------------------------------

void wxGrid::SetDefaultCellBackgroundColour( const wxColour& col )
{
    m_defaultCellAttr->SetBackgroundColour(col);
#ifdef __WXGTK__
    m_gridWin->SetBackgroundColour(col);
#endif
}

void wxGrid::SetDefaultCellTextColour( const wxColour& col )
{
    m_defaultCellAttr->SetTextColour(col);
}

void wxGrid::SetDefaultCellAlignment( int horiz, int vert )
{
    m_defaultCellAttr->SetAlignment(horiz, vert);
}

void wxGrid::SetDefaultCellFont( const wxFont& font )
{
    m_defaultCellAttr->SetFont(font);
}

void wxGrid::SetDefaultRenderer(wxGridCellRenderer *renderer)
{
    m_defaultCellAttr->SetRenderer(renderer);
}

void wxGrid::SetDefaultEditor(wxGridCellEditor *editor)
{
    m_defaultCellAttr->SetEditor(editor);
}

// ----------------------------------------------------------------------------
// access to the default attrbiutes
// ----------------------------------------------------------------------------

wxColour wxGrid::GetDefaultCellBackgroundColour()
{
    return m_defaultCellAttr->GetBackgroundColour();
}

wxColour wxGrid::GetDefaultCellTextColour()
{
    return m_defaultCellAttr->GetTextColour();
}

wxFont wxGrid::GetDefaultCellFont()
{
    return m_defaultCellAttr->GetFont();
}

void wxGrid::GetDefaultCellAlignment( int *horiz, int *vert )
{
    m_defaultCellAttr->GetAlignment(horiz, vert);
}

wxGridCellRenderer *wxGrid::GetDefaultRenderer() const
{
    return m_defaultCellAttr->GetRenderer(NULL,0,0);
}

wxGridCellEditor *wxGrid::GetDefaultEditor() const
{
    return m_defaultCellAttr->GetEditor(NULL,0,0);
}

// ----------------------------------------------------------------------------
// access to cell attributes
// ----------------------------------------------------------------------------

wxColour wxGrid::GetCellBackgroundColour(int row, int col)
{
    wxGridCellAttr *attr = GetCellAttr(row, col);
    wxColour colour = attr->GetBackgroundColour();
    attr->SafeDecRef();
    return colour;
}

wxColour wxGrid::GetCellTextColour( int row, int col )
{
    wxGridCellAttr *attr = GetCellAttr(row, col);
    wxColour colour = attr->GetTextColour();
    attr->SafeDecRef();
    return colour;
}

wxFont wxGrid::GetCellFont( int row, int col )
{
    wxGridCellAttr *attr = GetCellAttr(row, col);
    wxFont font = attr->GetFont();
    attr->SafeDecRef();
    return font;
}

void wxGrid::GetCellAlignment( int row, int col, int *horiz, int *vert )
{
    wxGridCellAttr *attr = GetCellAttr(row, col);
    attr->GetAlignment(horiz, vert);
    attr->SafeDecRef();
}

wxGridCellRenderer* wxGrid::GetCellRenderer(int row, int col)
{
    wxGridCellAttr* attr = GetCellAttr(row, col);
    wxGridCellRenderer* renderer = attr->GetRenderer(this, row, col);
    attr->DecRef();
    return renderer;
}

wxGridCellEditor* wxGrid::GetCellEditor(int row, int col)
{
    wxGridCellAttr* attr = GetCellAttr(row, col);
    wxGridCellEditor* editor = attr->GetEditor(this, row, col);
    attr->DecRef();
    return editor;
}

bool wxGrid::IsReadOnly(int row, int col) const
{
    wxGridCellAttr* attr = GetCellAttr(row, col);
    bool isReadOnly = attr->IsReadOnly();
    attr->DecRef();
    return isReadOnly;
}

// ----------------------------------------------------------------------------
// attribute support: cache, automatic provider creation, ...
// ----------------------------------------------------------------------------

bool wxGrid::CanHaveAttributes()
{
    if ( !m_table )
    {
        return FALSE;
    }

    return m_table->CanHaveAttributes();
}

void wxGrid::ClearAttrCache()
{
    if ( m_attrCache.row != -1 )
    {
        m_attrCache.attr->SafeDecRef();
        m_attrCache.row = -1;
    }
}

void wxGrid::CacheAttr(int row, int col, wxGridCellAttr *attr) const
{
    wxGrid *self = (wxGrid *)this;  // const_cast

    self->ClearAttrCache();
    self->m_attrCache.row = row;
    self->m_attrCache.col = col;
    self->m_attrCache.attr = attr;
    attr->SafeIncRef();
}

bool wxGrid::LookupAttr(int row, int col, wxGridCellAttr **attr) const
{
    if ( row == m_attrCache.row && col == m_attrCache.col )
    {
        *attr = m_attrCache.attr;
        (*attr)->SafeIncRef();

#ifdef DEBUG_ATTR_CACHE
        gs_nAttrCacheHits++;
#endif

        return TRUE;
    }
    else
    {
#ifdef DEBUG_ATTR_CACHE
        gs_nAttrCacheMisses++;
#endif
        return FALSE;
    }
}

wxGridCellAttr *wxGrid::GetCellAttr(int row, int col) const
{
    wxGridCellAttr *attr;
    if ( !LookupAttr(row, col, &attr) )
    {
        attr = m_table ? m_table->GetAttr(row, col) : (wxGridCellAttr *)NULL;
        CacheAttr(row, col, attr);
    }
    if (attr)
    {
        attr->SetDefAttr(m_defaultCellAttr);
    }
    else
    {
        attr = m_defaultCellAttr;
        attr->IncRef();
    }

    return attr;
}

wxGridCellAttr *wxGrid::GetOrCreateCellAttr(int row, int col) const
{
    wxGridCellAttr *attr;
    if ( !LookupAttr(row, col, &attr) || !attr )
    {
        wxASSERT_MSG( m_table,
                      _T("we may only be called if CanHaveAttributes() "
                         "returned TRUE and then m_table should be !NULL") );

        attr = m_table->GetAttr(row, col);
        if ( !attr )
        {
            attr = new wxGridCellAttr;

            // artificially inc the ref count to match DecRef() in caller
            attr->IncRef();

            m_table->SetAttr(attr, row, col);
        }

        CacheAttr(row, col, attr);
    }
    attr->SetDefAttr(m_defaultCellAttr);
    return attr;
}

// ----------------------------------------------------------------------------
// setting cell attributes: this is forwarded to the table
// ----------------------------------------------------------------------------

void wxGrid::SetRowAttr(int row, wxGridCellAttr *attr)
{
    if ( CanHaveAttributes() )
    {
        m_table->SetRowAttr(attr, row);
    }
    else
    {
        attr->SafeDecRef();
    }
}

void wxGrid::SetColAttr(int col, wxGridCellAttr *attr)
{
    if ( CanHaveAttributes() )
    {
        m_table->SetColAttr(attr, col);
    }
    else
    {
        attr->SafeDecRef();
    }
}

void wxGrid::SetCellBackgroundColour( int row, int col, const wxColour& colour )
{
    if ( CanHaveAttributes() )
    {
        wxGridCellAttr *attr = GetOrCreateCellAttr(row, col);
        attr->SetBackgroundColour(colour);
        attr->DecRef();
    }
}

void wxGrid::SetCellTextColour( int row, int col, const wxColour& colour )
{
    if ( CanHaveAttributes() )
    {
        wxGridCellAttr *attr = GetOrCreateCellAttr(row, col);
        attr->SetTextColour(colour);
        attr->DecRef();
    }
}

void wxGrid::SetCellFont( int row, int col, const wxFont& font )
{
    if ( CanHaveAttributes() )
    {
        wxGridCellAttr *attr = GetOrCreateCellAttr(row, col);
        attr->SetFont(font);
        attr->DecRef();
    }
}

void wxGrid::SetCellAlignment( int row, int col, int horiz, int vert )
{
    if ( CanHaveAttributes() )
    {
        wxGridCellAttr *attr = GetOrCreateCellAttr(row, col);
        attr->SetAlignment(horiz, vert);
        attr->DecRef();
    }
}

void wxGrid::SetCellRenderer(int row, int col, wxGridCellRenderer *renderer)
{
    if ( CanHaveAttributes() )
    {
        wxGridCellAttr *attr = GetOrCreateCellAttr(row, col);
        attr->SetRenderer(renderer);
        attr->DecRef();
    }
}

void wxGrid::SetCellEditor(int row, int col, wxGridCellEditor* editor)
{
    if ( CanHaveAttributes() )
    {
        wxGridCellAttr *attr = GetOrCreateCellAttr(row, col);
        attr->SetEditor(editor);
        attr->DecRef();
    }
}

void wxGrid::SetReadOnly(int row, int col, bool isReadOnly)
{
    if ( CanHaveAttributes() )
    {
        wxGridCellAttr *attr = GetOrCreateCellAttr(row, col);
        attr->SetReadOnly(isReadOnly);
        attr->DecRef();
    }
}

// ----------------------------------------------------------------------------
// Data type registration
// ----------------------------------------------------------------------------

void wxGrid::RegisterDataType(const wxString& typeName,
                              wxGridCellRenderer* renderer,
                              wxGridCellEditor* editor)
{
    m_typeRegistry->RegisterDataType(typeName, renderer, editor);
}


wxGridCellEditor* wxGrid::GetDefaultEditorForCell(int row, int col) const
{
    wxString typeName = m_table->GetTypeName(row, col);
    return GetDefaultEditorForType(typeName);
}

wxGridCellRenderer* wxGrid::GetDefaultRendererForCell(int row, int col) const
{
    wxString typeName = m_table->GetTypeName(row, col);
    return GetDefaultRendererForType(typeName);
}

wxGridCellEditor*
wxGrid::GetDefaultEditorForType(const wxString& typeName) const
{
    int index = m_typeRegistry->FindDataType(typeName);
    if (index == -1) {
        // Should we force the failure here or let it fallback to string handling???
        // wxFAIL_MSG(wxT("Unknown data type name"));
        return NULL;
    }
    return m_typeRegistry->GetEditor(index);
}

wxGridCellRenderer*
wxGrid::GetDefaultRendererForType(const wxString& typeName) const
{
    int index = m_typeRegistry->FindDataType(typeName);
    if (index == -1) {
        // Should we force the failure here or let it fallback to string handling???
        // wxFAIL_MSG(wxT("Unknown data type name"));
        return NULL;
    }
    return m_typeRegistry->GetRenderer(index);
}


// ----------------------------------------------------------------------------
// row/col size
// ----------------------------------------------------------------------------

void wxGrid::EnableDragRowSize( bool enable )
{
    m_canDragRowSize = enable;
}


void wxGrid::EnableDragColSize( bool enable )
{
    m_canDragColSize = enable;
}

void wxGrid::EnableDragGridSize( bool enable )
{
    m_canDragGridSize = enable;
}


void wxGrid::SetDefaultRowSize( int height, bool resizeExistingRows )
{
    m_defaultRowHeight = wxMax( height, WXGRID_MIN_ROW_HEIGHT );

    if ( resizeExistingRows )
    {
        InitRowHeights();

        CalcDimensions();
    }
}

void wxGrid::SetRowSize( int row, int height )
{
    wxCHECK_RET( row >= 0 && row < m_numRows, _T("invalid row index") );

    if ( m_rowHeights.IsEmpty() )
    {
        // need to really create the array
        InitRowHeights();
    }

    int h = wxMax( 0, height );
    int diff = h - m_rowHeights[row];

    m_rowHeights[row] = h;
    int i;
    for ( i = row;  i < m_numRows;  i++ )
    {
        m_rowBottoms[i] += diff;
    }
    CalcDimensions();
}

void wxGrid::SetDefaultColSize( int width, bool resizeExistingCols )
{
    m_defaultColWidth = wxMax( width, WXGRID_MIN_COL_WIDTH );

    if ( resizeExistingCols )
    {
        InitColWidths();

        CalcDimensions();
    }
}

void wxGrid::SetColSize( int col, int width )
{
    wxCHECK_RET( col >= 0 && col < m_numCols, _T("invalid column index") );

    // should we check that it's bigger than GetColMinimalWidth(col) here?

    if ( m_colWidths.IsEmpty() )
    {
        // need to really create the array
        InitColWidths();
    }

    int w = wxMax( 0, width );
    int diff = w - m_colWidths[col];
    m_colWidths[col] = w;

    int i;
    for ( i = col;  i < m_numCols;  i++ )
    {
        m_colRights[i] += diff;
    }
    CalcDimensions();
}


void wxGrid::SetColMinimalWidth( int col, int width )
{
    m_colMinWidths.Put(col, (wxObject *)width);
}

int wxGrid::GetColMinimalWidth(int col) const
{
    wxObject *obj = m_colMinWidths.Get(m_dragRowOrCol);
    return obj ? (int)obj : WXGRID_MIN_COL_WIDTH;
}

void wxGrid::AutoSizeColumn( int col, bool setAsMin )
{
    wxClientDC dc(m_gridWin);

    wxCoord width, widthMax = 0;
    for ( int row = 0; row < m_numRows; row++ )
    {
        wxGridCellAttr* attr = GetCellAttr(row, col);
        wxGridCellRenderer* renderer = attr->GetRenderer(this, row, col);
        if ( renderer )
        {
            width = renderer->GetBestSize(*this, *attr, dc, row, col).x;
            if ( width > widthMax )
            {
                widthMax = width;
            }
        }

        attr->DecRef();
    }

    // now also compare with the column label width
    dc.SetFont( GetLabelFont() );
    dc.GetTextExtent( GetColLabelValue(col), &width, NULL );
    if ( width > widthMax )
    {
        widthMax = width;
    }

    if ( !widthMax )
    {
        // empty column - give default width (notice that if widthMax is less
        // than default width but != 0, it's ok)
        widthMax = m_defaultColWidth;
    }
    else
    {
        // leave some space around text
        widthMax += 10;
    }

    SetColSize(col, widthMax);
    if ( setAsMin )
    {
        SetColMinimalWidth(col, widthMax);
    }
}

void wxGrid::AutoSizeColumns( bool setAsMin )
{
    for ( int col = 0; col < m_numCols; col++ )
    {
        AutoSizeColumn(col, setAsMin);
    }
}

//
// ------ cell value accessor functions
//

void wxGrid::SetCellValue( int row, int col, const wxString& s )
{
    if ( m_table )
    {
        m_table->SetValue( row, col, s.c_str() );
        if ( !GetBatchCount() )
        {
            wxClientDC dc( m_gridWin );
            PrepareDC( dc );
            DrawCell( dc, wxGridCellCoords(row, col) );
        }

        if ( m_currentCellCoords.GetRow() == row &&
             m_currentCellCoords.GetCol() == col &&
             IsCellEditControlEnabled())
        {
            HideCellEditControl();
            ShowCellEditControl(); // will reread data from table
        }
    }
}


//
// ------ Block, row and col selection
//

void wxGrid::SelectRow( int row, bool addToSelected )
{
    wxRect r;

    if ( IsSelection() && addToSelected )
    {
        wxRect  rect[4];
        bool    need_refresh[4];
        need_refresh[0] =
        need_refresh[1] =
        need_refresh[2] =
        need_refresh[3] = FALSE;

        int     i;

        wxCoord oldLeft   = m_selectedTopLeft.GetCol();
        wxCoord oldTop    = m_selectedTopLeft.GetRow();
        wxCoord oldRight  = m_selectedBottomRight.GetCol();
        wxCoord oldBottom = m_selectedBottomRight.GetRow();

        if ( oldTop > row )
        {
            need_refresh[0] = TRUE;
            rect[0] = BlockToDeviceRect( wxGridCellCoords ( row, 0 ),
                                         wxGridCellCoords ( oldTop - 1,
                                                            m_numCols - 1 ) );
            m_selectedTopLeft.SetRow( row );
        }

        if ( oldLeft > 0 )
        {
            need_refresh[1] = TRUE;
            rect[1] = BlockToDeviceRect( wxGridCellCoords ( oldTop, 0 ),
                                         wxGridCellCoords ( oldBottom,
                                                            oldLeft - 1 ) );

            m_selectedTopLeft.SetCol( 0 );
        }

        if ( oldBottom < row )
        {
            need_refresh[2] = TRUE;
            rect[2] = BlockToDeviceRect( wxGridCellCoords ( oldBottom + 1, 0 ),
                                         wxGridCellCoords ( row,
                                                            m_numCols - 1 ) );
            m_selectedBottomRight.SetRow( row );
        }

        if ( oldRight < m_numCols - 1 )
        {
            need_refresh[3] = TRUE;
            rect[3] = BlockToDeviceRect( wxGridCellCoords ( oldTop ,
                                                            oldRight + 1 ),
                                         wxGridCellCoords ( oldBottom,
                                                            m_numCols - 1 ) );
            m_selectedBottomRight.SetCol( m_numCols - 1 );
        }

        for (i = 0; i < 4; i++ )
            if ( need_refresh[i] && rect[i] != wxGridNoCellRect )
                m_gridWin->Refresh( FALSE, &(rect[i]) );
    }
    else
    {
        r = SelectionToDeviceRect();
        ClearSelection();
        if ( r != wxGridNoCellRect ) m_gridWin->Refresh( FALSE, &r );

        m_selectedTopLeft.Set( row, 0 );
        m_selectedBottomRight.Set( row, m_numCols-1 );
        r = SelectionToDeviceRect();
        m_gridWin->Refresh( FALSE, &r );
    }

    wxGridRangeSelectEvent gridEvt( GetId(),
                                    wxEVT_GRID_RANGE_SELECT,
                                    this,
                                    m_selectedTopLeft,
                                    m_selectedBottomRight );

    GetEventHandler()->ProcessEvent(gridEvt);
}


void wxGrid::SelectCol( int col, bool addToSelected )
{
    if ( IsSelection() && addToSelected )
    {
        wxRect  rect[4];
        bool    need_refresh[4];
        need_refresh[0] =
        need_refresh[1] =
        need_refresh[2] =
        need_refresh[3] = FALSE;
        int     i;

        wxCoord oldLeft   = m_selectedTopLeft.GetCol();
        wxCoord oldTop    = m_selectedTopLeft.GetRow();
        wxCoord oldRight  = m_selectedBottomRight.GetCol();
        wxCoord oldBottom = m_selectedBottomRight.GetRow();

        if ( oldLeft > col )
        {
            need_refresh[0] = TRUE;
            rect[0] = BlockToDeviceRect( wxGridCellCoords ( 0, col ),
                                         wxGridCellCoords ( m_numRows - 1,
                                                            oldLeft - 1 ) );
            m_selectedTopLeft.SetCol( col );
        }

        if ( oldTop  > 0 )
        {
            need_refresh[1] = TRUE;
            rect[1] = BlockToDeviceRect( wxGridCellCoords ( 0, oldLeft ),
                                         wxGridCellCoords ( oldTop - 1,
                                                            oldRight ) );
            m_selectedTopLeft.SetRow( 0 );
        }

        if ( oldRight < col )
        {
            need_refresh[2] = TRUE;
            rect[2] = BlockToDeviceRect( wxGridCellCoords ( 0, oldRight + 1 ),
                                         wxGridCellCoords ( m_numRows - 1,
                                                            col            ) );
            m_selectedBottomRight.SetCol( col );
        }

        if ( oldBottom < m_numRows - 1 )
        {
            need_refresh[3] = TRUE;
            rect[3] = BlockToDeviceRect( wxGridCellCoords ( oldBottom + 1,
                                                            oldLeft ),
                                         wxGridCellCoords ( m_numRows - 1,
                                                            oldRight ) );
            m_selectedBottomRight.SetRow( m_numRows - 1 );
        }

        for (i = 0; i < 4; i++ )
            if ( need_refresh[i] && rect[i] != wxGridNoCellRect )
                m_gridWin->Refresh( FALSE, &(rect[i]) );
    }
    else
    {
        wxRect r;

        r = SelectionToDeviceRect();
        ClearSelection();
        if ( r != wxGridNoCellRect ) m_gridWin->Refresh( FALSE, &r );

        m_selectedTopLeft.Set( 0, col );
        m_selectedBottomRight.Set( m_numRows-1, col );
        r = SelectionToDeviceRect();
        m_gridWin->Refresh( FALSE, &r );
    }

    wxGridRangeSelectEvent gridEvt( GetId(),
                                    wxEVT_GRID_RANGE_SELECT,
                                    this,
                                    m_selectedTopLeft,
                                    m_selectedBottomRight );

    GetEventHandler()->ProcessEvent(gridEvt);
}


void wxGrid::SelectBlock( int topRow, int leftCol, int bottomRow, int rightCol )
{
    int temp;
    wxGridCellCoords updateTopLeft, updateBottomRight;

    if ( topRow > bottomRow )
    {
        temp = topRow;
        topRow = bottomRow;
        bottomRow = temp;
    }

    if ( leftCol > rightCol )
    {
        temp = leftCol;
        leftCol = rightCol;
        rightCol = temp;
    }

    updateTopLeft = wxGridCellCoords( topRow, leftCol );
    updateBottomRight = wxGridCellCoords( bottomRow, rightCol );

    if ( m_selectedTopLeft != updateTopLeft ||
         m_selectedBottomRight != updateBottomRight )
    {
        // Compute two optimal update rectangles:
        // Either one rectangle is a real subset of the
        // other, or they are (almost) disjoint!
        wxRect  rect[4];
        bool    need_refresh[4];
        need_refresh[0] =
        need_refresh[1] =
        need_refresh[2] =
        need_refresh[3] = FALSE;
        int     i;

        // Store intermediate values
        wxCoord oldLeft   = m_selectedTopLeft.GetCol();
        wxCoord oldTop    = m_selectedTopLeft.GetRow();
        wxCoord oldRight  = m_selectedBottomRight.GetCol();
        wxCoord oldBottom = m_selectedBottomRight.GetRow();

        // Determine the outer/inner coordinates.
        if (oldLeft > leftCol)
        {
            temp = oldLeft;
            oldLeft = leftCol;
            leftCol = temp;
        }
        if (oldTop > topRow )
        {
            temp = oldTop;
            oldTop = topRow;
            topRow = temp;
        }
        if (oldRight < rightCol )
        {
            temp = oldRight;
            oldRight = rightCol;
            rightCol = temp;
        }
        if (oldBottom < bottomRow)
        {
            temp = oldBottom;
            oldBottom = bottomRow;
            bottomRow = temp;
        }

        // Now, either the stuff marked old is the outer
        // rectangle or we don't have a situation where one
        // is contained in the other.

        if ( oldLeft < leftCol )
        {
            need_refresh[0] = TRUE;
            rect[0] = BlockToDeviceRect( wxGridCellCoords ( oldTop,
                                                            oldLeft ),
                                         wxGridCellCoords ( oldBottom,
                                                            leftCol - 1 ) );
        }

        if ( oldTop  < topRow )
        {
            need_refresh[1] = TRUE;
            rect[1] = BlockToDeviceRect( wxGridCellCoords ( oldTop,
                                                            leftCol ),
                                         wxGridCellCoords ( topRow - 1,
                                                            rightCol ) );
        }

        if ( oldRight > rightCol )
        {
            need_refresh[2] = TRUE;
            rect[2] = BlockToDeviceRect( wxGridCellCoords ( oldTop,
                                                            rightCol + 1 ),
                                         wxGridCellCoords ( oldBottom,
                                                            oldRight ) );
        }

        if ( oldBottom > bottomRow )
        {
            need_refresh[3] = TRUE;
            rect[3] = BlockToDeviceRect( wxGridCellCoords ( bottomRow + 1,
                                                            leftCol ),
                                         wxGridCellCoords ( oldBottom,
                                                            rightCol ) );
        }


        // Change Selection
        m_selectedTopLeft = updateTopLeft;
        m_selectedBottomRight = updateBottomRight;

        // various Refresh() calls
        for (i = 0; i < 4; i++ )
            if ( need_refresh[i] && rect[i] != wxGridNoCellRect )
                m_gridWin->Refresh( FALSE, &(rect[i]) );
    }

    // only generate an event if the block is not being selected by
    // dragging the mouse (in which case the event will be generated in
    // the mouse event handler)
    if ( !m_isDragging )
    {
        wxGridRangeSelectEvent gridEvt( GetId(),
                                        wxEVT_GRID_RANGE_SELECT,
                                        this,
                                        m_selectedTopLeft,
                                        m_selectedBottomRight );

        GetEventHandler()->ProcessEvent(gridEvt);
    }
}

void wxGrid::SelectAll()
{
    m_selectedTopLeft.Set( 0, 0 );
    m_selectedBottomRight.Set( m_numRows-1, m_numCols-1 );

    m_gridWin->Refresh();
}


void wxGrid::ClearSelection()
{
    m_selectedTopLeft = wxGridNoCellCoords;
    m_selectedBottomRight = wxGridNoCellCoords;
}


// This function returns the rectangle that encloses the given block
// in device coords clipped to the client size of the grid window.
//
wxRect wxGrid::BlockToDeviceRect( const wxGridCellCoords &topLeft,
                                  const wxGridCellCoords &bottomRight )
{
    wxRect rect( wxGridNoCellRect );
    wxRect cellRect;

    cellRect = CellToRect( topLeft );
    if ( cellRect != wxGridNoCellRect )
    {
        rect = cellRect;
    }
    else
    {
        rect = wxRect( 0, 0, 0, 0 );
    }

    cellRect = CellToRect( bottomRight );
    if ( cellRect != wxGridNoCellRect )
    {
        rect += cellRect;
    }
    else
    {
        return wxGridNoCellRect;
    }

    // convert to scrolled coords
    //
    int left, top, right, bottom;
    CalcScrolledPosition( rect.GetLeft(), rect.GetTop(), &left, &top );
    CalcScrolledPosition( rect.GetRight(), rect.GetBottom(), &right, &bottom );

    int cw, ch;
    m_gridWin->GetClientSize( &cw, &ch );

    rect.SetLeft( wxMax(0, left) );
    rect.SetTop( wxMax(0, top) );
    rect.SetRight( wxMin(cw, right) );
    rect.SetBottom( wxMin(ch, bottom) );

    return rect;
}



//
// ------ Grid event classes
//

IMPLEMENT_DYNAMIC_CLASS( wxGridEvent, wxEvent )

wxGridEvent::wxGridEvent( int id, wxEventType type, wxObject* obj,
                          int row, int col, int x, int y,
                          bool control, bool shift, bool alt, bool meta )
        : wxNotifyEvent( type, id )
{
    m_row = row;
    m_col = col;
    m_x = x;
    m_y = y;
    m_control = control;
    m_shift = shift;
    m_alt = alt;
    m_meta = meta;

    SetEventObject(obj);
}


IMPLEMENT_DYNAMIC_CLASS( wxGridSizeEvent, wxEvent )

wxGridSizeEvent::wxGridSizeEvent( int id, wxEventType type, wxObject* obj,
                                  int rowOrCol, int x, int y,
                                  bool control, bool shift, bool alt, bool meta )
        : wxNotifyEvent( type, id )
{
    m_rowOrCol = rowOrCol;
    m_x = x;
    m_y = y;
    m_control = control;
    m_shift = shift;
    m_alt = alt;
    m_meta = meta;

    SetEventObject(obj);
}


IMPLEMENT_DYNAMIC_CLASS( wxGridRangeSelectEvent, wxEvent )

wxGridRangeSelectEvent::wxGridRangeSelectEvent(int id, wxEventType type, wxObject* obj,
                                               const wxGridCellCoords& topLeft,
                                               const wxGridCellCoords& bottomRight,
                                               bool control, bool shift, bool alt, bool meta )
        : wxNotifyEvent( type, id )
{
    m_topLeft     = topLeft;
    m_bottomRight = bottomRight;
    m_control     = control;
    m_shift       = shift;
    m_alt         = alt;
    m_meta        = meta;

    SetEventObject(obj);
}


#endif // ifndef wxUSE_NEW_GRID

