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


class wxSizerItem : public wxObject {
public:
    wxSizerItem();
    
    %name(SizerItemSpacer) wxSizerItem( int width, int height, int proportion, int flag, int border, wxObject* userData);
    %name(SizerItemWindow) wxSizerItem( wxWindow *window, int proportion, int flag, int border, wxObject* userData );
    %name(SizerItemSizer) wxSizerItem( wxSizer *sizer, int proportion, int flag, int border, wxObject* userData );

    void DeleteWindows();
    void DetachSizer();

    wxSize GetSize();
    wxSize CalcMin();
    void SetDimension( wxPoint pos, wxSize size );

    wxSize GetMinSize();
    void SetInitSize( int x, int y );

    %name(SetRatioWH) void SetRatio( int width, int height );
    %name(SetRatioSize) void SetRatio( wxSize size );
    void SetRatio( float ratio );
    float GetRatio();

    bool IsWindow();
    bool IsSizer();
    bool IsSpacer();

    void SetProportion( int proportion );
    int GetProportion();
    %pythoncode { SetOption = SetProportion}
    %pythoncode { GetOption = GetProportion}
    
    void SetFlag( int flag );
    int GetFlag();
    
    void SetBorder( int border );
    int GetBorder();

    wxWindow *GetWindow();
    void SetWindow( wxWindow *window );
    
    wxSizer *GetSizer();
    void SetSizer( wxSizer *sizer );
    
    const wxSize& GetSpacer();
    void SetSpacer( const wxSize &size );

    void Show( bool show );
    bool IsShown();

    wxPoint GetPosition();

    // wxObject* GetUserData();
    %extend {
        // Assume that the user data is a wxPyUserData object and return the contents
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
    }
};


//---------------------------------------------------------------------------

%{
// Figure out the type of the sizer item

struct wxPySizerItemInfo {
    wxPySizerItemInfo()
        : window(NULL), sizer(NULL), gotSize(False),
          size(wxDefaultSize), gotPos(False), pos(-1)
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
                info.gotSize = True;
            }

            // or a single int
            if (checkIdx && PyInt_Check(item)) {
                info.pos = PyInt_AsLong(item);
                info.gotPos = True;
            }
        }
    }

    if ( !(info.window || info.sizer || (checkSize && info.gotSize) || (checkIdx && info.gotPos)) ) {
        // no expected type, figure out what kind of error message to generate
        if ( !checkSize && !checkIdx )
            PyErr_SetString(PyExc_TypeError, "wxWindow or wxSizer expected for item");
        else if ( checkSize && !checkIdx )
            PyErr_SetString(PyExc_TypeError, "wxWindow, wxSizer, wxSize, or (w,h) expected for item");
        else if ( !checkSize && checkIdx)
            PyErr_SetString(PyExc_TypeError, "wxWindow, wxSizer or int (position) expected for item");
        else
            // can this one happen?
            PyErr_SetString(PyExc_TypeError, "wxWindow, wxSizer, wxSize, or (w,h) or int (position) expected for item");
    }

    return info;
}
%}




class wxSizer : public wxObject {
public:
    // wxSizer();      ****  abstract, can't instantiate
    // ~wxSizer();

    %extend {
        void _setOORInfo(PyObject* _self) {
            self->SetClientObject(new wxPyOORClientData(_self));
        }


        void Add(PyObject* item, int proportion=0, int flag=0, int border=0,
                  PyObject* userData=NULL) {
            
            wxPyUserData* data = NULL;
            bool blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, True, False);
            if ( userData && (info.window || info.sizer || info.gotSize) )
                data = new wxPyUserData(userData);
            wxPyEndBlockThreads(blocked);
            
            // Now call the real Add method if a valid item type was found
            if ( info.window )
                self->Add(info.window, proportion, flag, border, data);
            else if ( info.sizer )
                self->Add(info.sizer, proportion, flag, border, data);
            else if (info.gotSize)
                self->Add(info.size.GetWidth(), info.size.GetHeight(),
                          proportion, flag, border, data);
        }


        void Insert(int before, PyObject* item, int proportion=0, int flag=0,
                     int border=0, PyObject* userData=NULL) {

            wxPyUserData* data = NULL;
            bool blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, True, False);
            if ( userData && (info.window || info.sizer || info.gotSize) )
                data = new wxPyUserData(userData);
            wxPyEndBlockThreads(blocked);
            
