/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.cpp
// Purpose:     wxMessageDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "msgdlg.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <X11/Xlib.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>

#include "wx/app.h"
#include "wx/intl.h"
#include "wx/motif/msgdlg.h"
#include "wx/motif/private.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_CLASS(wxMessageDialog, wxDialog)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the callbacks for message box buttons
// ----------------------------------------------------------------------------

// the common part
static void msgboxCallBack(Widget w, int client_data, int id)
{
    // close the dialog
    XtUnmanageChild(w);

    wxMessageDialog *dlg = (wxMessageDialog *)client_data;
    dlg->SetResult(id);
}

static void msgboxCallBackOk(Widget w,
                             int client_data,
                             XmAnyCallbackStruct *call_data)
{
    msgboxCallBack(w, client_data, wxID_OK);
}

static void msgboxCallBackCancel(Widget w,
                                 int client_data,
                                 XmAnyCallbackStruct *call_data)
{
    msgboxCallBack(w, client_data, wxID_CANCEL);
}

static void msgboxCallBackClose(Widget w,
                                int client_data,
                                XmAnyCallbackStruct *call_data)
{
    msgboxCallBack(w, client_data, wxID_CANCEL);
}

// ----------------------------------------------------------------------------
// wxMessageDialog
// ----------------------------------------------------------------------------

wxMessageDialog::wxMessageDialog(wxWindow *parent,
                                 const wxString& message,
                                 const wxString& caption,
                                 long style,
                                 const wxPoint& pos)
{
    m_caption = caption;
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
}

int wxMessageDialog::ShowModal()
{
    Widget (*dialogCreateFunction)(Widget, String, ArgList, Cardinal) = NULL;
    if ( m_dialogStyle & wxYES_NO )
    {
        // if we have [Yes], it must be a question
        dialogCreateFunction = XmCreateQuestionDialog;

        // TODO we could support this by using the help button...
        wxASSERT_MSG( !(m_dialogStyle & wxCANCEL), "not supported" );
    }
    else if ( m_dialogStyle & wxICON_STOP )
    {
        // error dialog is the one with error icon...
        dialogCreateFunction = XmCreateErrorDialog;
    }
    else if ( m_dialogStyle & wxICON_EXCLAMATION )
    {
        // ...and the warning dialog too
        dialogCreateFunction = XmCreateWarningDialog;
    }
    else
    {
        // finally, use the info dialog by default
        dialogCreateFunction = XmCreateInformationDialog;
    }

    // prepare the arg list
    Arg args[2];
    int ac = 0;

    wxXmString text(m_message);
    wxXmString title(m_caption);
    XtSetArg(args[ac], XmNmessageString, text()); ac++;
    XtSetArg(args[ac], XmNdialogTitle, title()); ac++;

    // do create message box
    Widget wParent = m_parent ? GetWidget(m_parent) : NULL;
    if ( !wParent )
    {
        wxWindow *window = wxTheApp->GetTopWindow();
        if ( !window )
        {
            wxFAIL_MSG("can't show message box without parent window");

            return wxID_CANCEL;
        }

        wParent = GetWidget(window);
    }

    Widget wMsgBox = (*dialogCreateFunction)(wParent, "", args, ac);

    wxCHECK_MSG( wMsgBox, wxID_CANCEL, "msg box creation failed" );

    // remove the [Help] button which wouldn't do anything anyhow
    XtUnmanageChild(XmMessageBoxGetChild(wMsgBox, XmDIALOG_HELP_BUTTON));

    // and the [Cancel] button too if we were not asked for it
    if ( !(m_dialogStyle & wxCANCEL) )
    {
        Widget wBtnCancel = XmMessageBoxGetChild(wMsgBox,
                                                 XmDIALOG_CANCEL_BUTTON);

        // ... unless it's a wxYES_NO dialog in which case we just rename
        // [Cancel] to [No] instead
        if ( m_dialogStyle & wxYES_NO )
        {
            Widget wBtnOk = XmMessageBoxGetChild(wMsgBox,
                                                 XmDIALOG_OK_BUTTON);

            wxXmString yes(_("Yes")), no(_("No"));
            XtVaSetValues(wBtnOk, XmNlabelString, yes(), NULL);
            XtVaSetValues(wBtnCancel, XmNlabelString, no(), NULL);
        }
        else
        {
            XtUnmanageChild(wBtnCancel);
        }
    }

    // set the callbacks for the message box buttons
    XtAddCallback(wMsgBox, XmNokCallback,
                  (XtCallbackProc)msgboxCallBackOk, (XtPointer)this);
    XtAddCallback(wMsgBox, XmNcancelCallback,
                  (XtCallbackProc)msgboxCallBackCancel, (XtPointer)this);

    XtAddCallback(wMsgBox, XmNunmapCallback,
                  (XtCallbackProc)msgboxCallBackClose, (XtPointer)this);

    // show it as a modal dialog
    XtManageChild(wMsgBox);
    XtAddGrab(wMsgBox, True, False);

    // the m_result will be changed when message box goes away
    m_result = -1;

    // local message loop
    XtAppContext context = XtWidgetToApplicationContext(wParent);
    XEvent event;
    while ( m_result == -1 )
    {
        XtAppNextEvent(context, &event);
        XtDispatchEvent(&event);
    }

    // translate the result if necessary
    if ( m_dialogStyle & wxYES_NO )
    {
        if ( m_result == wxID_OK )
            m_result = wxID_YES;
        else if ( m_result == wxID_CANCEL )
            m_result = wxID_NO;
    }

    return m_result;
}

