/*-*- c++ -*-********************************************************
 * helpwxht.h - a help controller using wxHTML                      *
 *                                                                  *
 * (C) 1999 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$
 *******************************************************************/

#ifndef _WX_HELPWXHT_H_
#define _WX_HELPWXHT_H_

#if wxUSE_HELP
#if wxUSE_HTML

#ifdef __GNUG__
#   pragma interface "helpwxht.h"
#endif

#include "wx/generic/helphtml.h"


/**
   This class implements help via wxHTML.
   It requires the name of a directory containing the documentation
   and a file mapping numerical Section numbers to relative URLS.

   The map file contains two or three fields per line:
   numeric_id  relative_URL  [; comment/documentation]

   The numeric_id is the id used to look up the entry in
   DisplaySection()/DisplayBlock(). The relative_URL is a filename of
   an html file, relative to the help directory. The optional
   comment/documentation field (after a ';') is used for keyword
   searches, so some meaningful text here does not hurt.
   If the documentation itself contains a ';', only the part before
   that will be displayed in the listbox, but all of it used for search.

   Lines starting with ';' will be ignored.
*/

class WXDLLEXPORT wxHelpControllerHtml : public wxHTMLHelpControllerBase
{      
DECLARE_CLASS(wxHelpControllerHtml)
   public:
   wxHelpControllerHtml(void);
   ~wxHelpControllerHtml(void);

   /// Allows one to override the default settings for the help frame.
   virtual void SetFrameParameters(const wxString &title,
                                   const wxSize &size,
                                   const wxPoint &pos = wxDefaultPosition,
                                   bool newFrameEachTime = FALSE);
   /// Obtains the latest settings used by the help frame.
   virtual wxFrame * GetFrameParameters(wxSize *size = NULL,
                                        wxPoint *pos = NULL,
                                        bool *newFrameEachTime = NULL);
   
   
private:
   /// Call the browser using a relative URL.
   virtual bool DisplayHelp(wxString const &);
protected:
   friend class wxHelpFrame;
   class wxHelpFrame *m_Frame;
   wxString    m_FrameTitle;
   wxPoint     m_FramePosition;
   wxSize      m_FrameSize;
   bool        m_NewFrameEachTime;
   size_t      m_offset;
};

#endif // wxUSE_HELP
#endif // wxUSE_HTML

#endif // _WX_HELPWXHT_H_
