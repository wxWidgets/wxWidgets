/////////////////////////////////////////////////////////////////////////////
// Name:        _accel.i
// Purpose:     SWIG interface for wxAcceleratorTable
//
// Author:      Robin Dunn
//
// Created:     03-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%typemap(in) (int n, const wxAcceleratorEntry* entries) {
    $2 = wxAcceleratorEntry_LIST_helper($input);
    if ($2) $1 = PyList_Size($input);
    else    $1 = 0;
}

%typemap(freearg) wxAcceleratorEntry* entries {
     delete [] $1;
}



//---------------------------------------------------------------------------
%newgroup;


class wxAcceleratorEntry {
public:
    wxAcceleratorEntry(int flags = 0, int keyCode = 0, int cmd = 0, wxMenuItem *item = NULL);
    ~wxAcceleratorEntry();

    void Set(int flags, int keyCode, int cmd, wxMenuItem *item = NULL);

    void SetMenuItem(wxMenuItem *item);
    wxMenuItem *GetMenuItem() const;

    int GetFlags();
    int GetKeyCode();
    int GetCommand();
};


class wxAcceleratorTable : public wxObject {
public:
    // Can also accept a list of 3-tuples
    wxAcceleratorTable(int n, const wxAcceleratorEntry* entries);
    ~wxAcceleratorTable();

    bool Ok() const;
};

 
%immutable;
// See also wxPy_ReinitStockObjects in helpers.cpp
const wxAcceleratorTable wxNullAcceleratorTable;
%mutable;


wxAcceleratorEntry *wxGetAccelFromString(const wxString& label);

//---------------------------------------------------------------------------
