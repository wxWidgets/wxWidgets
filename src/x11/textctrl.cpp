/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textctrl.h"
#endif

#include "wx/textctrl.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/settings.h"
#include "wx/panel.h"
#include "wx/clipbrd.h"
#include "wx/tokenzr.h"

#include "wx/univ/inphand.h"
#include "wx/univ/renderer.h"
#include "wx/univ/colschem.h"
#include "wx/univ/theme.h"

//-----------------------------------------------------------------------------
//  helpers
//-----------------------------------------------------------------------------

wxSourceUndoStep::wxSourceUndoStep( wxSourceUndo type, int y1, int y2, wxTextCtrl *owner )
{
    m_type = type;
    m_y1 = y1;
    m_y2 = y2;
    m_owner = owner;
    
    m_cursorX = m_owner->GetCursorX();
    m_cursorY = m_owner->GetCursorY();
    
    if (m_type == wxSOURCE_UNDO_LINE)
    {
        m_text = m_owner->m_lines[m_y1].m_text;
    } else
    if (m_type == wxSOURCE_UNDO_ENTER)
    {
        m_text = m_owner->m_lines[m_y1].m_text;
    } else
    if (m_type == wxSOURCE_UNDO_BACK)
    {
        for (int i = m_y1; i < m_y2+2; i++)
        {
            if (i >= (int)m_owner->m_lines.GetCount())
                m_lines.Add( "" );
            else
                m_lines.Add( m_owner->m_lines[i].m_text );
        }
    } else
    if (m_type == wxSOURCE_UNDO_DELETE)
    {
        for (int i = m_y1; i < m_y2+1; i++)
        {
            m_lines.Add( m_owner->m_lines[i].m_text );
        }
    } else
    if (m_type == wxSOURCE_UNDO_PASTE)
    {
        m_text = m_owner->m_lines[m_y1].m_text;
    }
}

void wxSourceUndoStep::Undo()
{
    if (m_type == wxSOURCE_UNDO_LINE)
    {
        m_owner->m_lines[m_y1].m_text = m_text;
        m_owner->MoveCursor( m_cursorX, m_cursorY );
        m_owner->RefreshLine( m_y1 );
    } else
    if (m_type == wxSOURCE_UNDO_ENTER)
    {
        m_owner->m_lines[m_y1].m_text = m_text;
        m_owner->m_lines.RemoveAt( m_y1+1 );
        m_owner->MoveCursor( m_cursorX, m_cursorY );
        m_owner->RefreshDown( m_y1 );
    } else
    if (m_type == wxSOURCE_UNDO_BACK)
    {
        m_owner->m_lines[m_y1].m_text = m_lines[0];
        m_owner->m_lines.Insert( new wxSourceLine( m_lines[1] ), m_y1+1 );
        m_owner->MyAdjustScrollbars();
        m_owner->MoveCursor( m_cursorX, m_cursorY );
        m_owner->RefreshDown( m_y1 );
    } else
    if (m_type == wxSOURCE_UNDO_DELETE)
    {
        m_owner->m_lines[m_y1].m_text = m_lines[0];
        for (int i = 1; i < (int)m_lines.GetCount(); i++)
            m_owner->m_lines.Insert( new wxSourceLine( m_lines[i] ), m_y1+i );
        m_owner->MyAdjustScrollbars();
        m_owner->MoveCursor( m_cursorX, m_cursorY );
        m_owner->RefreshDown( m_y1 );
    } else
    if (m_type == wxSOURCE_UNDO_PASTE)
    {
        m_owner->m_lines[m_y1].m_text = m_text;
        for (int i = 0; i < m_y2-m_y1; i++)
            m_owner->m_lines.RemoveAt( m_y1+1 );
        m_owner->MyAdjustScrollbars();
        m_owner->MoveCursor( m_cursorX, m_cursorY );
        m_owner->RefreshDown( m_y1 );
    } else
    if (m_type == wxSOURCE_UNDO_INSERT_LINE)
    {
        m_owner->m_lines.RemoveAt( m_y1 );
        m_owner->MyAdjustScrollbars();
        m_owner->MoveCursor( 0, m_y1 );
        m_owner->RefreshDown( m_y1 );
    }
}

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxSourceLineArray);

//-----------------------------------------------------------------------------
//  wxTextCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl,wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
    EVT_PAINT(wxTextCtrl::OnPaint)
    EVT_CHAR(wxTextCtrl::OnChar)
    EVT_MOUSE_EVENTS(wxTextCtrl::OnMouse)
    EVT_IDLE(wxTextCtrl::OnIdle)
    
    EVT_MENU(wxID_CUT, wxTextCtrl::OnCut)
    EVT_MENU(wxID_COPY, wxTextCtrl::OnCopy)
    EVT_MENU(wxID_PASTE, wxTextCtrl::OnPaste)
    EVT_MENU(wxID_UNDO, wxTextCtrl::OnUndo)
    EVT_MENU(wxID_REDO, wxTextCtrl::OnRedo)

    EVT_UPDATE_UI(wxID_CUT, wxTextCtrl::OnUpdateCut)
    EVT_UPDATE_UI(wxID_COPY, wxTextCtrl::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_PASTE, wxTextCtrl::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_UNDO, wxTextCtrl::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxTextCtrl::OnUpdateRedo)
END_EVENT_TABLE()

void wxTextCtrl::Init()
{
    m_editable = TRUE;
    m_modified = FALSE;
    
    m_sourceFont = wxFont( 12, wxMODERN, wxNORMAL, wxNORMAL );

    m_undos.DeleteContents( TRUE );
    
    m_lang = wxSOURCE_LANG_NONE;
    
    m_capturing = FALSE;
    
    wxClientDC dc(this);
    dc.SetFont( m_sourceFont );
    m_lineHeight = dc.GetCharHeight();
    m_charWidth = dc.GetCharWidth();
    
    m_cursorX = 0;
    m_cursorY = 0;
    
    m_longestLine = 0;
    
    m_bracketX = -1;
    m_bracketY = -1;
    
    m_overwrite = FALSE;
    m_ignoreInput = FALSE;
    
    ClearSelection();
    
    m_keywordColour = wxColour( 10, 140, 10 );
    
    m_defineColour = *wxRED;
    
    m_variableColour = wxColour( 50, 120, 150 );
    
    m_commentColour = wxColour( 130, 130, 130 );
    
    m_stringColour = wxColour( 10, 140, 10 );
}

wxTextCtrl::wxTextCtrl( wxWindow *parent,
                        wxWindowID id,
                        const wxString &value,
                        const wxPoint &pos,
                        const wxSize &size,
                        long style,
                        const wxValidator& validator,
                        const wxString &name )
    : wxScrollHelper(this)
{
    Init();

    Create( parent, id, value, pos, size, style, validator, name );
}

bool wxTextCtrl::Create( wxWindow *parent,
                         wxWindowID id,
                         const wxString &value,
                         const wxPoint &pos,
                         const wxSize &size,
                         long style,
                         const wxValidator& validator,
                         const wxString &name )
{
    if ((style & wxBORDER_MASK) == 0)
        style |= wxBORDER_SUNKEN;
        
    if ((style & wxTE_MULTILINE) != 0)
        style |= wxALWAYS_SHOW_SB;
        
    wxTextCtrlBase::Create( parent, id, wxDefaultPosition, size,
                              style|wxVSCROLL|wxHSCROLL|wxNO_FULL_REPAINT_ON_RESIZE );
                              
    SetBackgroundColour( *wxWHITE );
    
    SetCursor( wxCursor( wxCURSOR_IBEAM ) );
    
    SetValue( value );

    wxSize size_best( DoGetBestSize() );
    wxSize new_size( size );
    if (new_size.x == -1)
        new_size.x = size_best.x;
    if (new_size.y == -1)
        new_size.y = size_best.y;
    if ((new_size.x != size.x) || (new_size.y != size.y))
        SetSize( new_size.x, new_size.y );
    
    // We create an input handler since it might be useful
    CreateInputHandler(wxINP_HANDLER_TEXTCTRL);
    
    return TRUE;
}

