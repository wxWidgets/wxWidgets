/////////////////////////////////////////////////////////////////////////////
// Name:        _sizers.i
// Purpose:     SWIG interface defs for the Sizers
//
// Author:      Robin Dunn
//
// Created:     18-Sept-1999
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

DocStr(wxSizerItem,
"The wx.SizerItem class is used to track the position, size and other
attributes of each item managed by a `wx.Sizer`. It is not usually
necessary to use this class because the sizer elements can also be
identified by their positions or window or sizer references but
sometimes it may be more convenient to use wx.SizerItem directly.
Also, custom classes derived from `wx.PySizer` will probably need to
use the collection of wx.SizerItems held by wx.Sizer when calculating
layout.

:see: `wx.Sizer`, `wx.GBSizerItem`", "");

class wxSizerItem : public wxObject {
public:
    DocCtorStr(
        wxSizerItem(),
        "Constructs an empty wx.SizerItem.  Either a window, sizer or spacer
size will need to be set before this item can be used in a Sizer.

You will probably never need to create a wx.SizerItem directly as they
are created automatically when the sizer's Add, Insert or Prepend
methods are called.

:see: `wx.SizerItemSpacer`, `wx.SizerItemWindow`, `wx.SizerItemSizer`", "");

    
    ~wxSizerItem();


    %extend {
        DocStr(
            wxSizerItem( wxWindow *window, int proportion, int flag,
                         int border, PyObject* userData=NULL ),
            "Constructs a `wx.SizerItem` for tracking a window.", "");

        %RenameCtor(SizerItemWindow, wxSizerItem( wxWindow *window, int proportion, int flag,
                                                  int border, PyObject* userData=NULL ))
        {
            wxPyUserData* data = NULL;
            if ( userData ) {
                wxPyBlock_t blocked = wxPyBeginBlockThreads();
                data = new wxPyUserData(userData);
                wxPyEndBlockThreads(blocked);
            }
            return new wxSizerItem(window, proportion, flag, border, data);
        }


        DocStr(
            wxSizerItem( int width, int height, int proportion, int flag,
                         int border, PyObject* userData=NULL),
            "Constructs a `wx.SizerItem` for tracking a spacer.", "");

        %RenameCtor(SizerItemSpacer,  wxSizerItem( int width, int height, int proportion, int flag,
                                                   int border, PyObject* userData=NULL))
        {
            wxPyUserData* data = NULL;
            if ( userData ) {
                wxPyBlock_t blocked = wxPyBeginBlockThreads();
                data = new wxPyUserData(userData);
                wxPyEndBlockThreads(blocked);
            }
            return new wxSizerItem(width, height, proportion, flag, border, data);
        }

        DocStr(
            wxSizerItem( wxSizer *sizer, int proportion, int flag,
                         int border, PyObject* userData=NULL ),
            "Constructs a `wx.SizerItem` for tracking a subsizer", "");

        %disownarg( wxSizer *sizer );
        %RenameCtor(SizerItemSizer,  wxSizerItem( wxSizer *sizer, int proportion, int flag,
                                                  int border, PyObject* userData=NULL ))
        {
            wxPyUserData* data = NULL;
            if ( userData ) {
                wxPyBlock_t blocked = wxPyBeginBlockThreads();
                data = new wxPyUserData(userData);
                wxPyEndBlockThreads(blocked);
            }
            return new wxSizerItem(sizer, proportion, flag, border, data);
        }
        %cleardisown( wxSizer *sizer );
    }



    DocDeclStr(
        void , DeleteWindows(),
        "Destroy the window or the windows in a subsizer, depending on the type
of item.", "");

    DocDeclStr(
        void , DetachSizer(),
        "Enable deleting the SizerItem without destroying the contained sizer.", "");


    DocDeclStr(
        wxSize , GetSize(),
        "Get the current size of the item, as set in the last Layout.", "");

    DocDeclStr(
        wxSize , CalcMin(),
        "Calculates the minimum desired size for the item, including any space
needed by borders.", "");

    DocDeclStr(
        void , SetDimension( const wxPoint& pos, const wxSize& size ),
        "Set the position and size of the space allocated for this item by the
sizer, and adjust the position and size of the item (window or
subsizer) to be within that space taking alignment and borders into
account.", "");


    DocDeclStr(
        wxSize , GetMinSize(),
        "Get the minimum size needed for the item.", "");

    DocDeclStr(
        wxSize , GetMinSizeWithBorder() const,
        "Get the minimum size needed for the item with space for the borders
added, if needed.", "");

    DocDeclStr(
        void , SetInitSize( int x, int y ),
        "", "");


    DocStr(SetRatio,
           "Set the ratio item attribute.", "");
    %Rename(SetRatioWH, void, SetRatio( int width, int height ));
    %Rename(SetRatioSize, void, SetRatio( const wxSize& size ));
    void SetRatio( float ratio );

    DocDeclStr(
        float , GetRatio(),
        "Set the ratio item attribute.", "");

    DocDeclStr(
        wxRect , GetRect(),
        "Returns the rectangle that the sizer item should occupy", "");


    DocDeclStr(
        bool , IsWindow(),
        "Is this sizer item a window?", "");

    DocDeclStr(
        bool , IsSizer(),
        "Is this sizer item a subsizer?", "");

    DocDeclStr(
        bool , IsSpacer(),
        "Is this sizer item a spacer?", "");


    DocDeclStr(
        void , SetProportion( int proportion ),
        "Set the proportion value for this item.", "");

    DocDeclStr(
        int , GetProportion(),
        "Get the proportion value for this item.", "");

    %pythoncode { SetOption = wx._deprecated(SetProportion, "Please use `SetProportion` instead.") }
    %pythoncode { GetOption = wx._deprecated(GetProportion, "Please use `GetProportion` instead.") }


    DocDeclStr(
        void , SetFlag( int flag ),
        "Set the flag value for this item.", "");

    DocDeclStr(
        int , GetFlag(),
        "Get the flag value for this item.", "");


    DocDeclStr(
        void , SetBorder( int border ),
        "Set the border value for this item.", "");

    DocDeclStr(
        int , GetBorder(),
        "Get the border value for this item.", "");



    DocDeclStr(
        wxWindow *, GetWindow(),
        "Get the window (if any) that is managed by this sizer item.", "");

    DocDeclStr(
        void , SetWindow( wxWindow *window ),
        "Set the window to be managed by this sizer item.", "");


    DocDeclStr(
        wxSizer *, GetSizer(),
        "Get the subsizer (if any) that is managed by this sizer item.", "");

    %disownarg( wxSizer *sizer );
    DocDeclStr(
        void , SetSizer( wxSizer *sizer ),
        "Set the subsizer to be managed by this sizer item.", "");
    %cleardisown( wxSizer *sizer );


    DocDeclStr(
        const wxSize& , GetSpacer(),
        "Get the size of the spacer managed by this sizer item.", "");

    DocDeclStr(
        void , SetSpacer( const wxSize &size ),
        "Set the size of the spacer to be managed by this sizer item.", "");


    DocDeclStr(
        void , Show( bool show ),
        "Set the show item attribute, which sizers use to determine if the item
is to be made part of the layout or not. If the item is tracking a
window then it is shown or hidden as needed.", "");

    DocDeclStr(
        bool , IsShown(),
        "Is the item to be shown in the layout?", "");


    DocDeclStr(
        wxPoint , GetPosition(),
        "Returns the current position of the item, as set in the last Layout.", "");


    // wxObject* GetUserData();
    %extend {
        // Assume that the user data is a wxPyUserData object and return the contents

        DocStr(GetUserData,
               "Returns the userData associated with this sizer item, or None if there
isn't any.", "");
        PyObject* GetUserData() {
            wxPyUserData* data = (wxPyUserData*)self->GetUserData();
            if (data) {
                Py_INCREF(data->m_obj);
                return data->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        DocStr(SetUserData,
               "Associate a Python object with this sizer item.", "");
        void SetUserData(PyObject* userData) {
            wxPyUserData* data = NULL;
            if ( userData ) {
                wxPyBlock_t blocked = wxPyBeginBlockThreads();
                data = new wxPyUserData(userData);
                wxPyEndBlockThreads(blocked);
            }
            self->SetUserData(data);
        }
    }
};


//---------------------------------------------------------------------------

%{
// Figure out the type of the sizer item

struct wxPySizerItemInfo {
    wxPySizerItemInfo()
        : window(NULL), sizer(NULL), gotSize(false),
          size(wxDefaultSize), gotPos(false), pos(-1)
    {}

    wxWindow* window;
    wxSizer*  sizer;
    bool      gotSize;
    wxSize    size;
    bool      gotPos;
    int       pos;
};

static wxPySizerItemInfo wxPySizerItemTypeHelper(PyObject* item, bool checkSize, bool checkIdx ) {

    wxPySizerItemInfo info;
    wxSize  size;
    wxSize* sizePtr = &size;

    // Find out what the type of the item is
    // try wxWindow
    if ( ! wxPyConvertSwigPtr(item, (void**)&info.window, wxT("wxWindow")) ) {
        PyErr_Clear();
        info.window = NULL;

        // try wxSizer
        if ( ! wxPyConvertSwigPtr(item, (void**)&info.sizer, wxT("wxSizer")) ) {
            PyErr_Clear();
            info.sizer = NULL;

            // try wxSize or (w,h)
            if ( checkSize && wxSize_helper(item, &sizePtr)) {
                info.size = *sizePtr;
                info.gotSize = true;
            }

            // or a single int
            if (checkIdx && PyInt_Check(item)) {
                info.pos = PyInt_AsLong(item);
                info.gotPos = true;
            }
        }
    }

    if ( !(info.window || info.sizer || (checkSize && info.gotSize) || (checkIdx && info.gotPos)) ) {
        // no expected type, figure out what kind of error message to generate
        if ( !checkSize && !checkIdx )
            PyErr_SetString(PyExc_TypeError, "wx.Window or wx.Sizer expected for item");
        else if ( checkSize && !checkIdx )
            PyErr_SetString(PyExc_TypeError, "wx.Window, wx.Sizer, wx.Size, or (w,h) expected for item");
        else if ( !checkSize && checkIdx)
            PyErr_SetString(PyExc_TypeError, "wx.Window, wx.Sizer or int (position) expected for item");
        else
            // can this one happen?
            PyErr_SetString(PyExc_TypeError, "wx.Window, wx.Sizer, wx.Size, or (w,h) or int (position) expected for item");
    }

    return info;
}
%}




DocStr(wxSizer,
"wx.Sizer is the abstract base class used for laying out subwindows in
a window.  You cannot use wx.Sizer directly; instead, you will have to
use one of the sizer classes derived from it such as `wx.BoxSizer`,
`wx.StaticBoxSizer`, `wx.GridSizer`, `wx.FlexGridSizer` and
`wx.GridBagSizer`.

The concept implemented by sizers in wxWidgets is closely related to
layout tools in other GUI toolkits, such as Java's AWT, the GTK
toolkit or the Qt toolkit. It is based upon the idea of the individual
subwindows reporting their minimal required size and their ability to
get stretched if the size of the parent window has changed. This will
most often mean that the programmer does not set the original size of
a dialog in the beginning, rather the dialog will assigned a sizer and
this sizer will be queried about the recommended size. The sizer in
turn will query its children, which can be normal windows or contorls,
empty space or other sizers, so that a hierarchy of sizers can be
constructed. Note that wxSizer does not derive from wxWindow and thus
do not interfere with tab ordering and requires very little resources
compared to a real window on screen.

What makes sizers so well fitted for use in wxWidgets is the fact that
every control reports its own minimal size and the algorithm can
handle differences in font sizes or different window (dialog item)
sizes on different platforms without problems. If for example the
standard font as well as the overall design of Mac widgets requires
more space than on Windows, then the initial size of a dialog using a
sizer will automatically be bigger on Mac than on Windows.", "

Sizers may also be used to control the layout of custom drawn items on
the window.  The `Add`, `Insert`, and `Prepend` functions return a
pointer to the newly added `wx.SizerItem`. Just add empty space of the
desired size and attributes, and then use the `wx.SizerItem.GetRect`
method to determine where the drawing operations should take place.

:note: If you wish to create a custom sizer class in wxPython you
    should derive the class from `wx.PySizer` in order to get
    Python-aware capabilities for the various virtual methods.

:see: `wx.SizerItem`

:todo: More dscriptive text here along with some pictures...

");

class wxSizer : public wxObject {
public:
    // wxSizer();      ****  abstract, can't instantiate

    ~wxSizer();

    %extend {
        void _setOORInfo(PyObject* _self) {
            if (!self->GetClientObject())
                self->SetClientObject(new wxPyOORClientData(_self));
        }

        DocAStr(Add,
                "Add(self, item, int proportion=0, int flag=0, int border=0,
    PyObject userData=None) -> wx.SizerItem",

                "Appends a child item to the sizer.", "

    :param item:  The item can be one of three kinds of objects:

        - **window**: A `wx.Window` to be managed by the sizer. Its
          minimal size (either set explicitly by the user or
          calculated internally when constructed with wx.DefaultSize)
          is interpreted as the minimal size to use when laying out
          item in the sizer.  This is particularly useful in
          connection with `wx.Window.SetSizeHints`.

        - **sizer**: The (child-)sizer to be added to the sizer. This
          allows placing a child sizer in a sizer and thus to create
          hierarchies of sizers (typically a vertical box as the top
          sizer and several horizontal boxes on the level beneath).

        - **size**: A `wx.Size` or a 2-element sequence of integers
          that represents the width and height of a spacer to be added
          to the sizer. Adding spacers to sizers gives more
          flexibility in the design of dialogs; imagine for example a
          horizontal box with two buttons at the bottom of a dialog:
          you might want to insert a space between the two buttons and
          make that space stretchable using the *proportion* value and
          the result will be that the left button will be aligned with
          the left side of the dialog and the right button with the
          right side - the space in between will shrink and grow with
          the dialog.

    :param proportion: Although the meaning of this parameter is
        undefined in wx.Sizer, it is used in `wx.BoxSizer` to indicate
        if a child of a sizer can change its size in the main
        orientation of the wx.BoxSizer - where 0 stands for not
        changeable and a value of more than zero is interpreted
        relative (a proportion of the total) to the value of other
        children of the same wx.BoxSizer. For example, you might have
        a horizontal wx.BoxSizer with three children, two of which are
        supposed to change their size with the sizer. Then the two
        stretchable windows should each be given *proportion* value of
        1 to make them grow and shrink equally with the sizer's
        horizontal dimension.  But if one of them had a *proportion*
        value of 2 then it would get a double share of the space
        available after the fixed size items are positioned.

    :param flag: This parameter can be used to set a number of flags
        which can be combined using the binary OR operator ``|``. Two
        main behaviours are defined using these flags. One is the
        border around a window: the *border* parameter determines the
        border width whereas the flags given here determine which
        side(s) of the item that the border will be added. The other
        flags determine how the sizer item behaves when the space
        allotted to the sizer changes, and is somewhat dependent on
        the specific kind of sizer used.

        +----------------------------+------------------------------------------+
        |- wx.TOP                    |These flags are used to specify           |
        |- wx.BOTTOM                 |which side(s) of the sizer item that      |
        |- wx.LEFT                   |the *border* width will apply to.         |
        |- wx.RIGHT                  |                                          |
        |- wx.ALL                    |                                          |
        |                            |                                          |
        +----------------------------+------------------------------------------+
        |- wx.EXPAND                 |The item will be expanded to fill         |
        |                            |the space allotted to the item.           |
        +----------------------------+------------------------------------------+
        |- wx.SHAPED                 |The item will be expanded as much as      |
        |                            |possible while also maintaining its       |
        |                            |aspect ratio                              |
        +----------------------------+------------------------------------------+
        |- wx.FIXED_MINSIZE          |Normally wx.Sizers will use               |
        |                            |`wx.Window.GetMinSize` or                 |
        |                            |`wx.Window.GetBestSize` to determine what |
        |                            |the minimal size of window items should   |
        |                            |be, and will use that size to calculate   |
        |                            |the layout. This allows layouts to adjust |
        |                            |when an item changes and it's best size   |
        |                            |becomes different. If you would rather    |
        |                            |have a window item stay the size it       |
        |                            |started with then use wx.FIXED_MINSIZE.   |
        +----------------------------+------------------------------------------+
        |- wx.ALIGN_CENTER           |The wx.ALIGN flags allow you to specify   |
        |- wx.ALIGN_LEFT             |the alignment of the item within the space|
        |- wx.ALIGN_RIGHT            |allotted to it by the sizer, ajusted for  |
        |- wx.ALIGN_TOP              |the border if any.                        |
        |- wx.ALIGN_BOTTOM           |                                          |
        |- wx.ALIGN_CENTER_VERTICAL  |                                          |
        |- wx.ALIGN_CENTER_HORIZONTAL|                                          |
        +----------------------------+------------------------------------------+


    :param border: Determines the border width, if the *flag*
        parameter is set to include any border flag.

    :param userData: Allows an extra object to be attached to the
        sizer item, for use in derived classes when sizing information
        is more complex than the *proportion* and *flag* will allow for.
");

        wxSizerItem*  Add(PyObject* item, int proportion=0, int flag=0, int border=0,
                          PyObject* userData=NULL) {

            wxPyUserData* data = NULL;
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, true, false);
            if ( userData && (info.window || info.sizer || info.gotSize) )
                data = new wxPyUserData(userData);
            if ( info.sizer )
                PyObject_SetAttrString(item,"thisown",Py_False);
            wxPyEndBlockThreads(blocked);

            // Now call the real Add method if a valid item type was found
            if ( info.window )
                return self->Add(info.window, proportion, flag, border, data);
            else if ( info.sizer )
                return self->Add(info.sizer, proportion, flag, border, data);
            else if (info.gotSize)
                return self->Add(info.size.GetWidth(), info.size.GetHeight(),
                                 proportion, flag, border, data);
            else
                return NULL;
        }

//    virtual wxSizerItem* AddSpacer(int size);
//    virtual wxSizerItem* AddStretchSpacer(int prop = 1);

        DocAStr(Insert,
                "Insert(self, int before, item, int proportion=0, int flag=0, int border=0,
    PyObject userData=None) -> wx.SizerItem",

                "Inserts a new item into the list of items managed by this sizer before
the item at index *before*.  See `Add` for a description of the parameters.", "");
        wxSizerItem* Insert(int before, PyObject* item, int proportion=0, int flag=0,
                            int border=0, PyObject* userData=NULL) {

            wxPyUserData* data = NULL;
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, true, false);
            if ( userData && (info.window || info.sizer || info.gotSize) )
                data = new wxPyUserData(userData);
            if ( info.sizer )
                PyObject_SetAttrString(item,"thisown",Py_False);
            wxPyEndBlockThreads(blocked);

            // Now call the real Insert method if a valid item type was found
            if ( info.window )
                return self->Insert(before, info.window, proportion, flag, border, data);
            else if ( info.sizer )
                return self->Insert(before, info.sizer, proportion, flag, border, data);
            else if (info.gotSize)
                return self->Insert(before, info.size.GetWidth(), info.size.GetHeight(),
                                    proportion, flag, border, data);
            else
                return NULL;
        }


//    virtual wxSizerItem* InsertSpacer(size_t index, int size);
//    virtual wxSizerItem* InsertStretchSpacer(size_t index, int prop = 1);

        DocAStr(Prepend,
                "Prepend(self, item, int proportion=0, int flag=0, int border=0,
    PyObject userData=None) -> wx.SizerItem",

               "Adds a new item to the begining of the list of sizer items managed by
this sizer.  See `Add` for a description of the parameters.", "");
        wxSizerItem* Prepend(PyObject* item, int proportion=0, int flag=0, int border=0,
                             PyObject* userData=NULL) {

            wxPyUserData* data = NULL;
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, true, false);
            if ( userData && (info.window || info.sizer || info.gotSize) )
                data = new wxPyUserData(userData);
            if ( info.sizer )
                PyObject_SetAttrString(item,"thisown",Py_False);
            wxPyEndBlockThreads(blocked);

            // Now call the real Prepend method if a valid item type was found
            if ( info.window )
                return self->Prepend(info.window, proportion, flag, border, data);
            else if ( info.sizer )
                return self->Prepend(info.sizer, proportion, flag, border, data);
            else if (info.gotSize)
                return self->Prepend(info.size.GetWidth(), info.size.GetHeight(),
                                     proportion, flag, border, data);
            else
                return NULL;
        }

//    virtual wxSizerItem* PrependSpacer(int size);
//    virtual wxSizerItem* PrependStretchSpacer(int prop = 1);

        
        DocAStr(Remove,
                "Remove(self, item) -> bool",
                "Removes an item from the sizer and destroys it.  This method does not
cause any layout or resizing to take place, call `Layout` to update
the layout on screen after removing a child from the sizer.  The
*item* parameter can be either a window, a sizer, or the zero-based
index of an item to remove.  Returns True if the child item was found
and removed.", "

:note: For historical reasons calling this method with a `wx.Window`
    parameter is depreacted, as it will not be able to destroy the
    window since it is owned by its parent.  You should use `Detach`
    instead.
");
        bool Remove(PyObject* item) {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, false, true);
            wxPyEndBlockThreads(blocked);
            if ( info.window )
                return self->Remove(info.window);
            else if ( info.sizer )
                return self->Remove(info.sizer);
            else if ( info.gotPos )
                return self->Remove(info.pos);
            else
                return false;
        }


        DocAStr(Detach,
                "Detach(self, item) -> bool",
                "Detaches an item from the sizer without destroying it.  This method
does not cause any layout or resizing to take place, call `Layout` to
do so.  The *item* parameter can be either a window, a sizer, or the
zero-based index of the item to be detached.  Returns True if the child item
was found and detached.", "");
        bool Detach(PyObject* item) {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, false, true);
            wxPyEndBlockThreads(blocked);
            if ( info.window )
                return self->Detach(info.window);
            else if ( info.sizer )
                return self->Detach(info.sizer);
            else if ( info.gotPos )
                return self->Detach(info.pos);
            else
                return false;
        }


        DocAStr(GetItem,
                "GetItem(self, item) -> wx.SizerItem",
                "Returns the `wx.SizerItem` which holds the *item* given.  The *item*
parameter can be either a window, a sizer, or the zero-based index of
the item to be found.", "");
        wxSizerItem* GetItem(PyObject* item) {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, false, true);
            wxPyEndBlockThreads(blocked);
            if ( info.window )
                return self->GetItem(info.window);
            else if ( info.sizer )
                return self->GetItem(info.sizer);
            else if ( info.gotPos )
                return self->GetItem(info.pos);
            else
                return NULL;
        }


        void _SetItemMinSize(PyObject* item, const wxSize& size) {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, false, true);
            wxPyEndBlockThreads(blocked);
            if ( info.window )
                self->SetItemMinSize(info.window, size);
            else if ( info.sizer )
                self->SetItemMinSize(info.sizer, size);
            else if ( info.gotPos )
                self->SetItemMinSize(info.pos, size);
        }
    }

    
    %Rename(_ReplaceWin,
            bool, Replace( wxWindow *oldwin, wxWindow *newwin, bool recursive = false ));
    %Rename(_ReplaceSizer,
            bool, Replace( wxSizer *oldsz, wxSizer *newsz, bool recursive = false ));
    %Rename(_ReplaceItem,
            bool, Replace( size_t index, wxSizerItem *newitem ));
    %pythoncode {
        def Replace(self, olditem, item, recursive=False):
            """
            Detaches the given ``olditem`` from the sizer and replaces it with
            ``item`` which can be a window, sizer, or `wx.SizerItem`.  The
            detached child is destroyed only if it is not a window, (because
            windows are owned by their parent, not the sizer.)  The
            ``recursive`` parameter can be used to search for the given
            element recursivly in subsizers.

            This method does not cause any layout or resizing to take place,
            call `Layout` to do so.

            Returns ``True`` if the child item was found and removed.
            """
            if isinstance(olditem, wx.Window):
                return self._ReplaceWin(olditem, item, recursive)
            elif isinstnace(olditem, wx.Sizer):
                return self._ReplaceSizer(olditem, item, recursive)
            elif isinstnace(olditem, int):
                return self._ReplaceItem(olditem, item)
            else:
                raise TypeError("Expected Window, Sizer, or integer for first parameter.")
    }
    
    
    DocDeclStr(
        void , SetContainingWindow(wxWindow *window),
        "Set (or unset) the window this sizer is used in.", "");
    
    DocDeclStr(
        wxWindow *, GetContainingWindow() const,
        "Get the window this sizer is used in.", "");
        

    %pythoncode {
    def SetItemMinSize(self, item, *args):
        """
        SetItemMinSize(self, item, Size size)

        Sets the minimum size that will be allocated for an item in the sizer.
        The *item* parameter can be either a window, a sizer, or the
        zero-based index of the item.  If a window or sizer is given then it
        will be searched for recursivly in subsizers if neccessary.
        """
        if len(args) == 2:
            %# for backward compatibility accept separate width,height args too
            return self._SetItemMinSize(item, args)
        else:
            return self._SetItemMinSize(item, args[0])
    }


    %disownarg( wxSizerItem *item ); 

    DocDeclAStrName(
        wxSizerItem* , Add( wxSizerItem *item ),
        "AddItem(self, SizerItem item)",
        "Adds a `wx.SizerItem` to the sizer.", "",
        AddItem);

    DocDeclAStrName(
        wxSizerItem* , Insert( size_t index, wxSizerItem *item ),
        "InsertItem(self, int index, SizerItem item)",
        "Inserts a `wx.SizerItem` to the sizer at the position given by *index*.", "",
        InsertItem);

    DocDeclAStrName(
        wxSizerItem* , Prepend( wxSizerItem *item ),
        "PrependItem(self, SizerItem item)",
        "Prepends a `wx.SizerItem` to the sizer.", "",
        PrependItem);

    %cleardisown( wxSizerItem *item );


    %pythoncode {
    def AddMany(self, items):
        """
        AddMany is a convenience method for adding several items
        to a sizer at one time.  Simply pass it a list of tuples,
        where each tuple consists of the parameters that you
        would normally pass to the `Add` method.
        """
        for item in items:
            if type(item) != type(()) or (len(item) == 2 and type(item[0]) == type(1)):
                item = (item, )
            self.Add(*item)

    def AddSpacer(self, *args, **kw):
        """AddSpacer(int size) --> SizerItem

        Add a spacer that is (size,size) pixels.
        """
        if args and type(args[0]) == int:
            return self.Add( (args[0],args[0] ), 0)
        else: %# otherwise stay compatible with old AddSpacer
            return self.Add(*args, **kw)
    def PrependSpacer(self, *args, **kw):
        """PrependSpacer(int size) --> SizerItem

        Prepend a spacer that is (size, size) pixels."""
        if args and type(args[0]) == int:
            return self.Prepend( (args[0],args[0] ), 0)
        else: %# otherwise stay compatible with old PrependSpacer
            return self.Prepend(*args, **kw)
    def InsertSpacer(self, index, *args, **kw):
        """InsertSpacer(int index, int size) --> SizerItem

        Insert a spacer at position index that is (size, size) pixels."""
        if args and type(args[0]) == int:
            return self.Insert( index, (args[0],args[0] ), 0)
        else: %# otherwise stay compatible with old InsertSpacer
            return self.Insert(index, *args, **kw)

                   
    def AddStretchSpacer(self, prop=1):
        """AddStretchSpacer(int prop=1) --> SizerItem

        Add a stretchable spacer."""
        return self.Add((0,0), prop)
    def PrependStretchSpacer(self, prop=1):
        """PrependStretchSpacer(int prop=1) --> SizerItem

        Prepend a stretchable spacer."""
        return self.Prepend((0,0), prop)
    def InsertStretchSpacer(self, index, prop=1):
        """InsertStretchSpacer(int index, int prop=1) --> SizerItem

        Insert a stretchable spacer."""
        return self.Insert(index, (0,0), prop)

            
    %# for backwards compatibility only, please do not use in new code
    def AddWindow(self, *args, **kw):
        """Compatibility alias for `Add`."""
        return self.Add(*args, **kw)
    def AddSizer(self, *args, **kw):
        """Compatibility alias for `Add`."""
        return self.Add(*args, **kw)

    def PrependWindow(self, *args, **kw):
        """Compatibility alias for `Prepend`."""
        return self.Prepend(*args, **kw)
    def PrependSizer(self, *args, **kw):
        """Compatibility alias for `Prepend`."""
        return self.Prepend(*args, **kw)

    def InsertWindow(self, *args, **kw):
        """Compatibility alias for `Insert`."""
        return self.Insert(*args, **kw)
    def InsertSizer(self, *args, **kw):
        """Compatibility alias for `Insert`."""
        return self.Insert(*args, **kw)

    def RemoveWindow(self, *args, **kw):
        """Compatibility alias for `Remove`."""
        return self.Remove(*args, **kw)
    def RemoveSizer(self, *args, **kw):
        """Compatibility alias for `Remove`."""
        return self.Remove(*args, **kw)
    def RemovePos(self, *args, **kw):
        """Compatibility alias for `Remove`."""
        return self.Remove(*args, **kw)

    }


    DocDeclStr(
        void , SetDimension( int x, int y, int width, int height ),
        "Call this to force the sizer to take the given dimension and thus
force the items owned by the sizer to resize themselves according to
the rules defined by the parameter in the `Add`, `Insert` or `Prepend`
methods.", "");

    DocDeclStr(
        void , SetMinSize( const wxSize &size ),
        "Call this to give the sizer a minimal size. Normally, the sizer will
calculate its minimal size based purely on how much space its children
need. After calling this method `GetMinSize` will return either the
minimal size as requested by its children or the minimal size set
here, depending on which is bigger.", "");


    DocDeclStr(
        wxSize , GetSize(),
        "Returns the current size of the space managed by the sizer.", "");

    DocDeclStr(
        wxPoint , GetPosition(),
        "Returns the current position of the sizer's managed space.", "");

    DocDeclStr(
        wxSize , GetMinSize(),
        "Returns the minimal size of the sizer. This is either the combined
minimal size of all the children and their borders or the minimal size
set by SetMinSize, depending on which is bigger.", "");


    %pythoncode {
    def GetSizeTuple(self):
        return self.GetSize().Get()
    def GetPositionTuple(self):
        return self.GetPosition().Get()
    def GetMinSizeTuple(self):
        return self.GetMinSize().Get()
    }

    DocDeclStr(
        virtual void , RecalcSizes(),
        "Using the sizes calculated by `CalcMin` reposition and resize all the
items managed by this sizer.  You should not need to call this directly as
it is called by `Layout`.", "");

    DocDeclStr(
        virtual wxSize , CalcMin(),
        "This method is where the sizer will do the actual calculation of its
children's minimal sizes.  You should not need to call this directly as
it is called by `Layout`.", "");


    DocDeclStr(
        void , Layout(),
        "This method will force the recalculation and layout of the items
controlled by the sizer using the current space allocated to the
sizer.  Normally this is called automatically from the owning window's
EVT_SIZE handler, but it is also useful to call it from user code when
one of the items in a sizer change size, or items are added or
removed.", "");


    DocDeclStr(
        wxSize , Fit( wxWindow *window ),
        "Tell the sizer to resize the *window* to match the sizer's minimal
size. This is commonly done in the constructor of the window itself in
order to set its initial size to match the needs of the children as
determined by the sizer.  Returns the new size.

For a top level window this is the total window size, not the client size.", "");

    DocDeclStr(
        void , FitInside( wxWindow *window ),
        "Tell the sizer to resize the *virtual size* of the *window* to match the
sizer's minimal size. This will not alter the on screen size of the
window, but may cause the addition/removal/alteration of scrollbars
required to view the virtual area in windows which manage it.

:see: `wx.ScrolledWindow.SetScrollbars`, `SetVirtualSizeHints`
", "");


    DocDeclStr(
        void , SetSizeHints( wxWindow *window ),
        "Tell the sizer to set (and `Fit`) the minimal size of the *window* to
match the sizer's minimal size. This is commonly done in the
constructor of the window itself if the window is resizable (as are
many dialogs under Unix and frames on probably all platforms) in order
to prevent the window from being sized smaller than the minimal size
required by the sizer.", "");

    DocDeclStr(
        void , SetVirtualSizeHints( wxWindow *window ),
        "Tell the sizer to set the minimal size of the window virtual area to
match the sizer's minimal size. For windows with managed scrollbars
this will set them appropriately.

:see: `wx.ScrolledWindow.SetScrollbars`
", "");


    DocDeclStr(
        void , Clear( bool deleteWindows=false ),
        "Clear all items from the sizer, optionally destroying the window items
as well.", "");

    DocDeclStr(
        void , DeleteWindows(),
        "Destroy all windows managed by the sizer.", "");



    // wxList& GetChildren();
    %extend {
        DocAStr(GetChildren,
                "GetChildren(self) -> list",
                "Returns a list of all the `wx.SizerItem` objects managed by the sizer.", "");
        PyObject* GetChildren() {
            wxSizerItemList& list = self->GetChildren();
            return wxPy_ConvertList(&list);
        }
    }


    // Manage whether individual windows or subsizers are considered
    // in the layout calculations or not.

    %extend {
        DocAStr(Show,
                "Show(self, item, bool show=True, bool recursive=false) -> bool",
                "Shows or hides an item managed by the sizer.  To make a sizer item
disappear or reappear, use Show followed by `Layout`.  The *item*
parameter can be either a window, a sizer, or the zero-based index of
the item.  Use the recursive parameter to show or hide an item in a
subsizer.  Returns True if the item was found.", "");
        bool Show(PyObject* item, bool show = true, bool recursive=false) {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, false, true);
            wxPyEndBlockThreads(blocked);
            if ( info.window )
                return self->Show(info.window, show, recursive);
            else if ( info.sizer )
                return self->Show(info.sizer, show, recursive);
            else if ( info.gotPos )
                return self->Show(info.pos, show);
            else
                return false;
        }

        DocAStr(IsShown,
                "IsShown(self, item)",
                "Determines if the item is currently shown. To make a sizer
item disappear or reappear, use Show followed by `Layout`.  The *item*
parameter can be either a window, a sizer, or the zero-based index of
the item.", "");
        bool IsShown(PyObject* item) {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, false, false);
            wxPyEndBlockThreads(blocked);
            if ( info.window )
                return self->IsShown(info.window);
            else if ( info.sizer )
                return self->IsShown(info.sizer);
            else if ( info.gotPos )
                return self->IsShown(info.pos);
            else
                return false;
        }
    }

    %pythoncode {
    def Hide(self, item, recursive=False):
        """
        A convenience method for `Show` (item, False, recursive).
        """
        return self.Show(item, False, recursive)
    }


    DocDeclStr(
        void , ShowItems(bool show),
        "Recursively call `wx.SizerItem.Show` on all sizer items.", "");

};


//---------------------------------------------------------------------------
// Use this one for deriving Python classes from
%{
// See pyclasses.h
IMP_PYCALLBACK___pure(wxPySizer, wxSizer, RecalcSizes);
IMP_PYCALLBACK_wxSize__pure(wxPySizer, wxSizer, CalcMin);
IMPLEMENT_DYNAMIC_CLASS(wxPySizer, wxSizer);
%}


DocStr(wxPySizer,
"wx.PySizer is a special version of `wx.Sizer` that has been
instrumented to allow the C++ virtual methods to be overloaded in
Python derived classes.  You would derive from this class if you are
wanting to implement a custom sizer in Python code.  Simply implement
`CalcMin` and `RecalcSizes` in the derived class and you're all set.
For example::

    class MySizer(wx.PySizer):
         def __init__(self):
             wx.PySizer.__init__(self)

         def CalcMin(self):
             for item in self.GetChildren():
                  # calculate the total minimum width and height needed
                  # by all items in the sizer according to this sizer's
                  # layout algorithm.
                  ...
             return wx.Size(width, height)

          def RecalcSizes(self):
              # find the space allotted to this sizer
              pos = self.GetPosition()
              size = self.GetSize()
              for item in self.GetChildren():
                  # Recalculate (if necessary) the position and size of
                  # each item and then call item.SetDimension to do the
                  # actual positioning and sizing of the items within the
                  # space alloted to this sizer.
                  ...
                  item.SetDimension(itemPos, itemSize)


When `Layout` is called it first calls `CalcMin` followed by
`RecalcSizes` so you can optimize a bit by saving the results of
`CalcMin` and reusing them in `RecalcSizes`.

:see: `wx.SizerItem`, `wx.Sizer.GetChildren`

", "");
class wxPySizer : public wxSizer {
public:
    %pythonAppend wxPySizer "self._setCallbackInfo(self, PySizer);self._setOORInfo(self)"

    DocCtorStr(
        wxPySizer(),
        "Creates a wx.PySizer.  Must be called from the __init__ in the derived
class.", "");

    void _setCallbackInfo(PyObject* self, PyObject* _class);
};


//---------------------------------------------------------------------------
%newgroup;


DocStr(wxBoxSizer,
"The basic idea behind a box sizer is that windows will most often be
laid out in rather simple basic geometry, typically in a row or a
column or nested hierarchies of either.  A wx.BoxSizer will lay out
its items in a simple row or column, depending on the orientation
parameter passed to the constructor.", "

It is the unique feature of a box sizer, that it can grow in both
directions (height and width) but can distribute its growth in the
main direction (horizontal for a row) *unevenly* among its children.
This is determined by the proportion parameter give to items when they
are added to the sizer. It is interpreted as a weight factor, i.e. it
can be zero, indicating that the window may not be resized at all, or
above zero. If several windows have a value above zero, the value is
interpreted relative to the sum of all weight factors of the sizer, so
when adding two windows with a value of 1, they will both get resized
equally and each will receive half of the available space after the
fixed size items have been sized.  If the items have unequal
proportion settings then they will receive a coresondingly unequal
allotment of the free space.

:see: `wx.StaticBoxSizer`
");

class  wxBoxSizer : public wxSizer {
public:
    %pythonAppend wxBoxSizer "self._setOORInfo(self)"

    DocCtorStr(
        wxBoxSizer(int orient = wxHORIZONTAL),
        "Constructor for a wx.BoxSizer. *orient* may be one of ``wx.VERTICAL``
or ``wx.HORIZONTAL`` for creating either a column sizer or a row
sizer.", "");


    DocDeclStr(
        int , GetOrientation(),
        "Returns the current orientation of the sizer.", "");

    DocDeclStr(
        void , SetOrientation(int orient),
        "Resets the orientation of the sizer.", "");

    %property(Orientation, GetOrientation, SetOrientation, doc="See `GetOrientation` and `SetOrientation`");
};

//---------------------------------------------------------------------------
%newgroup;


DocStr(wxStaticBoxSizer,
"wx.StaticBoxSizer derives from and functions identically to the
`wx.BoxSizer` and adds a `wx.StaticBox` around the items that the sizer
manages.  Note that this static box must be created separately and
passed to the sizer constructor.", "");

class  wxStaticBoxSizer : public wxBoxSizer {
public:
    %pythonAppend wxStaticBoxSizer "self._setOORInfo(self)"

    DocCtorStr(
        wxStaticBoxSizer(wxStaticBox *box, int orient = wxHORIZONTAL),
        "Constructor. It takes an associated static box and the orientation
*orient* as parameters - orient can be either of ``wx.VERTICAL`` or
``wx.HORIZONTAL``.", "");

    // TODO: wxStaticBoxSizer(int orient, wxWindow *win, const wxString& label = wxEmptyString);

    DocDeclStr(
        wxStaticBox *, GetStaticBox(),
        "Returns the static box associated with this sizer.", "");

};

//---------------------------------------------------------------------------
%newgroup;


DocStr(wxGridSizer,
"A grid sizer is a sizer which lays out its children in a
two-dimensional table with all cells having the same size.  In other
words, the width of each cell within the grid is the width of the
widest item added to the sizer and the height of each grid cell is the
height of the tallest item.  An optional vertical and/or horizontal
gap between items can also be specified (in pixels.)

Items are placed in the cells of the grid in the order they are added,
in row-major order.  In other words, the first row is filled first,
then the second, and so on until all items have been added. (If
neccessary, additional rows will be added as items are added.)  If you
need to have greater control over the cells that items are placed in
then use the `wx.GridBagSizer`.
", "");

class wxGridSizer: public wxSizer
{
public:
    %pythonAppend wxGridSizer "self._setOORInfo(self)"

    DocCtorStr(
        wxGridSizer( int rows=1, int cols=0, int vgap=0, int hgap=0 ),
        "Constructor for a wx.GridSizer. *rows* and *cols* determine the number
of columns and rows in the sizer - if either of the parameters is
zero, it will be calculated to from the total number of children in
the sizer, thus making the sizer grow dynamically. *vgap* and *hgap*
define extra space between all children.", "");

    DocDeclStr(
        void , SetCols( int cols ),
        "Sets the number of columns in the sizer.", "");

    DocDeclStr(
        void , SetRows( int rows ),
        "Sets the number of rows in the sizer.", "");

    DocDeclStr(
        void , SetVGap( int gap ),
        "Sets the vertical gap (in pixels) between the cells in the sizer.", "");

    DocDeclStr(
        void , SetHGap( int gap ),
        "Sets the horizontal gap (in pixels) between cells in the sizer", "");

    DocDeclStr(
        int , GetCols(),
        "Returns the number of columns in the sizer.", "");

    DocDeclStr(
        int , GetRows(),
        "Returns the number of rows in the sizer.", "");

    DocDeclStr(
        int , GetVGap(),
        "Returns the vertical gap (in pixels) between the cells in the sizer.", "");

    DocDeclStr(
        int , GetHGap(),
        "Returns the horizontal gap (in pixels) between cells in the sizer.", "");

    %pythoncode {
        def CalcRowsCols(self):
            """
            CalcRowsCols() -> (rows, cols)

            Calculates how many rows and columns will be in the sizer based
            on the current number of items and also the rows, cols specified
            in the constructor.
            """
            nitems = len(self.GetChildren())
            rows = self.GetRows()
            cols = self.GetCols()
            assert rows != 0 or cols != 0, "Grid sizer must have either rows or columns fixed"
            if cols != 0:
                rows = (nitems + cols - 1) / cols
            elif rows != 0:
                cols = (nitems + rows - 1) / rows
            return (rows, cols)
    }
    
    %property(Cols, GetCols, SetCols, doc="See `GetCols` and `SetCols`");
    %property(HGap, GetHGap, SetHGap, doc="See `GetHGap` and `SetHGap`");
    %property(Rows, GetRows, SetRows, doc="See `GetRows` and `SetRows`");
    %property(VGap, GetVGap, SetVGap, doc="See `GetVGap` and `SetVGap`");
};

//---------------------------------------------------------------------------
%newgroup;

enum wxFlexSizerGrowMode
{
    // don't resize the cells in non-flexible direction at all
    wxFLEX_GROWMODE_NONE,

    // uniformly resize only the specified ones (default)
    wxFLEX_GROWMODE_SPECIFIED,

    // uniformly resize all cells
    wxFLEX_GROWMODE_ALL
};





DocStr(wxFlexGridSizer,
"A flex grid sizer is a sizer which lays out its children in a
two-dimensional table with all table cells in one row having the same
height and all cells in one column having the same width, but all
rows or all columns are not necessarily the same height or width as in
the `wx.GridSizer`.

wx.FlexGridSizer can also size items equally in one direction but
unequally (\"flexibly\") in the other. If the sizer is only flexible
in one direction (this can be changed using `SetFlexibleDirection`), it
needs to be decided how the sizer should grow in the other (\"non
flexible\") direction in order to fill the available space. The
`SetNonFlexibleGrowMode` method serves this purpose.

", "");

class wxFlexGridSizer: public wxGridSizer
{
public:
    %pythonAppend wxFlexGridSizer "self._setOORInfo(self)"

    DocCtorStr(
        wxFlexGridSizer( int rows=1, int cols=0, int vgap=0, int hgap=0 ),
        "Constructor for a wx.FlexGridSizer. *rows* and *cols* determine the
number of columns and rows in the sizer - if either of the parameters
is zero, it will be calculated to from the total number of children in
the sizer, thus making the sizer grow dynamically. *vgap* and *hgap*
define extra space between all children.", "");


    DocDeclStr(
        void , AddGrowableRow( size_t idx, int proportion = 0  ),
        "Specifies that row *idx* (starting from zero) should be grown if there
is extra space available to the sizer.

The *proportion* parameter has the same meaning as the stretch factor
for the box sizers except that if all proportions are 0, then all
columns are resized equally (instead of not being resized at all).", "");

    DocDeclStr(
        void , RemoveGrowableRow( size_t idx ),
        "Specifies that row *idx* is no longer growable.", "");

    DocDeclStr(
        void , AddGrowableCol( size_t idx, int proportion = 0  ),
        "Specifies that column *idx* (starting from zero) should be grown if
there is extra space available to the sizer.

The *proportion* parameter has the same meaning as the stretch factor
for the box sizers except that if all proportions are 0, then all
columns are resized equally (instead of not being resized at all).", "");

    DocDeclStr(
        void , RemoveGrowableCol( size_t idx ),
        "Specifies that column *idx* is no longer growable.", "");


    DocDeclStr(
        void , SetFlexibleDirection(int direction),
        "Specifies whether the sizer should flexibly resize its columns, rows,
or both. Argument *direction* can be one of the following values.  Any
other value is ignored.

    ==============    =======================================
    wx.VERTICAL       Rows are flexibly sized.
    wx.HORIZONTAL     Columns are flexibly sized.
    wx.BOTH           Both rows and columns are flexibly sized
                      (this is the default value).
    ==============    =======================================

Note that this method does not trigger relayout.
", "");

    DocDeclStr(
        int , GetFlexibleDirection(),
        "Returns a value that specifies whether the sizer
flexibly resizes its columns, rows, or both (default).

:see: `SetFlexibleDirection`", "");



    DocDeclStr(
        void , SetNonFlexibleGrowMode(wxFlexSizerGrowMode mode),
        "Specifies how the sizer should grow in the non-flexible direction if
there is one (so `SetFlexibleDirection` must have been called
previously). Argument *mode* can be one of the following values:

    ==========================  =================================================
    wx.FLEX_GROWMODE_NONE       Sizer doesn't grow in the non flexible direction.
    wx.FLEX_GROWMODE_SPECIFIED  Sizer honors growable columns/rows set with
                                `AddGrowableCol` and `AddGrowableRow`. In this
                                case equal sizing applies to minimum sizes of
                                columns or rows (this is the default value).
    wx.FLEX_GROWMODE_ALL        Sizer equally stretches all columns or rows in
                                the non flexible direction, whether they are
                                growable or not in the flexbile direction.
    ==========================  =================================================

Note that this method does not trigger relayout.", "");

    DocDeclStr(
        wxFlexSizerGrowMode , GetNonFlexibleGrowMode(),
        "Returns the value that specifies how the sizer grows in the
non-flexible direction if there is one.

:see: `SetNonFlexibleGrowMode`", "");


    // Read-only access to the row heights and col widths arrays
    DocDeclAStr(
        const wxArrayInt& , GetRowHeights() const,
        "GetRowHeights(self) -> list",
        "Returns a list of integers representing the heights of each of the
rows in the sizer.", "");

    DocDeclAStr(
        const wxArrayInt& , GetColWidths() const,
        "GetColWidths(self) -> list",
        "Returns a list of integers representing the widths of each of the
columns in the sizer.", "");


    %property(ColWidths, GetColWidths, doc="See `GetColWidths`");
    %property(FlexibleDirection, GetFlexibleDirection, SetFlexibleDirection, doc="See `GetFlexibleDirection` and `SetFlexibleDirection`");
    %property(NonFlexibleGrowMode, GetNonFlexibleGrowMode, SetNonFlexibleGrowMode, doc="See `GetNonFlexibleGrowMode` and `SetNonFlexibleGrowMode`");
    %property(RowHeights, GetRowHeights, doc="See `GetRowHeights`");
    
};

//---------------------------------------------------------------------------

DocStr(wxStdDialogButtonSizer,
"A special sizer that knows how to order and position standard buttons
in order to conform to the current platform's standards.  You simply
need to add each `wx.Button` to the sizer, and be sure to create the
buttons using the standard ID's.  Then call `Realize` and the sizer
will take care of the rest.
", "");

class wxStdDialogButtonSizer: public wxBoxSizer
{
public:
    DocCtorStr(
        wxStdDialogButtonSizer(),
        "", "");

    DocDeclStr(
        void , AddButton(wxButton *button),
        "Use this to add the buttons to this sizer.  Do not use the `Add`
method in the base class.", "");

    DocDeclStr(
        void , Realize(),
        "This funciton needs to be called after all the buttons have been added
to the sizer.  It will reorder them and position them in a platform
specifc manner.", "");

    void SetAffirmativeButton( wxButton *button );
    void SetNegativeButton( wxButton *button );
    void SetCancelButton( wxButton *button );

    wxButton* GetAffirmativeButton() const;
    wxButton* GetApplyButton() const;
    wxButton* GetNegativeButton() const;
    wxButton* GetCancelButton() const;
    wxButton* GetHelpButton() const;
};


//---------------------------------------------------------------------------
