#ifndef _WX_PRINTDLG_H_BASE_
#define _WX_PRINTDLG_H_BASE_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "printdlg.h"
#endif

#include "wx/defs.h"

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/event.h"
#include "wx/dialog.h"
#include "wx/intl.h"
#include "wx/cmndata.h"


// ---------------------------------------------------------------------------
// wxPrintDialogBase: interface for the common dialog for printing.
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxPrintDialogBase : public wxDialog
{
public:
    wxPrintDialogBase() { }
    wxPrintDialogBase(wxWindow *parent, wxWindowID id = -1, const wxString &title = _("Print"),
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE );
            
    virtual int ShowModal() = 0;

    virtual wxPrintDialogData& GetPrintDialogData() = 0;
    virtual wxPrintData& GetPrintData() = 0;
    virtual wxDC *GetPrintDC() = 0;
    
private:
    DECLARE_ABSTRACT_CLASS(wxPrintDialogBase)
    DECLARE_NO_COPY_CLASS(wxPrintDialogBase)
};

// ---------------------------------------------------------------------------
// wxPrintDialog: the common dialog for printing.
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxPrintDialog : public wxObject
{
public:
    wxPrintDialog(wxWindow *parent, wxPrintDialogData* data = NULL);
    wxPrintDialog(wxWindow *parent, wxPrintData* data);
    ~wxPrintDialog();

    virtual int ShowModal();

    virtual wxPrintDialogData& GetPrintDialogData();
    virtual wxPrintData& GetPrintData();
    virtual wxDC *GetPrintDC();
    
private:
    wxPrintDialogBase  *m_pimpl;
    
private:
    DECLARE_DYNAMIC_CLASS(wxPrintDialog)
    DECLARE_NO_COPY_CLASS(wxPrintDialog)
};

#endif

#if defined(__WXUNIVERSAL__) && (!defined(__WXMSW__) || wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXMSW__)
#include "wx/msw/printdlg.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXGTK__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXX11__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXMGL__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXMAC__)
#include "wx/mac/printdlg.h"
#elif defined(__WXPM__)
#include "wx/generic/prntdlgg.h"
#endif

#if (defined(__WXUNIVERSAL__) && (!defined(__WXMSW__) || wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW) ) || (!defined(__WXMSW__) && !defined(__WXMAC__))
#define wxPrintSetupDialog wxGenericPrintSetupDialog
#define wxPageSetupDialog wxGenericPageSetupDialog
#endif

#endif
    // _WX_PRINTDLG_H_BASE_
