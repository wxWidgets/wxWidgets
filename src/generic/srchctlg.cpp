///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/srchctlg.cpp
// Purpose:     implements wxSearchCtrl as a composite control
// Author:      Vince Harron
// Created:     2006-02-19
// Copyright:   Vince Harron
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_SEARCHCTRL

#include "wx/srchctrl.h"

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/dcclient.h"
    #include "wx/menu.h"
    #include "wx/dcmemory.h"
#endif //WX_PRECOMP

#if !wxUSE_NATIVE_SEARCH_CONTROL

#include "wx/image.h"
#include "wx/utils.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the margin between the text control and the search/cancel buttons
static const wxCoord MARGIN = 2;

// arguments to wxColour::ChangeLightness() for making the search/cancel
// bitmaps foreground colour, respectively
static const int SEARCH_BITMAP_LIGHTNESS = 140; // slightly lighter
static const int CANCEL_BITMAP_LIGHTNESS = 160; // a bit more lighter

// ----------------------------------------------------------------------------
// wxSearchTextCtrl: text control used by search control
// ----------------------------------------------------------------------------

class wxSearchTextCtrl : public wxTextCtrl
{
public:
    wxSearchTextCtrl(wxSearchCtrl *search, const wxString& value, int style)
        : wxTextCtrl(search, wxID_ANY, value, wxDefaultPosition, wxDefaultSize,
                     (style & ~wxBORDER_MASK) | wxNO_BORDER | wxTE_PROCESS_ENTER)
    {
        m_search = search;

        SetHint(_("Search"));

        // Ensure that our best size is recomputed using our overridden
        // DoGetBestSize().
        InvalidateBestSize();
    }

    virtual wxWindow* GetMainWindowOfCompositeControl() override
    {
        return m_search;
    }

    // provide access to the base class protected methods to wxSearchCtrl which
    // needs to forward to them
    void DoSetValue(const wxString& value, int flags) override
    {
        wxTextCtrl::DoSetValue(value, flags);
    }

    bool DoLoadFile(const wxString& file, int fileType) override
    {
        return wxTextCtrl::DoLoadFile(file, fileType);
    }

    bool DoSaveFile(const wxString& file, int fileType) override
    {
        return wxTextCtrl::DoSaveFile(file, fileType);
    }

protected:
    void OnText(wxCommandEvent& eventText)
    {
        wxCommandEvent event(eventText);
        event.SetEventObject(m_search);
        event.SetId(m_search->GetId());

        m_search->GetEventHandler()->ProcessEvent(event);
    }

    void OnTextEnter(wxCommandEvent& WXUNUSED(event))
    {
        if ( !IsEmpty() )
        {
            wxCommandEvent event(wxEVT_SEARCH, m_search->GetId());
            event.SetEventObject(m_search);
            event.SetString(m_search->GetValue());

            m_search->ProcessWindowEvent(event);
        }
    }

#ifdef __WXMSW__
    // We increase the text control height to be the same as for the controls
    // with border as this is what we actually need here because even though
    // this control itself is borderless, it's inside wxSearchCtrl which does
    // have the border and so should have the same height as the normal text
    // entries with border.
    //
    // This is a bit ugly and it would arguably be better to use whatever size
    // the base class version returns and just centre the text vertically in
    // the search control but I failed to modify the code in LayoutControls()
    // to do this easily and as there is much in that code I don't understand
    // (notably what is the logic for buttons sizing?) I prefer to not touch it
    // at all.
    virtual wxSize DoGetBestSize() const override
    {
        const long flags = GetWindowStyleFlag();
        wxSearchTextCtrl* const self = const_cast<wxSearchTextCtrl*>(this);

        self->SetWindowStyleFlag((flags & ~wxBORDER_MASK) | wxBORDER_DEFAULT);
        wxSize size = wxTextCtrl::DoGetBestSize();

        // The calculation for no external borders in wxTextCtrl::DoGetSizeFromTextSize also
        // removes any padding around the value, which is wrong for this situation. So we
        // can't use wxBORDER_NONE to calculate a good height, in which case we just have to
        // assume a border in the code above and then subtract the space that would be taken up
        // by a themed border (the thin blue border and the white internal border).
        // Don't use FromDIP(4), this seems not needed.
        size.y -= 4;

        self->SetWindowStyleFlag(flags);

        return size;
    }
#endif // __WXMSW__

private:
    wxSearchCtrl* m_search;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxSearchTextCtrl, wxTextCtrl)
    EVT_TEXT(wxID_ANY, wxSearchTextCtrl::OnText)
    EVT_TEXT_ENTER(wxID_ANY, wxSearchTextCtrl::OnTextEnter)
    EVT_TEXT_MAXLEN(wxID_ANY, wxSearchTextCtrl::OnText)
