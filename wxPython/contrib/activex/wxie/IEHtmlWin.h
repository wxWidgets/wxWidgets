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

/*! \file iehtmlwin.h 
    \brief implements wxIEHtmlWin window class
*/ 
#ifndef _IEHTMLWIN_H_
#define _IEHTMLWIN_H_
#pragma warning( disable : 4101 4786)
#pragma warning( disable : 4786)


#include <wx/setup.h>
#include <wx/wx.h>
#include <exdisp.h>
#include <iostream>
using namespace std;

#include "wxactivex.h"


enum wxIEHtmlRefreshLevel 
{
    wxIEHTML_REFRESH_NORMAL = 0,
    wxIEHTML_REFRESH_IFEXPIRED = 1,
    wxIEHTML_REFRESH_CONTINUE = 2,
    wxIEHTML_REFRESH_COMPLETELY = 3
};

class IStreamAdaptorBase;

class wxIEHtmlWin : public wxActiveX
{
public:
    wxIEHtmlWin(wxWindow * parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxPanelNameStr);
    virtual ~wxIEHtmlWin();

    void LoadUrl(const wxString& url);
    bool LoadString(const wxString& html);
    bool LoadStream(istream *strm);
    bool LoadStream(wxInputStream *is);

    void SetCharset(const wxString& charset);
    void SetEditMode(bool seton);
    bool GetEditMode();
    wxString GetStringSelection(bool asHTML = false);
    wxString GetText(bool asHTML = false);

    bool GoBack();
    bool GoForward();
    bool GoHome();
    bool GoSearch();
    bool Refresh(wxIEHtmlRefreshLevel level);
    bool Stop();

    DECLARE_EVENT_TABLE();

protected:
    void SetupBrowser();
    bool LoadStream(IStreamAdaptorBase *pstrm);

    wxAutoOleInterface<IWebBrowser2>        m_webBrowser;
};

#endif /* _IEHTMLWIN_H_ */
