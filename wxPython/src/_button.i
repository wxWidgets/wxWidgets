/////////////////////////////////////////////////////////////////////////////
// Name:        _button.i
// Purpose:     SWIG interface defs for wxButton, wxBitmapButton
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup;

MAKE_CONST_WXSTRING(ButtonNameStr);

enum {
    wxBU_LEFT,
    wxBU_TOP,
    wxBU_RIGHT,
    wxBU_BOTTOM,

    wxBU_EXACTFIT,
    wxBU_AUTODRAW,
};

// enum wxStockItemID
// {
//     wxSTOCK_NONE = 0,
//     wxSTOCK_ADD,
//     wxSTOCK_APPLY,
//     wxSTOCK_BOLD,
//     wxSTOCK_CANCEL,
//     wxSTOCK_CLEAR,
//     wxSTOCK_CLOSE,
//     wxSTOCK_COPY,
//     wxSTOCK_CUT,
//     wxSTOCK_DELETE,
//     wxSTOCK_FIND,
//     wxSTOCK_FIND_AND_REPLACE,
//     wxSTOCK_GO_BACK,
//     wxSTOCK_GO_DOWN,
//     wxSTOCK_GO_FORWARD,
//     wxSTOCK_GO_UP,
//     wxSTOCK_HELP,
//     wxSTOCK_HOME,
//     wxSTOCK_INDENT,
//     wxSTOCK_INDEX,
//     wxSTOCK_ITALIC,
//     wxSTOCK_JUSTIFY_CENTER,
//     wxSTOCK_JUSTIFY_FILL,
//     wxSTOCK_JUSTIFY_LEFT,
//     wxSTOCK_JUSTIFY_RIGHT,
//     wxSTOCK_NEW,
//     wxSTOCK_NO,
//     wxSTOCK_OK,
//     wxSTOCK_OPEN,
//     wxSTOCK_PASTE,
//     wxSTOCK_PREFERENCES,
//     wxSTOCK_PRINT,
//     wxSTOCK_PRINT_PREVIEW,
//     wxSTOCK_PROPERTIES,
//     wxSTOCK_QUIT,
//     wxSTOCK_REDO,
//     wxSTOCK_REFRESH,
//     wxSTOCK_REMOVE,
//     wxSTOCK_REVERT_TO_SAVED,
//     wxSTOCK_SAVE,
//     wxSTOCK_SAVE_AS,
//     wxSTOCK_STOP,
//     wxSTOCK_UNDELETE,
//     wxSTOCK_UNDERLINE,
//     wxSTOCK_UNDO,
//     wxSTOCK_UNINDENT,
//     wxSTOCK_YES,
//     wxSTOCK_ZOOM_100,
//     wxSTOCK_ZOOM_FIT,
//     wxSTOCK_ZOOM_IN,
//     wxSTOCK_ZOOM_OUT
// };

//---------------------------------------------------------------------------