wxEND_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxSearchButton: search button used by search control
// ----------------------------------------------------------------------------

class wxSearchButton : public wxControl
{
public:
    wxSearchButton(wxSearchCtrl *search, int eventType, const wxBitmap& bmp)
        : wxControl(search, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
          m_search(search),
          m_eventType(eventType),
          m_bmp(bmp)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
    }

    void SetBitmapLabel(const wxBitmap& label)
    {
        m_bmp = label;
        InvalidateBestSize();
    }

    // The buttons in wxSearchCtrl shouldn't accept focus from keyboard because
    // this would interfere with the usual TAB processing: the user expects
    // that pressing TAB in the search control should switch focus to the next
    // control and not give it to the button inside the same control. Besides,
    // the search button can be already activated by pressing "Enter" so there
    // is really no reason for it to be able to get focus from keyboard.
    virtual bool AcceptsFocusFromKeyboard() const override { return false; }

    virtual wxWindow* GetMainWindowOfCompositeControl() override
    {
        return m_search;
    }

protected:
    wxSize DoGetBestSize() const override
    {
        return wxSize(m_bmp.GetWidth(), m_bmp.GetHeight());
    }

    void OnLeftUp(wxMouseEvent&)
    {
        wxCommandEvent event(m_eventType, m_search->GetId());
        event.SetEventObject(m_search);

        if ( m_eventType == wxEVT_SEARCH )
        {
            // it's convenient to have the string to search for directly in the
            // event instead of having to retrieve it from the control in the
            // event handler code later, so provide it here
            event.SetString(m_search->GetValue());
        }

        GetEventHandler()->ProcessEvent(event);

        m_search->SetFocus();

#if wxUSE_MENUS
        if ( m_eventType == wxEVT_SEARCH )
        {
            // this happens automatically, just like on Mac OS X
            m_search->PopupSearchMenu();
        }
#endif // wxUSE_MENUS
    }

    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);

        // Clear the background in case of a user bitmap with alpha channel
        dc.SetBrush(m_search->GetBackgroundColour());
        dc.Clear();

        // Draw the bitmap
        dc.DrawBitmap(m_bmp, 0,0, true);
    }


private:
    wxSearchCtrl *m_search;
    wxEventType   m_eventType;
    wxBitmap      m_bmp;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxSearchButton, wxControl)
    EVT_LEFT_UP(wxSearchButton::OnLeftUp)
    EVT_PAINT(wxSearchButton::OnPaint)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(wxSearchCtrl, wxSearchCtrlBase)
    EVT_SEARCH_CANCEL(wxID_ANY, wxSearchCtrl::OnCancelButton)
    EVT_SIZE(wxSearchCtrl::OnSize)
    EVT_DPI_CHANGED(wxSearchCtrl::OnDPIChanged)
wxEND_EVENT_TABLE()

wxIMPLEMENT_DYNAMIC_CLASS(wxSearchCtrl, wxSearchCtrlBase);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSearchCtrl creation
// ----------------------------------------------------------------------------

// creation
// --------

wxSearchCtrl::wxSearchCtrl()
{
    Init();
}

wxSearchCtrl::wxSearchCtrl(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    Init();

    Create(parent, id, value, pos, size, style, validator, name);
}

void wxSearchCtrl::Init()
{
    m_text = nullptr;
    m_searchButton = nullptr;
    m_cancelButton = nullptr;
#if wxUSE_MENUS
    m_menu = nullptr;
#endif // wxUSE_MENUS

    m_searchBitmapUser = false;
    m_cancelBitmapUser = false;
#if wxUSE_MENUS
    m_searchMenuBitmapUser = false;
#endif // wxUSE_MENUS
}

