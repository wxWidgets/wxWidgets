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
#include "wx/utils.h"
#include "wx/process.h"
#include "wx/spawnbrowser.h"
#include "wx/html/forcelnk.h"

// Include private headers
#include "wx/applet/applet.h"
#include "wx/applet/window.h"
#include "wx/applet/loadpage.h"

// Preprocessor Stuff
#include "wx/applet/prepinclude.h"
#include "wx/applet/prepecho.h"
#include "wx/applet/prepifelse.h"

/*---------------------------- Global variables ---------------------------*/

wxHashTable wxHtmlAppletWindow::m_Cookies;		
		
/*------------------------- Implementation --------------------------------*/

// Empty event handler. We include this event handler simply so that
// sub-classes of wxApplet can reference wxApplet in the event tables
// that they create as necessary.
BEGIN_EVENT_TABLE(wxHtmlAppletWindow, wxHtmlWindow)
	EVT_LOAD_PAGE(wxHtmlAppletWindow::OnLoadPage)
    EVT_PAGE_LOADED(wxHtmlAppletWindow::OnPageLoaded)
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
    wxToolBarBase *navBar,
    int navBackId,
    int navForwardId,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name,
    const wxString& docroot )
    : wxHtmlWindow(parent,id,pos,size,style,name)
{
    // Init our locks
    UnLock();

    // setup client navbars
    if (navBar) {
        m_NavBar = navBar;
        m_NavBackId = navBackId;
        m_NavForwardId = navForwardId;
        }
    else {
        m_NavBar = NULL;
        }

    // Set up docroot
    m_DocRoot = docroot;

    // Add HTML preprocessors
    // deleting preprocessors is done by the code within the window

    incPreprocessor = new wxIncludePrep(); // #include preprocessor
    incPreprocessor->ChangeDirectory(m_DocRoot);

    wxEchoPrep * echoPreprocessor = new wxEchoPrep(); // #echo preprocessor
    wxIfElsePrep * ifPreprocessor = new wxIfElsePrep();

    this->AddProcessor(incPreprocessor);
    this->AddProcessor(echoPreprocessor);
    this->AddProcessor(ifPreprocessor);
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
    const wxString& classId,
    const wxString& iName,
    const wxHtmlTag& params,
    const wxSize& size)
{
    // Dynamically create the class instance at runtime
    wxClassInfo *info = wxClassInfo::FindClass(classId.c_str());
    if (!info)
        return NULL;
    wxObject *obj = info->CreateObject();
    if (!obj)
        return NULL;
    wxApplet *applet = wxDynamicCast(obj,wxApplet);
    if (!applet)
        return NULL;
    if (!applet->Create(this,params,size)) {
        delete applet;
        return NULL;
        }
    m_AppletList.Append(iName,applet);
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
    const wxString& link)
{
    wxString    href(link);

    // Check for abs path. If it is not then tack on the path
    // supplied at creation.
    if (!wxIsAbsolutePath(href))
        href = m_DocRoot + href;

    // TODO: This needs to be made platform inde if possible.
    if (link.GetChar(0) == '?'){
        wxString cmd = link.BeforeFirst('=');
        wxString cmdValue = link.AfterFirst('=');
        if(!(cmd.CmpNoCase("?EXTERNAL"))){
            return wxSpawnBrowser(this, cmdValue.c_str());
            }
        if (!(cmd.CmpNoCase("?EXECUTE"))){
            wxProcess *child = new AppletProcess(this);
            wxExecute(cmdValue, false, child);
            return true;
            }
        if (!(cmd.CmpNoCase("?VIRTUAL"))){
            VirtualData& temp = *((VirtualData*)FindCookie(cmdValue));
            if (&temp) {
                href = temp.GetHref();
                }
            else {
#ifdef CHECKED
                wxLogError(_T("VIRTUAL LINK ERROR: '%s' does not exist."), cmdValue.c_str());
#endif
                return true;
                }
            }
        }

    // Inform all the applets that the new page is being loaded
    for (wxAppletList::Node *node = m_AppletList.GetFirst(); node; node = node->GetNext())
        (node->GetData())->OnLinkClicked(wxHtmlLinkInfo(href));
    Show(false);
    bool stat = wxHtmlWindow::LoadPage(href);
    Show(true);

    // Enable/Dis the navbar tools
    if (m_NavBar) {
        m_NavBar->EnableTool(m_NavForwardId,HistoryCanForward());
        m_NavBar->EnableTool(m_NavBackId,HistoryCanBack());
        }
    return stat;
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
    LoadPage(link.GetHref());
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
        if (!msg.GetSkipped()){
            wxMessageBox("BREAK");
            break;
            }
        }
}

