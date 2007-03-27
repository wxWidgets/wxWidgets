/////////////////////////////////////////////////////////////////////////////
// Name:        _stockobjs.i
// Purpose:     SWIG interface defining "stock" GDI objects
//
// Author:      Robin Dunn
//
// Created:     13-Sept-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup


class wxStockGDI
{
public:
    enum Item {
        BRUSH_BLACK,
        BRUSH_BLUE,
        BRUSH_CYAN,
        BRUSH_GREEN,
        BRUSH_GREY,
        BRUSH_LIGHTGREY,
        BRUSH_MEDIUMGREY,
        BRUSH_RED,
        BRUSH_TRANSPARENT,
        BRUSH_WHITE,
        COLOUR_BLACK,
        COLOUR_BLUE,
        COLOUR_CYAN,
        COLOUR_GREEN,
        COLOUR_LIGHTGREY,
        COLOUR_RED,
        COLOUR_WHITE,
        CURSOR_CROSS,
        CURSOR_HOURGLASS,
        CURSOR_STANDARD,
        FONT_ITALIC,
        FONT_NORMAL,
        FONT_SMALL,
        FONT_SWISS,
        PEN_BLACK,
        PEN_BLACKDASHED,
        PEN_CYAN,
        PEN_GREEN,
        PEN_GREY,
        PEN_LIGHTGREY,
        PEN_MEDIUMGREY,
        PEN_RED,
        PEN_TRANSPARENT,
        PEN_WHITE,
        ITEMCOUNT
    };

    wxStockGDI();
    virtual ~wxStockGDI();
    static void DeleteAll();

    static wxStockGDI& instance();

    static const wxBrush* GetBrush(Item item);
    static const wxColour* GetColour(Item item);
    static const wxCursor* GetCursor(Item item);
    static const wxPen* GetPen(Item item);

    virtual const wxFont* GetFont(Item item);

    %pythoncode {
    def _initStockObjects():
        import wx
        wx.ITALIC_FONT.this  = StockGDI.instance().GetFont(StockGDI.FONT_ITALIC).this
        wx.NORMAL_FONT.this  = StockGDI.instance().GetFont(StockGDI.FONT_NORMAL).this
        wx.SMALL_FONT.this   = StockGDI.instance().GetFont(StockGDI.FONT_SMALL).this
        wx.SWISS_FONT.this   = StockGDI.instance().GetFont(StockGDI.FONT_SWISS).this
                                              
        wx.BLACK_DASHED_PEN.this  = StockGDI.GetPen(StockGDI.PEN_BLACKDASHED).this
        wx.BLACK_PEN.this         = StockGDI.GetPen(StockGDI.PEN_BLACK).this
        wx.CYAN_PEN.this          = StockGDI.GetPen(StockGDI.PEN_CYAN).this
        wx.GREEN_PEN.this         = StockGDI.GetPen(StockGDI.PEN_GREEN).this
        wx.GREY_PEN.this          = StockGDI.GetPen(StockGDI.PEN_GREY).this
        wx.LIGHT_GREY_PEN.this    = StockGDI.GetPen(StockGDI.PEN_LIGHTGREY).this
        wx.MEDIUM_GREY_PEN.this   = StockGDI.GetPen(StockGDI.PEN_MEDIUMGREY).this
        wx.RED_PEN.this           = StockGDI.GetPen(StockGDI.PEN_RED).this
        wx.TRANSPARENT_PEN.this   = StockGDI.GetPen(StockGDI.PEN_TRANSPARENT).this
        wx.WHITE_PEN.this         = StockGDI.GetPen(StockGDI.PEN_WHITE).this

        wx.BLACK_BRUSH.this        = StockGDI.GetBrush(StockGDI.BRUSH_BLACK).this
        wx.BLUE_BRUSH.this         = StockGDI.GetBrush(StockGDI.BRUSH_BLUE).this
        wx.CYAN_BRUSH.this         = StockGDI.GetBrush(StockGDI.BRUSH_CYAN).this
        wx.GREEN_BRUSH.this        = StockGDI.GetBrush(StockGDI.BRUSH_GREEN).this
        wx.GREY_BRUSH.this         = StockGDI.GetBrush(StockGDI.BRUSH_GREY).this
        wx.LIGHT_GREY_BRUSH.this   = StockGDI.GetBrush(StockGDI.BRUSH_LIGHTGREY).this
        wx.MEDIUM_GREY_BRUSH.this  = StockGDI.GetBrush(StockGDI.BRUSH_MEDIUMGREY).this
        wx.RED_BRUSH.this          = StockGDI.GetBrush(StockGDI.BRUSH_RED).this
        wx.TRANSPARENT_BRUSH.this  = StockGDI.GetBrush(StockGDI.BRUSH_TRANSPARENT).this
        wx.WHITE_BRUSH.this        = StockGDI.GetBrush(StockGDI.BRUSH_WHITE).this

        wx.BLACK.this       = StockGDI.GetColour(StockGDI.COLOUR_BLACK).this
        wx.BLUE.this        = StockGDI.GetColour(StockGDI.COLOUR_BLUE).this
        wx.CYAN.this        = StockGDI.GetColour(StockGDI.COLOUR_CYAN).this
        wx.GREEN.this       = StockGDI.GetColour(StockGDI.COLOUR_GREEN).this
        wx.LIGHT_GREY.this  = StockGDI.GetColour(StockGDI.COLOUR_LIGHTGREY).this
        wx.RED.this         = StockGDI.GetColour(StockGDI.COLOUR_RED).this
        wx.WHITE.this       = StockGDI.GetColour(StockGDI.COLOUR_WHITE).this
    
        wx.CROSS_CURSOR.this      = StockGDI.GetCursor(StockGDI.CURSOR_CROSS).this
        wx.HOURGLASS_CURSOR.this  = StockGDI.GetCursor(StockGDI.CURSOR_HOURGLASS).this
        wx.STANDARD_CURSOR.this   = StockGDI.GetCursor(StockGDI.CURSOR_STANDARD).this

        wx.TheFontList.this       = _wxPyInitTheFontList().this
        wx.ThePenList.this        = _wxPyInitThePenList().this
        wx.TheBrushList.this      = _wxPyInitTheBrushList().this
        wx.TheColourDatabase.this = _wxPyInitTheColourDatabase().this

        
    _initStockObjects = staticmethod(_initStockObjects)
    }
};