bool wxSearchCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if ( !wxSearchCtrlBaseBaseClass::Create(parent, id, pos, size,
                                            style, validator, name) )
    {
        return false;
    }

    m_text = new wxSearchTextCtrl(this, value, style);

    m_searchButton = new wxSearchButton(this,
                                        wxEVT_SEARCH,
                                        m_searchBitmap);

    SetBackgroundColour( m_text->GetBackgroundColour() );
    m_text->SetBackgroundColour(wxColour());

    RecalcBitmaps();

    SetInitialSize(size);
    Move(pos);
    return true;
}

wxSearchCtrl::~wxSearchCtrl()
{
    delete m_text;
    delete m_searchButton;
    delete m_cancelButton;
#if wxUSE_MENUS
    delete m_menu;
#endif // wxUSE_MENUS
}


// search control specific interfaces
#if wxUSE_MENUS

void wxSearchCtrl::SetMenu( wxMenu* menu )
{
    if ( menu == m_menu )
    {
        // no change
        return;
    }
    bool hadMenu = (m_menu != nullptr);
    delete m_menu;
    m_menu = menu;

    if ( m_menu && !hadMenu )
    {
        m_searchButton->Show();
        m_searchButton->SetBitmapLabel(m_searchMenuBitmap);
        m_searchButton->Refresh();
    }
    else if ( !m_menu && hadMenu )
    {
        m_searchButton->SetBitmapLabel(m_searchBitmap);
        if ( m_searchButton->IsShown() )
        {
            m_searchButton->Refresh();
        }
    }
    LayoutControls();
}

wxMenu* wxSearchCtrl::GetMenu()
{
    return m_menu;
}

#endif // wxUSE_MENUS

void wxSearchCtrl::ShowSearchButton( bool show )
{
    if ( show == IsSearchButtonVisible() )
    {
        // no change
        return;
    }
    if ( show )
    {
        RecalcBitmaps();

        m_searchButton->Show();
    }
    else // Requested to hide it.
    {
        // Only hide the button if we don't need it for the menu, otherwise it
        // needs to remain shown.
        if ( !HasMenu() )
            m_searchButton->Hide();
    }

    LayoutControls();
}

bool wxSearchCtrl::IsSearchButtonVisible() const
{
    return m_searchButton->IsShown() || HasMenu();
}


void wxSearchCtrl::ShowCancelButton( bool show )
{
    if ( show == IsCancelButtonVisible() )
    {
        // no change
        return;
    }

    // This button is not shown initially, so create it on demand if necessary,
    // i.e. if it's the first time we show it.
    if ( !m_cancelButton )
    {
        m_cancelButton = new wxSearchButton(this,
                                            wxEVT_SEARCH_CANCEL,
                                            m_cancelBitmap);
        RecalcBitmaps();
    }

    m_cancelButton->Show(show);

    LayoutControls();
}

bool wxSearchCtrl::IsCancelButtonVisible() const
{
    return m_cancelButton && m_cancelButton->IsShown();
}

void wxSearchCtrl::SetDescriptiveText(const wxString& text)
{
    m_text->SetHint(text);
}

