/////////////////////////////////////////////////////////////////////////////
// Name:        image.i
// Purpose:     SWIG interface file for wxImage, wxImageHandler, etc.
//
// Author:      Robin Dunn
//
// Created:     28-Apr-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module image

%{
#include "helpers.h"
#include <wx/image.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i

//---------------------------------------------------------------------------

class wxImageHandler {
public:
    wxImageHandler();
    wxString GetName();
    wxString GetExtension();
    long GetType();
    wxString GetMimeType();

    //bool LoadFile(wxImage* image, wxInputStream& stream);
    //bool SaveFile(wxImage* image, wxOutputStream& stream);

    void SetName(const wxString& name);
    void SetExtension(const wxString& extension);
    void SetType(long type);
    void SetMimeType(const wxString& mimetype);
};

//---------------------------------------------------------------------------

class wxPNGHandler : public wxImageHandler {
public:
    wxPNGHandler();
};


class wxJPEGHandler : public wxImageHandler {
public:
    wxJPEGHandler();
};


class wxBMPHandler : public wxImageHandler {
public:
    wxBMPHandler();
};


class wxGIFHandler : public wxImageHandler {
public:
    wxGIFHandler();
};



//---------------------------------------------------------------------------

class wxImage {
public:
    wxImage( const wxString& name, long type = wxBITMAP_TYPE_PNG );
    ~wxImage();

    wxBitmap ConvertToBitmap();
    void Create( int width, int height );
    void Destroy();
    wxImage Scale( int width, int height );

    void SetRGB( int x, int y, unsigned char r, unsigned char g, unsigned char b );
    unsigned char GetRed( int x, int y );
    unsigned char GetGreen( int x, int y );
    unsigned char GetBlue( int x, int y );

    bool LoadFile( const wxString& name, long type = wxBITMAP_TYPE_PNG );
    %name(LoadMimeFile)bool LoadFile( const wxString& name, const wxString& mimetype );

    bool SaveFile( const wxString& name, int type );
    %name(SaveMimeFile)bool SaveFile( const wxString& name, const wxString& mimetype );

    bool Ok();
    int GetWidth();
    int GetHeight();

    unsigned char *GetData();
    void SetData( unsigned char *data );

    void SetMaskColour( unsigned char r, unsigned char g, unsigned char b );
    unsigned char GetMaskRed();
    unsigned char GetMaskGreen();
    unsigned char GetMaskBlue();
    void SetMask( bool mask = TRUE );
    bool HasMask();

};

// Alternate constructors
%new wxImage* wxNullImage();
%new wxImage* wxEmptyImage(int width, int height);
%new wxImage* wxImageFromMime(const wxString& name, const wxString& mimetype);
%new wxImage* wxImageFromBitmap(const wxBitmap &bitmap);
%{
    wxImage* wxNullImage() {
        return new wxImage;
    }

    wxImage* wxEmptyImage(int width, int height) {
        return new wxImage(width, height);
    }

    wxImage* wxImageFromMime(const wxString& name, const wxString& mimetype) {
        return new wxImage(name, mimetype);
    }

    wxImage* wxImageFromBitmap(const wxBitmap &bitmap) {
        return new wxImage(bitmap);
    }
%}

// Static Methods
void wxImage_AddHandler(wxImageHandler *handler);
%{
    void wxImage_AddHandler(wxImageHandler *handler) {
        wxImage::AddHandler(handler);
    }
%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------




