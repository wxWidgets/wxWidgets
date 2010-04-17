/*
 *  Name:        wx/qt/defs.h
 *  Author:      Peter Most
 *  RCS-ID:      $Id$
 *  Copyright:   (c) Peter Most
 *  Licence:     wxWindows licence
 */

#ifndef _WX_QT_DEFS_H_
#define _WX_QT_DEFS_H_

#ifdef __cplusplus

typedef class QWidget *WXWidget;

void wxMissingImplementation( const char fileName[], unsigned lineNumber,
    const char feature[] );

#define wxMISSING_IMPLEMENTATION( feature )\
    wxMissingImplementation( __FILE__, __LINE__, feature )

#endif 

#endif /* _WX_QT_DEFS_H_ */
