/////////////////////////////////////////////////////////////////////////////
// Name:        _statusbar.i
// Purpose:     SWIG interface defs for wxStatusBar
//
// Author:      Robin Dunn
//
// Created:     24-Aug-1998
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


enum {
    wxSB_NORMAL,
    wxSB_FLAT,
    wxSB_RAISED
};



      
// wxStatusBar: a window near the bottom of the frame used for status info
MustHaveApp(wxStatusBar);
class wxStatusBar : public wxWindow
{
public:
    %pythonAppend wxStatusBar         "self._setOORInfo(self)"
    %pythonAppend wxStatusBar()       ""
    %typemap(out) wxStatusBar*;    // turn off this typemap
    
    wxStatusBar(wxWindow* parent, wxWindowID id = -1,
                long style = wxDEFAULT_STATUSBAR_STYLE,
                const wxString& name = wxPyStatusLineNameStr);
    %RenameCtor(PreStatusBar, wxStatusBar());

    // Turn it back on again
    %typemap(out) wxStatusBar* { $result = wxPyMake_wxObject($1, $owner); }

    bool Create(wxWindow* parent, wxWindowID id=-1,
                long style = wxST_SIZEGRIP,
                const wxString& name = wxPyStatusLineNameStr);

    // set the number of fields and call SetStatusWidths(widths) if widths are
    // given
    virtual void SetFieldsCount(int number = 1 /*, const int *widths = NULL*/);
    int GetFieldsCount() const;

    virtual void SetStatusText(const wxString& text, int number = 0);
    virtual wxString GetStatusText(int number = 0) const;

    void PushStatusText(const wxString& text, int number = 0);
    void PopStatusText(int number = 0);


    // set status field widths as absolute numbers: positive widths mean that
    // the field has the specified absolute width, negative widths are
    // interpreted as the sizer options, i.e. the extra space (total space
    // minus the sum of fixed width fields) is divided between the fields with
    // negative width according to the abs value of the width (field with width
    // -2 grows twice as much as one with width -1 &c)
    virtual void SetStatusWidths(int widths, const int* widths_field); 


    // Set the field style. Use either wxSB_NORMAL (default) for a standard 3D 
    // border around a field, wxSB_FLAT for no border around a field, so that it 
    // appears flat or wxSB_POPOUT to make the field appear raised.
    // Setting field styles only works on wxMSW
    virtual void SetStatusStyles(int styles, const int* styles_field);
    
    
    // Get the position and size of the field's internal bounding rectangle
    %extend {
        wxRect GetFieldRect(int i) {
            wxRect r;
            self->GetFieldRect(i, r);
            return r;
        }
    }
    
    // sets the minimal vertical size of the status bar
    virtual void SetMinHeight(int height);

    // get the dimensions of the horizontal and vertical borders
    virtual int GetBorderX() const;
    virtual int GetBorderY() const;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %pythoncode {
        def GetFields(self):
            """Return a list of field values in the status bar. """
            return [self.GetStatusText(i) for i in range(self.GetFieldsCount())]
            
        def SetFields(self, items):
            """Set the values of the statusbar fields from a list of strings. """
            self.SetFieldsCount(len(items))
            for i in range(len(items)):
                self.SetStatusText(items[i], i)
    }
    
    %property(BorderX, GetBorderX, doc="See `GetBorderX`");
    %property(BorderY, GetBorderY, doc="See `GetBorderY`");
    %property(FieldRect, GetFieldRect, doc="See `GetFieldRect`");
    %property(Fields, GetFields, SetFields, doc="See `GetFields` and `SetFields`");
    %property(FieldsCount, GetFieldsCount, SetFieldsCount, doc="See `GetFieldsCount` and `SetFieldsCount`");
    %property(StatusText, GetStatusText, SetStatusText, doc="See `GetStatusText` and `SetStatusText`");
};


//---------------------------------------------------------------------------
