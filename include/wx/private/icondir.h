///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/icondir.h
// Purpose:     Declarations of structs used for loading MS icons
// Author:      wxWidgets team
// Created:     2017-05-19
// Copyright:   (c) 2017 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_ICONDIR_H_
#define _WX_PRIVATE_ICONDIR_H_

#include "wx/defs.h"          // wxUint* declarations


// Structs declared here are used for loading group icons from
// .ICO files or MS Windows resources.
// Icon entry and directory structs for .ICO files and
// MS Windows resources are very similar but not identical.

#if wxUSE_ICO_CUR

#if wxUSE_STREAMS

// icon entry in .ICO files
struct ICONDIRENTRY
{
    wxUint8         bWidth;               // Width of the image
    wxUint8         bHeight;              // Height of the image (times 2)
    wxUint8         bColorCount;          // Number of colors in image (0 if >=8bpp)
    wxUint8         bReserved;            // Reserved

    // these two are different in icons and cursors:
                                          // icon           or  cursor
    wxUint16        wPlanes;              // Color Planes   or  XHotSpot
    wxUint16        wBitCount;            // Bits per pixel or  YHotSpot

    wxUint32        dwBytesInRes;         // how many bytes in this resource?
    wxUint32        dwImageOffset;        // where in the file is this image
};

// icon directory in .ICO files
struct ICONDIR
{
    wxUint16     idReserved;   // Reserved
    wxUint16     idType;       // resource type (1 for icons, 2 for cursors)
    wxUint16     idCount;      // how many images?
};

#endif // wxUSE_STREAMS


#ifdef __WINDOWS__

#pragma pack(push)
#pragma pack(2)

// icon entry in MS Windows resources
struct GRPICONDIRENTRY
{
    wxUint8         bWidth;               // Width of the image
    wxUint8         bHeight;              // Height of the image (times 2)
    wxUint8         bColorCount;          // Number of colors in image (0 if >=8bpp)
    wxUint8         bReserved;            // Reserved

    // these two are different in icons and cursors:
                                          // icon           or  cursor
    wxUint16        wPlanes;              // Color Planes   or  XHotSpot
    wxUint16        wBitCount;            // Bits per pixel or  YHotSpot

    wxUint32        dwBytesInRes;         // how many bytes in this resource?

    wxUint16        nID;                  // actual icon resource ID
};

// icon directory in MS Windows resources
struct GRPICONDIR
{
    wxUint16        idReserved;   // Reserved
    wxUint16        idType;       // resource type (1 for icons, 2 for cursors)
    wxUint16        idCount;      // how many images?
    GRPICONDIRENTRY idEntries[1]; // The entries for each image
};

#pragma pack(pop)

#endif // __WINDOWS__

#endif // wxUSE_ICO_CUR

#endif // _WX_PRIVATE_ICONDIR_H_
