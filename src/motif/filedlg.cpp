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

#ifdef __GNUG__
#pragma implementation "filedlg.h"
#endif

#ifdef __VMS
#define XtDisplay XTDISPLAY
#define XtParent XTPARENT
#define XtWindow XTWINDOW
#endif

#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/filedlg.h"
#include "wx/intl.h"
#include "wx/app.h"
#include "wx/settings.h"

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

IMPLEMENT_CLASS(wxFileDialog, wxDialog)

#define DEFAULT_FILE_SELECTOR_SIZE 0
// Let Motif defines the size of File
// Selector Box (if 1), or fix it to
// wxFSB_WIDTH x wxFSB_HEIGHT (if 0)
#define wxFSB_WIDTH                600
#define wxFSB_HEIGHT               500


wxString wxFileSelector(const char *title,
                     const char *defaultDir, const char *defaultFileName,
                     const char *defaultExtension, const char *filter, int flags,
                     wxWindow *parent, int x, int y)
{
    // If there's a default extension specified but no filter, we create a suitable
    // filter.

    wxString filter2("");
    if ( defaultExtension && !filter )
        filter2 = wxString("*.") + wxString(defaultExtension) ;
    else if ( filter )
        filter2 = filter;

    wxString defaultDirString;
    if (defaultDir)
        defaultDirString = defaultDir;
    else
        defaultDirString = "";

    wxString defaultFilenameString;
    if (defaultFileName)
        defaultFilenameString = defaultFileName;
    else
        defaultFilenameString = "";

    wxFileDialog fileDialog(parent, title, defaultDirString, defaultFilenameString, filter2, flags, wxPoint(x, y));

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        return fileDialog.GetPath();
    }
    else
        return wxEmptyString;
}

wxString wxFileSelectorEx(const char *title,
                       const char *defaultDir,
                       const char *defaultFileName,
                       int* defaultFilterIndex,
                       const char *filter,
                       int       flags,
                       wxWindow* parent,
                       int       x,
                       int       y)

{
    wxFileDialog fileDialog(parent, title ? title : "", defaultDir ? defaultDir : "",
        defaultFileName ? defaultFileName : "", filter ? filter : "", flags, wxPoint(x, y));

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        *defaultFilterIndex = fileDialog.GetFilterIndex();
        return fileDialog.GetPath();
    }
    else
        return wxEmptyString;
}

wxString wxFileDialog::m_fileSelectorAnswer = "";
bool wxFileDialog::m_fileSelectorReturned = FALSE;

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

wxFileDialog::wxFileDialog(wxWindow *parent, const wxString& message,
                           const wxString& defaultDir, const wxString& defaultFileName, const wxString& wildCard,
                           long style, const wxPoint& pos)
{
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
    m_path = "";
    m_fileName = defaultFileName;
    m_dir = defaultDir;
    m_wildCard = wildCard;
    m_filterIndex = 1;
    m_pos = pos;
}