/****************************************************************************
PARAMETERS:
name    - Uniq wxString used as hash key
cookie  - wxObject data returned when name is found.

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
name - wxString uniq haskey used to remove item from hash

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

/****************************************************************************
PARAMETERS:
event   - Event to handle

REMARKS:
This function handles delayed LoadPage events posted from applets that
need to change the page for the current window to a new window.
****************************************************************************/
void wxHtmlAppletWindow::OnLoadPage(
    wxLoadPageEvent &event)
{
    // Test the mutex lock.
    if (!(IsLocked())){
        Lock();
        if (event.GetHtmlWindow() == this){
            if (LoadPage(event.GetHRef())){
                // wxPageLoadedEvent evt;
                // NOTE: This is reserved for later use as we might need to send
                // page loaded events to applets.
                }
            }
        UnLock();
        }
}

/****************************************************************************
PARAMETERS:
event   - Event to handle

REMARKS:
This function handles delayed LoadPage events posted from applets that
need to change the page for the current window to a new window.
****************************************************************************/
void wxHtmlAppletWindow::OnPageLoaded(
    wxPageLoadedEvent &)
{
    Enable(true);
}

/****************************************************************************
PARAMETERS:
none

REMARKS:
This function tries to lock the mutex. If it can't, returns
immediately with false.
***************************************************************************/
bool wxHtmlAppletWindow::TryLock()
{
    if (!m_mutexLock) {
        m_mutexLock = true;
        return true;
        }
    return false;
}

/****************************************************************************
PARAMETERS:
name    - name of the last applet that changed the data in this object
group   - name of the group the allplet belongs to.
href    - webpage to go to.

REMARKS:
VirtualData is used to store information on the virtual links.
****************************************************************************/
VirtualData::VirtualData(
    wxString& name,
    wxString& group,
    wxString& href )
{
    m_name = name;
    m_group = group;
    m_href = href;
}

/****************************************************************************
PARAMETERS:
REMARKS:
****************************************************************************/
void AppletProcess::OnTerminate(
    int,
    int )
{
    // we're not needed any more
    delete this;
}

#include "wx/html/m_templ.h"

/****************************************************************************
REMARKS:
Implementation for the <embed> HTML tag handler. This handler takes care
of automatically constructing the wxApplet objects of the appropriate
class based on the <embed> tag information.
****************************************************************************/
TAG_HANDLER_BEGIN(wxApplet, "WXAPPLET")

TAG_HANDLER_PROC(tag)
{
	wxWindow			*wnd;
	wxHtmlAppletWindow	*appletWindow;
	wxApplet			*applet;
    wxString            classId;
    wxString            name;
    int 				width, height;

	wnd = m_WParser->GetWindow();

	if ((appletWindow = wxDynamicCast(wnd,wxHtmlAppletWindow)) != NULL){
		tag.ScanParam("WIDTH", "%i", &width);
		tag.ScanParam("HEIGHT", "%i", &height);	
        if (tag.HasParam("CLASSID")){
            classId = tag.GetParam("CLASSID");
            if ( classId.IsNull() || classId.Len() == 0 ){
                wxMessageBox("wxApplet tag error: CLASSID is NULL or empty.","Error",wxICON_ERROR);
                return false;
                }
            if (tag.HasParam("NAME"))
                name = tag.GetParam("NAME");

            // If the name is NULL or len is zero then we assume that the html guy
            // didn't include the name param which is optional.
            if ( name.IsNull() || name.Len() == 0 )
                name = classId;

            // We got all the params and can now create the applet
			if ((applet = appletWindow->CreateApplet(classId, name, tag , wxSize(width, height))) != NULL){
				applet->Show(true);
				m_WParser->OpenContainer()->InsertCell(new wxHtmlWidgetCell(applet,0));
				}
            else
                wxMessageBox("wxApplet error: Could not create:" + classId + "," + name);
			}
        else{
            wxMessageBox("wxApplet tag error: Can not find CLASSID param.","Error",wxICON_ERROR);
            return false;
            }
        //Add more param parsing here. If or when spec changes.
        //For now we'll ignore any other params those HTML guys
        //might put in our tag.
        }

	return false;
}

TAG_HANDLER_END(wxApplet)

TAGS_MODULE_BEGIN(wxApplet)
    TAGS_MODULE_ADD(wxApplet)
TAGS_MODULE_END(wxApplet)

// This is our little forcelink hack.
FORCE_LINK(loadpage)