wxString wxSearchCtrl::GetDescriptiveText() const
{
    return m_text->GetHint();
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxSize wxSearchCtrl::DoGetBestClientSize() const
{
    wxSize size = m_text->GetBestSize();

    if ( IsSearchButtonVisible() )
    {
        size.x += m_searchButton->GetBestSize().x + FromDIP(MARGIN);
    }
    if ( IsCancelButtonVisible() )
    {
        size.x += m_cancelButton->GetBestSize().x + FromDIP(MARGIN);
    }

    int horizontalBorder = FromDIP(1) + (size.y - size.y * 14 / 21 ) / 2;
    size.x += 2 * horizontalBorder;

    return size;
}

void wxSearchCtrl::LayoutControls()
{
    if ( !m_text )
        return;

    const wxSize sizeTotal = GetClientSize();
    int width = sizeTotal.x,
        height = sizeTotal.y;

    wxSize sizeText = m_text->GetBestSize();
    // make room for the search menu & clear button
    int horizontalBorder = FromDIP(1) + ( sizeText.y - sizeText.y * 14 / 21 ) / 2;
    int x = 0;
    int textWidth = width;

    wxSize sizeSearch(0,0);
    wxSize sizeCancel(0,0);
    int searchMargin = 0;
    int cancelMargin = 0;
    if ( IsSearchButtonVisible() )
    {
        sizeSearch = m_searchButton->GetBestSize();
        searchMargin = FromDIP(MARGIN);
        x += horizontalBorder;
        textWidth -= horizontalBorder;
    }
    if ( IsCancelButtonVisible() )
    {
        sizeCancel = m_cancelButton->GetBestSize();
        cancelMargin = FromDIP(MARGIN);
        textWidth -= horizontalBorder;
    }

    if ( sizeSearch.x + sizeCancel.x > width )
    {
        sizeSearch.x = width/2;
        sizeCancel.x = width/2;
        searchMargin = 0;
        cancelMargin = 0;
    }
    textWidth -= sizeSearch.x + sizeCancel.x + searchMargin + cancelMargin + FromDIP(1);
    if (textWidth < 0) textWidth = 0;

    // position the subcontrols inside the client area

    if ( IsSearchButtonVisible() )
    {
        m_searchButton->SetSize(x, (height - sizeSearch.y) / 2,
                                sizeSearch.x, sizeSearch.y);
        x += sizeSearch.x;
        x += searchMargin;
    }

#ifdef __WXMSW__
    // The text control is too high up on Windows; normally a text control looks OK because
    // of the white border that's part of the theme border. We can also remove a pixel from
    // the height to fit the text control in, because the padding in EDIT_HEIGHT_FROM_CHAR_HEIGHT
    // is already generous.
    int textY = FromDIP(1);
#else
    int textY = 0;
#endif

    m_text->SetSize(x, textY, textWidth, height-textY);
    x += textWidth;

    if ( IsCancelButtonVisible() )
    {
        x += cancelMargin;
        m_cancelButton->SetSize(x, (height - sizeCancel.y) / 2,
                                sizeCancel.x, sizeCancel.y);
    }
}

wxWindowList wxSearchCtrl::GetCompositeWindowParts() const
{
    wxWindowList parts;
    parts.push_back(m_text);
    parts.push_back(m_searchButton);
    parts.push_back(m_cancelButton);
    return parts;
}

// accessors
// ---------

wxString wxSearchCtrl::DoGetValue() const
{
    return m_text->GetValue();
}
wxString wxSearchCtrl::GetRange(long from, long to) const
{
    return m_text->GetRange(from, to);
}

int wxSearchCtrl::GetLineLength(long lineNo) const
{
    return m_text->GetLineLength(lineNo);
}
wxString wxSearchCtrl::GetLineText(long lineNo) const
{
    return m_text->GetLineText(lineNo);
}
int wxSearchCtrl::GetNumberOfLines() const
{
    return m_text->GetNumberOfLines();
}

bool wxSearchCtrl::IsModified() const
{
    return m_text->IsModified();
}
bool wxSearchCtrl::IsEditable() const
{
    return m_text->IsEditable();
}

// more readable flag testing methods
bool wxSearchCtrl::IsSingleLine() const
{
    return m_text->IsSingleLine();
}
bool wxSearchCtrl::IsMultiLine() const
{
    return m_text->IsMultiLine();
}

// If the return values from and to are the same, there is no selection.
void wxSearchCtrl::GetSelection(long* from, long* to) const
{
    m_text->GetSelection(from, to);
}

wxString wxSearchCtrl::GetStringSelection() const
{
    return m_text->GetStringSelection();
}

// operations
// ----------

// editing
void wxSearchCtrl::Clear()
{
    m_text->Clear();
}
void wxSearchCtrl::Replace(long from, long to, const wxString& value)
{
    m_text->Replace(from, to, value);
}
void wxSearchCtrl::Remove(long from, long to)
{
    m_text->Remove(from, to);
}

// load/save the controls contents from/to the file
bool wxSearchCtrl::LoadFile(const wxString& file)
{
    return m_text->LoadFile(file);
}
bool wxSearchCtrl::SaveFile(const wxString& file)
{
    return m_text->SaveFile(file);
}

// sets/clears the dirty flag
void wxSearchCtrl::MarkDirty()
{
    m_text->MarkDirty();
}
void wxSearchCtrl::DiscardEdits()
{
    m_text->DiscardEdits();
}

// set the max number of characters which may be entered in a single line
// text control
void wxSearchCtrl::SetMaxLength(unsigned long len)
{
    m_text->SetMaxLength(len);
}

// writing text inserts it at the current position, appending always
// inserts it at the end
void wxSearchCtrl::WriteText(const wxString& text)
{
    m_text->WriteText(text);
}
void wxSearchCtrl::AppendText(const wxString& text)
{
    m_text->AppendText(text);
}

// insert the character which would have resulted from this key event,
// return true if anything has been inserted
bool wxSearchCtrl::EmulateKeyPress(const wxKeyEvent& event)
{
    return m_text->EmulateKeyPress(event);
}

// text control under some platforms supports the text styles: these
// methods allow to apply the given text style to the given selection or to
// set/get the style which will be used for all appended text
bool wxSearchCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    return m_text->SetStyle(start, end, style);
}
bool wxSearchCtrl::GetStyle(long position, wxTextAttr& style)
{
    return m_text->GetStyle(position, style);
}
bool wxSearchCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    return m_text->SetDefaultStyle(style);
}
const wxTextAttr& wxSearchCtrl::GetDefaultStyle() const
{
    return m_text->GetDefaultStyle();
}

