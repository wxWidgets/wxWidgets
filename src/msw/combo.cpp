/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/combo.cpp
// Purpose:     wxMSW wxComboCtrl
// Author:      Jaakko Salli
// Modified by:
// Created:     Apr-30-2006
// Copyright:   (c) 2005 Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COMBOCTRL

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/combobox.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/dialog.h"
    #include "wx/stopwatch.h"
#endif

#include "wx/dcbuffer.h"
#include "wx/combo.h"

#include "wx/msw/registry.h"
#include "wx/msw/uxtheme.h"
#include "wx/msw/dc.h"

#define NATIVE_TEXT_INDENT_XP       4
#define NATIVE_TEXT_INDENT_CLASSIC  2

#define COMBOBOX_ANIMATION_RESOLUTION   10

#define COMBOBOX_ANIMATION_DURATION     200  // In milliseconds

#define wxMSW_DESKTOP_USERPREFERENCESMASK_COMBOBOXANIM    (1<<2)


// ============================================================================
// implementation
// ============================================================================


wxBEGIN_EVENT_TABLE(wxComboCtrl, wxComboCtrlBase)
    EVT_PAINT(wxComboCtrl::OnPaintEvent)
    EVT_MOUSE_EVENTS(wxComboCtrl::OnMouseEvent)
#if wxUSE_COMBOCTRL_POPUP_ANIMATION
    EVT_TIMER(wxID_ANY, wxComboCtrl::OnTimerEvent)
#endif
wxEND_EVENT_TABLE()


wxIMPLEMENT_DYNAMIC_CLASS(wxComboCtrl, wxComboCtrlBase);

void wxComboCtrl::Init()
{
}

bool wxComboCtrl::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxString& value,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxValidator& validator,
                            const wxString& name)
{

    // Set border
    long border = style & wxBORDER_MASK;

    if ( !border )
    {
        if ( wxUxThemeIsActive() )
        {
            // For XP, have 1-width custom border, for older version use sunken
            border = wxBORDER_NONE;
            m_widthCustomBorder = 1;
        }
        else
            border = wxBORDER_SUNKEN;

        style = (style & ~(wxBORDER_MASK)) | border;
    }

    // create main window
    if ( !wxComboCtrlBase::Create(parent,
                           id,
                           value,
                           pos,
                           size,
                           style | wxFULL_REPAINT_ON_RESIZE,
                           validator,
                           name) )
        return false;

    if ( wxUxThemeIsActive() && ::wxGetWinVersion() >= wxWinVersion_Vista )
            m_iFlags |= wxCC_BUTTON_STAYS_DOWN |wxCC_BUTTON_COVERS_BORDER;

    if ( style & wxCC_STD_BUTTON )
        m_iFlags |= wxCC_POPUP_ON_MOUSE_UP;

    // Prepare background for double-buffering or better background theme
    // support, whichever is possible.
    SetDoubleBuffered(true);
    if ( !IsDoubleBuffered() )
        SetBackgroundStyle( wxBG_STYLE_PAINT );

    // Create textctrl, if necessary
    CreateTextCtrl( wxNO_BORDER );

    // Add keyboard input handlers for main control and textctrl
    InstallInputHandlers();

    // SetInitialSize should be called last
    SetInitialSize(size);

    return true;
}

wxComboCtrl::~wxComboCtrl()
{
}

void wxComboCtrl::OnResize()
{
    //
    // Recalculates button and textctrl areas

    // Technically Classic Windows style combo has more narrow button,
    // but the native renderer doesn't paint it well like that.
    int btnWidth = FromDIP(17);
    CalculateAreas(btnWidth);

    // Position textctrl using standard routine
    PositionTextCtrl();
}

// Draws non-XP GUI dotted line around the focus area
static void wxMSWDrawFocusRect( wxDC& dc, const wxRect& rect )
{
    /*
    RECT mswRect;
    mswRect.left = rect.x;
    mswRect.top = rect.y;
    mswRect.right = rect.x + rect.width;
    mswRect.bottom = rect.y + rect.height;
    HDC hdc = (HDC) dc.GetHDC();
    SetMapMode(hdc,MM_TEXT); // Just in case...
    DrawFocusRect(hdc,&mswRect);
    */
    // FIXME: Use DrawFocusRect code above (currently it draws solid line
    //   for caption focus but works ok for other stuff).
    //   Also, this code below may not work in future wx versions, since
    //   it employs wxCAP_BUTT hack to have line of width 1.
    dc.SetLogicalFunction(wxINVERT);

    wxPen pen(*wxBLACK, 1, wxPENSTYLE_DOT);
    pen.SetCap(wxCAP_BUTT);
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(rect);

    dc.SetLogicalFunction(wxCOPY);
}

