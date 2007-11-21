/****************************************************************************
*
*                       wxWindows HTML Applet Package
*
*               Copyright (C) 1991-2001 SciTech Software, Inc.
*                            All rights reserved.
*
*  ========================================================================
*
*    The contents of this file are subject to the wxWindows License
*    Version 3.0 (the "License"); you may not use this file except in
*    compliance with the License. You may obtain a copy of the License at
*    http://www.wxwindows.org/licence3.txt
*
*    Software distributed under the License is distributed on an
*    "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*    implied. See the License for the specific language governing
*    rights and limitations under the License.
*
*  ========================================================================
*
* Language:     ANSI C++
* Environment:  Any
*
* Description:  Main wxHtmlAppletWindow class implementation
*
****************************************************************************/

// Include private headers
#include "wx/applet/applet.h"
#include "wx/applet/window.h"
#include "wx/applet/loadpage.h"
#include "wx/applet/plugin.h"

// Preprocessor Stuff
#include "wx/applet/prepinclude.h"
#include "wx/applet/prepecho.h"
#include "wx/applet/prepifelse.h"

// wxWindows headers

// Kind of pointless to use precompiled headers when this is the only
// file in this lib that would need them.
#include "wx/defs.h"
#include "wx/spawnbrowser.h"
#include "wx/html/forcelnk.h"
#include "wx/log.h"
#include "wx/msgdlg.h"
#include "wx/tbarbase.h"

// crt
#ifdef __WXMSW__
#include <process.h>        // spawnl()
#endif

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
IMPLEMENT_CLASS(wxHtmlAppletWindow, wxHtmlWindow)

// Implement the dynamic class so it can be constructed dynamically
IMPLEMENT_DYNAMIC_CLASS(VirtualData, wxObject)
	
