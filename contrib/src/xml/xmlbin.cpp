/////////////////////////////////////////////////////////////////////////////
// Name:        xmlbin.cpp
// Purpose:     wxXmlIOHandlerBin 
// Author:      Vaclav Slavik
// Created:     2000/07/24
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
// nothing, already in xml.cpp
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/datstrm.h"
#include "wx/log.h"
#include "wx/intl.h"

#include "wx/xml/xmlio.h"




bool wxXmlIOHandlerBin::CanLoad(wxInputStream& stream)
{
    bool canread;
    canread = (ReadHeader(stream) == wxT("XMLBIN "));
    stream.SeekI(-9, wxFromCurrent);
    return canread;
}



wxString wxXmlIOHandlerBin::ReadHeader(wxInputStream& stream)
{
    wxUint8 version;
    char cheader[8];

    stream.Read(cheader, 8);
    cheader[7] = 0;
    stream.Read(&version, 1);
    
    if (version != 1) return wxEmptyString;
    else return wxString(cheader);
}



void wxXmlIOHandlerBin::WriteHeader(wxOutputStream& stream, const wxString& header)
{
    char cheader[8];
    size_t i;
    wxUint8 version = 1;
    
    for (i = 0; i < header.Length(); i++) cheader[i] = header[i];
    for (; i < 7; i++) cheader[i] = ' ';
    cheader[7] = 0;
    stream.Write(cheader, 8);
    stream.Write(&version, 1);
}



static bool SaveBinNode(wxDataOutputStream& ds, wxXmlNode *node)
{
    if (node)
    {
        ds << (wxUint8)1 <<
              (wxUint8)node->GetType() << 
              node->GetName() << node->GetContent();
        
        wxXmlProperty *prop = node->GetProperties();
        while (prop)
        {
            ds << (wxUint8)1;
            ds << prop->GetName() << prop->GetValue();
            prop = prop->GetNext();
            
        }
        ds << (wxUint8)0;
        
        SaveBinNode(ds, node->GetNext());
        SaveBinNode(ds, node->GetChildren());
    }  
    else
        ds << (wxUint8)0;
    
    return TRUE;
}



bool wxXmlIOHandlerBin::Save(wxOutputStream& stream, const wxXmlDocument& doc)
{
    WriteHeader(stream, "XMLBIN ");
    wxDataOutputStream ds(stream);
    ds << doc.GetVersion() << doc.GetEncoding();
    SaveBinNode(ds, doc.GetRoot());
    return stream.LastError() == wxSTREAM_NOERROR;
}



static wxXmlProperty *LoadBinProp(wxDataInputStream& ds)
{
    wxUint8 dummy;
    ds >> dummy;
    if (dummy == 0) return NULL;

    wxString name, value;
    ds >> name >> value;
    return new wxXmlProperty(name, value, LoadBinProp(ds));
}




static wxXmlNode *LoadBinNode(wxDataInputStream& ds, wxXmlNode *parent)
{
    wxUint8 type;
    wxString name, content;
    wxUint8 dummy;

    ds >> dummy;
    if (dummy == 0) return NULL;
    ds >> type >> name >> content;
    
    wxXmlProperty *prop = LoadBinProp(ds);

    wxXmlNode *nd = new wxXmlNode(parent, (wxXmlNodeType)type, name, content, 
                                  prop, LoadBinNode(ds, parent));
    LoadBinNode(ds, nd);
    return nd;
}



bool wxXmlIOHandlerBin::Load(wxInputStream& stream, wxXmlDocument& doc)
{
    ReadHeader(stream);
    wxDataInputStream ds(stream);
    wxString tmp;

    ds >> tmp;
    doc.SetVersion(tmp);
    ds >> tmp;
    doc.SetEncoding(tmp);

    doc.SetRoot(LoadBinNode(ds, NULL));
        
    return (doc.GetRoot() != NULL);
}