DocStr(wxButton,
"A button is a control that contains a text string, and is one of the most
common elements of a GUI.  It may be placed on a dialog box or panel, or
indeed almost any other window.", "

Window Styles
-------------
    ==============   ==========================================
    wx.BU_LEFT       Left-justifies the label. Windows and GTK+ only.
    wx.BU_TOP        Aligns the label to the top of the button.
                     Windows and GTK+ only.
    wx.BU_RIGHT      Right-justifies the bitmap label. Windows and GTK+ only.
    wx.BU_BOTTOM     Aligns the label to the bottom of the button.
                     Windows and GTK+ only.
    wx.BU_EXACTFIT   Creates the button as small as possible
                     instead of making it of the standard size
                     (which is the default behaviour.)
    ==============   ==========================================

Events
------
    ============     ==========================================
    EVT_BUTTON       Sent when the button is clicked.
    ============     ==========================================

:see: `wx.BitmapButton`
");
//, `wx.StockButton`


MustHaveApp(wxButton);

class wxButton : public wxControl
{
public:
    %pythonAppend wxButton         "self._setOORInfo(self)"
    %pythonAppend wxButton()       ""
    %typemap(out) wxButton*;    // turn off this typemap


    DocCtorStr(
        wxButton(wxWindow* parent, wxWindowID id=-1,
                 const wxString& label=wxPyEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxPyButtonNameStr),
        "Create and show a button.", "");

    DocCtorStrName(
        wxButton(),
        "Precreate a Button for 2-phase creation.", "",
        PreButton);

//     DocCtorStrName(
//         wxButton(wxWindow *parent, wxWindowID id,
//                  wxStockItemID stock,
//                  const wxString& descriptiveLabel = wxPyEmptyString,
//                  const wxPoint& pos = wxDefaultPosition,
//                  long style = 0,
//                  const wxValidator& validator = wxDefaultValidator,
//                  const wxString& name = wxButtonNameStr),
//         "Used to create a stock `wx.Button`.  Stock buttons are commonly used
// buttons such as OK or Cancel. They have standard label and dimensions
// and may have different appearance on some platforms (e.g. GTK+ 2
// decorates them with icons). Using this from is the preferred way of
// creating standard buttons.", "

//     :param stock: The stock ID of the button to create. One of the following:

//             ============================
//              ``wx.STOCK_NONE``
//              ``wx.STOCK_ADD``
//              ``wx.STOCK_APPLY``
//              ``wx.STOCK_BOLD``
//              ``wx.STOCK_CANCEL``
//              ``wx.STOCK_CLEAR``
//              ``wx.STOCK_CLOSE``
//              ``wx.STOCK_COPY``
//              ``wx.STOCK_CUT``
//              ``wx.STOCK_DELETE``
//              ``wx.STOCK_FIND``
//              ``wx.STOCK_FIND_AND_REPLACE``
//              ``wx.STOCK_GO_BACK``
//              ``wx.STOCK_GO_DOWN``
//              ``wx.STOCK_GO_FORWARD``
//              ``wx.STOCK_GO_UP``
//              ``wx.STOCK_HELP``
//              ``wx.STOCK_HOME``
//              ``wx.STOCK_INDENT``
//              ``wx.STOCK_INDEX``
//              ``wx.STOCK_ITALIC``
//              ``wx.STOCK_JUSTIFY_CENTER``
//              ``wx.STOCK_JUSTIFY_FILL``
//              ``wx.STOCK_JUSTIFY_LEFT``
//              ``wx.STOCK_JUSTIFY_RIGHT``
//              ``wx.STOCK_NEW``
//              ``wx.STOCK_NO``
//              ``wx.STOCK_OK``
//              ``wx.STOCK_OPEN``
//              ``wx.STOCK_PASTE``
//              ``wx.STOCK_PREFERENCES``
//              ``wx.STOCK_PRINT``
//              ``wx.STOCK_PRINT_PREVIEW``
//              ``wx.STOCK_PROPERTIES``
//              ``wx.STOCK_QUIT``
//              ``wx.STOCK_REDO``
//              ``wx.STOCK_REFRESH``
//              ``wx.STOCK_REMOVE``
//              ``wx.STOCK_REVERT_TO_SAVED``
//              ``wx.STOCK_SAVE``
//              ``wx.STOCK_SAVE_AS``
//              ``wx.STOCK_STOP``
//              ``wx.STOCK_UNDELETE``
//              ``wx.STOCK_UNDERLINE``
//              ``wx.STOCK_UNDO``
//              ``wx.STOCK_UNINDENT``
//              ``wx.STOCK_YES``
//              ``wx.STOCK_ZOOM_100``
//              ``wx.STOCK_ZOOM_FIT``
//              ``wx.STOCK_ZOOM_IN``
//              ``wx.STOCK_ZOOM_OUT``
//             ============================


//     :param descriptiveLabel: Optional label to be used on platforms
// where standard buttons have descriptive rather than generic
// labels. Mac is one such platforms, well-behaved Mac apps should use
// descriptive labels (e.g. \"Save\" and \"Don't Save\" instead of \"OK\"
// and \"Cancel\"). This argument is ignored on other platforms.",
//         StockButton);

    
    // Turn it back on again
    %typemap(out) wxButton* { $result = wxPyMake_wxObject($1, $owner); }


    DocDeclStr(
        bool , Create(wxWindow* parent, wxWindowID id=-1,
                      const wxString& label=wxPyEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxPyButtonNameStr),
        "Acutally create the GUI Button for 2-phase creation.", "");
    


    DocDeclStr(
        void , SetDefault(),
        "This sets the button to be the default item for the panel or dialog box.", "");
    

    DocDeclStr(
        static wxSize , GetDefaultSize(),
        "Returns the default button size for this platform.", "");   

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};



//---------------------------------------------------------------------------


DocStr(wxBitmapButton,
"A Button that contains a bitmap.  A bitmap button can be supplied with a
single bitmap, and wxWidgets will draw all button states using this bitmap. If
the application needs more control, additional bitmaps for the selected state,
unpressed focused state, and greyed-out state may be supplied.", "       

Window Styles
-------------
    ==============  =============================================
    wx.BU_AUTODRAW  If this is specified, the button will be drawn
                    automatically using the label bitmap only,
                    providing a 3D-look border. If this style is
                    not specified, the button will be drawn
                    without borders and using all provided
                    bitmaps. WIN32 only.
    wx.BU_LEFT      Left-justifies the label. WIN32 only.
    wx.BU_TOP       Aligns the label to the top of the button. WIN32
                    only.
    wx.BU_RIGHT     Right-justifies the bitmap label. WIN32 only.
    wx.BU_BOTTOM    Aligns the label to the bottom of the
                    button. WIN32 only.
    wx.BU_EXACTFIT  Creates the button as small as possible
                    instead of making it of the standard size
                    (which is the default behaviour.)
    ==============  =============================================

Events
------
     ===========   ==================================
     EVT_BUTTON    Sent when the button is clicked.
     ===========   ==================================

:see: `wx.Button`, `wx.Bitmap`
");

MustHaveApp(wxBitmapButton);

class wxBitmapButton : public wxButton
{
public:
    %pythonAppend wxBitmapButton         "self._setOORInfo(self)"
    %pythonAppend wxBitmapButton()       ""
    %typemap(out) wxBitmapButton*;    // turn off this typemap

    DocCtorStr(
        wxBitmapButton(wxWindow* parent, wxWindowID id=-1,
                       const wxBitmap& bitmap = wxNullBitmap,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxBU_AUTODRAW,
                       const wxValidator& validator = wxDefaultValidator,
                       const wxString& name = wxPyButtonNameStr),
        "Create and show a button with a bitmap for the label.", "");

    DocCtorStrName(
        wxBitmapButton(),
        "Precreate a BitmapButton for 2-phase creation.", "",
        PreBitmapButton);

    // Turn it back on again
    %typemap(out) wxBitmapButton* { $result = wxPyMake_wxObject($1, $owner); }


    DocDeclStr(
        bool , Create(wxWindow* parent, wxWindowID id=-1,
                      const wxBitmap& bitmap = wxNullBitmap,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxBU_AUTODRAW,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxPyButtonNameStr),
        "Acutally create the GUI BitmapButton for 2-phase creation.", "");
    

    DocDeclStr(
        wxBitmap , GetBitmapLabel(),
        "Returns the label bitmap (the one passed to the constructor).", "");
    
    DocDeclStr(
        wxBitmap , GetBitmapDisabled(),
        "Returns the bitmap for the disabled state.", "");
    
    DocDeclStr(
        wxBitmap , GetBitmapFocus(),
        "Returns the bitmap for the focused state.", "");
    

    DocDeclStr(
        wxBitmap , GetBitmapSelected(),
        "Returns the bitmap for the selected state.", "");
    

    DocDeclStr(
        void , SetBitmapDisabled(const wxBitmap& bitmap),
        "Sets the bitmap for the disabled button appearance.", "");
    

    DocDeclStr(
        void , SetBitmapFocus(const wxBitmap& bitmap),
        "Sets the bitmap for the button appearance when it has the keyboard focus.", "");
    

    DocDeclStr(
        void , SetBitmapSelected(const wxBitmap& bitmap),
        "Sets the bitmap for the selected (depressed) button appearance.", "");
    

    DocDeclStr(
        void , SetBitmapLabel(const wxBitmap& bitmap),
        "Sets the bitmap label for the button.  This is the bitmap used for the
unselected state, and for all other states if no other bitmaps are provided.", "");
    

    void SetMargins(int x, int y);
    int GetMarginX() const;
    int GetMarginY() const;
};


//---------------------------------------------------------------------------