//-----------------------------------------------------------------------------
//  public methods
//-----------------------------------------------------------------------------

wxString wxTextCtrl::GetValue() const
{
    wxString ret;
    for (size_t i = 0; i < m_lines.GetCount(); i++)
    {
        ret += m_lines[i].m_text;
        if (i < m_lines.GetCount())
            ret += wxT('\n');
    }
    
    return ret;
}

void wxTextCtrl::SetValue(const wxString& value)
{
    m_modified = TRUE;
    m_cursorX = 0;
    m_cursorY = 0;
    ClearSelection();
    m_lines.Clear();
    m_longestLine = 0;
    
    int pos = 0;
    for (;;)
    {
        // TODO make more efficient
        wxString tmp = value;
        tmp.Remove( 0, pos );
        pos = tmp.Find( '\n' );
        if (pos == -1)
        {
            if (tmp.Len() > m_longestLine)
                m_longestLine = tmp.Len();
                
            m_lines.Add( new wxSourceLine( tmp ) );
            break;
            
        }
        else
        {
            if (pos > m_longestLine)
                m_longestLine = pos;
                
            tmp.Remove( pos, tmp.Len()-pos );
            m_lines.Add( new wxSourceLine( tmp ) );
        }
    }
    
    MyAdjustScrollbars();
    
    Refresh();
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    if (lineNo >= (long)m_lines.GetCount())
        return 0;
        
    return m_lines[lineNo].m_text.Len();
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    if (lineNo >= (long)m_lines.GetCount())
        return wxT("");
        
    return m_lines[lineNo].m_text;
}

int wxTextCtrl::GetNumberOfLines() const
{
    return  m_lines.GetCount();
}

bool wxTextCtrl::IsModified() const
{
    return m_modified;
}

bool wxTextCtrl::IsEditable() const
{
    return m_editable;
}

void wxTextCtrl::GetSelection(long* from, long* to) const
{
}

void wxTextCtrl::Clear()
{
    m_modified = TRUE;
    m_cursorX = 0;
    m_cursorY = 0;
    ClearSelection();
    m_lines.Clear();
    SetScrollbars( m_charWidth, m_lineHeight, 0, 0, 0, 0 );
    Refresh();
    m_undos.Clear();
}

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
}

void wxTextCtrl::Remove(long from, long to)
{
    
}

void wxTextCtrl::DiscardEdits()
{
    ClearSelection();
    Refresh();
}

void wxTextCtrl::SetMaxLength(unsigned long len)
{
}

void wxTextCtrl::WriteText(const wxString& text2)
{
    m_modified = TRUE;
    
    wxString text( text2 );
    wxArrayString lines;
    int pos;
    while ( (pos = text.Find('\n')) != -1 )
    {
       lines.Add( text.Left( pos ) );
       text.Remove( 0, pos+1 );
    }
    lines.Add( text );
    int count = (int)lines.GetCount();
    
    wxString tmp1( m_lines[m_cursorY].m_text );
    wxString tmp2( tmp1 );
    int len = (int)tmp1.Len();
    
    if (len < m_cursorX)
    {
        wxString tmp;
        for (int i = 0; i < m_cursorX-len; i++)
            tmp.Append( ' ' );
        m_lines[m_cursorY].m_text.Append( tmp );
        tmp1.Append( tmp );
        tmp2.Append( tmp );
    }

    tmp1.Remove( m_cursorX );
    tmp2.Remove( 0, m_cursorX );
    tmp1.Append( lines[0] );
    
    if (count == 1)
    {
        m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_LINE, m_cursorY, m_cursorY, this ) );
        
        tmp1.Append( tmp2 );
        m_lines[m_cursorY].m_text = tmp1;
        RefreshLine( m_cursorY );
    }
    else
    {
        m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_PASTE, m_cursorY, m_cursorY+count-1, this ) );
        
        m_lines[m_cursorY].m_text = tmp1;
        int i;
        for (i = 1; i < count; i++)
            m_lines.Insert( new wxSourceLine( lines[i] ), m_cursorY+i );
        m_lines[m_cursorY+i-1].m_text.Append( tmp2 );
        
        MyAdjustScrollbars();
        RefreshDown( m_cursorY );
    }
}

void wxTextCtrl::AppendText(const wxString& text)
{
    // Leaves cursor garbage

    m_cursorY = m_lines.GetCount()-1;
    m_cursorX = m_lines[m_cursorY].m_text.Len();
    
    WriteText( text );
    
    Refresh();
}

bool wxTextCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    return FALSE;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    long ret = 0;
    
    for (size_t i = 0; i < m_lines.GetCount(); i++)
    {
        if (i < (size_t)y)
        {
            ret += m_lines[i].m_text.Len();
            continue;
        }
        
        if ((size_t)x < m_lines[i].m_text.Len())
            return (ret + x);
        else
            return (ret + m_lines[i].m_text.Len());
    }
     
    return ret;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    if (m_lines.GetCount() == 0)
    {
        if (x) *x = 0;
        if (y) *y = 0;

        return (pos == 0);
    }

    long xx = 0;
    long yy = 0;
    
    for (size_t i = 0; i < m_lines.GetCount(); i++)
    {
        pos -= m_lines[i].m_text.Len();
        if (pos <= 0)
        {
            xx = -pos;
            if (x) *x = xx;
            if (y) *y = yy;
            return TRUE;
        }
        yy++;
    }
    
    // Last pos
    xx = m_lines[ m_lines.GetCount()-1 ].m_text.Len();
    if (x) *x = xx;
    if (y) *y = yy;
    
    return FALSE;
}

void wxTextCtrl::ShowPosition(long pos)
{
}

void wxTextCtrl::Copy()
{
    if (!HasSelection()) return;
    
    wxString sel;
    
    int selStartY = m_selStartY;
    int selEndY = m_selEndY;
    int selStartX = m_selStartX;
    int selEndX = m_selEndX;
    
    if ((selStartY > selEndY) ||
        ((selStartY == selEndY) && (selStartX > selEndX)))
    {
        int tmp = selStartX;
        selStartX = selEndX;
        selEndX = tmp;
        tmp = selStartY;
        selStartY = selEndY;
        selEndY = tmp;
    }
    
    if (selStartY == selEndY)
    {
        sel = m_lines[selStartY].m_text;
        
        if (selStartX >= (int)sel.Len()) return;
        if (selEndX > (int)sel.Len())
            selEndX = sel.Len();
        
        sel.Remove( selEndX, sel.Len()-selEndX );
        sel.Remove( 0, selStartX );
    }
    else
    {
        wxString tmp( m_lines[selStartY].m_text );
        
        if (selStartX < (int)tmp.Len())
        {
            tmp.Remove( 0, selStartX );
            sel = tmp;
            sel.Append( "\n" );
        }
        for (int i = selStartY+1; i < selEndY; i++)
        {
            sel.Append( m_lines[i].m_text );
            sel.Append( "\n" );
        }
        tmp = m_lines[selEndY].m_text;
        if (selEndX > (int)tmp.Len())
            selEndX = tmp.Len();
        if (selEndX > 0)
        {
            tmp.Remove( selEndX, tmp.Len()-selEndX );
            sel.Append( tmp );
        }
    }
    
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData( new wxTextDataObject( sel ) );
        wxTheClipboard->Close();
    }
}

