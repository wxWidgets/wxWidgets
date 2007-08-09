/*
                wxActiveX Library Licence, Version 3
                ====================================

  Copyright (C) 2003 Lindsay Mathieson [, ...]

  Everyone is permitted to copy and distribute verbatim copies
  of this licence document, but changing it is not allowed.

                       wxActiveX LIBRARY LICENCE
     TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
  
  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public Licence as published by
  the Free Software Foundation; either version 2 of the Licence, or (at
  your option) any later version.
  
  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
  General Public Licence for more details.

  You should have received a copy of the GNU Library General Public Licence
  along with this software, usually in a file named COPYING.LIB.  If not,
  write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA 02111-1307 USA.

  EXCEPTION NOTICE

  1. As a special exception, the copyright holders of this library give
  permission for additional uses of the text contained in this release of
  the library as licenced under the wxActiveX Library Licence, applying
  either version 3 of the Licence, or (at your option) any later version of
  the Licence as published by the copyright holders of version 3 of the
  Licence document.

  2. The exception is that you may use, copy, link, modify and distribute
  under the user's own terms, binary object code versions of works based
  on the Library.

  3. If you copy code from files distributed under the terms of the GNU
  General Public Licence or the GNU Library General Public Licence into a
  copy of this library, as this licence permits, the exception does not
  apply to the code that you add in this way.  To avoid misleading anyone as
  to the status of such modified files, you must delete this exception
  notice from such code and/or adjust the licensing conditions notice
  accordingly.

  4. If you write modifications of your own for this library, it is your
  choice whether to permit this exception to apply to your modifications. 
  If you do not wish that, you must delete the exception notice from such
  code and/or adjust the licensing conditions notice accordingly.
*/

#include "IEHtmlWin.h"
#include "wx/gauge.h"

// Define a new frame type: this is going to be our main frame
class wxIEFrame : public wxFrame
{
public:
	wxIEHtmlWin	*m_ie;
	wxGauge *m_gauge;

    // ctor(s)
    wxIEFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize);

    // event handlers (these functions should _not_ be virtual)
	void OnSize(wxSizeEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnEditMode(wxCommandEvent& event);
    void OnEditModeUI(wxUpdateUIEvent& event);
	void OnBrowse(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
    void OnTestHTML(wxCommandEvent& event);
    void OnTestSelect(wxCommandEvent& event);
    void OnTestHTMLSelect(wxCommandEvent& event);
    void OnTestGetText(wxCommandEvent& event);
    void OnTestHTMLGetText(wxCommandEvent& event);
    void OnTestHome(wxCommandEvent& event);
    void OnTestActiveX(wxCommandEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

    void OnMSHTMLStatusTextChangeX(wxActiveXEvent& event);
    void OnMSHTMLBeforeNavigate2X(wxActiveXEvent& event);
    void OnMSHTMLTitleChangeX(wxActiveXEvent& event);
	void OnMSHTMLNewWindow2X(wxActiveXEvent& event);
    void OnMSHTMLProgressChangeX(wxActiveXEvent& event);
};