            // Now call the real Insert method if a valid item type was found
            if ( info.window )
                self->Insert(before, info.window, proportion, flag, border, data);
            else if ( info.sizer )
                self->Insert(before, info.sizer, proportion, flag, border, data);
            else if (info.gotSize)
                self->Insert(before, info.size.GetWidth(), info.size.GetHeight(),
                             proportion, flag, border, data);
        }



        void Prepend(PyObject* item, int proportion=0, int flag=0, int border=0,
                     PyObject* userData=NULL) {

            wxPyUserData* data = NULL;
            bool blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, True, False);
            if ( userData && (info.window || info.sizer || info.gotSize) )
                data = new wxPyUserData(userData);
            wxPyEndBlockThreads(blocked);
            
            // Now call the real Prepend method if a valid item type was found
            if ( info.window )
                self->Prepend(info.window, proportion, flag, border, data);
            else if ( info.sizer )
                self->Prepend(info.sizer, proportion, flag, border, data);
            else if (info.gotSize)
                self->Prepend(info.size.GetWidth(), info.size.GetHeight(),
                              proportion, flag, border, data);
        }

        
        bool Remove(PyObject* item) {
            bool blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, False, True);
            wxPyEndBlockThreads(blocked);
            if ( info.window )
                return self->Remove(info.window);
            else if ( info.sizer )
                return self->Remove(info.sizer);
            else if ( info.gotPos )
                return self->Remove(info.pos);
            else 
                return False;
        }

        
        void _SetItemMinSize(PyObject* item, const wxSize& size) {
            bool blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, False, True);
            wxPyEndBlockThreads(blocked);
            if ( info.window )
                self->SetItemMinSize(info.window, size);
            else if ( info.sizer )
                self->SetItemMinSize(info.sizer, size);
            else if ( info.gotPos )
                self->SetItemMinSize(info.pos, size);
        }
    }

    %name(AddItem) void Add( wxSizerItem *item );
    %name(InsertItem) void Insert( size_t index, wxSizerItem *item );
    %name(PrependItem) void Prepend( wxSizerItem *item );


    %pythoncode {
    def AddMany(self, widgets):
        """
        AddMany is a convenience method for adding several items
        to a sizer at one time.  Simply pass it a list of tuples,
        where each tuple consists of the parameters that you
        would normally pass to the `Add` method.
        """
        for childinfo in widgets:
            if type(childinfo) != type(()) or (len(childinfo) == 2 and type(childinfo[0]) == type(1)):
                childinfo = (childinfo, )
            self.Add(*childinfo)

    %# for backwards compatibility only, please do not use in new code
    AddWindow     = wx._deprecated(Add, "AddWindow is deprecated, use `Add` instead.")
    AddSizer      = wx._deprecated(Add, "AddSizer is deprecated, use `Add` instead.")
    AddSpacer     = wx._deprecated(Add, "AddSpacer is deprecated, use `Add` instead.")
    PrependWindow = wx._deprecated(Prepend, "PrependWindow is deprecated, use `Prepend` instead.")
    PrependSizer  = wx._deprecated(Prepend, "PrependSizer is deprecated, use `Prepend` instead.")
    PrependSpacer = wx._deprecated(Prepend, "PrependSpacer is deprecated, use `Prepend` instead.")
    InsertWindow  = wx._deprecated(Insert, "InsertWindow is deprecated, use `Insert` instead.")
    InsertSizer   = wx._deprecated(Insert, "InsertSizer is deprecated, use `Insert` instead.")
    InsertSpacer  = wx._deprecated(Insert, "InsertSpacer is deprecated, use `Insert` instead.")
    RemoveWindow  = wx._deprecated(Remove, "RemoveWindow is deprecated, use `Remove` instead.")
    RemoveSizer   = wx._deprecated(Remove, "RemoveSizer is deprecated, use `Remove` instead.")
    RemovePos     = wx._deprecated(Remove, "RemovePos is deprecated, use `Remove` instead.")


    def SetItemMinSize(self, item, *args):
        if len(args) == 2:
            return self._SetItemMinSize(item, args)
        else:
            return self._SetItemMinSize(item, args[0])
    }


    void SetDimension( int x, int y, int width, int height );
    void SetMinSize( const wxSize &size );

    wxSize GetSize();
    wxPoint GetPosition();
    wxSize GetMinSize();

    %pythoncode {
    def GetSizeTuple(self):
        return self.GetSize().asTuple()
    def GetPositionTuple(self):
        return self.GetPosition().asTuple()
    def GetMinSizeTuple(self):
        return self.GetMinSize().asTuple()
    }

    virtual void RecalcSizes();
    virtual wxSize CalcMin();

    void Layout();

    wxSize Fit( wxWindow *window );
    void FitInside( wxWindow *window );

    void SetSizeHints( wxWindow *window );
    void SetVirtualSizeHints( wxWindow *window );

    void Clear( bool delete_windows=False );
    void DeleteWindows();


    // wxList& GetChildren();
    %extend {
        PyObject* GetChildren() {
            wxSizerItemList& list = self->GetChildren();
            return wxPy_ConvertList(&list);
        }
    }


    // Manage whether individual windows or sub-sizers are considered
    // in the layout calculations or not.

    %extend {
        void Show(PyObject* item, bool show = True) {
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, False, False);
            if ( info.window )
                self->Show(info.window, show);
            else if ( info.sizer )
                self->Show(info.sizer, show);
        }

        
        void Hide(PyObject* item) {
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, False, False);
            if ( info.window )
                self->Hide(info.window);
            else if ( info.sizer )
                self->Hide(info.sizer);
        }

        
        bool IsShown(PyObject* item) {
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, False, False);
            if ( info.window ) 
                return self->IsShown(info.window);
            else if ( info.sizer ) 
                return self->IsShown(info.sizer);
            else
                return False;
        }
    }


    // Recursively call wxWindow::Show() on all sizer items.
    void ShowItems(bool show);

};