void wxTextCtrl::Cut()
{
   Copy();
   
   Delete();
}

void wxTextCtrl::Paste()
{
    Delete();
    
    if (!wxTheClipboard->Open()) return;
    
    if (!wxTheClipboard->IsSupported( wxDF_TEXT ))
    {
        wxTheClipboard->Close();
        
        return;
    }
    
    wxTextDataObject data;
    
    bool ret = wxTheClipboard->GetData( data );
    
    wxTheClipboard->Close();
    
    if (!ret) return;
    
    m_modified = TRUE;
    
    wxString text( data.GetText() );
    wxArrayString lines;
    int pos;
    while ( (pos = text.Find('\n')) != -1 )
    {
       lines.Add( text.Left( pos ) );
       text.Remove( 0, pos+1 );
    }
    lines.Add( text );
    int count = (int)lines.GetCount();
    
    wxString tmp1( m_lines[m_cursorY].m_text );
    wxString tmp2( tmp1 );
    int len = (int)tmp1.Len();
    
    if (len < m_cursorX)
    {
        wxString tmp;
        for (int i = 0; i < m_cursorX-len; i++)
            tmp.Append( ' ' );
        m_lines[m_cursorY].m_text.Append( tmp );
        tmp1.Append( tmp );
        tmp2.Append( tmp );
    }

    tmp1.Remove( m_cursorX );
    tmp2.Remove( 0, m_cursorX );
    tmp1.Append( lines[0] );
    
    if (count == 1)
    {
        m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_LINE, m_cursorY, m_cursorY, this ) );
        
        tmp1.Append( tmp2 );
        m_lines[m_cursorY].m_text = tmp1;
        RefreshLine( m_cursorY );
    }
    else
    {
        m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_PASTE, m_cursorY, m_cursorY+count-1, this ) );
        
        m_lines[m_cursorY].m_text = tmp1;
        int i;
        for (i = 1; i < count; i++)
            m_lines.Insert( new wxSourceLine( lines[i] ), m_cursorY+i );
        m_lines[m_cursorY+i-1].m_text.Append( tmp2 );
        
        MyAdjustScrollbars();
        RefreshDown( m_cursorY );
    }
}

void wxTextCtrl::Undo()
{
    if (m_undos.GetCount() == 0) return;
    
    wxNode *node = m_undos.Nth( m_undos.GetCount()-1 );
    wxSourceUndoStep *undo = (wxSourceUndoStep*) node->Data();
    
    undo->Undo();
    
    delete node;
    
    m_modified = TRUE;
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
}

void wxTextCtrl::SetInsertionPointEnd()
{
}

long wxTextCtrl::GetInsertionPoint() const
{
    return XYToPosition( m_cursorX, m_cursorY );
}

long wxTextCtrl::GetLastPosition() const
{
    size_t lineCount = m_lines.GetCount() - 1;
    return XYToPosition( m_lines[lineCount].m_text.Len()-1, lineCount );
}

void wxTextCtrl::SetSelection(long from, long to)
{
}

void wxTextCtrl::SetEditable(bool editable)
{
    m_editable = editable;
}

bool wxTextCtrl::Enable( bool enable )
{
    return FALSE;
}

bool wxTextCtrl::SetFont(const wxFont& font)
{
    return FALSE;
}

bool wxTextCtrl::SetForegroundColour(const wxColour& colour)
{
    return wxWindow::SetForegroundColour( colour );
}

bool wxTextCtrl::SetBackgroundColour(const wxColour& colour)
{
    return wxWindow::SetBackgroundColour( colour );
}

//-----------------------------------------------------------------------------
//  private code and handlers
//-----------------------------------------------------------------------------

void wxTextCtrl::SearchForBrackets()
{
    int oldBracketY = m_bracketY;
    int oldBracketX = m_bracketX;
    
    if (m_cursorY < 0 || m_cursorY >= (int)m_lines.GetCount()) return;
        
    wxString current = m_lines[m_cursorY].m_text;
    
    // reverse search first

    char bracket = ' ';
    
    if (m_cursorX > 0)
        bracket = current[m_cursorX-1];
        
    if (bracket == ')' || bracket == ']' || bracket == '}')
    {
        char antibracket = '(';
        if (bracket == ']') antibracket = '[';
        if (bracket == '}') antibracket = '{';
        
        int count = 1;
    
        int endY = m_cursorY-60;
        if (endY < 0) endY = 0;
        for (int y = m_cursorY; y >= endY; y--)
        {
            current = m_lines[y].m_text;
            if (y == m_cursorY)
                current.erase(m_cursorX-1,current.Len()-m_cursorX+1);
            
            for (int n = current.Len()-1; n >= 0; n--)
            {
                // ignore chars
                if (current[n] == '\'')
                {
                    for (int m = n-1; m >= 0; m--)
                    {
                        if (current[m] == '\'')
                        {
                            if (m == 0 || current[m-1] != '\\')
                                break;
                        }
                        n = m-1;
                    }
                    continue;
                }
                
                // ignore strings
                if (current[n] == '\"')
                {
                    for (int m = n-1; m >= 0; m--)
                    {
                        if (current[m] == '\"')
                        {
                            if (m == 0 || current[m-1] != '\\')
                                break;
                        }
                        n = m-1;
                    }
                    continue;
                }
            
                if (current[n] == antibracket)
                {
                    count--;
                    if (count == 0)
                    {
                        m_bracketY = y;
                        m_bracketX = n;
                        if (oldBracketY != m_bracketY && oldBracketY != -1)
                            RefreshLine( oldBracketY );
                        if (m_bracketY != oldBracketY || m_bracketX != oldBracketX)
                            RefreshLine( m_bracketY );
                        return;
                    }
                }
                else if (current[n] == bracket)
                {
                    count++;
                }
            }
        }
    }
    
    // then forward

    bracket = ' ';
    if ((int)current.Len() > m_cursorX)
        bracket = current[m_cursorX];
    if (bracket == '(' || bracket == '[' || bracket == '{')
    {
        char antibracket = ')';
        if (bracket == '[') antibracket = ']';
        if (bracket == '{') antibracket = '}';
        
        int count = 1;
    
        int endY = m_cursorY+60;
        if (endY > (int)(m_lines.GetCount()-1)) endY = m_lines.GetCount()-1;
        for (int y = m_cursorY; y <= endY; y++)
        {
            current = m_lines[y].m_text;
            int start = 0;
            if (y == m_cursorY)
                start = m_cursorX+1;
            
            for (int n = start; n < (int)current.Len(); n++)
            {
                // ignore chars
                if (current[n] == '\'')
                {
                    for (int m = n+1; m < (int)current.Len(); m++)
                    {
                        if (current[m] == '\'')
                        {
                            if (m == 0 || (current[m-1] != '\\') || (m >= 2 && current[m-2] == '\\'))
                                break;
                        }
                        n = m+1;
                    }
                    continue;
                }
                
                // ignore strings
                if (current[n] == '\"')
                {
                    for (int m = n+1; m < (int)current.Len(); m++)
                    {
                        if (current[m] == '\"')
                        {
                            if (m == 0 || (current[m-1] != '\\') || (m >= 2 && current[m-2] == '\\'))
                                break;
                        }
                        n = m+1;
                    }
                    continue;
                }
                
                if (current[n] == antibracket)
                {
                    count--;
                    if (count == 0)
                    {
                        m_bracketY = y;
                        m_bracketX = n;
                        if (oldBracketY != m_bracketY && oldBracketY != -1)
                            RefreshLine( oldBracketY );
                        if (m_bracketY != oldBracketY || m_bracketX != oldBracketX)
                            RefreshLine( m_bracketY );
                        return;
                    }
                }
                else if (current[n] == bracket)
                {
                    count++;
                }
            }
        }
    }
    
    if (oldBracketY != -1)
    {
        m_bracketY = -1;
        RefreshLine( oldBracketY );
    }
}