// translate between the position (which is just an index in the text ctrl
// considering all its contents as a single strings) and (x, y) coordinates
// which represent column and line.
long wxSearchCtrl::XYToPosition(long x, long y) const
{
    return m_text->XYToPosition(x, y);
}
bool wxSearchCtrl::PositionToXY(long pos, long *x, long *y) const
{
    return m_text->PositionToXY(pos, x, y);
}

void wxSearchCtrl::ShowPosition(long pos)
{
    m_text->ShowPosition(pos);
}

// find the character at position given in pixels
//
// NB: pt is in device coords (not adjusted for the client area origin nor
//     scrolling)
wxTextCtrlHitTestResult wxSearchCtrl::HitTest(const wxPoint& pt, long *pos) const
{
    return m_text->HitTest(pt, pos);
}
wxTextCtrlHitTestResult wxSearchCtrl::HitTest(const wxPoint& pt,
                                        wxTextCoord *col,
                                        wxTextCoord *row) const
{
    return m_text->HitTest(pt, col, row);
}

// Clipboard operations
void wxSearchCtrl::Copy()
{
    m_text->Copy();
}
void wxSearchCtrl::Cut()
{
    m_text->Cut();
}
void wxSearchCtrl::Paste()
{
    m_text->Paste();
}

bool wxSearchCtrl::CanCopy() const
{
    return m_text->CanCopy();
}
bool wxSearchCtrl::CanCut() const
{
    return m_text->CanCut();
}
bool wxSearchCtrl::CanPaste() const
{
    return m_text->CanPaste();
}

// Undo/redo
void wxSearchCtrl::Undo()
{
    m_text->Undo();
}
void wxSearchCtrl::Redo()
{
    m_text->Redo();
}

bool wxSearchCtrl::CanUndo() const
{
    return m_text->CanUndo();
}
bool wxSearchCtrl::CanRedo() const
{
    return m_text->CanRedo();
}

// Insertion point
void wxSearchCtrl::SetInsertionPoint(long pos)
{
    m_text->SetInsertionPoint(pos);
}
void wxSearchCtrl::SetInsertionPointEnd()
{
    m_text->SetInsertionPointEnd();
}
long wxSearchCtrl::GetInsertionPoint() const
{
    return m_text->GetInsertionPoint();
}
long wxSearchCtrl::GetLastPosition() const
{
    return m_text->GetLastPosition();
}

void wxSearchCtrl::SetSelection(long from, long to)
{
    m_text->SetSelection(from, to);
}
void wxSearchCtrl::SelectAll()
{
    m_text->SelectAll();
}

void wxSearchCtrl::SetEditable(bool editable)
{
    m_text->SetEditable(editable);
}

bool wxSearchCtrl::SetFont(const wxFont& font)
{
    if ( !wxSearchCtrlBase::SetFont(font) )
        return false;

    // Recreate the bitmaps as their size may have changed.
    RecalcBitmaps();

    return true;
}

