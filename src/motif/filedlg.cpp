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

#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/filedlg.h"
#include "wx/intl.h"

#include <Xm/Xm.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxFileDialog, wxDialog)
#endif

char *wxFileSelector(const char *title,
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
        strcpy(wxBuffer, (const char *)fileDialog.GetPath());
        return wxBuffer;
    }
    else
        return NULL;
}

char *wxFileSelectorEx(const char *title,
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
        strcpy(wxBuffer, (const char *)fileDialog.GetPath());
        return wxBuffer;
    }
    else
        return NULL;
}

// TODO: Motif file selector code
#if 0
char *wxFileSelectorAnswer = NULL;
Bool wxFileSelectorReturned = FALSE;

void wxFileSelCancel(Widget fs, XtPointer client_data, XmFileSelectionBoxCallbackStruct *cbs)
{
  wxFileSelectorAnswer = NULL;
  wxFileSelectorReturned = TRUE;
}

void wxFileSelOk(Widget fs, XtPointer client_data, XmFileSelectionBoxCallbackStruct *cbs)
{
  char *filename = NULL;
  if (!XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename)) {
    wxFileSelectorAnswer = NULL;
    wxFileSelectorReturned = TRUE;
  } else {
    if (filename) {
      if (wxFileSelectorAnswer) delete[] wxFileSelectorAnswer;
      wxFileSelectorAnswer = copystring(filename);
      XtFree(filename);
    }
    wxFileSelectorReturned = TRUE;
  }
}


char *wxMotifFileSelector(char *message,
                     char *default_path, char *default_filename, 
                     char *default_extension, char *wildcard, int flags,
                     wxWindow *parent, int x, int y)
{
  wxBeginBusyCursor();
  static char fileBuf[512];
  Widget parentWidget = 0;
  if (parent)
  {
    if (parent->IsKindOf(CLASSINFO(wxFrame)))
      parentWidget = ((wxFrame *)parent)->frameShell;
    else if (parent->IsKindOf(CLASSINFO(wxDialogBox)))
      parentWidget = ((wxDialogBox *)parent)->dialogShell;
    else
      parentWidget = (Widget)parent->handle;
  }
  else if (wxTheApp->wx_frame)
    parentWidget = wxTheApp->wx_frame->frameShell;

  Widget fileSel = XmCreateFileSelectionDialog(parentWidget, "file_selector", NULL, 0);
  XtUnmanageChild(XmFileSelectionBoxGetChild(fileSel, XmDIALOG_HELP_BUTTON));

  Widget shell = XtParent(fileSel);

  if (message)
    XtVaSetValues(shell, XmNtitle, message, NULL);

  char *entirePath = NULL;

  if (default_path && default_filename)
  {
    sprintf(wxBuffer, "%s/%s", default_path, default_filename);
    entirePath = copystring(wxBuffer);
  }
  else if (default_path && !default_filename)
  {
    sprintf(wxBuffer, "%s/", default_path);
    entirePath = copystring(wxBuffer);
  }
  else if ((!default_path) && default_filename)
  {
    sprintf(wxBuffer, "%s", default_filename);
    entirePath = copystring(wxBuffer);
  }

  if (entirePath)
  {
    Widget selectionWidget = XmFileSelectionBoxGetChild(fileSel, XmDIALOG_TEXT);
    XmTextSetString(selectionWidget, entirePath);
    delete[] entirePath;
  }

  if (wildcard)
  {
    if (default_path)
      sprintf(wxBuffer, "%s/%s", default_path, wildcard);
    else
      sprintf(wxBuffer, "%s", wildcard);

    Widget filterWidget = XmFileSelectionBoxGetChild(fileSel, XmDIALOG_FILTER_TEXT);
    XmTextSetString(filterWidget, wxBuffer);
    XmFileSelectionDoSearch(fileSel, NULL);
  }

  // Suggested by Terry Gitnick, 16/9/97, because of change in Motif
  // file selector on Solaris 1.5.1.
  if ( default_path )
  {
      XmString thePath = XmStringCreateLtoR (default_path,
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

  XtManageChild(fileSel);

  if (wxFileSelectorAnswer)
    delete[] wxFileSelectorAnswer;

  wxFileSelectorAnswer = NULL;
  wxFileSelectorReturned = FALSE;

  wxEndBusyCursor();

  XtAddGrab(XtParent(fileSel), TRUE, FALSE);
  XEvent event;
  while (!wxFileSelectorReturned)
  {
    XtAppNextEvent(wxTheApp->appContext, &event);
    XtDispatchEvent(&event);
  }
  XtRemoveGrab(XtParent(fileSel));

  XmUpdateDisplay(wxTheApp->topLevel); // Experimental

//  XtDestroyWidget(fileSel);
  XtUnmapWidget(XtParent(fileSel));
  XtDestroyWidget(XtParent(fileSel));

  // Now process all events, because otherwise
  // this might remain on the screen
  XSync(XtDisplay(wxTheApp->topLevel), FALSE);
  while (XtAppPending(wxTheApp->appContext))
  {
    XFlush(XtDisplay(wxTheApp->topLevel));
    XtAppNextEvent(wxTheApp->appContext, &event);
    XtDispatchEvent(&event);
  }

  if (wxFileSelectorAnswer)
  {
    strcpy(fileBuf, wxFileSelectorAnswer);
    return fileBuf;
  }
  else return NULL;
}
#endif

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
}

int wxFileDialog::ShowModal()
{
    // TODO
    return wxID_CANCEL;
}

// Generic file load/save dialog
static char *
wxDefaultFileSelector(bool load, const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
  char *ext = (char *)extension;
  
  char prompt[50];
  wxString str;
  if (load)
    str = "Load %s file";
  else
    str = "Save %s file";
  sprintf(prompt, wxGetTranslation(str), what);

  if (*ext == '.') ext++;
  char wild[60];
  sprintf(wild, "*.%s", ext);

  return wxFileSelector (prompt, NULL, default_name, ext, wild, 0, parent);
}

// Generic file load dialog
char *
wxLoadFileSelector(const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
  return wxDefaultFileSelector(TRUE, what, extension, default_name, parent);
}


// Generic file save dialog
char *
wxSaveFileSelector(const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
  return wxDefaultFileSelector(FALSE, what, extension, default_name, parent);
}