// draw focus background on area in a way typical on platform
void
wxComboCtrl::PrepareBackground( wxDC& dc, const wxRect& rect, int flags ) const
{
#if wxUSE_UXTHEME
    wxUxThemeHandle hTheme(this, L"COMBOBOX");
#endif

    wxSize sz = GetClientSize();
    bool isEnabled;
    bool doDrawFocusRect; // also selected

    // For smaller size control (and for disabled background) use less spacing
    int focusSpacingX;
    int focusSpacingY;

    if ( !(flags & wxCONTROL_ISSUBMENU) )
    {
        // Drawing control
        isEnabled = IsThisEnabled();
        doDrawFocusRect = ShouldDrawFocus();

#if wxUSE_UXTHEME
        // Windows-style: for smaller size control (and for disabled background) use less spacing
        if ( hTheme )
        {
            // WinXP  Theme
            focusSpacingX = isEnabled ? 2 : 1;
            focusSpacingY = sz.y > (GetCharHeight()+2) && isEnabled ? 2 : 1;
        }
        else
#endif
        {
            // Classic Theme
            if ( isEnabled )
            {
                focusSpacingX = 1;
                focusSpacingY = 1;
            }
            else
            {
                focusSpacingX = 0;
                focusSpacingY = 0;
            }
        }
    }
    else
    {
        // Drawing a list item
        isEnabled = true; // they are never disabled
        doDrawFocusRect = flags & wxCONTROL_SELECTED ? true : false;

        focusSpacingX = 0;
        focusSpacingY = 0;
    }

    // Set the background sub-rectangle for selection, disabled etc
    wxRect selRect(rect);
    selRect.y += focusSpacingY;
    selRect.height -= (focusSpacingY*2);

    int wcp = 0;

    if ( !(flags & wxCONTROL_ISSUBMENU) )
        wcp += m_widthCustomPaint;

    selRect.x += wcp + focusSpacingX;
    selRect.width -= wcp + (focusSpacingX*2);

    wxColour fgCol;
    wxColour bgCol;
    bool doDrawDottedEdge = false;
    bool doDrawSelRect = true;

    // TODO: doDrawDottedEdge = true when focus has arrived to control via tab.
    //       (and other cases which are not that apparent).

    if ( isEnabled )
    {
        // If popup is hidden and this control is focused,
        // then draw the focus-indicator (selbgcolor background etc.).
        if ( doDrawFocusRect )
        {
            // NB: We can't really use XP visual styles to get TMT_TEXTCOLOR since
            //     it is not properly defined for combo boxes. Instead, they expect
            //     you to use DrawThemeText.
            //
            //    Here is, however, sample code how to get theme colours:
            //
            //    COLORREF cref;
            //    theme->GetThemeColor(hTheme,EP_EDITTEXT,ETS_NORMAL,TMT_TEXTCOLOR,&cref);
            //    dc.SetTextForeground( wxRGBToColour(cref) );
            if ( (m_iFlags & wxCC_FULL_BUTTON) && !(flags & wxCONTROL_ISSUBMENU) )
            {
                // Vista style read-only combo
                fgCol = GetForegroundColour();
                bgCol = GetBackgroundColour();
                doDrawSelRect = false;
                doDrawDottedEdge = true;
            }
            else
            {
                fgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
                bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
            }
        }
        else
        {
            fgCol = GetForegroundColour();
            bgCol = GetBackgroundColour();
            doDrawSelRect = false;
        }
    }
    else
    {
        fgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
        bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    }

    dc.SetTextForeground(fgCol);
    dc.SetBrush(bgCol);
    if ( doDrawSelRect )
    {
        dc.SetPen(bgCol);
        dc.DrawRectangle(selRect);
    }

    if ( doDrawDottedEdge )
        wxMSWDrawFocusRect(dc, selRect);

    // Don't clip exactly to the selection rectangle so we can draw
    // to the non-selected area in front of it.
    wxRect clipRect(rect.x,rect.y,
                    (selRect.x+selRect.width)-rect.x-1,rect.height);
    dc.SetClippingRegion(clipRect);
}