void wxTextCtrl::Delete()
{
    if (!HasSelection()) return;
    
    m_modified = TRUE;
    
    int selStartY = m_selStartY;
    int selEndY = m_selEndY;
    int selStartX = m_selStartX;
    int selEndX = m_selEndX;
    
    if ((selStartY > selEndY) ||
        ((selStartY == selEndY) && (selStartX > selEndX)))
    {
        int tmp = selStartX;
        selStartX = selEndX;
        selEndX = tmp;
        tmp = selStartY;
        selStartY = selEndY;
        selEndY = tmp;
    }
    
    int len = (int)m_lines[selStartY].m_text.Len();
        
    if (selStartY == selEndY)
    {
        m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_LINE, selStartY, selStartY, this ) );
    
        wxString tmp( m_lines[selStartY].m_text );
        if (selStartX < len)
        {
            if (selEndX > len)
                selEndX = len;
            tmp.Remove( selStartX, selEndX-selStartX );
            m_lines[selStartY].m_text = tmp;
        }
        ClearSelection();
        m_cursorX = selStartX;
        RefreshLine( selStartY );
    }
    else
    {
        m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_DELETE, selStartY, selEndY, this ) );
    
        if (selStartX < len)
            m_lines[selStartY].m_text.Remove( selStartX );
        
        for (int i = 0; i < selEndY-selStartY-1; i++)
            m_lines.RemoveAt( selStartY+1 );
            
        if (selEndX < (int)m_lines[selStartY+1].m_text.Len())
            m_lines[selStartY+1].m_text.Remove( 0, selEndX );
        else
            m_lines[selStartY+1].m_text.Remove( 0 );
            
        m_lines[selStartY].m_text.Append( m_lines[selStartY+1].m_text );
        m_lines.RemoveAt( selStartY+1 );
    
        ClearSelection();
        MoveCursor( selStartX, selStartY );
        MyAdjustScrollbars();
        
        RefreshDown( selStartY );
    }
}

void wxTextCtrl::DeleteLine()
{
    if (HasSelection()) return;
    
    if (m_cursorY < 0 || m_cursorY >= (int)m_lines.GetCount()-1) return;  // TODO
        
    m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_DELETE, m_cursorY, m_cursorY+1, this ) );
    
    m_lines.RemoveAt( m_cursorY );
    m_cursorX = 0;
    if (m_cursorY >= (int)m_lines.GetCount()) m_cursorY--;
    
    MyAdjustScrollbars();
    RefreshDown( m_cursorY );
}

void wxTextCtrl::DoChar( char c )
{
    m_modified = TRUE;
    
    m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_LINE, m_cursorY, m_cursorY, this ) );
    
    wxString tmp( m_lines[m_cursorY].m_text );
    tmp.Trim();
    if (m_cursorX >= (int)tmp.Len())
    {
        int len = tmp.Len();
        for (int i = 0; i < m_cursorX - len; i++)
            tmp.Append( ' ' );
        tmp.Append( c );
    }
    else
    {
        if (m_overwrite)
            tmp.SetChar( m_cursorX, c );
        else
            tmp.insert( m_cursorX, 1, c );
    }
    
    m_lines[m_cursorY].m_text = tmp;
    
    if (tmp.Len() > m_longestLine)
    {
        m_longestLine = tmp.Len();
        MyAdjustScrollbars();
    }
    
    m_cursorX++;
    
    int y = m_cursorY*m_lineHeight;
    int x = (m_cursorX-1)*m_charWidth;
    CalcScrolledPosition( x, y, &x, &y );
    wxRect rect( x+2, y+2, 10000, m_lineHeight );
    Refresh( TRUE, &rect );
    
    int size_x = 0;
    int size_y = 0;
    GetClientSize( &size_x, &size_y );
    size_x /= m_charWidth;
    
    int view_x = 0;
    int view_y = 0;
    GetViewStart( &view_x, &view_y );
    
    if (m_cursorX < view_x)
        Scroll( m_cursorX, -1 );
    else if (m_cursorX > view_x+size_x-1)
        Scroll( m_cursorX-size_x+1, -1 );
}

void wxTextCtrl::DoBack()
{
    m_modified = TRUE;
    
    if (m_cursorX == 0)
    {
        if (m_cursorY == 0) return;
        
        m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_BACK, m_cursorY-1, m_cursorY, this ) );
        
        wxString tmp1( m_lines[m_cursorY-1].m_text );
        tmp1.Trim();
        wxString tmp2( m_lines[m_cursorY].m_text );
        tmp2.Trim();
        m_cursorX = tmp1.Len();
        m_cursorY--;
        tmp1.Append( tmp2 );
        m_lines[m_cursorY].m_text = tmp1;
        m_lines.RemoveAt( m_cursorY+1 );
        
        MyAdjustScrollbars();
        RefreshDown( m_cursorY-1 );
    }
    else
    {
        m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_LINE, m_cursorY, m_cursorY, this ) );
        
        if (m_cursorX <= (int)m_lines[m_cursorY].m_text.Len())
            m_lines[m_cursorY].m_text.Remove( m_cursorX-1, 1 );
        m_cursorX--;
        
        int y = m_cursorY*m_lineHeight;
        int x = m_cursorX*m_charWidth;
        CalcScrolledPosition( x, y, &x, &y );
        wxRect rect( x+2, y+2, 10000, m_lineHeight );
        Refresh( TRUE, &rect );
    }
}

void wxTextCtrl::DoDelete()
{
    m_modified = TRUE;
    
    wxString tmp( m_lines[m_cursorY].m_text );
    tmp.Trim();
    int len = (int)tmp.Len();
    if (m_cursorX >= len)
    {
        if (m_cursorY == (int)m_lines.GetCount()-1) return;

        m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_DELETE, m_cursorY, m_cursorY+1, this ) );
        
        for (int i = 0; i < (m_cursorX-len); i++)
            tmp += ' ';
            
        tmp += m_lines[m_cursorY+1].m_text;
        
        m_lines[m_cursorY] = tmp;
        m_lines.RemoveAt( m_cursorY+1 );
        
        MyAdjustScrollbars();
        RefreshDown( m_cursorY );
    }
    else
    {
        m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_LINE, m_cursorY, m_cursorY, this ) );
    
        tmp.Remove( m_cursorX, 1 );
        m_lines[m_cursorY].m_text = tmp;
        
        int y = m_cursorY*m_lineHeight;
        int x = m_cursorX*m_charWidth;
        CalcScrolledPosition( x, y, &x, &y );
        wxRect rect( x+2, y+2, 10000, m_lineHeight );
        Refresh( TRUE, &rect );
    }
}

