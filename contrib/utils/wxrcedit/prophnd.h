/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "prophnd.h"
#endif

#ifndef _PROPHND_H_
#define _PROPHND_H_


class WXDLLEXPORT wxXmlNode;
class WXDLLEXPORT wxList;
class WXDLLEXPORT wxListCtrl;
class WXDLLEXPORT wxPanel;
class WXDLLEXPORT wxWindow;

#include "wx/xml/xml.h"
#include "wx/dynarray.h"

enum PropertyType
{
    PROP_TEXT = 0,
    PROP_FLAGS = 1,
    PROP_COLOR = 2,
    PROP_BOOL = 3,
    PROP_INTEGER = 4,
    PROP_COORD = 5,
    PROP_NOT_IMPLEMENTED = 6,
};
#define PROP_TYPES_CNT  7

class PropertyInfo
{
    public:
        PropertyType Type;
        wxString Name;
        wxString MoreInfo;
};

WX_DECLARE_OBJARRAY(PropertyInfo, PropertyInfoArray);


class PropertyHandler;

class PropsListInfo : public wxObject
{
    public:
        PropsListInfo(int index, PropertyHandler *hnd, wxXmlNode *node, 
                      PropertyInfo *pi, wxListCtrl *listctrl) :
                         m_Index(index), m_Handler(hnd), m_Node(node), 
                        m_PropInfo(pi), m_ListCtrl(listctrl) {}
        virtual ~PropsListInfo() {}
        
        int m_Index;
        PropertyHandler *m_Handler;
        wxXmlNode *m_Node;
        PropertyInfo *m_PropInfo;
        wxListCtrl *m_ListCtrl;
};




class PropertyHandler
{
    public:
        PropertyHandler() {}
        
        int CreateListItem(wxListCtrl *listctrl, wxXmlNode *node, PropertyInfo *pi);
        
        virtual wxString GetBriefValue(wxXmlNode *node, PropertyInfo *pi);
        virtual wxPanel *CreateEditPanel(wxWindow *parent, PropsListInfo *pli) = 0;
};



class TextPropertyHandler : public PropertyHandler
{
    public:
        TextPropertyHandler() {}
        virtual wxPanel *CreateEditPanel(wxWindow *parent, PropsListInfo *pli);
};


class CoordPropertyHandler : public PropertyHandler
{
    public:
        CoordPropertyHandler() {}
        virtual wxPanel *CreateEditPanel(wxWindow *parent, PropsListInfo *pli);
};


class BoolPropertyHandler : public PropertyHandler
{
    public:
        BoolPropertyHandler() {}
        virtual wxString GetBriefValue(wxXmlNode *node, PropertyInfo *pi);
        virtual wxPanel *CreateEditPanel(wxWindow *parent, PropsListInfo *pli);
};


class FlagsPropertyHandler : public PropertyHandler
{
    public:
        FlagsPropertyHandler() {}
        virtual wxPanel *CreateEditPanel(wxWindow *parent, PropsListInfo *pli);
};



class NotImplPropertyHandler : public PropertyHandler
{
    public:
        NotImplPropertyHandler() {}
        virtual wxPanel *CreateEditPanel(wxWindow *parent, PropsListInfo *pli);
};


#endif 