bool wxSearchCtrl::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxSearchCtrlBase::SetBackgroundColour(colour) )
        return false;

    // When the background changes, re-render the bitmaps so that the correct
    // colour shows in their "transparent" area.
    RecalcBitmaps();

    return true;
}


// Autocomplete
bool wxSearchCtrl::DoAutoCompleteStrings(const wxArrayString &choices)
{
    return m_text->AutoComplete( choices );
}

bool wxSearchCtrl::DoAutoCompleteFileNames(int flags)
{
    return flags == wxFILE ? m_text->AutoCompleteFileNames() : m_text->AutoCompleteDirectories();
}

bool wxSearchCtrl::DoAutoCompleteCustom(wxTextCompleter *completer)
{
    return m_text->AutoComplete(completer);
}


// search control generic only
void wxSearchCtrl::SetSearchBitmap( const wxBitmap& bitmap )
{
    m_searchBitmap = bitmap;
    m_searchBitmapUser = bitmap.IsOk();
    if ( m_searchBitmapUser )
    {
        if ( m_searchButton && !HasMenu() )
        {
            m_searchButton->SetBitmapLabel( m_searchBitmap );
        }
    }
    else
    {
        // the user bitmap was just cleared, generate one
        RecalcBitmaps();
    }
}

#if wxUSE_MENUS

void wxSearchCtrl::SetSearchMenuBitmap( const wxBitmap& bitmap )
{
    m_searchMenuBitmap = bitmap;
    m_searchMenuBitmapUser = bitmap.IsOk();
    if ( m_searchMenuBitmapUser )
    {
        if ( m_searchButton && m_menu )
        {
            m_searchButton->SetBitmapLabel( m_searchMenuBitmap );
        }
    }
    else
    {
        // the user bitmap was just cleared, generate one
        RecalcBitmaps();
    }
}

#endif // wxUSE_MENUS

void wxSearchCtrl::SetCancelBitmap( const wxBitmap& bitmap )
{
    m_cancelBitmap = bitmap;
    m_cancelBitmapUser = bitmap.IsOk();
    if ( m_cancelBitmapUser )
    {
        if ( m_cancelButton )
        {
            m_cancelButton->SetBitmapLabel( m_cancelBitmap );
        }
    }
    else
    {
        // the user bitmap was just cleared, generate one
        RecalcBitmaps();
    }
}

// Note that overriding DoSetValue() is currently insufficient because the base
// class ChangeValue() only updates m_hintData of this object (which is null
// anyhow), instead of updating m_text->m_hintData, see #16998.
void wxSearchCtrl::ChangeValue(const wxString& value)
{
    m_text->ChangeValue(value);
}

void wxSearchCtrl::DoSetValue(const wxString& value, int flags)
{
    m_text->DoSetValue(value, flags);
}

bool wxSearchCtrl::DoLoadFile(const wxString& file, int fileType)
{
    return m_text->DoLoadFile(file, fileType);
}

bool wxSearchCtrl::DoSaveFile(const wxString& file, int fileType)
{
    return m_text->DoSaveFile(file, fileType);
}

bool wxSearchCtrl::ShouldInheritColours() const
{
    return true;
}

// icons are rendered at 3-8 times larger than necessary and downscaled for
// antialiasing
static int GetMultiplier()
{
    int depth = ::wxDisplayDepth();

    if  ( depth >= 24 )
    {
        return 8;
    }
    return 6;
}

