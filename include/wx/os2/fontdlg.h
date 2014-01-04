/////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/fontdlg.h
// Purpose:     wxFontDialog class. Use generic version if no
//              platform-specific implementation.
// Author:      David Webster
// Modified by:
// Created:     10/06/99
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTDLG_H_
#define _WX_FONTDLG_H_

#include "wx/dialog.h"

/*
 * Font dialog
 */

class WXDLLIMPEXP_CORE wxFontDialog: public wxFontDialogBase
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

protected:
    DECLARE_DYNAMIC_CLASS(wxFontDialog)
}; // end of CLASS wxFontDialog

#endif
    // _WX_FONTDLG_H_