%pythoncode {
%# Create an uninitialized instance for the stock objects, they will
%# be initialized later when the wx.App object is created.
ITALIC_FONT  = Font.__new__(Font)
NORMAL_FONT  = Font.__new__(Font)
SMALL_FONT   = Font.__new__(Font)
SWISS_FONT   = Font.__new__(Font)
                                   
BLACK_DASHED_PEN  = Pen.__new__(Pen)
BLACK_PEN         = Pen.__new__(Pen)
CYAN_PEN          = Pen.__new__(Pen)
GREEN_PEN         = Pen.__new__(Pen)
GREY_PEN          = Pen.__new__(Pen)
LIGHT_GREY_PEN    = Pen.__new__(Pen)
MEDIUM_GREY_PEN   = Pen.__new__(Pen)
RED_PEN           = Pen.__new__(Pen)
TRANSPARENT_PEN   = Pen.__new__(Pen)
WHITE_PEN         = Pen.__new__(Pen)

BLACK_BRUSH        = Brush.__new__(Brush)
BLUE_BRUSH         = Brush.__new__(Brush)
CYAN_BRUSH         = Brush.__new__(Brush)
GREEN_BRUSH        = Brush.__new__(Brush)
GREY_BRUSH         = Brush.__new__(Brush)
LIGHT_GREY_BRUSH   = Brush.__new__(Brush)
MEDIUM_GREY_BRUSH  = Brush.__new__(Brush)
RED_BRUSH          = Brush.__new__(Brush)
TRANSPARENT_BRUSH  = Brush.__new__(Brush)
WHITE_BRUSH        = Brush.__new__(Brush)

BLACK       = Colour.__new__(Colour)
BLUE        = Colour.__new__(Colour)
CYAN        = Colour.__new__(Colour)
GREEN       = Colour.__new__(Colour)
LIGHT_GREY  = Colour.__new__(Colour)
RED         = Colour.__new__(Colour)
WHITE       = Colour.__new__(Colour)

CROSS_CURSOR      = Cursor.__new__(Cursor)
HOURGLASS_CURSOR  = Cursor.__new__(Cursor)
STANDARD_CURSOR   = Cursor.__new__(Cursor)
}                                        


%immutable;
%threadWrapperOff;

const wxBitmap wxNullBitmap;
const wxIcon   wxNullIcon;
const wxCursor wxNullCursor;
const wxPen    wxNullPen;
const wxBrush  wxNullBrush;
const wxPalette wxNullPalette;
const wxFont   wxNullFont;
const wxColour wxNullColour;

%threadWrapperOn;
%mutable;


//---------------------------------------------------------------------------


class wxGDIObjListBase {
public:
    wxGDIObjListBase();
    ~wxGDIObjListBase();
};


class wxPenList : public wxGDIObjListBase {
public:

    wxPen* FindOrCreatePen(const wxColour& colour, int width, int style);
};


class wxBrushList : public wxGDIObjListBase {
public:

    wxBrush * FindOrCreateBrush(const wxColour& colour, int style=wxSOLID);
};


class wxFontList : public wxGDIObjListBase {
public:

    wxFont * FindOrCreateFont(int point_size, int family, int style, int weight,
                              bool underline = false,
                              const wxString& facename = wxPyEmptyString,
                              wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
};

//---------------------------------------------------------------------------

MustHaveApp(wxColourDatabase);

class wxColourDatabase {
public:
    wxColourDatabase();
    ~wxColourDatabase();

    // find colour by name or name for the given colour
    wxColour Find(const wxString& name) const;
    wxString FindName(const wxColour& colour) const;
    %pythoncode { FindColour = Find }
    
    // add a new colour to the database
    void AddColour(const wxString& name, const wxColour& colour);

    %extend {
        void Append(const wxString& name, int red, int green, int blue) {
            self->AddColour(name, wxColour(red, green, blue));
        }
    }
};


//---------------------------------------------------------------------------
%newgroup

%inline {
    wxFontList* _wxPyInitTheFontList() { return wxTheFontList; }
    wxPenList* _wxPyInitThePenList() { return wxThePenList; }
    wxBrushList* _wxPyInitTheBrushList() { return wxTheBrushList; }
    wxColourDatabase* _wxPyInitTheColourDatabase() { return wxTheColourDatabase; }
}

%pythoncode {
    
%# Create an uninitialized instance for the stock objects, they will
%# be initialized later when the wx.App object is created.
TheFontList       = FontList.__new__(FontList)
ThePenList        = PenList.__new__(PenList)
TheBrushList      = BrushList.__new__(BrushList)
TheColourDatabase = ColourDatabase.__new__(ColourDatabase)
}


//---------------------------------------------------------------------------
 
%pythoncode { NullColor = NullColour }




