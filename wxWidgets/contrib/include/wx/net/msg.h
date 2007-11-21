/////////////////////////////////////////////////////////////////////////////
// Name:        msg.h
// Purpose:     wxMailMessage
// Author:      Julian Smart
// Modified by:
// Created:     2001-08-21
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSG_H_
#define _WX_MSG_H_

#ifdef WXMAKINGDLL_NETUTILS
    #define WXDLLIMPEXP_NETUTILS WXEXPORT
    #define WXDLLIMPEXP_DATA_NETUTILS(type) WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_NETUTILS WXIMPORT
    #define WXDLLIMPEXP_DATA_NETUTILS(type) WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_NETUTILS
    #define WXDLLIMPEXP_DATA_NETUTILS(type) type
#endif


/*
 * wxMailMessage
 * Encapsulates an email message
 */

class WXDLLIMPEXP_NETUTILS wxMailMessage
{
public:

    // A common usage
    wxMailMessage(const wxString& subject, const wxString& to,
        const wxString& body, const wxString& from = wxEmptyString,
        const wxString& attachment = wxEmptyString,
        const wxString& attachmentTitle = wxEmptyString)
    {
        m_to.Add(to);
        m_subject = subject;
        m_body = body;
        m_from = from;
        if (!attachment.IsEmpty())
        {
            m_attachments.Add(attachment);
            m_attachmentTitles.Add(attachmentTitle);
        }
    }

    wxMailMessage() {};

//// Accessors

    void AddTo(const wxString& to) { m_to.Add(to); }
    void AddCc(const wxString& cc) { m_cc.Add(cc); }
    void AddBcc(const wxString& bcc) { m_bcc.Add(bcc); }
    void AddAttachment(const wxString& attach, const wxString& title = wxEmptyString)
    { m_attachments.Add(attach); m_attachmentTitles.Add(title); }

    void SetSubject(const wxString& subject) { m_subject = subject; }
    void SetBody(const wxString& body) { m_body = body; }
    void SetFrom(const wxString& from) { m_from = from; }

public:
    wxArrayString  m_to;               //The To: Recipients
    wxString       m_from;             //The From: email address (optional)
    wxArrayString  m_cc;               //The CC: Recipients
    wxArrayString  m_bcc;              //The BCC Recipients
    wxString       m_subject;         //The Subject of the message
    wxString       m_body;            //The Body of the message
    wxArrayString  m_attachments;      //Files to attach to the email
    wxArrayString  m_attachmentTitles; //Titles to use for the email file attachments
};

#endif // _WX_MSG_H_