void wxComboCtrl::OnPaintEvent( wxPaintEvent& WXUNUSED(event) )
{
    // TODO: Convert drawing in this function to Windows API Code

    wxSize sz = GetClientSize();
    wxDC* dcPtr = wxAutoBufferedPaintDCFactory(this);
    wxDC& dc = *dcPtr;

    const wxRect& rectButton = m_btnArea;
    wxRect rectTextField = m_tcArea;

    // FIXME: Either SetBackgroundColour or GetBackgroundColour
    //        doesn't work under Vista, so here's a temporary
    //        workaround.
    //        In the theme-less rendering code below, this fixes incorrect
    //        background on read-only comboboxes (they are gray, but should be
    //        white).
    wxColour bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

#if wxUSE_UXTHEME
    const bool isEnabled = IsThisEnabled();

    wxMSWDCImpl *impl = (wxMSWDCImpl*) dc.GetImpl();
    HDC hDc = GetHdcOf(*impl);
    HWND hWnd = GetHwndOf(this);

    wxUxThemeHandle hTheme(this, L"COMBOBOX");
#endif // wxUSE_UXTHEME

    wxRect borderRect(0,0,sz.x,sz.y);

    if ( m_iFlags & wxCC_IFLAG_BUTTON_OUTSIDE )
    {
        borderRect = m_tcArea;
        borderRect.Inflate(1);
    }

    int drawButFlags = 0;

#if wxUSE_UXTHEME
    if ( hTheme )
    {
        const bool useVistaComboBox = ::wxGetWinVersion() >= wxWinVersion_Vista;

        RECT rFull;
        wxCopyRectToRECT(borderRect, rFull);

        RECT rButton;
        wxCopyRectToRECT(rectButton, rButton);

        RECT rBorder;
        wxCopyRectToRECT(borderRect, rBorder);

        bool isNonStdButton = (m_iFlags & wxCC_IFLAG_BUTTON_OUTSIDE) ||
                              (m_iFlags & wxCC_IFLAG_HAS_NONSTANDARD_BUTTON);

        //
        // Get some states for themed drawing
        int butState;

        if ( !isEnabled )
        {
            butState = CBXS_DISABLED;
        }
        // Vista will display the drop-button as depressed always
        // when the popup window is visilbe
        else if ( (m_btnState & wxCONTROL_PRESSED) ||
                  (useVistaComboBox && !IsPopupWindowState(Hidden)) )
        {
            butState = CBXS_PRESSED;
        }
        else if ( m_btnState & wxCONTROL_CURRENT )
        {
            butState = CBXS_HOT;
        }
        else
        {
            butState = CBXS_NORMAL;
        }

        int comboBoxPart = 0;  // For XP, use the 'default' part
        RECT* rUseForBg = &rBorder;

        bool drawFullButton = false;
        int bgState = butState;
        const bool isFocused = (FindFocus() == GetMainWindowOfCompositeControl()) ? true : false;

        if ( useVistaComboBox )
        {
            // Draw the entire control as a single button?
            if ( !isNonStdButton )
            {
                if ( HasFlag(wxCB_READONLY) )
                    drawFullButton = true;
            }

            if ( drawFullButton )
            {
                comboBoxPart = CP_READONLY;
                rUseForBg = &rFull;

                // It should be safe enough to update this flag here.
                m_iFlags |= wxCC_FULL_BUTTON;
            }
            else
            {
                comboBoxPart = CP_BORDER;
                m_iFlags &= ~wxCC_FULL_BUTTON;

                if ( isFocused )
                    bgState = CBB_FOCUSED;
                else
                    bgState = CBB_NORMAL;
            }
        }

        //
        // Draw parent's background, if necessary
        RECT* rUseForTb = NULL;

        if ( ::IsThemeBackgroundPartiallyTransparent( hTheme, comboBoxPart, bgState ) )
            rUseForTb = &rFull;
        else if ( m_iFlags & wxCC_IFLAG_BUTTON_OUTSIDE )
            rUseForTb = &rButton;

        if ( rUseForTb )
            ::DrawThemeParentBackground( hWnd, hDc, rUseForTb );

        //
        // Draw the control background (including the border)
        if ( m_widthCustomBorder > 0 )
        {
            ::DrawThemeBackground( hTheme, hDc, comboBoxPart, bgState, rUseForBg, NULL );
        }
        else
        {
            // No border. We can't use theme, since it cannot be relied on
            // to deliver borderless drawing, even with DrawThemeBackgroundEx.
            dc.SetBrush(bgCol);
            dc.SetPen(bgCol);
            dc.DrawRectangle(borderRect);
        }

        //
        // Draw the drop-button
        if ( !isNonStdButton )
        {
            drawButFlags = Button_BitmapOnly;

            int butPart = CP_DROPDOWNBUTTON;

            if ( useVistaComboBox )
            {
                if ( drawFullButton )
                {
                    // We need to alter the button style slightly before
                    // drawing the actual button (but it was good above
                    // when background etc was done).
                    if ( butState == CBXS_HOT || butState == CBXS_PRESSED )
                        butState = CBXS_NORMAL;
                }

                if ( m_btnSide == wxRIGHT )
                    butPart = CP_DROPDOWNBUTTONRIGHT;
                else
                    butPart = CP_DROPDOWNBUTTONLEFT;

            }
            ::DrawThemeBackground( hTheme, hDc, butPart, butState, &rButton, NULL );
        }
        else if ( useVistaComboBox &&
                  (m_iFlags & wxCC_IFLAG_BUTTON_OUTSIDE) )
        {
            // We'll do this, because DrawThemeParentBackground
            // doesn't seem to be reliable on Vista.
            drawButFlags |= Button_PaintBackground;
        }
    }
    else
#endif
    {
        // Windows 2000 and earlier
        drawButFlags = Button_PaintBackground;

        dc.SetBrush(bgCol);
        dc.SetPen(bgCol);
        dc.DrawRectangle(borderRect);
    }

    // Button rendering (may only do the bitmap on button, depending on the flags)
    DrawButton( dc, rectButton, drawButFlags );

    // Paint required portion of the custom image on the control
    if ( (!m_text || m_widthCustomPaint) )
    {
        wxASSERT( m_widthCustomPaint >= 0 );

        // this is intentionally here to allow drawed rectangle's
        // right edge to be hidden
        if ( m_text )
            rectTextField.width = m_widthCustomPaint;

        dc.SetFont( GetFont() );

        dc.SetClippingRegion(rectTextField);
        if ( m_popupInterface )
            m_popupInterface->PaintComboControl(dc,rectTextField);
        else
            wxComboPopup::DefaultPaintComboControl(this,dc,rectTextField);
    }

    delete dcPtr;
}