void wxTextCtrl::DoReturn()
{
    m_modified = TRUE;
    
    m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_ENTER, m_cursorY, m_cursorY, this ) );
    
    wxString tmp( m_lines[m_cursorY].m_text );
    size_t indent = tmp.find_first_not_of( ' ' );
    if (indent == wxSTRING_MAXLEN) indent = 0;
    tmp.Trim();
    if (m_cursorX >= (int)tmp.Len())
    {
        int cursorX = indent;
        int cursorY = m_cursorY + 1;
        
        wxString new_tmp;
        for (size_t i = 0; i < indent; i++) new_tmp.Append( ' ' );
        m_lines.Insert( new wxSourceLine( new_tmp ), cursorY );
        
        MyAdjustScrollbars();
        MoveCursor( cursorX, cursorY );
        RefreshDown( m_cursorY );
    }
    else
    {
        wxString tmp1( tmp );
        tmp1.Remove( m_cursorX, tmp.Len()-m_cursorX );
        m_lines[m_cursorY].m_text = tmp1;
       
        wxString tmp2( tmp );
        tmp2.Remove( 0, m_cursorX );
       
        int cursorX = indent;
        int cursorY = m_cursorY + 1;
        
        wxString new_tmp;
        for (size_t i = 0; i < indent; i++) new_tmp.Append( ' ' );
        new_tmp.Append( tmp2 );
        m_lines.Insert( new wxSourceLine( new_tmp ), cursorY );
        
        MyAdjustScrollbars();
        MoveCursor( cursorX, cursorY );
        RefreshDown( m_cursorY-1 );
    }
}

void wxTextCtrl::DoDClick()
{
    wxString line( m_lines[ m_cursorY ].m_text );
    if (m_cursorX >= (int)line.Len()) return;
    int p = m_cursorX;
    char ch = line[p];
    if (((ch >= 'a') && (ch <= 'z')) ||
        ((ch >= 'A') && (ch <= 'Z')) ||
        ((ch >= '0') && (ch <= '9')) ||
        (ch == '_'))
    {
        m_selStartY = m_cursorY;
        m_selEndY = m_cursorY;
        if (p > 0)
        {
            ch = line[p-1];
            while (((ch >= 'a') && (ch <= 'z')) ||
                   ((ch >= 'A') && (ch <= 'Z')) ||
                   ((ch >= '0') && (ch <= '9')) ||
                   (ch == '_'))
            {
                p--;
                if (p == 0) break;
                ch = line[p-1];
            }
        }
        m_selStartX = p;
        
        p = m_cursorX;
        if (p < (int)line.Len())
        {
            ch = line[p];
            while (((ch >= 'a') && (ch <= 'z')) ||
                   ((ch >= 'A') && (ch <= 'Z')) ||
                   ((ch >= '0') && (ch <= '9')) ||
                   (ch == '_'))
            {
                if (p >= (int)line.Len()) break;
                p++;
                ch = line[p];
            }
        }
        m_selEndX = p;
        RefreshLine( m_cursorY );
    }
}

wxString wxTextCtrl::GetNextToken( wxString &line, int &pos )
{
    wxString ret;
    int len = (int)line.Len();
    for (int p = pos; p < len; p++)
    {
        if ((m_lang == wxSOURCE_LANG_PYTHON) || (m_lang == wxSOURCE_LANG_PERL))
        {
            if (line[p] == '#')
            {
                for (int q = p; q < len; q++)
                    ret.Append( line[q] );
                pos = p;
                return ret;
            }
        }
        else
        {
            if ((line[p] == '/') && (p+1 < len) && (line[p+1] == '/'))
            {
                for (int q = p; q < len; q++)
                    ret.Append( line[q] );
                pos = p;
                return ret;
            }
        }
        
        if (line[p] == '"')
        {
            ret.Append( line[p] );
            for (int q = p+1; q < len; q++)
            {
                ret.Append( line[q] );
                if ((line[q] == '"') && ((line[q-1] != '\\') || (q >= 2 && line[q-2] == '\\')))
                   break;
            }
            pos = p;
            return ret;
        }
    
        if (line[p] == '\'')
        {
            ret.Append( line[p] );
            for (int q = p+1; q < len; q++)
            {
                ret.Append( line[q] );
                if ((line[q] == '\'') && ((line[q-1] != '\\') || (q >= 2 && line[q-2] == '\\')))
                   break;
            }
            pos = p;
            return ret;
        }
    
        if (((line[p] >= 'a') && (line[p] <= 'z')) ||
            ((line[p] >= 'A') && (line[p] <= 'Z')) ||
            (line[p] == '_') ||
            (line[p] == '#'))
        {
           ret.Append( line[p] );
           for (int q = p+1; q < len; q++)
           {
                if (((line[q] >= 'a') && (line[q] <= 'z')) ||
                   ((line[q] >= 'A') && (line[q] <= 'Z')) ||
                   ((line[q] >= '0') && (line[q] <= '9')) ||
                   (line[q] == '_'))
                {
                    ret.Append( line[q] );
                    continue;
                }
                else
                {
                    pos = p;
                    return ret;
                }
           }
           pos = p;
           return ret;
        }
    }
    
    return ret;
}

