/*-*- c++ -*-********************************************************
 * wxlparser.h : parsers,  import/export for wxLayoutList           *
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$
 *******************************************************************/
#ifndef WXLPARSER_H
#   define   WXLPARSER_H

#ifdef __GNUG__
#   pragma interface "wxlparser.h"
#endif

#ifndef   NULL
#   define   NULL 0
#endif

enum wxLayoutExportType
{
   WXLO_EXPORT_TEXT,        
   WXLO_EXPORT_HTML,        
   WXLO_EXPORT_OBJECT
};

enum wxLayoutExportMode
{
   WXLO_EXPORT_AS_MASK = 0x0f,
   WXLO_EXPORT_AS_TEXT = 0x00,
   WXLO_EXPORT_AS_TEXT_AND_COMMANDS = 0x01,
   WXLO_EXPORT_AS_HTML = 0x02,
   WXLO_EXPORT_AS_OBJECTS = 0x03,
   
   WXLO_EXPORT_WITH_MASK = 0xf0,
   WXLO_EXPORT_WITH_CRLF = 0x00,
   WXLO_EXPORT_WITH_LF_ONLY = 0x10
};

struct wxLayoutExportObject
{
   wxLayoutExportType type;
   union
   {
      wxString           *text;
      wxLayoutObject *object;
   }content;
   ~wxLayoutExportObject()
      {
         if(type == WXLO_EXPORT_TEXT || type == WXLO_EXPORT_HTML)
            delete content.text;
      }
};

struct wxLayoutExportStatus
{
   wxLayoutExportStatus(wxLayoutList *list)
      {
         list->GetDefaults()->GetStyle(&m_si);
         m_line = list->GetFirstLine();
         m_iterator = m_line->GetFirstObject();
      }
   
   wxLayoutLine      * m_line;
   wxLOiterator        m_iterator;
   wxLayoutStyleInfo   m_si;
};


#ifdef OS_WIN
/// import text into a wxLayoutList (including linefeeds):
void wxLayoutImportText(wxLayoutList *list, wxString const &str,
                        int withflag = WXLO_EXPORT_WITH_CRLF);

wxLayoutExportObject *wxLayoutExport(wxLayoutExportStatus *status,
                               int mode = WXLO_EXPORT_AS_TEXT|WXLO_EXPORT_WITH_CRLF); 
#else
/// import text into a wxLayoutList (including linefeeds):
void wxLayoutImportText(wxLayoutList *list, wxString const &str,
                        int withflag = WXLO_EXPORT_WITH_LF_ONLY);

/// export text in a given format FIXME-MT: not thread safe, uses static variable
wxLayoutExportObject *wxLayoutExport(wxLayoutExportStatus *status,
                                     int mode = WXLO_EXPORT_AS_TEXT|WXLO_EXPORT_WITH_LF_ONLY
                                     );
#endif

#endif //WXLPARSER_H