static void wxChangeListBoxColours(wxWindow* WXUNUSED(win), Widget widget)
{
    wxWindow::DoChangeBackgroundColour((WXWidget) widget, *wxWHITE);

    // Change colour of the scrolled areas of the listboxes
    Widget listParent = XtParent (widget);
    wxWindow::DoChangeBackgroundColour((WXWidget) listParent, *wxWHITE, TRUE);

    Widget hsb = (Widget) 0;
    Widget vsb = (Widget) 0;
    XtVaGetValues (listParent,
        XmNhorizontalScrollBar, &hsb,
        XmNverticalScrollBar, &vsb,
        NULL);

   /* TODO: should scrollbars be affected? Should probably have separate
    * function to change them (by default, taken from wxSystemSettings)
    */
    wxColour backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);
    wxWindow::DoChangeBackgroundColour((WXWidget) hsb, backgroundColour, TRUE);
    wxWindow::DoChangeBackgroundColour((WXWidget) vsb, backgroundColour, TRUE);

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
    {
        parentWidget = (Widget) m_parent->GetTopWidget();
    }
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
        XtVaSetValues(shell, XmNtitle, (char*) (const char*) m_message, NULL);

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

    if (entirePath != "")
    {
        XmTextSetString(selectionWidget, (char*) (const char*) entirePath);
    }

    if (m_wildCard != "")
    {
        wxString filter("");
        if (m_dir != "")
            filter = m_dir + wxString("/") + m_wildCard;
        else
            filter = m_wildCard;

        XmTextSetString(filterWidget, (char*) (const char*) filter);
        XmFileSelectionDoSearch(fileSel, NULL);
    }

    // Suggested by Terry Gitnick, 16/9/97, because of change in Motif
    // file selector on Solaris 1.5.1.
    if ( m_dir != "" )
    {
        XmString thePath = XmStringCreateLtoR ((char*) (const char*) m_dir,
            XmSTRING_DEFAULT_CHARSET);

        XtVaSetValues (fileSel,
            XmNdirectory, thePath,
            NULL);

        XmStringFree(thePath);
    }

    XtAddCallback(fileSel, XmNcancelCallback, (XtCallbackProc)wxFileSelCancel, (XtPointer)NULL);
    XtAddCallback(fileSel, XmNokCallback, (XtCallbackProc)wxFileSelOk, (XtPointer)NULL);

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
    //    DoChangeBackgroundColour((WXWidget) fileSel, m_backgroundColour);
    DoChangeBackgroundColour((WXWidget) filterWidget, *wxWHITE);
    DoChangeBackgroundColour((WXWidget) selectionWidget, *wxWHITE);

    wxChangeListBoxColours(this, dirListWidget);
    wxChangeListBoxColours(this, fileListWidget);

    XtManageChild(fileSel);

    m_fileSelectorAnswer = "";
    m_fileSelectorReturned = FALSE;

    wxEndBusyCursor();

    XtAddGrab(XtParent(fileSel), TRUE, FALSE);
    XEvent event;
    while (!m_fileSelectorReturned)
    {
        XtAppProcessEvent((XtAppContext) wxTheApp->GetAppContext(), XtIMAll);
    }
    XtRemoveGrab(XtParent(fileSel));

    XmUpdateDisplay((Widget) wxTheApp->GetTopLevelWidget()); // Experimental

    //  XtDestroyWidget(fileSel);
    XtUnmapWidget(XtParent(fileSel));
    XtDestroyWidget(XtParent(fileSel));

    // Now process all events, because otherwise
    // this might remain on the screen
    XSync(XtDisplay((Widget) wxTheApp->GetTopLevelWidget()), FALSE);
    while (XtAppPending((XtAppContext) wxTheApp->GetAppContext()))
    {
        XFlush(XtDisplay((Widget) wxTheApp->GetTopLevelWidget()));
        XtAppNextEvent((XtAppContext) wxTheApp->GetAppContext(), &event);
        XtDispatchEvent(&event);
    }

    m_path = m_fileSelectorAnswer;
    m_fileName = wxFileNameFromPath(m_fileSelectorAnswer);
    m_dir = wxPathOnly(m_path);

    if (m_fileName == "")
        return wxID_CANCEL;
    else
        return wxID_OK;
}

// Generic file load/save dialog
static wxString
wxDefaultFileSelector(bool load, const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
    char *ext = (char *)extension;

    wxString prompt;
    wxString str;
    if (load)
        str = _("Load %s file");
    else
        str = _("Save %s file");
    prompt.Printf(str, what);

    if (*ext == '.')
        ext++;
    wxString wild;
    wild.Printf("*.%s", ext);

    return wxFileSelector (prompt, NULL, default_name, ext, wild, 0, parent);
}

// Generic file load dialog
wxString wxLoadFileSelector(const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
    return wxDefaultFileSelector(TRUE, what, extension, default_name, parent);
}


// Generic file save dialog
wxString wxSaveFileSelector(const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
    return wxDefaultFileSelector(FALSE, what, extension, default_name, parent);
}


