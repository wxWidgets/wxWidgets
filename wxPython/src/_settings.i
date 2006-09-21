/////////////////////////////////////////////////////////////////////////////
// Name:        _settings.i
// Purpose:     SWIG interface defs for wxSystemSettings and wxSystemOptions
//
// Author:      Robin Dunn
//
// Created:     3-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
%}

//---------------------------------------------------------------------------
%newgroup;


enum wxSystemFont
{
    wxSYS_OEM_FIXED_FONT = 10,
    wxSYS_ANSI_FIXED_FONT,
    wxSYS_ANSI_VAR_FONT,
    wxSYS_SYSTEM_FONT,
    wxSYS_DEVICE_DEFAULT_FONT,
    wxSYS_DEFAULT_PALETTE,
    wxSYS_SYSTEM_FIXED_FONT,
    wxSYS_DEFAULT_GUI_FONT,
    wxSYS_ICONTITLE_FONT
};



enum wxSystemColour
{
    wxSYS_COLOUR_SCROLLBAR,
    wxSYS_COLOUR_BACKGROUND,
    wxSYS_COLOUR_DESKTOP = wxSYS_COLOUR_BACKGROUND,
    wxSYS_COLOUR_ACTIVECAPTION,
    wxSYS_COLOUR_INACTIVECAPTION,
    wxSYS_COLOUR_MENU,
    wxSYS_COLOUR_WINDOW,
    wxSYS_COLOUR_WINDOWFRAME,
    wxSYS_COLOUR_MENUTEXT,
    wxSYS_COLOUR_WINDOWTEXT,
    wxSYS_COLOUR_CAPTIONTEXT,
    wxSYS_COLOUR_ACTIVEBORDER,
    wxSYS_COLOUR_INACTIVEBORDER,
    wxSYS_COLOUR_APPWORKSPACE,
    wxSYS_COLOUR_HIGHLIGHT,
    wxSYS_COLOUR_HIGHLIGHTTEXT,
    wxSYS_COLOUR_BTNFACE,
    wxSYS_COLOUR_3DFACE = wxSYS_COLOUR_BTNFACE,
    wxSYS_COLOUR_BTNSHADOW,
    wxSYS_COLOUR_3DSHADOW = wxSYS_COLOUR_BTNSHADOW,
    wxSYS_COLOUR_GRAYTEXT,
    wxSYS_COLOUR_BTNTEXT,
    wxSYS_COLOUR_INACTIVECAPTIONTEXT,
    wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_BTNHILIGHT = wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_3DHIGHLIGHT = wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_3DHILIGHT = wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_3DDKSHADOW,
    wxSYS_COLOUR_3DLIGHT,
    wxSYS_COLOUR_INFOTEXT,
    wxSYS_COLOUR_INFOBK,
    wxSYS_COLOUR_LISTBOX,
    wxSYS_COLOUR_HOTLIGHT,
    wxSYS_COLOUR_GRADIENTACTIVECAPTION,
    wxSYS_COLOUR_GRADIENTINACTIVECAPTION,
    wxSYS_COLOUR_MENUHILIGHT,
    wxSYS_COLOUR_MENUBAR,

    wxSYS_COLOUR_MAX
};




enum wxSystemMetric
{
    wxSYS_MOUSE_BUTTONS = 1,
    wxSYS_BORDER_X,
    wxSYS_BORDER_Y,
    wxSYS_CURSOR_X,
    wxSYS_CURSOR_Y,
    wxSYS_DCLICK_X,
    wxSYS_DCLICK_Y,
    wxSYS_DRAG_X,
    wxSYS_DRAG_Y,
    wxSYS_EDGE_X,
    wxSYS_EDGE_Y,
    wxSYS_HSCROLL_ARROW_X,
    wxSYS_HSCROLL_ARROW_Y,
    wxSYS_HTHUMB_X,
    wxSYS_ICON_X,
    wxSYS_ICON_Y,
    wxSYS_ICONSPACING_X,
    wxSYS_ICONSPACING_Y,
    wxSYS_WINDOWMIN_X,
    wxSYS_WINDOWMIN_Y,
    wxSYS_SCREEN_X,
    wxSYS_SCREEN_Y,
    wxSYS_FRAMESIZE_X,
    wxSYS_FRAMESIZE_Y,
    wxSYS_SMALLICON_X,
    wxSYS_SMALLICON_Y,
    wxSYS_HSCROLL_Y,
    wxSYS_VSCROLL_X,
    wxSYS_VSCROLL_ARROW_X,
    wxSYS_VSCROLL_ARROW_Y,
    wxSYS_VTHUMB_Y,
    wxSYS_CAPTION_Y,
    wxSYS_MENU_Y,
    wxSYS_NETWORK_PRESENT,
    wxSYS_PENWINDOWS_PRESENT,
    wxSYS_SHOW_SOUNDS,
    wxSYS_SWAP_BUTTONS
};



enum wxSystemFeature
{
    wxSYS_CAN_DRAW_FRAME_DECORATIONS = 1,
    wxSYS_CAN_ICONIZE_FRAME,
    wxSYS_TABLET_PRESENT 
};



enum wxSystemScreenType
{
    wxSYS_SCREEN_NONE = 0,  //   not yet defined

    wxSYS_SCREEN_TINY,      //   <
    wxSYS_SCREEN_PDA,       //   >= 320x240
    wxSYS_SCREEN_SMALL,     //   >= 640x480
    wxSYS_SCREEN_DESKTOP    //   >= 800x600
};



//---------------------------------------------------------------------------

MustHaveApp(wxSystemSettings::GetColour);
MustHaveApp(wxSystemSettings::GetFont);
MustHaveApp(wxSystemSettings::GetMetric);
MustHaveApp(wxSystemSettings::HasFeature);
MustHaveApp(wxSystemSettings::GetScreenType);
MustHaveApp(wxSystemSettings::SetScreenType);

class wxSystemSettings
{
public:
    // get a standard system colour
    static wxColour GetColour(wxSystemColour index);

    // get a standard system font
    static wxFont GetFont(wxSystemFont index);

    // get a system-dependent metric
    static int GetMetric(wxSystemMetric index, wxWindow* win=NULL);

    // return True if the port has certain feature
    static bool HasFeature(wxSystemFeature index);


    // Get system screen design (desktop, pda, ..) used for
    // laying out various dialogs.
    static wxSystemScreenType GetScreenType();

    // Override default.
    static void SetScreenType( wxSystemScreenType screen );

};


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(WINDOW_DEFAULT_VARIANT);

class wxSystemOptions : public wxObject
{
public:
    wxSystemOptions();

    // User-customizable hints to wxWindows or associated libraries
    // These could also be used to influence GetSystem... calls, indeed
    // to implement SetSystemColour/Font/Metric

    static void SetOption(const wxString& name, const wxString& value);
    %Rename(SetOptionInt,  static void, SetOption(const wxString& name, int value));
    static wxString GetOption(const wxString& name) ;
    static int GetOptionInt(const wxString& name) ;
    static bool HasOption(const wxString& name) ;
    static bool IsFalse(const wxString& name);
};



%pythoncode {
    %# Until the new native control for wxMac is up to par, still use the generic one.
    SystemOptions.SetOptionInt("mac.listctrl.always_use_generic", 1)
}


//---------------------------------------------------------------------------