wxBitmap wxSearchCtrl::RenderSearchBitmap( int x, int y, bool renderDrop )
{
    wxColour bg = GetBackgroundColour();
    wxColour fg = GetForegroundColour().ChangeLightness(SEARCH_BITMAP_LIGHTNESS);

    //===============================================================================
    // begin drawing code
    //===============================================================================
    // image stats

    // force width:height ratio
    if ( 14*x > y*20 )
    {
        // x is too big
        x = y*20/14;
    }
    else
    {
        // y is too big
        y = x*14/20;
    }

    // glass 11x11, top left corner
    // handle (9,9)-(13,13)
    // drop (13,16)-(19,6)-(16,9)

    int multiplier = GetMultiplier();
    int penWidth = multiplier * 2;

    penWidth = penWidth * x / 20;

    wxBitmap bitmap( multiplier*x, multiplier*y );
    wxMemoryDC mem;
    mem.SelectObject(bitmap);

    // clear background
    mem.SetBrush( wxBrush(bg) );
    mem.SetPen( wxPen(bg) );
    mem.DrawRectangle(0,0,bitmap.GetWidth(),bitmap.GetHeight());

    // draw drop glass
    mem.SetBrush( wxBrush(fg) );
    mem.SetPen( wxPen(fg) );
    int glassBase = 5 * x / 20;
    int glassFactor = 2*glassBase + 1;
    int radius = multiplier*glassFactor/2;
    mem.DrawCircle(radius,radius,radius);
    mem.SetBrush( wxBrush(bg) );
    mem.SetPen( wxPen(bg) );
    mem.DrawCircle(radius,radius,radius-penWidth);

    // draw handle
    int lineStart = radius + (radius-penWidth/2) * 707 / 1000; // 707 / 1000 = 0.707 = 1/sqrt(2);

    mem.SetPen( wxPen(fg) );
    mem.SetBrush( wxBrush(fg) );
    int handleCornerShift = penWidth * 707 / 1000 / 2; // 707 / 1000 = 0.707 = 1/sqrt(2);
    handleCornerShift = wxMax( handleCornerShift, 1 );
    int handleBase = 4 * x / 20;
    int handleLength = 2*handleBase+1;
    wxPoint handlePolygon[] =
    {
        wxPoint(-handleCornerShift,+handleCornerShift),
        wxPoint(+handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*handleLength/2+handleCornerShift,multiplier*handleLength/2-handleCornerShift),
        wxPoint(multiplier*handleLength/2-handleCornerShift,multiplier*handleLength/2+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon),handlePolygon,lineStart,lineStart);

    // draw drop triangle
    int triangleX = 13 * x / 20;
    int triangleY = 5 * x / 20;
    int triangleBase = 3 * x / 20;
    int triangleFactor = triangleBase*2+1;
    if ( renderDrop )
    {
        wxPoint dropPolygon[] =
        {
            wxPoint(multiplier*0,multiplier*0), // triangle left
            wxPoint(multiplier*triangleFactor-1,multiplier*0), // triangle right
            wxPoint(multiplier*triangleFactor/2,multiplier*triangleFactor/2), // triangle bottom
        };
        mem.DrawPolygon(WXSIZEOF(dropPolygon),dropPolygon,multiplier*triangleX,multiplier*triangleY);
    }
    mem.SelectObject(wxNullBitmap);

    //===============================================================================
    // end drawing code
    //===============================================================================

    if ( multiplier != 1 )
    {
        wxBitmap::Rescale(bitmap, wxSize(x, y));
    }
    if ( !renderDrop )
    {
        // Trim the edge where the arrow would have gone
        bitmap = bitmap.GetSubBitmap(wxRect(0,0, y,y));
    }

    return bitmap;
}