void wxTextCtrl::DrawLine( wxDC &dc, int x, int y, const wxString &line2, int lineNum )
{
    int selStartY = m_selStartY;
    int selEndY = m_selEndY;
    int selStartX = m_selStartX;
    int selEndX = m_selEndX;
    
    if ((selStartY > selEndY) ||
        ((selStartY == selEndY) && (selStartX > selEndX)))
    {
        int tmp = selStartX;
        selStartX = selEndX;
        selEndX = tmp;
        tmp = selStartY;
        selStartY = selEndY;
        selEndY = tmp;
    }

    wxString line( line2 );
    if (HasFlag(wxTE_PASSWORD))
    {
        size_t len = line.Len();
        line = wxString( wxT('*'), len );
    }

    wxString keyword( ' ', line.Len() );
    wxString define( ' ', line.Len() );
    wxString variable( ' ', line.Len() );
    wxString comment( ' ', line.Len() );
    wxString my_string( ' ', line.Len() );
    
    if (lineNum == m_bracketY)
    {
        wxString red( ' ', line.Len() );
        if (m_bracketX < (int)line.Len())
        {
            red.SetChar( m_bracketX, line[m_bracketX] );
            line.SetChar( m_bracketX, ' ' );
            dc.SetTextForeground( *wxRED );
            dc.DrawText( red, x, y );
            dc.SetTextForeground( *wxBLACK );
        }
    }

    int pos = 0;
    wxString token( GetNextToken( line, pos) );
    while (!token.IsNull())
    {
        if (m_keywords.Index( token ) != wxNOT_FOUND)
        {
            int end_pos = pos + (int)token.Len();
            for (int i = pos; i < end_pos; i++)
            {
                keyword.SetChar( i, line[i] );
                line.SetChar( i, ' ' );
            }
        } else
        if (m_defines.Index( token ) != wxNOT_FOUND)
        {
            int end_pos = pos + (int)token.Len();
            for (int i = pos; i < end_pos; i++)
            {
                define.SetChar( i, line[i] );
                line.SetChar( i, ' ' );
            }
        } else
        if ((m_variables.Index( token ) != wxNOT_FOUND) ||
            ((token.Len() > 2) && (token[0] == 'w') && (token[1] == 'x')))
        {
            int end_pos = pos + (int)token.Len();
            for (int i = pos; i < end_pos; i++)
            {
                variable.SetChar( i, line[i] );
                line.SetChar( i, ' ' );
            }
        } else
        if ((token.Len() >= 2) && (token[0] == '/') && (token[1] == '/') && (m_lang == wxSOURCE_LANG_CPP))
        {
            int end_pos = pos + (int)token.Len();
            for (int i = pos; i < end_pos; i++)
            {
                comment.SetChar( i, line[i] );
                line.SetChar( i, ' ' );
            }
        } else
        if ((token[0] == '#') &&
            ((m_lang == wxSOURCE_LANG_PYTHON) || (m_lang == wxSOURCE_LANG_PERL)))
        {
            int end_pos = pos + (int)token.Len();
            for (int i = pos; i < end_pos; i++)
            {
                comment.SetChar( i, line[i] );
                line.SetChar( i, ' ' );
            }
            
        } else
        if ((token[0] == '"') || (token[0] == '\''))
        {
            int end_pos = pos + (int)token.Len();
            for (int i = pos; i < end_pos; i++)
            {
                my_string.SetChar( i, line[i] );
                line.SetChar( i, ' ' );
            }
        }
        pos += token.Len();
        token = GetNextToken( line, pos);
    }

    if ((lineNum < selStartY) || (lineNum > selEndY))
    {
        dc.DrawText( line, x, y );
        dc.SetTextForeground( m_keywordColour );
        dc.DrawText( keyword, x, y );
        dc.SetTextForeground( m_defineColour );
        dc.DrawText( define, x, y );
        dc.SetTextForeground( m_variableColour );
        dc.DrawText( variable, x, y );
        dc.SetTextForeground( m_commentColour );
        dc.DrawText( comment, x, y );
        dc.SetTextForeground( m_stringColour );
        dc.DrawText( my_string, x, y );
        dc.SetTextForeground( *wxBLACK );
        return;
    }
    
    if (selStartY == selEndY)
    {
        int i;
        wxString tmp1( line );
        wxString tmp2( line );
        dc.DrawRectangle( selStartX*m_charWidth+2, lineNum*m_lineHeight+2, 
                          (selEndX-selStartX)*m_charWidth, m_lineHeight );
        for (i = selStartX; i < selEndX; i++)
            if ((int)tmp1.Len() > i)
                tmp1.SetChar( i, ' ' );
        dc.DrawText( tmp1, x, y );
        for (i = 0; i < selStartX; i++)
            if ((int)tmp2.Len() > i)
                tmp2.SetChar( i, ' ' );
        for (i = selEndX; i < (int)tmp2.Len(); i++)
            if ((int)tmp2.Len() > i)
                tmp2.SetChar( i, ' ' );
        dc.SetTextForeground( *wxWHITE );
        dc.DrawText( tmp2, x, y );
        dc.SetTextForeground( *wxBLACK );
    } else
    if ((lineNum > selStartY) && (lineNum < selEndY))
    {
        dc.DrawRectangle( 0+2, lineNum*m_lineHeight+2, 10000, m_lineHeight );
        dc.SetTextForeground( *wxWHITE );
        dc.DrawText( line, x, y );
        dc.SetTextForeground( *wxBLACK );
    } else
    if (lineNum == selStartY)
    {
        int i;
        wxString tmp1( line );
        wxString tmp2( line );
        dc.DrawRectangle( selStartX*m_charWidth+2, lineNum*m_lineHeight+2, 
                          10000, m_lineHeight );
        for (i = selStartX; i < (int)tmp1.Len(); i++)
            tmp1.SetChar( i, ' ' );
        dc.DrawText( tmp1, x, y );
        for (i = 0; i < selStartX; i++)
            if ((int)tmp2.Len() > i)
                tmp2.SetChar( i, ' ' );
        dc.SetTextForeground( *wxWHITE );
        dc.DrawText( tmp2, x, y );
        dc.SetTextForeground( *wxBLACK );
    } else
    if (lineNum == selEndY)
    {
        int i;
        wxString tmp1( line );
        wxString tmp2( line );
        dc.DrawRectangle( 0+2, lineNum*m_lineHeight+2, 
                          selEndX*m_charWidth, m_lineHeight );
        for (i = 0; i < selEndX; i++)
            if ((int)tmp1.Len() > i)
               tmp1.SetChar( i, ' ' );
        dc.DrawText( tmp1, x, y );
        for (i = selEndX; i < (int)tmp2.Len(); i++)
            tmp2.SetChar( i, ' ' );
        dc.SetTextForeground( *wxWHITE );
        dc.DrawText( tmp2, x, y );
        dc.SetTextForeground( *wxBLACK );
    }
    
    dc.SetTextForeground( m_keywordColour );
    dc.DrawText( keyword, x, y );
    dc.SetTextForeground( m_defineColour );
    dc.DrawText( define, x, y );
    dc.SetTextForeground( m_variableColour );
    dc.DrawText( variable, x, y );
    dc.SetTextForeground( m_commentColour );
    dc.DrawText( comment, x, y );
    dc.SetTextForeground( m_stringColour );
    dc.DrawText( my_string, x, y );
    dc.SetTextForeground( *wxBLACK );
}

void wxTextCtrl::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc(this);
    
    if (m_lines.GetCount() == 0) return;
    
    PrepareDC( dc );
    
    dc.SetFont( m_sourceFont );
    
    int scroll_y = 0;
    GetViewStart( NULL, &scroll_y );
    
    int size_x = 0;
    int size_y = 0;
    GetClientSize( &size_x, &size_y );
    
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.SetBrush( wxBrush( wxTHEME_COLOUR(HIGHLIGHT), wxSOLID ) );
    int upper = wxMin( (int)m_lines.GetCount(), scroll_y+(size_y/m_lineHeight)+1 );
    for (int i = scroll_y; i < upper; i++)
    {
        int x = 0+2;
        int y = i*m_lineHeight+2;
        int w = 10000;
        int h = m_lineHeight;
        CalcScrolledPosition( x,y,&x,&y );
        if (IsExposed(x,y,w,h))
            DrawLine( dc, 0+2, i*m_lineHeight+2, m_lines[i].m_text, i );
    }
    
    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawRectangle( m_cursorX*m_charWidth+2, m_cursorY*m_lineHeight+2, 2, m_lineHeight );
}

void wxTextCtrl::OnMouse( wxMouseEvent &event )
{
    if (m_lines.GetCount() == 0) return;

    
#if 0  // there is no middle button on iPAQs
    if (event.MiddleDown())
    {
        Paste( TRUE );
        return;
    }
#endif
    
    if (event.LeftDClick())
    {
        DoDClick();
        return;
    }
    
    if (event.LeftDown())
    { 
        m_capturing = TRUE;
        CaptureMouse();
    }
    
    if (event.LeftUp())
    {   
        m_capturing = FALSE;
        ReleaseMouse();
    }

    if (event.LeftDown() || 
        (event.LeftIsDown() && m_capturing))
    {
        int x = event.GetX();
        int y = event.GetY();
        CalcUnscrolledPosition( x, y, &x, &y );
        x /= m_charWidth;
        y /= m_lineHeight;
        MoveCursor( 
            wxMin( 1000, wxMax( 0, x ) ), 
            wxMin( (int)m_lines.GetCount()-1, wxMax( 0, y ) ),
            event.ShiftDown() || !event.LeftDown() );
    }
}

