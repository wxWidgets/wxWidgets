/////////////////////////////////////////////////////////////////////////////
// Name:        _control.i
// Purpose:     SWIG interface defs for wxControl and other base classes
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

MAKE_CONST_WXSTRING(ControlNameStr);

//---------------------------------------------------------------------------
%newgroup;


DocStr(wxControl,
"This is the base class for a control or 'widget'.

A control is generally a small window which processes user input
and/or displays one or more item of data.", "");

MustHaveApp(wxControl);

class wxControl : public wxWindow
{
public:
    %pythonAppend wxControl         "self._setOORInfo(self)"
    %pythonAppend wxControl()       ""
    %typemap(out) wxControl*;    // turn off this typemap

    DocCtorStr(
        wxControl(wxWindow *parent,
                  wxWindowID id=-1,
                  const wxPoint& pos=wxDefaultPosition,
                  const wxSize& size=wxDefaultSize,
                  long style=0,
                  const wxValidator& validator=wxDefaultValidator,
                  const wxString& name=wxPyControlNameStr),
        "Create a Control.  Normally you should only call this from a subclass'
__init__ as a plain old wx.Control is not very useful.", "");

    DocCtorStrName(
        wxControl(),
        "Precreate a Control control for 2-phase creation", "",
        PreControl);

    // Turn it back on again
    %typemap(out) wxControl* { $result = wxPyMake_wxObject($1, $owner); }


    DocDeclStr(
        bool , Create(wxWindow *parent,
                      wxWindowID id=-1,
                      const wxPoint& pos=wxDefaultPosition,
                      const wxSize& size=wxDefaultSize,
                      long style=0,
                      const wxValidator& validator=wxDefaultValidator,
                      const wxString& name=wxPyControlNameStr),
        "Do the 2nd phase and create the GUI control.", "");
    

    DocDeclStr(
        int , GetAlignment() const,
        "Get the control alignment (left/right/centre, top/bottom/centre)", "");
    

    DocDeclStr(
        wxString , GetLabelText() const,
        "Get just the text of the label, without mnemonic characters ('&')", "");
    

    
    DocDeclStr(
        void , Command(wxCommandEvent& event),
        "Simulates the effect of the user issuing a command to the item.

:see: `wx.CommandEvent`
", "");
   

//     DocDeclStr(
//         bool , GetAdjustMinSizeFlag(),
//         "Returns whether the minsize should be adjusted for this control when
// `SetLabel` or `SetFont` are called.", "");
    
//     DocDeclStr(
//         void , SetAdjustMinSizeFlag(bool adjust),
//         "By default controls will readjust their size and minsize when
// `SetLabel` or `SetFont` are called.  This flag will allow you to
// control this behavior.", "

// :see: `GetAdjustMinSizeFlag`
// ");

 
    DocDeclStr(
        static wxString , RemoveMnemonics(const wxString& str),
        "removes the mnemonics characters", "");

    
// Link error...
//     DocDeclStr(
//         static wxString , EscapeMnemonics(const wxString& str),
//         "escapes the mnemonics characters ('&') by doubling them", "");
    
     
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %property(Alignment, GetAlignment, doc="See `GetAlignment`");
    %property(LabelText, GetLabelText, doc="See `GetLabelText`");

};


//---------------------------------------------------------------------------
%newgroup;




DocStr(wxItemContainer,
"The wx.ItemContainer class defines an interface which is implemented
by all controls which have string subitems, each of which may be
selected, such as `wx.ListBox`, `wx.CheckListBox`, `wx.Choice` as well
as `wx.ComboBox` which implements an extended interface deriving from
this one.

It defines the methods for accessing the control's items and although
each of the derived classes implements them differently, they still
all conform to the same interface.

The items in a wx.ItemContainer have (non empty) string labels and,
optionally, client data associated with them.
", "");

class wxItemContainer
{
public:
    // wxItemContainer()  ** It's an ABC
    

    %extend {
        DocStr(Append,
               "Adds the item to the control, associating the given data with the item
if not None.  The return value is the index of the newly added item
which may be different from the last one if the control is sorted (e.g.
has wx.LB_SORT or wx.CB_SORT style).", "");
        int Append(const wxString& item, PyObject* clientData=NULL) {
            if (clientData) {
                wxPyClientData* data = new wxPyClientData(clientData);
                return self->Append(item, data);
            } else
                return self->Append(item);
        }
    }
    
    DocDeclAStrName(
        void , Append(const wxArrayString& strings),
        "AppendItems(self, List strings)",
        "Apend several items at once to the control.  Notice that calling this
method may be much faster than appending the items one by one if you
need to add a lot of items.", "",
        AppendItems);

    
    %extend {
        DocStr(Insert,
               "Insert an item into the control before the item at the ``pos`` index,
optionally associating some data object with the item.", "");
        int Insert(const wxString& item, /*unsigned*/ int pos, PyObject* clientData=NULL) {
            if (clientData) {
                wxPyClientData* data = new wxPyClientData(clientData);
                return self->Insert(item, pos, data);
            } else
                return self->Insert(item, pos);
        }
    }
    
    
    DocDeclStr(
        virtual void , Clear(),
        "Removes all items from the control.", "");
    
    DocDeclStr(
        virtual void , Delete(/*unsigned*/ int n),
        "Deletes the item at the zero-based index 'n' from the control. Note
that it is an error (signalled by a `wx.PyAssertionError` exception if
enabled) to remove an item with the index negative or greater or equal
than the number of items in the control.", "");
    


    
     %extend {
        DocStr(GetClientData,
               "Returns the client data associated with the given item, (if any.)", "");
        PyObject* GetClientData(/*unsigned*/ int n) {
            wxPyClientData* data = (wxPyClientData*)self->GetClientObject(n);
            if (data) {
                Py_INCREF(data->m_obj);
                return data->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        DocStr(SetClientData,
               "Associate the given client data with the item at position n.", "");
        void SetClientData(/*unsigned*/ int n, PyObject* clientData) {
            wxPyClientData* data = new wxPyClientData(clientData);
            self->SetClientObject(n, data);
        }
    }


    
    DocDeclStr(
        virtual /*unsigned*/ int , GetCount() const,
        "Returns the number of items in the control.", "");
    
    DocDeclStr(
        bool , IsEmpty() const,
        "Returns True if the control is empty or False if it has some items.", "");
    
    DocDeclStr(
        virtual wxString , GetString(/*unsigned*/ int n) const,
        "Returns the label of the item with the given index.", "");
    
    DocDeclStr(
        wxArrayString , GetStrings() const,
        "", "");
    
    DocDeclStr(
        virtual void , SetString(/*unsigned*/ int n, const wxString& s),
        "Sets the label for the given item.", "");
    
    DocDeclStr(
        virtual int , FindString(const wxString& s) const,
        "Finds an item whose label matches the given string.  Returns the
zero-based position of the item, or ``wx.NOT_FOUND`` if the string was not
found.", "");
    

    DocDeclStr(
        virtual void , SetSelection(int n),
        "Sets the item at index 'n' to be the selected item.", "");
    
    DocDeclStr(
        virtual int , GetSelection() const,
        "Returns the index of the selected item or ``wx.NOT_FOUND`` if no item
is selected.", "");
    

    bool SetStringSelection(const wxString& s);

    DocDeclStr(
        wxString , GetStringSelection() const,
        "Returns the label of the selected item or an empty string if no item
is selected.", "");
    

    DocDeclStr(
        void , Select(int n),
        "This is the same as `SetSelection` and exists only because it is
slightly more natural for controls which support multiple selection.", "");
    

    %pythoncode {
        def GetItems(self):
            """Return a list of the strings in the control"""
            return [self.GetString(i) for i in xrange(self.GetCount())]
            
        def SetItems(self, items):
            """Clear and set the strings in the control from a list"""
            self.Clear()
            for i in items:
                self.Append(i)        
    }
    
    %property(Count, GetCount, doc="See `GetCount`");
    %property(Items, GetItems, SetItems, doc="See `GetItems` and `SetItems`");
    %property(Selection, GetSelection, SetSelection, doc="See `GetSelection` and `SetSelection`");
    %property(StringSelection, GetStringSelection, SetStringSelection, doc="See `GetStringSelection` and `SetStringSelection`");
    %property(Strings, GetStrings, doc="See `GetStrings`");
    
};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxControlWithItems,
"wx.ControlWithItems combines the ``wx.ItemContainer`` class with the
wx.Control class, and is used for the base class of various controls
that have items.", "");

class wxControlWithItems : public wxControl, public wxItemContainer
{
public:
};

//---------------------------------------------------------------------------

