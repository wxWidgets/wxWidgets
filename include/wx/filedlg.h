#ifndef _WX_FILEDLG_H_BASE_
#define _WX_FILEDLG_H_BASE_

enum
{
    wxOPEN  = 1,
    wxSAVE = 2,
    wxOVERWRITE_PROMPT = 4,
    wxHIDE_READONLY = 8,
    wxFILE_MUST_EXIST = 16,
    wxMULTIPLE = 32
};

#if defined(__WXMSW__)
#include "wx/msw/filedlg.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/filedlg.h"
#elif defined(__WXGTK__)
#include "wx/generic/filedlgg.h"
#elif defined(__WXQT__)
#include "wx/qt/filedlg.h"
#elif defined(__WXMAC__)
#include "wx/mac/filedlg.h"
#elif defined(__WXPM__)
#include "wx/os2/filedlg.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/filedlg.h"
#endif


#endif
    // _WX_FILEDLG_H_BASE_
