
//----------------------------------------------------------------------
// Constructor and Destructor

wxStyledTextCtrl::wxStyledTextCtrl(wxWindow *parent,
                                   wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxString& name)
{
    Create(parent, id, pos, size, style, name);
}


bool wxStyledTextCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    style |= wxVSCROLL | wxHSCROLL | wxWANTS_CHARS | wxCLIP_CHILDREN;

    // We want to use a specific class name for this window in wxMSW to make it
    // possible to configure screen readers to handle it specifically.
    bool created =
#ifdef __WXMSW__
        CreateUsingMSWClass(
            wxApp::GetRegisteredClassName(wxT("Scintilla")),
            parent, id, pos, size, style, name
        );
#else
        wxControl::Create(
            parent, id, pos, size, style, wxDefaultValidator, name
        );
#endif
    if ( !created )
        return false;

    m_swx = new ScintillaWX(this);
    m_stopWatch.Start();
    m_lastKeyDownConsumed = false;
    m_vScrollBar = nullptr;
    m_hScrollBar = nullptr;
    // Put Scintilla into unicode (UTF-8) mode
    SetCodePage(wxSTC_CP_UTF8);

    SetInitialSize(size);

    // Reduces flicker on GTK+/X11
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Make sure it can take the focus
    SetCanFocus(true);

    // STC doesn't support RTL languages at all
    SetLayoutDirection(wxLayout_LeftToRight);

    // Rely on native double buffering by default, except under Mac where it
    // doesn't work for some reason, see #18085.
#if wxALWAYS_NATIVE_DOUBLE_BUFFER && !defined(__WXMAC__)
    SetBufferedDraw(false);
#else
    SetBufferedDraw(true);
#endif

#if wxUSE_GRAPHICS_DIRECT2D
    SetFontQuality(wxSTC_EFF_QUALITY_DEFAULT);
#endif

    // Use colours appropriate for the current system colour theme.
    auto attr = wxTextCtrl::GetClassDefaultAttributes();
    StyleSetForeground(wxSTC_STYLE_DEFAULT, attr.colFg);
    StyleSetBackground(wxSTC_STYLE_DEFAULT, attr.colBg);
    SetCaretForeground(attr.colFg);

    // We also need to set this one because its foreground is hardcoded as
    // black in Scintilla sources.
    StyleSetForeground(wxSTC_STYLE_LINENUMBER, attr.colFg);

    // And foreground for this one is hardcoded as white.
    StyleSetForeground(wxSTC_STYLE_CALLTIP, attr.colFg);

    SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));

    return true;
}


wxStyledTextCtrl::~wxStyledTextCtrl() {
}


//----------------------------------------------------------------------

wxIntPtr wxStyledTextCtrl::SendMsg(int msg, wxUIntPtr wp, wxIntPtr lp) const
{
    return m_swx->WndProc(msg, wp, lp);
}