void wxComboCtrl::OnMouseEvent( wxMouseEvent& event )
{
    int mx = event.m_x;
    bool isOnButtonArea = m_btnArea.Contains(mx,event.m_y);
    int handlerFlags = isOnButtonArea ? wxCC_MF_ON_BUTTON : 0;

    if ( PreprocessMouseEvent(event,isOnButtonArea) )
        return;

    if ( (m_windowStyle & (wxCC_SPECIAL_DCLICK|wxCB_READONLY)) == wxCB_READONLY )
    {
        // if no textctrl and no special double-click, then the entire control acts
        // as a button
        handlerFlags |= wxCC_MF_ON_BUTTON;
        if ( HandleButtonMouseEvent(event,handlerFlags) )
            return;
    }
    else
    {
        if ( isOnButtonArea || HasCapture() ||
             (m_widthCustomPaint && mx < (m_tcArea.x+m_widthCustomPaint)) )
        {
            handlerFlags |= wxCC_MF_ON_CLICK_AREA;

            if ( HandleButtonMouseEvent(event,handlerFlags) )
                return;
        }
        else if ( m_btnState )
        {
            // otherwise need to clear the hover status
            m_btnState = 0;
            RefreshRect(m_btnArea);
        }
    }

    //
    // This will handle left_down and left_dclick events outside button in a Windows-like manner.
    // See header file for further information on this method.
    HandleNormalMouseEvent(event);

}

