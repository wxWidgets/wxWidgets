/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.cpp
// Purpose:     wxFileDialog
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "filedlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __VMS
#define XtDisplay XTDISPLAY
#define XtParent XTPARENT
#define XtWindow XTWINDOW
#endif

#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/filedlg.h"
#include "wx/intl.h"
#include "wx/app.h"
#include "wx/settings.h"
#include "wx/tokenzr.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/Label.h>
#include <Xm/BulletinB.h>
#include <Xm/Frame.h>
#include <Xm/Text.h>
#include <Xm/DialogS.h>
#include <Xm/FileSB.h>
#include <Xm/RowColumn.h>
#include <Xm/LabelG.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

IMPLEMENT_CLASS(wxFileDialog, wxFileDialogBase)

#define DEFAULT_FILE_SELECTOR_SIZE 0
// Let Motif defines the size of File
// Selector Box (if 1), or fix it to
// wxFSB_WIDTH x wxFSB_HEIGHT (if 0)
#define wxFSB_WIDTH                600
#define wxFSB_HEIGHT               500


wxString wxFileDialog::m_fileSelectorAnswer = "";
bool wxFileDialog::m_fileSelectorReturned = FALSE;

static void wxFileSelClose(Widget WXUNUSED(w),
                           void* WXUNUSED(client_data),
                           XmAnyCallbackStruct *WXUNUSED(call_data))
{
    wxFileDialog::m_fileSelectorAnswer = "";
    wxFileDialog::m_fileSelectorReturned = TRUE;
}

void wxFileSelCancel( Widget WXUNUSED(fs), XtPointer WXUNUSED(client_data),
                     XmFileSelectionBoxCallbackStruct *WXUNUSED(cbs) )
{
    wxFileDialog::m_fileSelectorAnswer = "";
    wxFileDialog::m_fileSelectorReturned = TRUE;
}

void wxFileSelOk(Widget WXUNUSED(fs), XtPointer WXUNUSED(client_data), XmFileSelectionBoxCallbackStruct *cbs)
{
    char *filename = NULL;
    if (!XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename)) {
        wxFileDialog::m_fileSelectorAnswer = "";
        wxFileDialog::m_fileSelectorReturned = TRUE;
    } else {
        if (filename) {
            wxFileDialog::m_fileSelectorAnswer = filename;
            XtFree(filename);
        }
        wxFileDialog::m_fileSelectorReturned = TRUE;
    }
}

static wxString ParseWildCard( const wxString& wild )
{
#ifdef __WXDEBUG__
    static const wxChar* msg =
        _T("Motif file dialog does not understand this ")
        _T("wildcard syntax");
#endif

    wxStringTokenizer tok( wild, _T("|") );

    wxCHECK_MSG( tok.CountTokens() <= 2, _T("*.*"), msg );

    if( tok.CountTokens() == 1 ) return wild;

    // CountTokens == 2
    tok.GetNextToken();
    wxStringTokenizer tok2( tok.GetNextToken(), _T(";") );

    wxCHECK_MSG( tok2.CountTokens() == 1, tok2.GetNextToken(), msg );
    return tok2.GetNextToken();
}

wxFileDialog::wxFileDialog(wxWindow *parent, const wxString& message,
                           const wxString& defaultDir, const wxString& defaultFileName, const wxString& wildCard,
                           long style, const wxPoint& pos)
             :wxFileDialogBase(parent, message, defaultDir, defaultFileName, wildCard, style, pos)
{
    m_filterIndex = 1;
}

static void wxChangeListBoxColours(wxWindow* WXUNUSED(win), Widget widget)
{
    wxDoChangeBackgroundColour((WXWidget) widget, *wxWHITE);

    // Change colour of the scrolled areas of the listboxes
    Widget listParent = XtParent (widget);
#if 0
    wxDoChangeBackgroundColour((WXWidget) listParent, *wxWHITE, TRUE);
#endif

    Widget hsb = (Widget) 0;
    Widget vsb = (Widget) 0;
    XtVaGetValues (listParent,
        XmNhorizontalScrollBar, &hsb,
        XmNverticalScrollBar, &vsb,
        NULL);

   /* TODO: should scrollbars be affected? Should probably have separate
    * function to change them (by default, taken from wxSystemSettings)
    */
    wxColour backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    wxDoChangeBackgroundColour((WXWidget) hsb, backgroundColour, TRUE);
    wxDoChangeBackgroundColour((WXWidget) vsb, backgroundColour, TRUE);

    if (hsb)
      XtVaSetValues (hsb,
        XmNtroughColor, backgroundColour.AllocColour(XtDisplay(hsb)),
        NULL);
    if (vsb)
      XtVaSetValues (vsb,
        XmNtroughColor, backgroundColour.AllocColour(XtDisplay(vsb)),
        NULL);
}