// Define the wxAppletList implementation
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxAppletList)

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
    const wxString& name)
    : wxHtmlWindow(parent,id,pos,size,style,name), m_AppletList(wxKEY_STRING)
{
    // Init our locks
    UnLock();

    // setup client navbars
    if (navBar) {
        m_NavBarEnabled = true;
        m_NavBar = navBar;
        m_NavBackId = navBackId;
        m_NavForwardId = navForwardId;
        }
    else {
        m_NavBarEnabled = false;
        m_NavBar = NULL;
        }

    m_NavBackId = navBackId;
    m_NavForwardId = navForwardId;

    // Add HTML preprocessors
    // deleting preprocessors is done by the code within the window
    incPreprocessor = new wxIncludePrep(); // #include preprocessor
    incPreprocessor->ChangeDirectory(m_FS); // give it access to our filesys object
    this->AddProcessor(incPreprocessor);
    this->AddProcessor(new wxEchoPrep());
    this->AddProcessor(new wxIfElsePrep());
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
dc  - wxDC object to draw on

REMARKS:
****************************************************************************/
void wxHtmlAppletWindow::OnDraw(
    wxDC& dc)
{
    wxHtmlWindow::OnDraw(dc);
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
    wxObject *obj = wxCreateDynamicObject(classId.c_str());
    wxApplet *applet = wxDynamicCast(obj,wxApplet);
    if (!applet)
        return NULL;
    if (!applet->Create(this,params,size)) {
        delete applet;
        return NULL;
        }
    else {
        // do some fixups on the size if its screwed up
        wxSize nsize = applet->GetBestSize();
        if (nsize.x < size.x) nsize.x = size.x;
        if (nsize.y < size.y) nsize.y = size.y;
        applet->SetSize(nsize);
        }

    m_AppletList.Append(iName,(wxObject*)applet);
    return applet;
}

/****************************************************************************
PARAMETERS:
classId       - Name of the Plugin class to create an object for

RETURNS:
Pointer to the wxplugIn created, or NULL if unable to create the PlugIn.

REMARKS:
This function is used to create new wxPlugIn objects dynamically based on the
class name as a string. This allows instances of wxPlugIn classes to be
created dynamically based on string values embedded in the custom tags of an
HTML page.
****************************************************************************/
bool wxHtmlAppletWindow::CreatePlugIn(
    const wxString& classId,
    const wxString& cmdLine)
{
    // Dynamically create the class instance at runtime, execute it
    // and then destroy it.
    wxObject *obj = wxCreateDynamicObject(classId.c_str());
    wxPlugIn *plugIn = wxDynamicCast(obj,wxPlugIn);
    if (!plugIn)
        return false;
    if (!plugIn->Create(this)) {
        delete plugIn;
        return false;
        }
    plugIn->Run(cmdLine);
    delete plugIn;
    return true;
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
    wxString href(link);

    if (link.GetChar(0) == '?'){
        wxString cmd = link.BeforeFirst('=');
        wxString cmdValue = link.AfterFirst('=');

        // Launches the default Internet browser for the system.
        if(!(cmd.CmpNoCase("?EXTERNAL"))) {
            return wxSpawnBrowser(this, cmdValue.c_str());
            }

        // Launches an external program on the system.
        if (!(cmd.CmpNoCase("?EXECUTE"))) {
            int waitflag = P_NOWAIT;
            bool ret;
            wxString currentdir;
            wxString filename, path, ext;

            // Parse the params sent to the execute command. For now the only
            // parm is "wait". wait will cause spawn wait, default is nowait.
            // Since we only need one param for now I am not going to make this
            // any smater then it needs to be. If we need more params later i'll
            // fix it.
            int i = cmdValue.Find('(');
            if (i != -1) {
                wxString param = cmdValue.AfterFirst('(');
                cmdValue.Truncate(i);
                if (!param.CmpNoCase("wait)"))
                    waitflag = P_WAIT;
                }

            currentdir = wxGetCwd();
            //we don't want to change the path of the virtual file system so we have to use these
            //functions rather than the filesystem
            wxSplitPath(cmdValue, &path, &filename, &ext);
            if (path.CmpNoCase("") != 0) wxSetWorkingDirectory(path);

            ret = !spawnl( waitflag, cmdValue , NULL );
            //HACK should use wxExecute
            //ret = wxExecute(filename, bool sync = FALSE, wxProcess *callback = NULL)
            wxSetWorkingDirectory(currentdir);

            return ret;
            }

        // Looks for a href in a variable stored as a cookie. The href can be
        // changed on the fly.
        if (!(cmd.CmpNoCase("?VIRTUAL"))){
            wxObject *obj = FindCookie(cmdValue);
            VirtualData *virtData = wxDynamicCast(obj,VirtualData);
            if (virtData) {
                // recurse and loadpage, just in case the link is like another
                // ? link
                return LoadPage(virtData->GetHref());
                }
            else {
#ifdef CHECKED
                wxLogError(_T("VIRTUAL LINK ERROR: '%s' does not exist."), cmdValue.c_str());
#endif
                return true;
                }
            }

        // This launches a qlet - It is like an applet but is more generic in that it
        // can be of any wxWin type so it then has the freedom to do more stuff.
        if (!(cmd.CmpNoCase("?WXPLUGIN"))){
            if (!cmdValue.IsNull()) {
                // TODO: We are going to need to add code to parse the command line
                //       parameters string in here in the future...
                wxString cmdLine = link.AfterFirst('(');
                cmdLine = cmdLine.BeforeLast(')');
                if (!CreatePlugIn(cmdValue,cmdLine)) {
#ifdef CHECKED
                    wxLogError(_T("Launch PlugIn ERROR: '%s' does not exist."), cmdValue.c_str());
#endif
                    }
                }
             return true;
            }

        // This used in a link or href will take you back in the history.
        if (!(cmd.CmpNoCase("?BACK"))){
            HistoryBack();
            return true;
            }

        // This used in a link or href will take you forward in the history
        if (!(cmd.CmpNoCase("?FORWARD"))){
            HistoryForward();
            return true;
            }
        }

    // Inform all the applets that the new page is being loaded
    for (wxAppletList::Node *node = m_AppletList.GetFirst(); node; node = node->GetNext())
        (node->GetData())->OnLinkClicked(wxHtmlLinkInfo(href));
    Show(false);

    m_openedlast = href;
    bool stat = wxHtmlWindow::LoadPage(href);
    Show(true);

    // Enable/Dis the navbar tools
    if (m_NavBarEnabled) {
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
REMARKS:
This function is used to disable the navigation bars. If you want to
toggle to the navbars off you must call this function.
****************************************************************************/
void wxHtmlAppletWindow::DisableNavBar()
{
    m_NavBarEnabled = false;
}

/****************************************************************************
REMARKS:
This function is used to enable the nav bars. If you toggle the nav bars on
you must call this function.
****************************************************************************/
void wxHtmlAppletWindow::EnableNavBar()
{
    m_NavBarEnabled = true;
}

/****************************************************************************
REMARKS:
This function is used to set the nav bar to a new nav bar if you deleted the
one that you were useing. Usally this happens when you toggle a nav bar
on or off.
****************************************************************************/
void wxHtmlAppletWindow::SetNavBar(wxToolBarBase *navBar)
{
    m_NavBar = navBar;
}

/****************************************************************************
PARAMETERS:
msg - wxEvent message to be sent to all wxApplets

REMARKS:
This function is called by the wxApplet's when they need to send a message
to all other applets on the current page. This is the primary form of
communication between applets on the page if they need to inform each
other of internal information.

****************************************************************************/
void wxHtmlAppletWindow::SendAppletMessage(
    wxAppletEvent& msg)
{
    // TODO: make a named target for messages, only send a message to the correct
    // named applet

    // Process all applets in turn and send them the message
    for (wxAppletList::Node *node = m_AppletList.GetFirst(); node; node = node->GetNext()) {
        (node->GetData())->OnMessage(msg);
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
    if (!(IsLocked())) {
        Lock();
        if (event.GetHtmlWindow() == this) {
            if (LoadPage(event.GetHRef())) {
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
    wxString& href,
    wxString& plugin )
{
    m_name = name;
    m_group = group;
    m_href = href;
    m_plugIn = plugin;
}

/****************************************************************************
PARAMETERS:
REMARKS:
VirtualData is used to store information on the virtual links.
****************************************************************************/
VirtualData::VirtualData()
{
    m_name.Empty();
    m_group.Empty();
    m_href.Empty();
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

    // Get access to our wxHtmlAppletWindow class
	wnd = m_WParser->GetWindow();
	if ((appletWindow = wxDynamicCast(wnd,wxHtmlAppletWindow)) == NULL)
        return false;

    // Parse the applet dimensions from the tag
    wxSize size = wxDefaultSize;
    int al;
    if (tag.HasParam("WIDTH")) {
        tag.GetParamAsInt("WIDTH", &width);
        size.SetWidth(width);
        }
    if (tag.HasParam("HEIGHT")) {
        tag.GetParamAsInt("HEIGHT", &height);
        size.SetHeight(height);
        }

    // Parse the applet alignment from the tag
    al = wxHTML_ALIGN_BOTTOM;
    if (tag.HasParam(wxT("ALIGN"))) {
        wxString alstr = tag.GetParam(wxT("ALIGN"));
        alstr.MakeUpper();  // for the case alignment was in ".."
        if (alstr == wxT("TEXTTOP") || alstr == wxT("TOP"))
            al = wxHTML_ALIGN_TOP;
        else if ((alstr == wxT("CENTER")) || (alstr == wxT("ABSCENTER")))
            al = wxHTML_ALIGN_CENTER;
        }

    // Create the applet based on it's class
    if (tag.HasParam("CLASSID")) {
        classId = tag.GetParam("CLASSID");
        if (classId.IsNull() || classId.Len() == 0) {
            wxMessageBox("wxApplet tag error: CLASSID is NULL or empty.","Error",wxICON_ERROR);
            return false;
            }
        if (tag.HasParam("NAME"))
            name = tag.GetParam("NAME");

        // If the name is NULL or len is zero then we assume that the html guy
        // didn't include the name param which is optional.
        if (name.IsNull() || name.Len() == 0)
            name = classId;

        // We got all the params and can now create the applet
        if ((applet = appletWindow->CreateApplet(classId, name, tag , size)) != NULL) {
            applet->Show(true);
            m_WParser->OpenContainer()->InsertCell(new wxHtmlAppletCell(applet,al));
            }
        else
            wxMessageBox("wxApplet error: Could not create:" + classId + "," + name);
        }
    else {
        wxMessageBox("wxApplet tag error: Can not find CLASSID param.","Error",wxICON_ERROR);
        return false;
        }

    // Add more param parsing here. If or when spec changes.
    // For now we'll ignore any other params those HTML guys
    // might put in our tag.
	return true;
}

TAG_HANDLER_END(wxApplet)

TAGS_MODULE_BEGIN(wxApplet)
    TAGS_MODULE_ADD(wxApplet)
TAGS_MODULE_END(wxApplet)

/****************************************************************************
REMARKS:
Constructor for the HTML cell class to store our wxApplet windows in
the HTML page to be rendered by wxHTML.
****************************************************************************/
wxHtmlAppletCell::wxHtmlAppletCell(
    wxWindow *wnd,
    int align)
    : wxHtmlCell()
{
    int sx, sy;
    m_Wnd = wnd;
    m_Wnd->GetSize(&sx, &sy);
    m_Width = sx, m_Height = sy;
    switch (align) {
        case wxHTML_ALIGN_TOP :
            m_Descent = m_Height;
            break;
        case wxHTML_ALIGN_CENTER :
            m_Descent = m_Height / 2;
            break;
        case wxHTML_ALIGN_BOTTOM :
        default :
            m_Descent = 0;
            break;
        }
    SetCanLiveOnPagebreak(FALSE);
}

/****************************************************************************
REMARKS:
Implementation for the HTML cell class to store our wxApplet windows in
the HTML page to be rendered by wxHTML.
****************************************************************************/
wxHtmlAppletCell::~wxHtmlAppletCell()
{
    delete m_Wnd;
}

/****************************************************************************
REMARKS:
Code to draw the html applet cell
****************************************************************************/
void wxHtmlAppletCell::Draw(
    wxDC& WXUNUSED(dc),
    int WXUNUSED(x),
    int WXUNUSED(y),
    int WXUNUSED(view_y1),
    int WXUNUSED(view_y2))
{
    int         absx = 0, absy = 0, stx, sty;
    wxHtmlCell  *c = this;

    while (c) {
        absx += c->GetPosX();
        absy += c->GetPosY();
        c = c->GetParent();
        }
    ((wxScrolledWindow*)(m_Wnd->GetParent()))->GetViewStart(&stx, &sty);
    m_Wnd->Move(absx - wxHTML_SCROLL_STEP * stx, absy  - wxHTML_SCROLL_STEP * sty);
}

/****************************************************************************
REMARKS:
Code to draw the html applet cell invisibly
****************************************************************************/
void wxHtmlAppletCell::DrawInvisible(
    wxDC& WXUNUSED(dc),
    int WXUNUSED(x),
    int WXUNUSED(y))
{
    int         absx = 0, absy = 0, stx, sty;
    wxHtmlCell  *c = this;

    while (c) {
        absx += c->GetPosX();
        absy += c->GetPosY();
        c = c->GetParent();
        }
    ((wxScrolledWindow*)(m_Wnd->GetParent()))->GetViewStart(&stx, &sty);
    m_Wnd->Move(absx - wxHTML_SCROLL_STEP * stx, absy  - wxHTML_SCROLL_STEP * sty);
}

/****************************************************************************
REMARKS:
Code to layout the html applet cell.
****************************************************************************/
void wxHtmlAppletCell::Layout(
    int w)
{
    int sx, sy;
    m_Wnd->GetSize(&sx, &sy);
    m_Width = sx, m_Height = sy;
    wxHtmlCell::Layout(w);
}

// This is our little forcelink hack.
FORCE_LINK(loadpage)

