/////////////////////////////////////////////////////////////////////////////
// Name:        search.h
// Purpose:     wxSearchEngine - class for searching keywords
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#if wxUSE_HTML

#ifndef __SEARCH_H__
#define __SEARCH_H__

#ifdef __GNUG__
#pragma interface
#endif


#include <wx/stream.h>

//--------------------------------------------------------------------------------
// wxSearchEngine
//                  This class takes input streams and scans them for occurence
//                  of keyword(s)
//--------------------------------------------------------------------------------


class wxSearchEngine : public wxObject
{
    private:
        char *m_Keyword;

    public:
        wxSearchEngine() : wxObject() {m_Keyword = NULL;}
        ~wxSearchEngine() {if (m_Keyword) free(m_Keyword);}

        virtual void LookFor(const wxString& keyword);
            // Sets the keyword we will be searching for

        virtual bool Scan(wxInputStream *stream);
            // Scans the stream for the keyword.
            // Returns TRUE if the stream contains keyword, fALSE otherwise
};




#endif

#endif