/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "nodesdb.h"
#endif

#ifndef _NODESDB_H_
#define _NODESDB_H_

#include "wx/dynarray.h"

class WXDLLEXPORT wxXmlNode;
class WXDLLEXPORT wxString;
class WXDLLEXPORT wxPathList;


class PropertyInfo
{
    public:
        PropertyInfo() {}
        PropertyInfo(const wxString& atype, const wxString& aname, const wxString& amoreinfo)
          : Type(atype), Name(aname), MoreInfo(amoreinfo) {}
       
        PropertyInfo& operator=(const PropertyInfo& p)
        {
            Type = p.Type; Name = p.Name; MoreInfo = p.MoreInfo;
            return *this;
        }
       
        wxString Type;
        wxString Name;
        wxString MoreInfo;
};

WX_DECLARE_OBJARRAY(PropertyInfo, PropertyInfoArray);


class NodeInfo
{
    public:
        wxString NodeClass;
        wxString Type;   
        PropertyInfoArray Props;
        wxArrayString DerivedFrom;
        bool Abstract;
        wxString ChildType;
        int Icon;
    
        void Read(const wxString& filename, wxPathList& list);
};

WX_DECLARE_OBJARRAY(NodeInfo, NodeInfoArray);



class NodesDb
{
    public:
        NodesDb();
    
        void Load();
        void LoadDir(const wxString& path);
        void LoadFile(const wxString& file);
        
        NodeInfoArray& GetNodesInfo() { return m_Infos; }
    
        static NodesDb *Get();
    
    private:
        static NodesDb *ms_Instance;
        NodeInfoArray m_Infos;
        wxArrayString m_Paths;
};



#endif 
