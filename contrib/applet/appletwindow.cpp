/****************************************************************************
*
*                       wxWindows HTML Applet Package
*
*               Copyright (C) 1991-2001 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code is a proprietary trade secret of     |
*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
*  |written authorization from SciTech to possess or use this code, you |
*  |may be subject to civil and/or criminal penalties.                  |
*  |                                                                    |
*  |If you received this code in error or you would like to report      |
*  |improper use, please immediately contact SciTech Software, Inc. at  |
*  |530-894-8400.                                                       |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C++
* Environment:  Any
*
* Description:  Main wxHtmlAppletWindow class implementation
*
****************************************************************************/

// For compilers that support precompilation
#include "wx/wxprec.h"

// Include private headers
#include "wx/applet/applet.h"

/*------------------------- Implementation --------------------------------*/

// Empty event handler. We include this event handler simply so that
// sub-classes of wxApplet can reference wxApplet in the event tables
// that they create as necessary.
BEGIN_EVENT_TABLE(wxHtmlAppletWindow, wxHtmlWindow)
END_EVENT_TABLE()

// Implement the class functions for wxHtmlAppletWindow
IMPLEMENT_CLASS(wxHtmlAppletWindow, wxHtmlWindow);

// Define the wxAppletList implementation
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxAppletList);

/****************************************************************************
REMARKS:
Constructor for the applet window class.
****************************************************************************/
wxHtmlAppletWindow::wxHtmlAppletWindow(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name)
    : wxHtmlWindow(parent,id,pos,size,style,name)
{
    // Ensure all cookie data is destroyed when window is killed
    m_Cookies.DeleteContents(true);
}

/****************************************************************************
REMARKS:
Destructor for the applet window class.
****************************************************************************/
wxHtmlAppletWindow::~wxHtmlAppletWindow()
{
}

/****************************************************************************
PARAMETERS:
className       - Name of the applet class to create an object for
size            - Initial size of the applet to be created

RETURNS:
Pointer to the wxApplet created, or NULL if unable to create the applet.

REMARKS:
This function is used to create new wxApplet objects dynamically based on the
class name as a string. This allows instances of wxApplet classes to be
created dynamically based on string values embedded in the custom tags of an
HTML page.
****************************************************************************/
wxApplet *wxHtmlAppletWindow::CreateApplet(
    const wxString& className,          
    const wxSize& size)
{
    // We presently only allow one applet per page of the same class!
    if (m_AppletList.Find(className))
        return NULL;

    // Dynamically create the class instance at runtime
    wxClassInfo *info = wxClassInfo::FindClass(className.c_str());
    if (!info)
        return NULL;
    wxObject *obj = info->CreateObject();
    if (!obj)
        return NULL;
    wxApplet *applet = wxDynamicCast(obj,wxApplet);
    if (!applet)
        return NULL;
    if (!applet->Create(this,size)) {
        delete applet;
        return NULL;
        }
    m_AppletList.Append(className,applet);
    return applet;
}

/****************************************************************************
PARAMETERS:
appletName      - Name of the applet class to find

RETURNS:
Pointer to the wxApplet found, or NULL if not found.

REMARKS:
Find an instance of an applet based on it's name
****************************************************************************/
wxApplet *wxHtmlAppletWindow::FindApplet(
    const wxString& appletName)
{
    wxAppletList::Node *node = m_AppletList.Find(appletName);
    if (!node)
        return NULL;
    return node->GetData();
}           

/****************************************************************************
PARAMETERS:
applet      - Pointer to the applet object to remove from the list

RETURNS:
True if the applet was found and removed, false if not. The applet itself
is *not* destroyed!

REMARKS:
Remove an applet from the manager. Called during applet destruction
****************************************************************************/
bool wxHtmlAppletWindow::RemoveApplet(
    const wxApplet *applet)
{
    for (wxAppletList::Node *node = m_AppletList.GetFirst(); node; node = node->GetNext()) {
        if (node->GetData() == applet) {
            m_AppletList.DeleteNode(node);
            return true;
            }
        }
    return false;
}           

/****************************************************************************
PARAMETERS:
URL - New URL for the page to load

RETURNS:
True if page loaded successfully, false if not

REMARKS:
Remove an applet from the manager. Called during applet destruction
****************************************************************************/
bool wxHtmlAppletWindow::LoadPage(
    const wxString& hRef)
{
    for (wxAppletList::Node *node = m_AppletList.GetFirst(); node; node = node->GetNext())
        (node->GetData())->OnLinkClicked(hRef);
    return wxHtmlWindow::LoadPage(hRef);
}

/****************************************************************************
PARAMETERS:
URL - String URL that we are navigating to

REMARKS:
Called when the user navigates to a new URL from the current page. We simply
call the LoadPage function above to load the new page and display it.
****************************************************************************/
void wxHtmlAppletWindow::OnLinkClicked(
    const wxHtmlLinkInfo& link)
{
    for (wxAppletList::Node *node = m_AppletList.GetFirst(); node; node = node->GetNext())
        (node->GetData())->OnLinkClicked(link);
    wxHtmlWindow::LoadPage(link.GetHref());
}

/****************************************************************************
REMARKS:
Called when the user navigates forward within the HTML history stack.
We call all the applets in turn allowing them to handle the navigation
command prior to being destructed when the current page is destroyed.
****************************************************************************/
bool wxHtmlAppletWindow::HistoryForward()
{
    if (!HistoryCanForward())   
        return false;
    for (wxAppletList::Node *node = m_AppletList.GetFirst(); node; node = node->GetNext())
        (node->GetData())->OnHistoryForward();
    return wxHtmlWindow::HistoryForward();
}

