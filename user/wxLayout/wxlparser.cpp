/*-*- c++ -*-********************************************************
 * wxlparser.h : parsers,  import/export for wxLayoutList           *
 *                                                                  *
 * (C) 1998,1999 by Karsten Ballüder (Ballueder@usa.net)            *
 *                                                                  *
 * $Id$
 *******************************************************************/

#ifdef __GNUG__
#   pragma implementation "wxlparser.h"
#endif

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

#include "Mpch.h"

#ifdef M_PREFIX
#   include "gui/wxllist.h"
#   include "gui/wxlparser.h"
#else
#   include "wxllist.h"
#   include "wxlparser.h"
#endif

#define   BASE_SIZE 12

inline static bool IsEndOfLine(const char *p, int mode)
{
   // in addition to Unix EOL convention we also (but not instead) understand
   // the DOS one under Windows
   return
      (mode == WXLO_EXPORT_WITH_CRLF) ?
      ((*p == '\r') && (*(p + 1) == '\n')) 
      :
      (((*p == '\r') && (*(p + 1) == '\n'))||(*p == '\n'));
}

void wxLayoutImportText(wxLayoutList *list, wxString const &str, int withflag)
{
   if(str.Length() == 0)
      return;
   char * cptr = (char *)str.c_str(); // string gets changed only temporarily
   const char * begin = cptr;
   char  backup;
   
   for(;;)
   {
      begin = cptr;
      while( *cptr && !IsEndOfLine(cptr, withflag) )
         cptr++;
      backup = *cptr;
      *cptr = '\0';
      list->Insert(begin);
      *cptr = backup;

      // check if it's the end of this line
      if ( IsEndOfLine(cptr, withflag) )
      {
         // if it was "\r\n", skip the following '\n'
         if ( *cptr == '\r' )
            cptr++;
         list->LineBreak();
      }
      else if(backup == '\0') // reached end of string
         break;
      cptr++;
   }
}

static
wxString wxLayoutExportCmdAsHTML(wxLayoutObjectCmd const & cmd,
                                 wxLayoutStyleInfo *styleInfo)
{
   static char buffer[20];
   wxString html;
   
   wxLayoutStyleInfo *si = cmd.GetStyle();

   int size, sizecount;
   
   html += "<font ";

   if(si->m_fg_valid)
   {
      html +="color=";
      sprintf(buffer,"\"#%02X%02X%02X\"", si->m_fg.Red(),si->m_fg.Green(),si->m_fg.Blue());
      html += buffer;
   }

   if(si->m_bg_valid)
   {
      html += " bgcolor=";
      sprintf(buffer,"\"#%02X%02X%02X\"", si->m_bg.Red(),si->m_bg.Green(),si->m_bg.Blue());
      html += buffer;
   }
   
   switch(si->family)
   {
   case wxSWISS:
   case wxMODERN:
      html += " face=\"Arial,Helvetica\""; break;
   case wxROMAN:
      html += " face=\"Times New Roman, Times\""; break;
   case wxTELETYPE:
      html += " face=\"Courier New, Courier\""; break;
   default:
      ;
   }

   size = BASE_SIZE; sizecount = 0;
   while(size < si->size && sizecount < 5)
   {
      sizecount ++;
      size = (size*12)/10;
   }
   while(size > si->size && sizecount > -5)
   {
      sizecount --;
      size = (size*10)/12;
   }
   html += "size=";
   sprintf(buffer,"%+1d", sizecount);
   html += buffer;

   html +=">";

   if(styleInfo != NULL)
      html ="</font>"+html; // terminate any previous font command

   if((si->weight == wxBOLD) && ( (!styleInfo) || (styleInfo->weight != wxBOLD)))
      html += "<b>";
   else
      if(si->weight != wxBOLD && ( styleInfo && (styleInfo->weight == wxBOLD)))
         html += "</b>";

   if(si->style == wxSLANT)
      si->style = wxITALIC; // the same for html
   
   if((si->style == wxITALIC) && ( (!styleInfo) || (styleInfo->style != wxITALIC)))
      html += "<i>";
   else
      if(si->style != wxITALIC && ( styleInfo && (styleInfo->style == wxITALIC)))
         html += "</i>";

   if(si->underline && ( (!styleInfo) || ! styleInfo->underline))
      html += "<u>";
   else if(si->underline == false && ( styleInfo && styleInfo->underline))
      html += "</u>";

   
   *styleInfo = *si; // update last style info
   
   return html;
}



wxLayoutExportStatus::wxLayoutExportStatus(wxLayoutList *list)
{
   m_si = *list->GetDefaults();
   m_line = list->GetFirstLine();
   m_iterator = m_line->GetFirstObject();
}
   


#define   WXLO_IS_TEXT(type) \
( type == WXLO_TYPE_TEXT \
  || (type == WXLO_TYPE_CMD \
      && mode == WXLO_EXPORT_AS_HTML))


wxLayoutExportObject *wxLayoutExport(wxLayoutExportStatus *status,
                                     int mode, int flags)
{
   wxASSERT(status);
   wxLayoutExportObject * export;
   
   if(status->m_iterator == NULLIT) // end of line
   {
      if(!status->m_line || status->m_line->GetNextLine() == NULL)
         // reached end of list
         return NULL;
   }
   export = new wxLayoutExportObject();
   wxLayoutObjectType type;
   if(status->m_iterator != NULLIT)
   {
      type = (** status->m_iterator).GetType();
      if( mode == WXLO_EXPORT_AS_OBJECTS || ! WXLO_IS_TEXT(type)) // simple case
      {
         export->type = WXLO_EXPORT_OBJECT;
         export->content.object = *status->m_iterator;
         status->m_iterator++;
         return export;
      }
   }
   else
   {  // iterator == NULLIT
      if(mode == WXLO_EXPORT_AS_OBJECTS)
      {
         export->type = WXLO_EXPORT_EMPTYLINE;
         export->content.object = NULL; //empty line
         status->m_line = status->m_line->GetNextLine();
         if(status->m_line)
            status->m_iterator = status->m_line->GetFirstObject();
         return export;
      }
      else
         type = WXLO_TYPE_TEXT;
   }

   wxString *str = new wxString();
   // text must be concatenated
   for(;;)
   {
      while(status->m_iterator == NULLIT)
      {
         if(flags & WXLO_EXPORT_AS_HTML)
            *str += "<br>";
         if(flags & WXLO_EXPORT_WITH_CRLF)
            *str += "\r\n";
         else
            *str += '\n';

         status->m_line = status->m_line->GetNextLine();
         if(status->m_line)
            status->m_iterator = status->m_line->GetFirstObject();
         else
            break; // end of list
      }
      if(! status->m_line)  // reached end of list, fall through
         break; 
      type = (** status->m_iterator).GetType();
      if(type == WXLO_TYPE_ICON)
         break;
      switch(type)
      {
      case WXLO_TYPE_TEXT:
         *str += ((wxLayoutObjectText *)*status->m_iterator)->GetText();
         break;
      case WXLO_TYPE_CMD:
         if(mode == WXLO_EXPORT_AS_HTML)
            *str += wxLayoutExportCmdAsHTML(*(wxLayoutObjectCmd const
                                              *)*status->m_iterator, & status->m_si);
         break;
      default:  // ignore icons
         ;
      }
      status->m_iterator++;
   }

   export->type = (mode == WXLO_EXPORT_AS_HTML)
      ?  WXLO_EXPORT_HTML : WXLO_EXPORT_TEXT;
   export->content.text = str;
   return export;
}

