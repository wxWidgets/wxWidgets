/////////////////////////////////////////////////////////////////////////////
// Name:        xh_choic.cpp
// Purpose:     XML resource for wxChoice
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_choic.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_choic.h"
#include "wx/choice.h"

wxChoiceXmlHandler::wxChoiceXmlHandler() 
: wxXmlResourceHandler() , m_InsideBox(FALSE)
{
    ADD_STYLE(wxCB_SORT);
    AddWindowStyles();
}

wxObject *wxChoiceXmlHandler::DoCreateResource()
{ 
    if( m_Class == wxT("wxChoice"))
    {
        // find the selection
        long selection = GetLong( wxT("selection"), -1 );

        // need to build the list of strings from children
        m_InsideBox = TRUE;
        CreateChildrenPrivately( NULL, GetParamNode(wxT("content")));
        wxString *strings = (wxString *) NULL;
        if( strList.GetCount() > 0 )
        {
            strings = new wxString[strList.GetCount()];
            int count = strList.GetCount();
            for( int i = 0; i < count; i++ )
                strings[i]=strList[i];
        }


        wxChoice *control = new wxChoice(m_ParentAsWindow,
                                    GetID(),
                                    GetPosition(), GetSize(),
                                    strList.GetCount(),
                                    strings,
                                    GetStyle(),
                                    wxDefaultValidator,
                                    GetName()
                                    );

        if( selection != -1 )
            control->SetSelection( selection );

        SetupWindow(control);

        if( strings != NULL )
            delete [] strings;
        strList.Clear();    // dump the strings   

        return control;
    }
    else
    {
        // on the inside now.
        // handle <item>Label</item>
        
        // add to the list
        strList.Add( GetNodeContent(m_Node) );

        return NULL;
    }

}



bool wxChoiceXmlHandler::CanHandle(wxXmlNode *node)
{
    return (IsOfClass(node, wxT("wxChoice")) ||
           (m_InsideBox && node->GetName() == wxT("item"))
           );
}