int wxFileDialog::ShowModal()
{
    wxBeginBusyCursor();

    //  static char fileBuf[512];
    Widget parentWidget = (Widget) 0;
    if (m_parent)
        parentWidget = (Widget) m_parent->GetTopWidget();
    else
        parentWidget = (Widget) wxTheApp->GetTopLevelWidget();
    // prepare the arg list
    Arg args[10];
    int ac = 0;

    wxComputeColours (XtDisplay(parentWidget), & m_backgroundColour,
        (wxColour*) NULL);

    XtSetArg(args[ac], XmNbackground, g_itemColors[wxBACK_INDEX].pixel); ac++;
    XtSetArg(args[ac], XmNtopShadowColor, g_itemColors[wxTOPS_INDEX].pixel); ac++;
    XtSetArg(args[ac], XmNbottomShadowColor, g_itemColors[wxBOTS_INDEX].pixel); ac++;
    XtSetArg(args[ac], XmNforeground, g_itemColors[wxFORE_INDEX].pixel); ac++;


    Widget fileSel = XmCreateFileSelectionDialog(parentWidget, "file_selector", args, ac);
    XtUnmanageChild(XmFileSelectionBoxGetChild(fileSel, XmDIALOG_HELP_BUTTON));

    Widget filterWidget = XmFileSelectionBoxGetChild(fileSel, XmDIALOG_FILTER_TEXT);
    Widget selectionWidget = XmFileSelectionBoxGetChild(fileSel, XmDIALOG_TEXT);
    Widget dirListWidget = XmFileSelectionBoxGetChild(fileSel, XmDIALOG_DIR_LIST);
    Widget fileListWidget = XmFileSelectionBoxGetChild(fileSel, XmDIALOG_LIST);

    // code using these vars disabled
#if 0
    Widget okWidget = XmFileSelectionBoxGetChild(fileSel, XmDIALOG_OK_BUTTON);
    Widget applyWidget = XmFileSelectionBoxGetChild(fileSel, XmDIALOG_APPLY_BUTTON);
    Widget cancelWidget = XmFileSelectionBoxGetChild(fileSel, XmDIALOG_CANCEL_BUTTON);
#endif


    Widget shell = XtParent(fileSel);

    if (!m_message.IsNull())
        XtVaSetValues(shell,
                      XmNtitle, wxConstCast(m_message.c_str(), char),
                      NULL);

    wxString entirePath("");

    if ((m_dir != "") && (m_fileName != ""))
    {
        entirePath = m_dir + wxString("/") + m_fileName;
    }
    else if ((m_dir != "") && (m_fileName == ""))
    {
        entirePath = m_dir + wxString("/");
    }
    else if ((m_dir == "") && (m_fileName != ""))
    {
        entirePath = m_fileName;
    }

    if (m_wildCard != "")
    {
        // return something understandable by Motif
        wxString wildCard = ParseWildCard( m_wildCard );
        wxString filter;
        if (m_dir != "")
            filter = m_dir + wxString("/") + wildCard;
        else
            filter = wildCard;

        XmTextSetString(filterWidget, wxConstCast(filter.c_str(), char));
        XmFileSelectionDoSearch(fileSel, NULL);
    }

    // Suggested by Terry Gitnick, 16/9/97, because of change in Motif
    // file selector on Solaris 1.5.1.
    if ( m_dir != "" )
    {
        wxXmString thePath( m_dir );

        XtVaSetValues (fileSel,
            XmNdirectory, thePath(),
            NULL);
    }

    if (entirePath != "")
    {
        XmTextSetString(selectionWidget,
                        wxConstCast(entirePath.c_str(), char));
    }

    XtAddCallback(fileSel, XmNcancelCallback, (XtCallbackProc)wxFileSelCancel, (XtPointer)NULL);
    XtAddCallback(fileSel, XmNokCallback, (XtCallbackProc)wxFileSelOk, (XtPointer)NULL);
    XtAddCallback(fileSel, XmNunmapCallback,
                  (XtCallbackProc)wxFileSelClose, (XtPointer)this);

    //#if XmVersion > 1000
    // I'm not sure about what you mean with XmVersion.
    // If this is for Motif1.1/Motif1.2, then check XmVersion>=1200
    // (Motif1.1.4 ==> XmVersion 1100 )
    // Nevertheless, I put here a #define, so anyone can choose in (I)makefile...
    //
#if !DEFAULT_FILE_SELECTOR_SIZE
    int width = wxFSB_WIDTH;
    int height = wxFSB_HEIGHT;
    XtVaSetValues(fileSel,
        XmNwidth, width,
        XmNheight, height,
        XmNresizePolicy, XmRESIZE_NONE,
        NULL);
#endif
    //    wxDoChangeBackgroundColour((WXWidget) fileSel, m_backgroundColour);
    wxDoChangeBackgroundColour((WXWidget) filterWidget, *wxWHITE);
    wxDoChangeBackgroundColour((WXWidget) selectionWidget, *wxWHITE);

    wxChangeListBoxColours(this, dirListWidget);
    wxChangeListBoxColours(this, fileListWidget);

    XtManageChild(fileSel);

    m_fileSelectorAnswer = "";
    m_fileSelectorReturned = FALSE;

    wxEndBusyCursor();

    XtAddGrab(XtParent(fileSel), TRUE, FALSE);
    XtAppContext context = (XtAppContext) wxTheApp->GetAppContext();
    XEvent event;
    while (!m_fileSelectorReturned)
    {
        XtAppNextEvent(context, &event);
        XtDispatchEvent(&event);
    }
    XtRemoveGrab(XtParent(fileSel));

    // XmUpdateDisplay((Widget) wxTheApp->GetTopLevelWidget()); // Experimental

    Display* display = XtDisplay(fileSel);

    XtUnmapWidget(XtParent(fileSel));
    XtDestroyWidget(XtParent(fileSel));

    // Now process all events, because otherwise
    // this might remain on the screen
    wxFlushEvents(display);

    m_path = m_fileSelectorAnswer;
    m_fileName = wxFileNameFromPath(m_fileSelectorAnswer);
    m_dir = wxPathOnly(m_path);

    if (m_fileName == "")
        return wxID_CANCEL;
    else
        return wxID_OK;
}