void wxTextCtrl::OnChar( wxKeyEvent &event )
{
    if (m_lines.GetCount() == 0) return;

    int size_x = 0;
    int size_y = 0;
    GetClientSize( &size_x, &size_y );
    size_x /= m_charWidth;
    size_y /= m_lineHeight;
    size_y--;
    
    if (event.ShiftDown())
    {
        switch (event.GetKeyCode())
        {
            case '4': event.m_keyCode = WXK_LEFT;   break;
            case '8': event.m_keyCode = WXK_UP;     break;
            case '6': event.m_keyCode = WXK_RIGHT;  break;
            case '2': event.m_keyCode = WXK_DOWN;   break;
            case '9': event.m_keyCode = WXK_PRIOR;  break;
            case '3': event.m_keyCode = WXK_NEXT;   break;
            case '7': event.m_keyCode = WXK_HOME;   break;
            case '1': event.m_keyCode = WXK_END;    break;
            case '0': event.m_keyCode = WXK_INSERT; break;
        }
    }
    
    switch (event.GetKeyCode())
    {
        case WXK_UP:
        {
            if (m_ignoreInput) return;
            if (m_cursorY > 0)
                MoveCursor( m_cursorX, m_cursorY-1, event.ShiftDown() );
            m_ignoreInput = TRUE;
            return;
        }
        case WXK_DOWN:
        {
            if (m_ignoreInput) return;
            if (m_cursorY < (int)(m_lines.GetCount()-1))
                MoveCursor( m_cursorX, m_cursorY+1, event.ShiftDown() );
            m_ignoreInput = TRUE;
            return;
        }
        case WXK_LEFT:
        {
            if (m_ignoreInput) return;
            if (m_cursorX > 0)
            {
                MoveCursor( m_cursorX-1, m_cursorY, event.ShiftDown() );
            }
            else
            {
                if (m_cursorY > 0)
                    MoveCursor( m_lines[m_cursorY-1].m_text.Len(), m_cursorY-1, event.ShiftDown() );
            }
            m_ignoreInput = TRUE;
            return;
        }
        case WXK_RIGHT:
        {
            if (m_ignoreInput) return;
            if (m_cursorX < 1000)
                MoveCursor( m_cursorX+1, m_cursorY, event.ShiftDown() );
            m_ignoreInput = TRUE;
            return;
        }
        case WXK_HOME:
        {
            if (event.ControlDown())
                MoveCursor( 0, 0, event.ShiftDown() );
            else
                MoveCursor( 0, m_cursorY, event.ShiftDown() );
            return;
        }
        case WXK_END:
        {
            if (event.ControlDown())
                MoveCursor( 0, m_lines.GetCount()-1, event.ShiftDown() );
            else
                MoveCursor( m_lines[m_cursorY].m_text.Len(), m_cursorY, event.ShiftDown() );
            return;
        }
        case WXK_NEXT:
        {
            if (m_ignoreInput) return;
            MoveCursor( m_cursorX, wxMin( (int)(m_lines.GetCount()-1), m_cursorY+size_y ), event.ShiftDown() );
            m_ignoreInput = TRUE;
            return;
        }
        case WXK_PRIOR:
        {
            if (m_ignoreInput) return;
            MoveCursor( m_cursorX, wxMax( 0, m_cursorY-size_y ), event.ShiftDown() );
            m_ignoreInput = TRUE;
            return;
        }
        case WXK_INSERT:
        {
            if (event.ShiftDown())
                Paste();
            else if (event.ControlDown())
                Copy();
            else
                m_overwrite = !m_overwrite;
            return;
        }
        case WXK_RETURN:
        {
            if (IsSingleLine())
            {
                event.Skip();
                return;
            }
            
            if (HasSelection())
                Delete();
            DoReturn();
            return;
        }
        case WXK_TAB:
        {
            if (HasSelection())
                Delete();
            bool save_overwrite = m_overwrite;
            m_overwrite = FALSE;
            int i = 4-(m_cursorX % 4);
            if (i == 0) i = 4;
            for (int c = 0; c < i; c++)
                 DoChar( ' ' );
            m_overwrite = save_overwrite;
            return;
        }
        case WXK_BACK:
        {
            if (HasSelection())
                Delete();
            else
                DoBack();
            return;
        }
        case WXK_DELETE:
        {
            if (HasSelection())
                Delete();
            else
                DoDelete();
            return;
        }
        default: 
        {
            if (  (event.KeyCode() >= 'a') &&
                  (event.KeyCode() <= 'z') &&
                  (event.AltDown()) )
            {
                // Alt-F etc.
                event.Skip();
                return;
            }
            
            if (  (event.KeyCode() >= 32) && 
                  (event.KeyCode() <= 255) &&
                 !(event.ControlDown() && !event.AltDown()) ) // filters out Ctrl-X but leaves Alt-Gr
            {
                if (HasSelection())
                    Delete();
                DoChar( (char) event.KeyCode() );
                return;
            }
        }
    }
    
    event.Skip();
}

void wxTextCtrl::OnIdle( wxIdleEvent &event )
{
    m_ignoreInput = FALSE;
    
    SearchForBrackets();
    
    event.Skip( TRUE );
}

void wxTextCtrl::Indent()
{
    int startY = m_cursorY;
    int endY = m_cursorY;
    if (HasSelection())
    {
        startY = m_selStartY;
        endY = m_selEndY;
        if (endY < startY)
        {
            int tmp = startY;
            startY = endY;
            endY = tmp;
        }
    }
    
    m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_LINE, startY, endY, this ) );
    
    for (int i = startY; i <= endY; i++)
    {
        m_lines[i].m_text.insert( 0u, "    " );
        RefreshLine( i );
    }
}

void wxTextCtrl::Unindent()
{
    int startY = m_cursorY;
    int endY = m_cursorY;
    if (HasSelection())
    {
        startY = m_selStartY;
        endY = m_selEndY;
        if (endY < startY)
        {
            int tmp = startY;
            startY = endY;
            endY = tmp;
        }
    }
    
    m_undos.Append( new wxSourceUndoStep( wxSOURCE_UNDO_LINE, startY, endY, this ) );
    
    for (int i = startY; i <= endY; i++)
    {
        for (int n = 0; n < 4; n++)
        {
            if (m_lines[i].m_text[0u] == ' ')
                m_lines[i].m_text.erase(0u,1u);
        }
        RefreshLine( i );
    }
}
bool wxTextCtrl::HasSelection()
{
    return ((m_selStartY != m_selEndY) || (m_selStartX != m_selEndX));
}

void wxTextCtrl::ClearSelection()
{
    m_selStartX = -1;
    m_selStartY = -1;
    m_selEndX = -1;
    m_selEndY = -1;
}

void wxTextCtrl::RefreshLine( int n )
{
    int y = n*m_lineHeight;
    int x = 0;
    CalcScrolledPosition( x, y, &x, &y );
    wxRect rect( 0+2, y+2, 10000, m_lineHeight );
    Refresh( TRUE, &rect );
}

void wxTextCtrl::RefreshDown( int n )
{
    int size_x = 0;
    int size_y = 0;
    GetClientSize( &size_x, &size_y );

    int view_x = 0;
    int view_y = 0;
    GetViewStart( &view_x, &view_y );

    if (n < view_y)
    {
        Refresh();
    }
    else
    {
        int y = n*m_lineHeight;
        int x = 0;
        CalcScrolledPosition( x, y, &x, &y );
    
        wxRect rect( 0+2, y+2, 10000, size_y );
        Refresh( TRUE, &rect );
    }
}

