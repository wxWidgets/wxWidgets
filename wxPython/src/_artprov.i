/////////////////////////////////////////////////////////////////////////////
// Name:        _artprov.i
// Purpose:     SWIG interface stuff for wxArtProvider
//
// Author:      Robin Dunn
//
// Created:     18-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/artprov.h>
%}

//---------------------------------------------------------------------------


// Art clients
MAKE_CONST_WXSTRING(ART_TOOLBAR);
MAKE_CONST_WXSTRING(ART_MENU);
MAKE_CONST_WXSTRING(ART_FRAME_ICON);
MAKE_CONST_WXSTRING(ART_CMN_DIALOG);
MAKE_CONST_WXSTRING(ART_HELP_BROWSER);
MAKE_CONST_WXSTRING(ART_MESSAGE_BOX);
MAKE_CONST_WXSTRING(ART_OTHER);

// Art IDs
MAKE_CONST_WXSTRING(ART_ADD_BOOKMARK);
MAKE_CONST_WXSTRING(ART_DEL_BOOKMARK);
MAKE_CONST_WXSTRING(ART_HELP_SIDE_PANEL);
MAKE_CONST_WXSTRING(ART_HELP_SETTINGS);
MAKE_CONST_WXSTRING(ART_HELP_BOOK);
MAKE_CONST_WXSTRING(ART_HELP_FOLDER);
MAKE_CONST_WXSTRING(ART_HELP_PAGE);
MAKE_CONST_WXSTRING(ART_GO_BACK);
MAKE_CONST_WXSTRING(ART_GO_FORWARD);
MAKE_CONST_WXSTRING(ART_GO_UP);
MAKE_CONST_WXSTRING(ART_GO_DOWN);
MAKE_CONST_WXSTRING(ART_GO_TO_PARENT);
MAKE_CONST_WXSTRING(ART_GO_HOME);
MAKE_CONST_WXSTRING(ART_FILE_OPEN);
MAKE_CONST_WXSTRING(ART_PRINT);
MAKE_CONST_WXSTRING(ART_HELP);
MAKE_CONST_WXSTRING(ART_TIP);
MAKE_CONST_WXSTRING(ART_REPORT_VIEW);
MAKE_CONST_WXSTRING(ART_LIST_VIEW);
MAKE_CONST_WXSTRING(ART_NEW_DIR);
MAKE_CONST_WXSTRING(ART_FOLDER);
MAKE_CONST_WXSTRING(ART_GO_DIR_UP);
MAKE_CONST_WXSTRING(ART_EXECUTABLE_FILE);
MAKE_CONST_WXSTRING(ART_NORMAL_FILE);
MAKE_CONST_WXSTRING(ART_TICK_MARK);
MAKE_CONST_WXSTRING(ART_CROSS_MARK);
MAKE_CONST_WXSTRING(ART_ERROR);
MAKE_CONST_WXSTRING(ART_QUESTION);
MAKE_CONST_WXSTRING(ART_WARNING);
MAKE_CONST_WXSTRING(ART_INFORMATION);
MAKE_CONST_WXSTRING(ART_MISSING_IMAGE);

//---------------------------------------------------------------------------

%{  // Python aware wxArtProvider
class wxPyArtProvider : public wxArtProvider  {
public:

    virtual wxBitmap CreateBitmap(const wxArtID& id,
                                  const wxArtClient& client,
                                  const wxSize& size) {
        wxBitmap rval = wxNullBitmap;
        wxPyBeginBlockThreads();
        if ((wxPyCBH_findCallback(m_myInst, "CreateBitmap"))) {
            PyObject* so = wxPyConstructObject((void*)&size, wxT("wxSize"), 0);
            PyObject* ro;
            wxBitmap* ptr;
            PyObject* s1, *s2;
            s1 = wx2PyString(id);
            s2 = wx2PyString(client);
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(OOO)", s1, s2, so));
            Py_DECREF(so);
            Py_DECREF(s1);
            Py_DECREF(s2);
            if (ro) {
                if (wxPyConvertSwigPtr(ro, (void**)&ptr, wxT("wxBitmap")))
                    rval = *ptr;
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads();
        return rval;
    }

    PYPRIVATE;
};
%}

// The one for SWIG to see
%name(ArtProvider) class wxPyArtProvider /*: public wxObject*/
{
public:

    %addtofunc wxPyArtProvider "self._setCallbackInfo(self, ArtProvider)"
    wxPyArtProvider();
    ~wxPyArtProvider();
    
    void _setCallbackInfo(PyObject* self, PyObject* _class);

    // Add new provider to the top of providers stack.
    static void PushProvider(wxPyArtProvider *provider);

    // Remove latest added provider and delete it.
    static bool PopProvider();

    // Remove provider. The provider must have been added previously!
    // The provider is _not_ deleted.
    static bool RemoveProvider(wxPyArtProvider *provider);

    // Query the providers for bitmap with given ID and return it. Return
    // wxNullBitmap if no provider provides it.
    static wxBitmap GetBitmap(const wxString& id,
                              const wxString& client = wxPyART_OTHER,
                              const wxSize& size = wxDefaultSize);

    // Query the providers for icon with given ID and return it. Return
    // wxNullIcon if no provider provides it.
    static wxIcon GetIcon(const wxString& id,
                          const wxString& client = wxPyART_OTHER,
                          const wxSize& size = wxDefaultSize);

};



//---------------------------------------------------------------------------
%init %{
    wxPyPtrTypeMap_Add("wxArtProvider", "wxPyArtProvider");
%}
//---------------------------------------------------------------------------
