/////////////////////////////////////////////////////////////////////////////
// Name:        xh_radbx.cpp
// Purpose:     XML resource for wxRadioBox
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_radbx.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_radbx.h"
#include "wx/radiobox.h"

#if wxUSE_RADIOBOX

wxRadioBoxXmlHandler::wxRadioBoxXmlHandler() 
: wxXmlResourceHandler() , m_InsideBox(FALSE)
{
    ADD_STYLE(wxRA_SPECIFY_COLS);
    ADD_STYLE(wxRA_HORIZONTAL);
    ADD_STYLE(wxRA_SPECIFY_ROWS);
    ADD_STYLE(wxRA_VERTICAL);
    AddWindowStyles();
}

wxObject *wxRadioBoxXmlHandler::DoCreateResource()
{ 
    if( m_Class == wxT("wxRadioBox"))
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


        wxRadioBox *control = new wxRadioBox(m_ParentAsWindow,
                                    GetID(),
                                    GetText(wxT("label")),
                                    GetPosition(), GetSize(),
                                    strList.GetCount(),
                                    strings,
                                    GetLong( wxT("dimension"), 1 ),
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
        // handle <item selected="boolean">Label</item>

        // add to the list
        strList.Add( GetNodeContent(m_Node) );

        return NULL;
    }

}



bool wxRadioBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return (IsOfClass(node, wxT("wxRadioBox")) ||
           (m_InsideBox && node->GetName() == wxT("item"))
           );
}

#endif
