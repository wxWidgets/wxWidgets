 /////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/dirdlg.mm
// Purpose:     wxMessageDialog for wxCocoa
// Author:      Gareth Simpson
// Created:     2007-10-09
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MSGDLG


#include "wx/msgdlg.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif


#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#import <AppKit/NSAlert.h>
// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_CLASS(wxCocoaMessageDialog, wxDialog)

// ----------------------------------------------------------------------------
// wxDirDialog
// ----------------------------------------------------------------------------

wxCocoaMessageDialog::wxCocoaMessageDialog(wxWindow *parent,
                        const wxString& message,
                        const wxString& caption,
                        long style,
                        const wxPoint& pos) : wxDialog(parent,wxID_ANY,caption, pos, wxDefaultSize, style)
{

    m_caption = caption;
    m_message = message;

    //wxTopLevelWindows.Append((wxWindowBase*)this);
    wxTopLevelWindows.Append(this);

    wxASSERT(CreateBase(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,style,wxDefaultValidator,wxDialogNameStr));

    if ( parent )
        parent->AddChild(this);


    m_cocoaNSWindow = nil;
    m_cocoaNSView = nil;

    m_yes = _("Yes");
    m_no  = _("No");
    m_ok  = _("OK");
    m_cancel = _("Cancel");

    SetMessageDialogStyle(style);
}

wxCocoaMessageDialog::~wxCocoaMessageDialog()
{
}

int wxCocoaMessageDialog::ShowModal()
{
    wxAutoNSAutoreleasePool thePool;

    NSAlert *alert = [[[NSAlert alloc] init] autorelease];

    const long style = GetMessageDialogStyle();

    NSAlertStyle nsStyle = NSInformationalAlertStyle;
    if (style & wxICON_EXCLAMATION)
        nsStyle = NSWarningAlertStyle;
    else if (style & wxICON_HAND)
        nsStyle = NSCriticalAlertStyle;
    else if (style & wxICON_INFORMATION)
        nsStyle = NSInformationalAlertStyle;
    else if (style & wxICON_QUESTION)
        nsStyle = NSInformationalAlertStyle;

    [alert setAlertStyle:nsStyle];




    // work out what to display
    // if the extended text is empty then we use the caption as the title
    // and the message as the text (for backwards compatibility)
    // but if the extended message is not empty then we use the message as the title
    // and the extended message as the text because that makes more sense
    if (m_extendedMessage.empty())
    {
        [alert setMessageText:wxNSStringWithWxString(m_caption)];
        [alert setInformativeText:wxNSStringWithWxString(m_message)];
    }
    else
    {
        [alert setMessageText:wxNSStringWithWxString(m_message)];
        [alert setInformativeText:wxNSStringWithWxString(m_extendedMessage)];
    }

    //    The wxReturn value corresponding to each button
    int buttonId[4] = { 0, 0, 0, wxID_CANCEL /* time-out */ };
    if (style & wxYES_NO)
    {
        if ( style & wxNO_DEFAULT )
        {
            [alert addButtonWithTitle:wxNSStringWithWxString(m_no)];
            [alert addButtonWithTitle:wxNSStringWithWxString(m_yes)];
            buttonId[0] = wxID_NO;
            buttonId[1] = wxID_YES;
        }
        else
        {
            [alert addButtonWithTitle:wxNSStringWithWxString(m_yes)];
            [alert addButtonWithTitle:wxNSStringWithWxString(m_no)];
            buttonId[0] = wxID_YES;
            buttonId[1] = wxID_NO;
        }
        if (style & wxCANCEL)
        {
            [alert addButtonWithTitle:wxNSStringWithWxString(m_cancel)];
            buttonId[2] = wxID_CANCEL;
        }
    }
    else
    {
        // the MSW implementation even shows an OK button if it is not specified, we'll do the same
        buttonId[0] = wxID_OK;
        // using null as default title does not work on earlier systems
        [alert addButtonWithTitle:wxNSStringWithWxString(m_ok)];
        if (style & wxCANCEL)
        {
            [alert addButtonWithTitle:wxNSStringWithWxString(m_cancel)];
            buttonId[1] = wxID_CANCEL;
        }
    }

    int ret = [alert runModal];


    return buttonId[ret-NSAlertFirstButtonReturn];
}

bool wxCocoaMessageDialog::SetYesNoLabels(const wxString& yes,const wxString& no)
{
    m_yes = yes;
    m_yes.Replace(_("&"),_(""));
    m_no = no;
    m_no.Replace(_("&"),_(""));
    return true;
}
bool wxCocoaMessageDialog::SetYesNoCancelLabels(const wxString& yes, const wxString& no, const wxString& cancel)
{
    m_yes = yes;
    m_yes.Replace(_("&"),_(""));
    m_no = no;
    m_no.Replace(_("&"),_(""));
    m_cancel = cancel;
    m_cancel.Replace(_("&"),_(""));
    return true;
}
bool wxCocoaMessageDialog::SetOKLabel(const wxString& ok)
{
    m_ok = ok;
    m_ok.Replace(_("&"),_(""));
    return true;
}
bool wxCocoaMessageDialog::SetOKCancelLabels(const wxString& ok, const wxString& cancel)
{
    m_ok = ok;
    m_ok.Replace(_("&"),_(""));
    m_cancel = cancel;
    m_cancel.Replace(_("&"),_(""));
    return true;
}

#endif // wxUSE_DIRDLG

