/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.h
// Purpose:     wxFontDialog class. Use generic version if no
//              platform-specific implementation.
// Author:      David Webster
// Modified by:
// Created:     10/06/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTDLG_H_
#define _WX_FONTDLG_H_

#include "wx/dialog.h"
#include "wx/cmndata.h"

/*
 * Font dialog
 */

class WXDLLEXPORT wxFontDialog: public wxFontDialogBase
{
public:
    wxFontDialog() : wxFontDialogBase() { /* must be Create()d later */ }
    wxFontDialog (wxWindow* pParent) : wxFontDialogBase(pParent) { Create(pParent); }
    wxFontDialog( wxWindow*         pParent
                 ,const wxFontData& rData
                )
                : wxFontDialogBase( pParent
                                   ,rData
                                  )
    {
        Create( pParent
               ,rData
              );
    }

    virtual int ShowModal();

    //
    // Deprecated interface, don't use
    //
    wxFontDialog( wxWindow*         pParent
                 ,const wxFontData* pData
                )
                : wxFontDialogBase( pParent
                                   ,pData
                                  )
    {
        Create( pParent
               ,pData
              );
    }
protected:
    DECLARE_DYNAMIC_CLASS(wxFontDialog)
}; // end of CLASS wxFontDialog

#endif
    // _WX_FONTDLG_H_

