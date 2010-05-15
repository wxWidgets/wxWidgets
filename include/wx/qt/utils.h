///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/utils.h
// Purpose:     utility classes and/or functions
// Author:      Peter Most
// Created:     15/05/10
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_UTILS_H_
#define _WX_QT_UTILS_H_

void wxMissingImplementation( const char fileName[], unsigned lineNumber,
    const char feature[] );

#define wxMISSING_IMPLEMENTATION( feature )\
    wxMissingImplementation( __FILE__, __LINE__, feature )

#endif // _WX_QT_UTILS_H_
