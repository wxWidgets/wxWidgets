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

//#include "Mpch.h"
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
      ((mode & WXLO_EXPORT_WITH_MASK) == WXLO_EXPORT_WITH_CRLF) ?
      ((*p == '\r') && (*(p + 1) == '\n')) 
      :
      (((*p == '\r') && (*(p + 1) == '\n'))||(*p == '\n'));
}

void wxLayoutImportText(wxLayoutList &list, String const &str, int withflag)
{
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
      list.Insert(begin);
      *cptr = backup;

      // check if it's the end of this line
      if ( IsEndOfLine(cptr, withflag) )
      {
         // if it was "\r\n", skip the following '\n'
         if ( *cptr == '\r' )
            cptr++;
         list.LineBreak();
      }
      else if(backup == '\0') // reached end of string
         break;
      cptr++;
   }
}

static
String wxLayoutExportCmdAsHTML(wxLayoutObjectCmd const & cmd,
                               wxLayoutStyleInfo **lastStylePtr)
{
   static char buffer[20];
   String html;
   
   wxLayoutStyleInfo *si = cmd.GetStyle();
   wxLayoutStyleInfo *last_si = NULL;

   int size, sizecount;
   
   if(lastStylePtr != NULL)
      last_si = *lastStylePtr;
   
   html += "<font ";

   html +="color=";
   sprintf(buffer,"\"#%02X%02X%02X\"", si->fg_red,si->fg_green,si->fg_blue);
   html += buffer;


   html += " bgcolor=";
   sprintf(buffer,"\"#%02X%02X%02X\"", si->bg_red,si->bg_green,si->bg_blue);
   html += buffer;

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

   if(last_si != NULL)
      html ="</font>"+html; // terminate any previous font command

   if((si->weight == wxBOLD) && ( (!last_si) || (last_si->weight != wxBOLD)))
      html += "<b>";
   else
      if(si->weight != wxBOLD && ( last_si && (last_si->weight == wxBOLD)))
         html += "</b>";

   if(si->style == wxSLANT)
      si->style = wxITALIC; // the same for html
   
   if((si->style == wxITALIC) && ( (!last_si) || (last_si->style != wxITALIC)))
      html += "<i>";
   else
      if(si->style != wxITALIC && ( last_si && (last_si->style == wxITALIC)))
         html += "</i>";

   if(si->underline && ( (!last_si) || ! last_si->underline))
      html += "<u>";
   else if(si->underline == false && ( last_si && last_si->underline))
      html += "</u>";
   
   if(last_si)
      delete last_si;
   *lastStylePtr = si;
   return html;
}



#define   WXLO_IS_TEXT(type) \
( (type == WXLO_TYPE_TEXT || type == WXLO_TYPE_LINEBREAK) \
  || (type == WXLO_TYPE_CMD \
      && (mode & WXLO_EXPORT_AS_MASK) == WXLO_EXPORT_AS_HTML))


  
wxLayoutExportObject *wxLayoutExport(wxLayoutList &list,
                                     wxLayoutList::iterator &from,
                                     int mode)
{
   if(from == list.end())
      return NULL;
   
   wxLayoutObjectType   type = (*from)->GetType();
   wxLayoutExportObject     * export = new wxLayoutExportObject();

   static wxLayoutStyleInfo *s_si = NULL;
   
   if( (mode & WXLO_EXPORT_AS_MASK) == WXLO_EXPORT_AS_OBJECTS || ! WXLO_IS_TEXT(type)) // simple case
   {
      export->type = WXLO_EXPORT_OBJECT;
      export->content.object = *from;
      from++;
      return export;
   }

   String *str = new String();
   
   // text must be concatenated
   while(from != list.end() && WXLO_IS_TEXT(type))
   {
      switch(type)
      {
      case WXLO_TYPE_TEXT:
         *str += ((wxLayoutObjectText *)*from)->GetText();
         break;
      case WXLO_TYPE_LINEBREAK:
         if((mode & WXLO_EXPORT_AS_MASK) == WXLO_EXPORT_AS_HTML)
            *str += "<br>";
         if((mode & WXLO_EXPORT_WITH_CRLF) == WXLO_EXPORT_WITH_CRLF)
            *str += "\r\n";
         else
            *str += '\n';
         break;
      case WXLO_TYPE_CMD:
         wxASSERT_MSG( (mode&WXLO_EXPORT_AS_MASK) == WXLO_EXPORT_AS_HTML,
                       "reached cmd object in text mode" );
         
         *str += wxLayoutExportCmdAsHTML(*(wxLayoutObjectCmd const
                                           *)*from, &s_si);
         break;
      default:  // ignore icons
         ;
      }
      from++;
      if(from != list.end())
         type = (*from)->GetType();
   }
   export->type = ((mode & WXLO_EXPORT_AS_MASK) == WXLO_EXPORT_AS_HTML) ?  WXLO_EXPORT_HTML : WXLO_EXPORT_TEXT;
   export->content.text = str;
   return export;
}