void wxTextCtrl::MoveCursor( int new_x, int new_y, bool shift, bool centre )
{
    // if (IsSingleLine())
    {
        if (new_x > m_lines[new_y].m_text.Len())
            new_x = m_lines[new_y].m_text.Len();
    }

    if ((new_x == m_cursorX) && (new_y == m_cursorY)) return;

    bool no_cursor_refresh = FALSE;

    if (shift)
    {
        int x,y,w,h;
        
        if (!HasSelection())
        {
            m_selStartX = m_cursorX;
            m_selStartY = m_cursorY;
            
            x = 0;
            w = 10000;
            if (new_y > m_selStartY)
            {
                y = m_selStartY*m_lineHeight;
                h = (new_y-m_selStartY+1)*m_lineHeight;
            }
            else if (new_y == m_selStartY)
            {
                y = m_selStartY*m_lineHeight;
                h = m_lineHeight;
            }
            else
            {
                y = new_y*m_lineHeight;
                h = (-new_y+m_selStartY+1)*m_lineHeight;
            }
            
            no_cursor_refresh = TRUE;
            m_cursorX = new_x;
            m_cursorY = new_y;
        }
        else
        {
            if (new_y == m_selEndY)
            {
                y = new_y *m_lineHeight;
                h = m_lineHeight;
                if (m_selEndX > new_x)
                {
                    x = new_x*m_charWidth;
                    w = (m_selEndX-new_x)*m_charWidth;
                }
                else
                {
                    x = m_selEndX*m_charWidth;
                    w = (-m_selEndX+new_x)*m_charWidth;
                }
            }
            else
            {
                x = 0;
                w = 10000;
                if (new_y > m_selEndY)
                {
                    y = m_selEndY*m_lineHeight;
                    h = (new_y-m_selEndY+1) * m_lineHeight;
                }
                else
                {
                    y = new_y*m_lineHeight;
                    h = (-new_y+m_selEndY+1) * m_lineHeight;
                }
                no_cursor_refresh = TRUE;
                m_cursorX = new_x;
                m_cursorY = new_y;
            }
        }
        
        m_selEndX = new_x;
        m_selEndY = new_y;
        
        CalcScrolledPosition( x, y, &x, &y );
        wxRect rect( x+2, y+2, w, h );
        Refresh( TRUE, &rect );
    }
    else
    {
        if (HasSelection())
        {
            int ry1 = m_selEndY;
            int ry2 = m_selStartY;
            m_selEndX = -1;
            m_selEndY = -1;
            m_selStartX = -1;
            m_selStartY = -1;
            
            if (ry1 > ry2)
            {
                int tmp = ry2;
                ry2 = ry1;
                ry1 = tmp;
            }
        
            int x = 0;
            int y = ry1*m_lineHeight;
            CalcScrolledPosition( x, y, &x, &y );
            wxRect rect( 0+2, y+2, 10000, (ry2-ry1+1)*m_lineHeight );
        
            Refresh( TRUE, &rect );
        }
    }
    
/*
    printf( "startx %d starty %d endx %d endy %d\n", 
            m_selStartX, m_selStartY, m_selEndX, m_selEndY );
            
    printf( "has %d\n", (int)HasSelection() );
*/

    if (!no_cursor_refresh)
    {
        int x = m_cursorX*m_charWidth;
        int y = m_cursorY*m_lineHeight;
        CalcScrolledPosition( x, y, &x, &y );
        wxRect rect( x+2, y+2, 4, m_lineHeight+2 );
        
        m_cursorX = new_x;
        m_cursorY = new_y;
    
        Refresh( TRUE, &rect );
        
        wxClientDC dc(this);
        PrepareDC( dc );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.SetBrush( *wxRED_BRUSH );
        dc.DrawRectangle( m_cursorX*m_charWidth+2, m_cursorY*m_lineHeight+2, 2, m_lineHeight );
    }
    
    int size_x = 0;
    int size_y = 0;
    GetClientSize( &size_x, &size_y );
    size_x /= m_charWidth;
    size_y /= m_lineHeight;
    
    int view_x = 0;
    int view_y = 0;
    GetViewStart( &view_x, &view_y );
    
    if (centre)
    {
        int sy = m_cursorY - (size_y/2);
        if (sy < 0) sy = 0;
        Scroll( -1, sy );
    }
    else
    {
        if (m_cursorY < view_y)
            Scroll( -1, m_cursorY );
        else if (m_cursorY > view_y+size_y-1)
            Scroll( -1, m_cursorY-size_y+1 );
    }
    
    if (m_cursorX < view_x)
        Scroll( m_cursorX, -1 );
    else if (m_cursorX > view_x+size_x-1)
        Scroll( m_cursorX-size_x+1, -1 );
}

void wxTextCtrl::MyAdjustScrollbars()
{
    if (IsSingleLine())
        return;

    int y_range = m_lines.GetCount();

    int height = 0;
    GetClientSize( NULL, &height );
    height -= 4;
    if (height >= m_lines.GetCount() *m_lineHeight)
        y_range = 0;
    
    int view_x = 0;
    int view_y = 0;
    GetViewStart( &view_x, &view_y );
    
    SetScrollbars( m_charWidth, m_lineHeight, m_longestLine+2, y_range, view_x, view_y );
}

//-----------------------------------------------------------------------------
//  clipboard handlers
//-----------------------------------------------------------------------------

void wxTextCtrl::OnCut(wxCommandEvent& WXUNUSED(event))
{
    Cut();
}

void wxTextCtrl::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    Copy();
}

void wxTextCtrl::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    Paste();
}

void wxTextCtrl::OnUndo(wxCommandEvent& WXUNUSED(event))
{
    Undo();
}

void wxTextCtrl::OnRedo(wxCommandEvent& WXUNUSED(event))
{
    Redo();
}

void wxTextCtrl::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable( CanCut() );
}

void wxTextCtrl::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( CanCopy() );
}

void wxTextCtrl::OnUpdatePaste(wxUpdateUIEvent& event)
{
    event.Enable( CanPaste() );
}

void wxTextCtrl::OnUpdateUndo(wxUpdateUIEvent& event)
{
    event.Enable( CanUndo() );
}

void wxTextCtrl::OnUpdateRedo(wxUpdateUIEvent& event)
{
    event.Enable( CanRedo() );
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    if (IsSingleLine())
    {
        wxSize ret(80, m_lineHeight + 4);
        
        if (HasFlag(wxBORDER_SUNKEN) || HasFlag(wxBORDER_RAISED))
            ret.y += 4;
            
        if (HasFlag(wxBORDER_SIMPLE))
            ret.y += 2;
            
        return ret;
    }
    else
    {
        return wxSize(80, 60);
    }
}

// ----------------------------------------------------------------------------
// freeze/thaw
// ----------------------------------------------------------------------------

void wxTextCtrl::Freeze()
{
}

void wxTextCtrl::Thaw()
{
}

// ----------------------------------------------------------------------------
// text control scrolling
// ----------------------------------------------------------------------------

bool wxTextCtrl::ScrollLines(int lines)
{
    wxFAIL_MSG( "wxTextCtrl::ScrollLines not implemented");

    return FALSE;
}

bool wxTextCtrl::ScrollPages(int pages)
{
    wxFAIL_MSG( "wxTextCtrl::ScrollPages not implemented");
    
    return FALSE;
}

