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

%{
    DECLARE_DEF_STRING(ControlNameStr);
%}

//---------------------------------------------------------------------------
%newgroup;


//  This is the base class for a control or 'widget'.
//
//  A control is generally a small window which processes user input and/or
//  displays one or more item of data.
class wxControl : public wxWindow
{
public:
    %addtofunc wxControl         "self._setOORInfo(self)"
    %addtofunc wxControl()       ""

    wxControl(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos=wxDefaultPosition,
              const wxSize& size=wxDefaultSize,
              long style=0,
              const wxValidator& validator=wxDefaultValidator,
              const wxString& name=wxPyControlNameStr);

    %name(PreControl)wxControl();

    bool Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos=wxDefaultPosition,
                       const wxSize& size=wxDefaultSize,
                       long style=0,
                       const wxValidator& validator=wxDefaultValidator,
                       const wxString& name=wxPyControlNameStr);


    // Simulates the effect of the user issuing a command to the item. See
    // wxCommandEvent.
    void Command(wxCommandEvent& event);

    // Return a control's text.
    wxString GetLabel();

    // Sets the item's text.
    void SetLabel(const wxString& label);

};


//---------------------------------------------------------------------------
%newgroup;


// wxItemContainer defines an interface which is implemented by all controls
// which have string subitems each of which may be selected.
//
// Examples: wxListBox, wxCheckListBox, wxChoice and wxComboBox (which
// implements an extended interface deriving from this one)
class wxItemContainer
{
public:
    // wxItemContainer() { m_clientDataItemsType = wxClientData_None; }  ** It's an ABC


    // int Append(const wxString& item)
    // int Append(const wxString& item, void *clientData)
    // int Append(const wxString& item, wxClientData *clientData)
    %extend {
        // Adds the item to the control, associating the given data with the
        // item if not None.
        int Append(const wxString& item, PyObject* clientData=NULL) {
            if (clientData) {
                wxPyClientData* data = new wxPyClientData(clientData);
                return self->Append(item, data);
            } else
                return self->Append(item);
        }
    }
    
    // append several items at once to the control
    %name(AppendItems) void Append(const wxArrayString& strings);

    // int Insert(const wxString& item, int pos)
    // int Insert(const wxString& item, int pos, void *clientData);
    // int Insert(const wxString& item, int pos, wxClientData *clientData);
    %extend {
        int Insert(const wxString& item, int pos, PyObject* clientData=NULL) {
            if (clientData) {
                wxPyClientData* data = new wxPyClientData(clientData);
                return self->Insert(item, pos, data);
            } else
                return self->Insert(item, pos);
        }
    }
    
    
    // deleting items
    virtual void Clear();
    virtual void Delete(int n);


    // accessing strings
    virtual int GetCount() const;
    bool IsEmpty() const;
    virtual wxString GetString(int n) const;
    wxArrayString GetStrings() const;
    virtual void SetString(int n, const wxString& s);
    virtual int FindString(const wxString& s) const;

    
    // selection
    virtual void Select(int n);
    virtual int GetSelection() const;

    wxString GetStringSelection() const;


    // client data stuff
    %extend {
          // Returns the client data associated with the given item, (if any.)
        PyObject* GetClientData(int n) {
            wxPyClientData* data = (wxPyClientData*)self->GetClientObject(n);
            if (data) {
                Py_INCREF(data->m_obj);
                return data->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        // Associate the given client data with the item at position n.
        void SetClientData(int n, PyObject* clientData) {
            wxPyClientData* data = new wxPyClientData(clientData);
            self->SetClientObject(n, data);
        }
    }
      
};


//---------------------------------------------------------------------------
%newgroup;

class wxControlWithItems : public wxControl, public wxItemContainer
{
public:
};

//---------------------------------------------------------------------------

