/////////////////////////////////////////////////////////////////////////////
// Name:        _gdiobj.i
// Purpose:     SWIG interface for wxGDIObject
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

MustHaveApp(wxGDIObject);

class wxGDIObject : public wxObject {
public:
    wxGDIObject();
    ~wxGDIObject();

    bool GetVisible();
    void SetVisible( bool visible );

    bool IsNull();

};

//---------------------------------------------------------------------------
