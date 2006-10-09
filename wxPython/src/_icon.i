/////////////////////////////////////////////////////////////////////////////
// Name:        _icon.i
// Purpose:     SWIG interface for wxIcon and related classes
//
// Author:      Robin Dunn
//
// Created:     7-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%{
#include <wx/iconbndl.h>
%}
//---------------------------------------------------------------------------


MustHaveApp(wxIcon);

class wxIcon : public wxGDIObject
{
public:
    wxIcon(const wxString& name, wxBitmapType type,
           int desiredWidth = -1, int desiredHeight = -1);
    ~wxIcon();

    // alternate constructors
    %RenameCtor(EmptyIcon, wxIcon());
    %RenameCtor(IconFromLocation,  wxIcon(const wxIconLocation& loc));
    %extend {
        %RenameCtor(IconFromBitmap, wxIcon(const wxBitmap& bmp))
        {
            wxIcon* icon = new wxIcon();
            icon->CopyFromBitmap(bmp);
            return icon;
        }
        %RenameCtor(IconFromXPMData, wxIcon(PyObject* listOfStrings))
        {
            char**  cArray = NULL;
            wxIcon* icon;

            cArray = ConvertListOfStrings(listOfStrings);
            if (! cArray)
                return NULL;
            icon = new wxIcon(cArray);
            delete [] cArray;
            return icon;
        }
    }

    
#ifndef __WXMAC__
    bool LoadFile(const wxString& name, wxBitmapType type);
#endif

    // wxGDIImage methods
#ifdef __WXMSW__
    long GetHandle();
    %extend {
        void SetHandle(long handle) { self->SetHandle((WXHANDLE)handle); }
    }
#endif
    bool IsOk();
    %pythoncode { Ok = IsOk }
    int GetWidth();
    int GetHeight();
    int GetDepth();
    void SetWidth(int w);
    void SetHeight(int h);
    void SetDepth(int d);
#ifdef __WXMSW__
    void SetSize(const wxSize& size);
#endif
    void CopyFromBitmap(const wxBitmap& bmp);

    %pythoncode { def __nonzero__(self): return self.IsOk() }
    
    %property(Depth, GetDepth, SetDepth, doc="See `GetDepth` and `SetDepth`");
    %property(Height, GetHeight, SetHeight, doc="See `GetHeight` and `SetHeight`");
    %property(Width, GetWidth, SetWidth, doc="See `GetWidth` and `SetWidth`");

};

//---------------------------------------------------------------------------

class wxIconLocation
{
public:
    // ctor takes the name of the file where the icon is
    %extend {
        wxIconLocation(const wxString* filename = &wxPyEmptyString, int num = 0) {
#ifdef __WXMSW__
            return new wxIconLocation(*filename, num);
#else
            return new wxIconLocation(*filename);
#endif
        }
    }

    ~wxIconLocation();


    // returns True if this object is valid/initialized
    bool IsOk() const;
    %pythoncode { def __nonzero__(self): return self.IsOk() }

    // set/get the icon file name
    void SetFileName(const wxString& filename);
    const wxString& GetFileName() const;

    %extend {
        void SetIndex(int num) {
#ifdef __WXMSW__
            self->SetIndex(num);
#else
            // do nothing
#endif
        }

        int GetIndex() {
#ifdef __WXMSW__
            return self->GetIndex();
#else
            return -1;
#endif
        }
    }

    %property(FileName, GetFileName, SetFileName, doc="See `GetFileName` and `SetFileName`");
    %property(Index, GetIndex, SetIndex, doc="See `GetIndex` and `SetIndex`");
    
};




//---------------------------------------------------------------------------

class wxIconBundle
{
public:
    // default constructor
    wxIconBundle();

    // initializes the bundle with the icon(s) found in the file
    %RenameCtor(IconBundleFromFile, wxIconBundle( const wxString& file, long type ));

    // initializes the bundle with a single icon
    %RenameCtor(IconBundleFromIcon, wxIconBundle( const wxIcon& icon ));

    ~wxIconBundle();

    // adds the icon to the collection, if the collection already
    // contains an icon with the same width and height, it is
    // replaced
    void AddIcon( const wxIcon& icon );

    // adds all the icons contained in the file to the collection,
    // if the collection already contains icons with the same
    // width and height, they are replaced
    %Rename(AddIconFromFile,void, AddIcon( const wxString& file, long type ));

    // returns the icon with the given size; if no such icon exists,
    // returns the icon with size wxSYS_ICON_[XY]; if no such icon exists,
    // returns the first icon in the bundle
    const wxIcon& GetIcon( const wxSize& size ) const;

    %property(Icon, GetIcon, doc="See `GetIcon`");
};

//---------------------------------------------------------------------------