//---------------------------------------------------------------------------
// Use this one for deriving Python classes from
%{
// See pyclasses.h    
IMP_PYCALLBACK___pure(wxPySizer, wxSizer, RecalcSizes);
IMP_PYCALLBACK_wxSize__pure(wxPySizer, wxSizer, CalcMin);
IMPLEMENT_DYNAMIC_CLASS(wxPySizer, wxSizer);
%}



class wxPySizer : public wxSizer {
public:
    %pythonAppend wxPySizer "self._setCallbackInfo(self, PySizer);self._setOORInfo(self)"

    wxPySizer();
    void _setCallbackInfo(PyObject* self, PyObject* _class);
};


//---------------------------------------------------------------------------
%newgroup;

class  wxBoxSizer : public wxSizer {
public:
    %pythonAppend wxBoxSizer "self._setOORInfo(self)"

    wxBoxSizer(int orient = wxHORIZONTAL);

    int GetOrientation();
    void SetOrientation(int orient);
    void RecalcSizes();
    wxSize CalcMin();
};

//---------------------------------------------------------------------------
%newgroup;

class  wxStaticBoxSizer : public wxBoxSizer {
public:
    %pythonAppend wxStaticBoxSizer "self._setOORInfo(self)"

    wxStaticBoxSizer(wxStaticBox *box, int orient = wxHORIZONTAL);
    
    wxStaticBox *GetStaticBox();
    void RecalcSizes();
    wxSize CalcMin();
};

//---------------------------------------------------------------------------
%newgroup;

class wxGridSizer: public wxSizer
{
public:
    %pythonAppend wxGridSizer "self._setOORInfo(self)"

    wxGridSizer( int rows=1, int cols=0, int vgap=0, int hgap=0 );

    void RecalcSizes();
    wxSize CalcMin();

    void SetCols( int cols );
    void SetRows( int rows );
    void SetVGap( int gap );
    void SetHGap( int gap );
    int GetCols();
    int GetRows();
    int GetVGap();
    int GetHGap();
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


class wxFlexGridSizer: public wxGridSizer
{
public:
    %pythonAppend wxFlexGridSizer "self._setOORInfo(self)"

    wxFlexGridSizer( int rows=1, int cols=0, int vgap=0, int hgap=0 );
    
    void RecalcSizes();
    wxSize CalcMin();

    void AddGrowableRow( size_t idx, int proportion = 0  );
    void RemoveGrowableRow( size_t idx );
    void AddGrowableCol( size_t idx, int proportion = 0  );
    void RemoveGrowableCol( size_t idx );

    // the sizer cells may grow in both directions, not grow at all or only
    // grow in one direction but not the other

    // the direction may be wxVERTICAL, wxHORIZONTAL or wxBOTH (default)
    void SetFlexibleDirection(int direction);
    int GetFlexibleDirection();

    // note that the grow mode only applies to the direction which is not
    // flexible
    void SetNonFlexibleGrowMode(wxFlexSizerGrowMode mode);
    wxFlexSizerGrowMode GetNonFlexibleGrowMode();

    // Read-only access to the row heights and col widths arrays
    const wxArrayInt& GetRowHeights() const;
    const wxArrayInt& GetColWidths() const;
};

//---------------------------------------------------------------------------