wxBitmap wxSearchCtrl::RenderCancelBitmap( int x, int y )
{
    wxColour bg = GetBackgroundColour();
    wxColour fg = GetForegroundColour().ChangeLightness(CANCEL_BITMAP_LIGHTNESS);

    //===============================================================================
    // begin drawing code
    //===============================================================================
    // image stats

    // total size 14x14
    // force 1:1 ratio
    if ( x > y )
    {
        // x is too big
        x = y;
    }
    else
    {
        // y is too big
        y = x;
    }

    // 14x14 circle
    // cross line starts (4,4)-(10,10)
    // drop (13,16)-(19,6)-(16,9)

    int multiplier = GetMultiplier();

    int penWidth = multiplier * x / 14;

    wxBitmap bitmap( multiplier*x, multiplier*y );
    wxMemoryDC mem;
    mem.SelectObject(bitmap);

    // clear background
    mem.SetBrush( wxBrush(bg) );
    mem.SetPen( wxPen(bg) );
    mem.DrawRectangle(0,0,bitmap.GetWidth(),bitmap.GetHeight());

    // draw drop glass
    mem.SetBrush( wxBrush(fg) );
    mem.SetPen( wxPen(fg) );
    int radius = multiplier*x/2;
    mem.DrawCircle(radius,radius,radius);

    // draw cross
    int lineStartBase = 4 * x / 14;
    int lineLength = x - 2*lineStartBase;

    mem.SetPen( wxPen(bg) );
    mem.SetBrush( wxBrush(bg) );
    int handleCornerShift = penWidth/2;
    handleCornerShift = wxMax( handleCornerShift, 1 );
    wxPoint handlePolygon[] =
    {
        wxPoint(-handleCornerShift,+handleCornerShift),
        wxPoint(+handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*lineLength+handleCornerShift,multiplier*lineLength-handleCornerShift),
        wxPoint(multiplier*lineLength-handleCornerShift,multiplier*lineLength+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon),handlePolygon,multiplier*lineStartBase,multiplier*lineStartBase);
    wxPoint handlePolygon2[] =
    {
        wxPoint(+handleCornerShift,+handleCornerShift),
        wxPoint(-handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*lineLength-handleCornerShift,-multiplier*lineLength-handleCornerShift),
        wxPoint(multiplier*lineLength+handleCornerShift,-multiplier*lineLength+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon2),handlePolygon2,multiplier*lineStartBase,multiplier*(x-lineStartBase));

    // Stop drawing on the bitmap before possibly calling wxBitmap::Rescale()
    // below.
    mem.SelectObject(wxNullBitmap);

    //===============================================================================
    // end drawing code
    //===============================================================================

    if ( multiplier != 1 )
    {
        wxBitmap::Rescale(bitmap, wxSize(x, y));
    }

    return bitmap;
}

void wxSearchCtrl::RecalcBitmaps()
{
    if ( !m_text )
    {
        return;
    }
    wxSize sizeText = m_text->GetBestSize();

    int bitmapHeight = sizeText.y - FromDIP(4);
    int bitmapWidth  = sizeText.y * 20 / 14;

    if ( !m_searchBitmapUser )
    {
        if (
            !m_searchBitmap.IsOk() ||
            m_searchBitmap.GetHeight() != bitmapHeight ||
            m_searchBitmap.GetWidth() != bitmapWidth
            )
        {
            m_searchBitmap = RenderSearchBitmap(bitmapWidth,bitmapHeight,false);
            if ( !HasMenu() )
            {
                m_searchButton->SetBitmapLabel(m_searchBitmap);
            }
        }
        // else this bitmap was set by user, don't alter
    }

#if wxUSE_MENUS
    if ( !m_searchMenuBitmapUser )
    {
        if (
            !m_searchMenuBitmap.IsOk() ||
            m_searchMenuBitmap.GetHeight() != bitmapHeight ||
            m_searchMenuBitmap.GetWidth() != bitmapWidth
            )
        {
            m_searchMenuBitmap = RenderSearchBitmap(bitmapWidth,bitmapHeight,true);
            if ( m_menu )
            {
                m_searchButton->SetBitmapLabel(m_searchMenuBitmap);
            }
        }
        // else this bitmap was set by user, don't alter
    }
#endif // wxUSE_MENUS

    if ( m_cancelButton && !m_cancelBitmapUser )
    {
        if (
            !m_cancelBitmap.IsOk() ||
            m_cancelBitmap.GetHeight() != bitmapHeight ||
            m_cancelBitmap.GetWidth() != bitmapWidth
            )
        {
            m_cancelBitmap = RenderCancelBitmap(bitmapWidth,bitmapHeight);
            m_cancelButton->SetBitmapLabel(m_cancelBitmap);
        }
        // else this bitmap was set by user, don't alter
    }
}

void wxSearchCtrl::OnCancelButton( wxCommandEvent& event )
{
    m_text->Clear();
    event.Skip();
}

void wxSearchCtrl::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    LayoutControls();
}

void wxSearchCtrl::OnDPIChanged(wxDPIChangedEvent &event)
{
    RecalcBitmaps();

    event.Skip();
}

#if wxUSE_MENUS

void wxSearchCtrl::PopupSearchMenu()
{
    if ( m_menu )
    {
        wxSize size = GetSize();
        PopupMenu( m_menu, 0, size.y );
    }
}

#endif // wxUSE_MENUS

#endif // !wxUSE_NATIVE_SEARCH_CONTROL

#endif // wxUSE_SEARCHCTRL
