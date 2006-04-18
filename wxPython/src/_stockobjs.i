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
};


%pythoncode {
%# This function makes a class used to do delayed initialization of some
%# stock wx objects.  When they are used the first time then an init function
%# is called to make the real instance, which is then used to replace the
%# original instance and class seen by the programmer.
def _wxPyMakeDelayedInitWrapper(initFunc):
    class _wxPyStockObjectWrapper(object):
        def __init__(self, *args):
            self._args = args
        def __getattr__(self, name):
            obj = initFunc(*self._args)
            self.__class__ = obj.__class__
            self.__dict__ = obj.__dict__
            return getattr(self, name)
        def __str__(self):
            return self.__getattr__("__str__")()
        def __repr__(self):
            return self.__getattr__("__repr__")()
    return _wxPyStockObjectWrapper
    
def _wxPyFontInit(id):
    return StockGDI.instance().GetFont(id)
                                              
_wxPyStockPen    = _wxPyMakeDelayedInitWrapper(StockGDI.GetPen)
_wxPyStockBrush  = _wxPyMakeDelayedInitWrapper(StockGDI.GetBrush)
_wxPyStockCursor = _wxPyMakeDelayedInitWrapper(StockGDI.GetCursor)
_wxPyStockColour = _wxPyMakeDelayedInitWrapper(StockGDI.GetColour)                    
_wxPyStockFont   = _wxPyMakeDelayedInitWrapper(_wxPyFontInit)


ITALIC_FONT  = _wxPyStockCursor(StockGDI.FONT_ITALIC)
NORMAL_FONT  = _wxPyStockCursor(StockGDI.FONT_NORMAL)
SMALL_FONT   = _wxPyStockCursor(StockGDI.FONT_SMALL)
SWISS_FONT   = _wxPyStockCursor(StockGDI.FONT_SWISS)
                                              
BLACK_DASHED_PEN  = _wxPyStockPen(StockGDI.PEN_BLACKDASHED)
BLACK_PEN         = _wxPyStockPen(StockGDI.PEN_BLACK)
CYAN_PEN          = _wxPyStockPen(StockGDI.PEN_CYAN)
GREEN_PEN         = _wxPyStockPen(StockGDI.PEN_GREEN)
GREY_PEN          = _wxPyStockPen(StockGDI.PEN_GREY)
LIGHT_GREY_PEN    = _wxPyStockPen(StockGDI.PEN_LIGHTGREY)
MEDIUM_GREY_PEN   = _wxPyStockPen(StockGDI.PEN_MEDIUMGREY)
RED_PEN           = _wxPyStockPen(StockGDI.PEN_RED)
TRANSPARENT_PEN   = _wxPyStockPen(StockGDI.PEN_TRANSPARENT)
WHITE_PEN         = _wxPyStockPen(StockGDI.PEN_WHITE)

BLACK_BRUSH        = _wxPyStockBrush(StockGDI.BRUSH_BLACK)
BLUE_BRUSH         = _wxPyStockBrush(StockGDI.BRUSH_BLUE)
CYAN_BRUSH         = _wxPyStockBrush(StockGDI.BRUSH_CYAN)
GREEN_BRUSH        = _wxPyStockBrush(StockGDI.BRUSH_GREEN)
GREY_BRUSH         = _wxPyStockBrush(StockGDI.BRUSH_GREY)
LIGHT_GREY_BRUSH   = _wxPyStockBrush(StockGDI.BRUSH_LIGHTGREY)
MEDIUM_GREY_BRUSH  = _wxPyStockBrush(StockGDI.BRUSH_MEDIUMGREY)
RED_BRUSH          = _wxPyStockBrush(StockGDI.BRUSH_RED)
TRANSPARENT_BRUSH  = _wxPyStockBrush(StockGDI.BRUSH_TRANSPARENT)
WHITE_BRUSH        = _wxPyStockBrush(StockGDI.BRUSH_WHITE)

BLACK       = _wxPyStockColour(StockGDI.COLOUR_BLACK)
BLUE        = _wxPyStockColour(StockGDI.COLOUR_BLUE)
CYAN        = _wxPyStockColour(StockGDI.COLOUR_CYAN)
GREEN       = _wxPyStockColour(StockGDI.COLOUR_GREEN)
LIGHT_GREY  = _wxPyStockColour(StockGDI.COLOUR_LIGHTGREY)
RED         = _wxPyStockColour(StockGDI.COLOUR_RED)
WHITE       = _wxPyStockColour(StockGDI.COLOUR_WHITE)
    
CROSS_CURSOR      = _wxPyStockCursor(StockGDI.CURSOR_CROSS)
HOURGLASS_CURSOR  = _wxPyStockCursor(StockGDI.CURSOR_HOURGLASS)
STANDARD_CURSOR   = _wxPyStockCursor(StockGDI.CURSOR_STANDARD)
    
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


// %inline {
//     const wxBitmap& _wxPyInitNullBitmap() { return wxNullBitmap; }
//     const wxIcon&   _wxPyInitNullIcon() { return wxNullIcon; }
//     const wxCursor& _wxPyInitNullCursor() { return wxNullCursor; }
//     const wxPen&    _wxPyInitNullPen() { return wxNullPen; }
//     const wxBrush&  _wxPyInitNullBrush() { return wxNullBrush; }
//     const wxPalette& _wxPyInitNullPalette() { return wxNullPalette; }
//     const wxFont&   _wxPyInitNullFont() { return wxNullFont; }
//     const wxColour& _wxPyInitNullColour() { return wxNullColour; }
// }

// %pythoncode {
// NullBitmap   = _wxPyMakeDelayedInitWrapper(_wxPyInitNullBitmap)()
// NullIcon     = _wxPyMakeDelayedInitWrapper(_wxPyInitNullIcon)()
// NullCursor   = _wxPyMakeDelayedInitWrapper(_wxPyInitNullCursor)()
// NullPen      = _wxPyMakeDelayedInitWrapper(_wxPyInitNullPen)()
// NullBrush    = _wxPyMakeDelayedInitWrapper(_wxPyInitNullBrush)()
// NullPalette  = _wxPyMakeDelayedInitWrapper(_wxPyInitNullPalette)()
// NullFont     = _wxPyMakeDelayedInitWrapper(_wxPyInitNullFont)()
// NullColour   = _wxPyMakeDelayedInitWrapper(_wxPyInitNullColour)()   
// }


//---------------------------------------------------------------------------


class wxGDIObjListBase {
public:
    wxGDIObjListBase();
    ~wxGDIObjListBase();
};


class wxPenList : public wxGDIObjListBase {
public:

    wxPen* FindOrCreatePen(const wxColour& colour, int width, int style);

    void AddPen(wxPen* pen);
    void RemovePen(wxPen* pen);
    %pythoncode {
        AddPen = wx._deprecated(AddPen)
        RemovePen = wx._deprecated(RemovePen)
    }            
//    int GetCount();
};


class wxBrushList : public wxGDIObjListBase {
public:

    wxBrush * FindOrCreateBrush(const wxColour& colour, int style=wxSOLID);

    void AddBrush(wxBrush *brush);
    void RemoveBrush(wxBrush *brush);
    %pythoncode {
        AddBrush = wx._deprecated(AddBrush)
        RemoveBrush = wx._deprecated(RemoveBrush)
    }
//    int GetCount();
};


class wxFontList : public wxGDIObjListBase {
public:

    wxFont * FindOrCreateFont(int point_size, int family, int style, int weight,
                              bool underline = false,
                              const wxString& facename = wxPyEmptyString,
                              wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    void AddFont(wxFont* font);
    void RemoveFont(wxFont *font);
    %pythoncode {
        AddFont = wx._deprecated(AddFont)
        RemoveFont = wx._deprecated(RemoveFont)
    }

//    int GetCount();
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
wxTheFontList       = _wxPyMakeDelayedInitWrapper(_wxPyInitTheFontList)()
wxThePenList        = _wxPyMakeDelayedInitWrapper(_wxPyInitThePenList)()
wxTheBrushList      = _wxPyMakeDelayedInitWrapper(_wxPyInitTheBrushList)()
wxTheColourDatabase = _wxPyMakeDelayedInitWrapper(_wxPyInitTheColourDatabase)()
}

//---------------------------------------------------------------------------
 
%pythoncode { NullColor = NullColour }