/****************************************************************************
REMARKS:
Called when the user navigates backwards within the HTML history stack.
We call all the applets in turn allowing them to handle the navigation
command prior to being destructed when the current page is destroyed.
****************************************************************************/
bool wxHtmlAppletWindow::HistoryBack()
{
    if (!HistoryCanBack())  
        return false;
    for (wxAppletList::Node *node = m_AppletList.GetFirst(); node; node = node->GetNext())
        (node->GetData())->OnHistoryBack();
    return wxHtmlWindow::HistoryBack();
}

/****************************************************************************
PARAMETERS:
msg - wxEvent message to be sent to all wxApplets

REMARKS:
This function is called by the wxApplet's when they need to send a message
to all other applets on the current page. This is the primary form of
communication between applets on the page if they need to inform each
other of internal information.

Note that the event handling terminates as soon as the first wxApplet
handles the event. If the event should be handled by all wxApplet's,
the event handlers for the applets should not reset the wxEvent::Skip()
value (ie: by default it is true).
****************************************************************************/
void wxHtmlAppletWindow::SendMessage(
    wxEvent& msg)
{   
    // Preset the skip flag
    msg.Skip();
    
    // Process all applets in turn and send them the message
    for (wxAppletList::Node *node = m_AppletList.GetFirst(); node; node = node->GetNext()) {
        (node->GetData())->OnMessage(msg);
        if (!msg.GetSkipped())
            break;
        }
}

/****************************************************************************
PARAMETERS:
msg - wxEvent message to be sent to all wxApplets

RETURNS:
True if new cookie was added, false if cookie with same name already exists.

REMARKS:
This function is called by the wxApplet's when they need register a cookie
of data in the applet window's cookie table. Cookies are arbitrary data
objects that are references by unique name's by the wxApplet. These
values can be used to store and retrieve data that needs to remain
persisent across invocations of the wxApplet. Ie: The first time an
applet is created it would use the cookie to store data to maintain
it's present state so that if you navigated back to the same page
is would be able to re-load the prior state as though the applet
was never actually destructed.

Note: If a cookie with the same name already exists, this function returns
      false. Hence if you wish to replace a cookie you should first call
      UnRegisterCookie to ensure the cookie is deleted and then call this
      function.
****************************************************************************/
bool wxHtmlAppletWindow::RegisterCookie(
    const wxString& name,
    wxObject *cookie)
{
    // Fail if the named cookie already exists!
    if (m_Cookies.Get(name))
        return false;
    m_Cookies.Put(name,cookie); 
    return true;
}

/****************************************************************************
PARAMETERS:
msg - wxEvent message to be sent to all wxApplets

RETURNS:
True if found and deleted, false if not found in table.

REMARKS:
This function is called by the wxApplet's when they need de-register a
cookie of data in the applet window's cookie table. The data in the
cookie itself is also deleted before it is removed from the table.
****************************************************************************/
bool wxHtmlAppletWindow::UnRegisterCookie(
    const wxString& name)
{
    wxObject *data = m_Cookies.Delete(name);
    if (data) {
        delete data;
        return true;
        }
    return false;
}

/****************************************************************************
PARAMETERS:
msg - wxEvent message to be sent to all wxApplets

RETURNS:
Pointer to the cookie data if found, NULL if not found.

REMARKS:
This function is called by the wxApplet's when they need to find a cookie
of data given it's public name. If the cookie is not found, NULL is
returned.
****************************************************************************/
wxObject *wxHtmlAppletWindow::FindCookie(
    const wxString& name)
{
    return m_Cookies.Get(name);
}

#include "wx/html/m_templ.h"

/****************************************************************************
REMARKS:
Implementation for the <embed> HTML tag handler. This handler takes care
of automatically constructing the wxApplet objects of the appropriate
class based on the <embed> tag information.
****************************************************************************/
TAG_HANDLER_BEGIN(Embed, "EMBED")

TAG_HANDLER_PROC(tag)
{
    wxWindow            *wnd;
    wxHtmlAppletWindow  *appletWindow;
    wxApplet            *applet;
    int                 width, height;
    int                 floatPercent = 0;

    wnd = m_WParser->GetWindow();
    if ((appletWindow = wxDynamicCast(wnd,wxHtmlAppletWindow)) != NULL) {
        tag.ScanParam("WIDTH", "%i", &width);
        tag.ScanParam("HEIGHT", "%i", &height);
        if (tag.HasParam("FLOAT"))
            tag.ScanParam("FLOAT", "%i", &floatPercent);
        if (tag.HasParam("APPLET")) {
            if ((applet = appletWindow->CreateApplet(tag.GetParam("APPLET"), wxSize(width, height))) != NULL) {
                applet->Show(true);
                m_WParser->OpenContainer()->InsertCell(new wxHtmlWidgetCell(applet,floatPercent));
                }
            }
        else if (tag.HasParam("TEXT")) {
            // TODO: Somehow get the text returned from this class displayed on the page!
            }
        }
    return false;
}

TAG_HANDLER_END(Embed)

TAGS_MODULE_BEGIN(Embed)
    TAGS_MODULE_ADD(Embed)
TAGS_MODULE_END(Embed)

