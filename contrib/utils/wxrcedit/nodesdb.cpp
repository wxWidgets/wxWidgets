/////////////////////////////////////////////////////////////////////////////
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "nodesdb.h"
#endif

// For compilers that support precompilation, includes _T("wx/wx.h").
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wx.h"
#include "wx/textfile.h"
#include "wx/tokenzr.h"
#include "wx/dir.h"
#include "nodesdb.h"
#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(NodeInfoArray);
WX_DEFINE_OBJARRAY(PropertyInfoArray);




void NodeInfo::Read(const wxString& filename, wxPathList& list)
{
    wxString tp;
    wxString nd, cht;
    bool ab = FALSE;
    long icn = -1;

    NodeClass.Empty();

    wxString path = list.FindValidPath(filename);
    if (path.IsEmpty()) return;
    
    wxTextFile tf;
    tf.Open(path);
    
    if (!tf.IsOpened()) return;

    for (size_t i = 0; i < tf.GetLineCount(); i++)
    {
        if (tf[i].IsEmpty() || tf[i][0] == _T('#')) continue;
        wxStringTokenizer tkn(tf[i], _T(' '));
        wxString s = tkn.GetNextToken();
        if (s == _T("node"))
            nd = tkn.GetNextToken();
        else if (s == _T("childtype"))
            cht = tkn.GetNextToken();
        else if (s == _T("icon"))
            tkn.GetNextToken().ToLong(&icn);
        else if (s == _T("derived"))
        {
            if (tkn.GetNextToken() == _T("from"))
            {
                s = tkn.GetNextToken();
                DerivedFrom.Add(s);
                Read(s + _T(".df"), list);
            }
        }
        else if (s == _T("abstract"))
            ab = true;
        else if (s == _T("type"))
        {
            tp = tkn.GetNextToken();
        }
        else if (s == _T("var"))
        {
            PropertyInfo pi;
            pi.Name = tkn.GetNextToken();
            tkn.GetNextToken();
            pi.Type = tkn.GetNextToken();
            if (tkn.HasMoreTokens()) pi.MoreInfo = tkn.GetNextToken();
            
            bool fnd = FALSE;
            for (size_t j = 0; j < Props.GetCount(); j++)
            {
                if (Props[j].Name == pi.Name)
                {
                    if (Props[j].Type == pi.Type && pi.Type == _T("flags"))
                        Props[j].MoreInfo << _T(',') << pi.MoreInfo;
                    else
                        Props[j] = pi;
                    fnd = TRUE;
                }
            }
            
            if (!fnd) Props.Add(pi);
        }
    }
    
    if (!nd.IsEmpty()) NodeClass = nd;
    if (!cht.IsEmpty()) ChildType = cht;
    if (!!tp) Type = tp;
    if (icn != -1) Icon = icn;
    Abstract = ab;
}









NodesDb* NodesDb::ms_Instance = NULL;

NodesDb *NodesDb::Get()
{
    if (ms_Instance == NULL) 
    {
        (void)new NodesDb;
    }
    return ms_Instance;
}


NodesDb::NodesDb()
{
    ms_Instance = this;

    m_Paths.Add(_T("."));
    m_Paths.Add(_T("./df"));
#ifdef __UNIX__ 
    m_Paths.Add(wxGetHomeDir() + _T("/.wxrcedit"));
    #ifdef wxINSTALL_PREFIX
    m_Paths.Add(wxINSTALL_PREFIX _T("/share/wx/wxrcedit"));
    #endif
#endif

    Load();
}



void NodesDb::Load()
{
    for (size_t i = 0; i < m_Paths.GetCount(); i++)
        LoadDir(m_Paths[i]);
}



void NodesDb::LoadDir(const wxString& path)
{
    if (!wxDirExists(path)) return;
    
    wxDir dir(path);
    wxString filename;
    bool cont;

    cont = dir.GetFirst(&filename, "*.df");
    while (cont)
    {
        LoadFile(filename);
        cont = dir.GetNext(&filename);
    }
}



void NodesDb::LoadFile(const wxString& file)
{
    NodeInfo *ni = new NodeInfo; 
    ni->Type = wxEmptyString;
    ni->Icon = 0;
    wxPathList paths;
    size_t i;
    
    for (i = 0; i < m_Paths.GetCount(); i++)
        paths.Add(m_Paths[i]);
    
    ni->Read(file, paths);
    
    // maybe we already parsed it?
    for (i = 0; i < m_Infos.GetCount(); i++)
        if (m_Infos[i].NodeClass == ni->NodeClass) return;
    
    m_Infos.Add(ni);
}
