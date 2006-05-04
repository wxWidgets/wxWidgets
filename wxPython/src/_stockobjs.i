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
        wx.ITALIC_FONT  = StockGDI.instance().GetFont(StockGDI.FONT_ITALIC)
        wx.NORMAL_FONT  = StockGDI.instance().GetFont(StockGDI.FONT_NORMAL)
        wx.SMALL_FONT   = StockGDI.instance().GetFont(StockGDI.FONT_SMALL)
        wx.SWISS_FONT   = StockGDI.instance().GetFont(StockGDI.FONT_SWISS)
                                              
        wx.BLACK_DASHED_PEN  = StockGDI.GetPen(StockGDI.PEN_BLACKDASHED)
        wx.BLACK_PEN         = StockGDI.GetPen(StockGDI.PEN_BLACK)
        wx.CYAN_PEN          = StockGDI.GetPen(StockGDI.PEN_CYAN)
        wx.GREEN_PEN         = StockGDI.GetPen(StockGDI.PEN_GREEN)
        wx.GREY_PEN          = StockGDI.GetPen(StockGDI.PEN_GREY)
        wx.LIGHT_GREY_PEN    = StockGDI.GetPen(StockGDI.PEN_LIGHTGREY)
        wx.MEDIUM_GREY_PEN   = StockGDI.GetPen(StockGDI.PEN_MEDIUMGREY)
        wx.RED_PEN           = StockGDI.GetPen(StockGDI.PEN_RED)
        wx.TRANSPARENT_PEN   = StockGDI.GetPen(StockGDI.PEN_TRANSPARENT)
        wx.WHITE_PEN         = StockGDI.GetPen(StockGDI.PEN_WHITE)

        wx.BLACK_BRUSH        = StockGDI.GetBrush(StockGDI.BRUSH_BLACK)
        wx.BLUE_BRUSH         = StockGDI.GetBrush(StockGDI.BRUSH_BLUE)
        wx.CYAN_BRUSH         = StockGDI.GetBrush(StockGDI.BRUSH_CYAN)
        wx.GREEN_BRUSH        = StockGDI.GetBrush(StockGDI.BRUSH_GREEN)
        wx.GREY_BRUSH         = StockGDI.GetBrush(StockGDI.BRUSH_GREY)
        wx.LIGHT_GREY_BRUSH   = StockGDI.GetBrush(StockGDI.BRUSH_LIGHTGREY)
        wx.MEDIUM_GREY_BRUSH  = StockGDI.GetBrush(StockGDI.BRUSH_MEDIUMGREY)
        wx.RED_BRUSH          = StockGDI.GetBrush(StockGDI.BRUSH_RED)
        wx.TRANSPARENT_BRUSH  = StockGDI.GetBrush(StockGDI.BRUSH_TRANSPARENT)
        wx.WHITE_BRUSH        = StockGDI.GetBrush(StockGDI.BRUSH_WHITE)

        wx.BLACK       = StockGDI.GetColour(StockGDI.COLOUR_BLACK)
        wx.BLUE        = StockGDI.GetColour(StockGDI.COLOUR_BLUE)
        wx.CYAN        = StockGDI.GetColour(StockGDI.COLOUR_CYAN)
        wx.GREEN       = StockGDI.GetColour(StockGDI.COLOUR_GREEN)
        wx.LIGHT_GREY  = StockGDI.GetColour(StockGDI.COLOUR_LIGHTGREY)
        wx.RED         = StockGDI.GetColour(StockGDI.COLOUR_RED)
        wx.WHITE       = StockGDI.GetColour(StockGDI.COLOUR_WHITE)
    
        wx.CROSS_CURSOR      = StockGDI.GetCursor(StockGDI.CURSOR_CROSS)
        wx.HOURGLASS_CURSOR  = StockGDI.GetCursor(StockGDI.CURSOR_HOURGLASS)
        wx.STANDARD_CURSOR   = StockGDI.GetCursor(StockGDI.CURSOR_STANDARD)
        
    _initStockObjects = staticmethod(_initStockObjects)
    }
};



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
    
wxTheFontList       = _wxPyMakeDelayedInitWrapper(_wxPyInitTheFontList)()
wxThePenList        = _wxPyMakeDelayedInitWrapper(_wxPyInitThePenList)()
wxTheBrushList      = _wxPyMakeDelayedInitWrapper(_wxPyInitTheBrushList)()
wxTheColourDatabase = _wxPyMakeDelayedInitWrapper(_wxPyInitTheColourDatabase)()
}

//---------------------------------------------------------------------------
 
%pythoncode { NullColor = NullColour }