#if wxUSE_COMBOCTRL_POPUP_ANIMATION
static wxUint32 GetUserPreferencesMask()
{
    static wxUint32 userPreferencesMask = 0;
    static bool valueSet = false;

    if ( valueSet )
        return userPreferencesMask;

    wxRegKey* pKey = NULL;
    wxRegKey key1(wxRegKey::HKCU, wxT("Software\\Policies\\Microsoft\\Control Panel"));
    wxRegKey key2(wxRegKey::HKCU, wxT("Software\\Policies\\Microsoft\\Windows\\Control Panel"));
    wxRegKey key3(wxRegKey::HKCU, wxT("Control Panel\\Desktop"));

    if ( key1.Exists() )
        pKey = &key1;
    else if ( key2.Exists() )
        pKey = &key2;
    else if ( key3.Exists() )
        pKey = &key3;

    if ( pKey && pKey->Open(wxRegKey::Read) )
    {
        wxMemoryBuffer buf;
        if ( pKey->HasValue(wxT("UserPreferencesMask")) &&
             pKey->QueryValue(wxT("UserPreferencesMask"), buf) )
        {
            if ( buf.GetDataLen() >= 4 )
            {
                wxUint32* p = (wxUint32*) buf.GetData();
                userPreferencesMask = *p;
            }
        }
    }

    valueSet = true;

    return userPreferencesMask;
}
#endif

#if wxUSE_COMBOCTRL_POPUP_ANIMATION
void wxComboCtrl::DoTimerEvent()
{
    bool stopTimer = false;

    wxWindow* win = GetPopupWindow();
    wxWindow* popup = GetPopupControl()->GetControl();

    // Popup was hidden before it was fully shown?
    if ( IsPopupWindowState(Hidden) )
    {
        stopTimer = true;
    }
    else
    {
        wxMilliClock_t t = ::wxGetLocalTimeMillis();
        const wxRect& rect = m_animRect;

#if wxUSE_LONGLONG
        int pos = (int) (t-m_animStart).GetLo();
#else
        int pos = (int) (t-m_animStart);
#endif
        if ( pos < COMBOBOX_ANIMATION_DURATION )
        {
            int height = rect.height;
            //int h0 = rect.height;
            int h = (((pos*256)/COMBOBOX_ANIMATION_DURATION)*height)/256;
            int y = (height - h);
            if ( y < 0 )
                y = 0;

            if ( m_animFlags & ShowAbove )
            {
                win->SetSize( rect.x, rect.y + height - h, rect.width, h );
            }
            else
            {
                // Note that apparently Move() should be called after
                // SetSize() to reduce (or even eliminate) animation garbage
                win->SetSize( rect.x, rect.y, rect.width, h );
                popup->Move( 0, -y );
            }
        }
        else
        {
            stopTimer = true;
        }
    }

    if ( stopTimer )
    {
        m_animTimer.Stop();
        DoShowPopup( m_animRect, m_animFlags );
        popup->Move( 0, 0 );

        // Do a one final refresh to clean up the rare cases of animation
        // garbage
        win->Refresh();
    }
}
#endif

#if wxUSE_COMBOCTRL_POPUP_ANIMATION
bool wxComboCtrl::AnimateShow( const wxRect& rect, int flags )
{
    if ( GetUserPreferencesMask() & wxMSW_DESKTOP_USERPREFERENCESMASK_COMBOBOXANIM )
    {
        m_animStart = ::wxGetLocalTimeMillis();
        m_animRect = rect;
        m_animFlags = flags;

        wxWindow* win = GetPopupWindow();
        win->SetSize( rect.x, rect.y, rect.width, 0 );
        win->Show();

        m_animTimer.SetOwner( this, wxID_ANY );
        m_animTimer.Start( COMBOBOX_ANIMATION_RESOLUTION, wxTIMER_CONTINUOUS );

        DoTimerEvent();

        return false;
    }

    return true;
}
#endif

wxCoord wxComboCtrl::GetNativeTextIndent() const
{
    if ( wxUxThemeIsActive() )
        return NATIVE_TEXT_INDENT_XP;
    return NATIVE_TEXT_INDENT_CLASSIC;
}

bool wxComboCtrl::IsKeyPopupToggle(const wxKeyEvent& event) const
{
    const bool isPopupShown = IsPopupShown();

    switch ( event.GetKeyCode() )
    {
        case WXK_F4:
            // F4 toggles the popup in the native comboboxes, so emulate them
            if ( !event.AltDown() )
                return true;
            break;

        case WXK_ESCAPE:
            if ( isPopupShown )
                return true;
            break;

        case WXK_DOWN:
        case WXK_UP:
        case WXK_NUMPAD_DOWN:
        case WXK_NUMPAD_UP:
            // Arrow keys (and mouse wheel) toggle the popup in the native
            // combo boxes
            if ( event.AltDown() )
                return true;
            break;
    }

    return false;
}

#endif // wxUSE_COMBOCTRL
