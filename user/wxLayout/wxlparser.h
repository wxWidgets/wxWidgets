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
   WXLO_EXPORT_AS_TEXT,
   WXLO_EXPORT_AS_TEXT_AND_COMMANDS,
   WXLO_EXPORT_AS_HTML,
   WXLO_EXPORT_AS_OBJECTS
};

struct wxLayoutExportObject
{
   wxLayoutExportType type;
   union
   {
      String           *text;
      wxLayoutObjectBase *object;
   }content;
   ~wxLayoutExportObject()
      {
         if(type == WXLO_EXPORT_TEXT || type == WXLO_EXPORT_HTML)
            delete content.text;
      }
};

/// import text into a wxLayoutList (including linefeeds):
void wxLayoutImportText(wxLayoutList &list, String const &str);


wxLayoutExportObject *wxLayoutExport(wxLayoutList &list,
                               wxLayoutList::iterator &from,
                               wxLayoutExportMode WXLO_EXPORT_AS_TEXT);

#endif //WXLPARSER_H
