/*
 * Program: canvas
 *
 * Author: Robert Roebling
 *
 * Copyright: (C) 1998, Robert Roebling
 *
 */

#ifdef __GNUG__
    #pragma implementation "test.cpp"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "test.h"
#include "smile.xpm"

//-----------------------------------------------------
// class MywxCanvasObjectRef
//-----------------------------------------------------

BEGIN_EVENT_TABLE(MywxCanvasObjectRef,wxCanvasObjectRef)
  EVT_MOUSE_EVENTS( MywxCanvasObjectRef::OnMouseEvent )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(MywxCanvasObjectRef, wxCanvasObjectRef)

MywxCanvasObjectRef::MywxCanvasObjectRef(double x, double y,wxCanvasObjectGroup* group)
    :wxCanvasObjectRef(x,y,group)
{
}

MywxCanvasObjectRef::MywxCanvasObjectRef()
    :wxCanvasObjectRef(0,0,NULL)
{
}

MywxCanvasObjectRef::~MywxCanvasObjectRef()
{
}

void MywxCanvasObjectRef::OnMouseEvent(wxMouseEvent &event)
{
    if (!m_dragable)
    {
        event.Skip();
        return;
    }

    static double xprev;
    static double yprev;

    //new position of the mouse relative within the object
    double x = m_admin->DeviceToLogicalX(event.GetX());
    double y = m_admin->DeviceToLogicalY(event.GetY());

    if (event.LeftDown())
    {
        CaptureMouse();
        if (m_dragmode != DRAG_REDRAW)
            DragStart();
    }
    else if (event.LeftUp())
    {
        ReleaseMouse();
        if (m_dragmode != DRAG_REDRAW)
            DragEnd();
    }
    else if (IsCapturedMouse())
    {
        if (m_dragmode != DRAG_REDRAW)
            DragRelative(x-xprev,y-yprev);
        else
            MoveRelative(x-xprev,y-yprev);
        m_admin->UpdateNow();
    }
    xprev=x;
    yprev=y;
    //well do something extra
    if (IsCapturedMouse())
        m_admin->GetActive()->SetCursor(*wxHOURGLASS_CURSOR);
    else
        m_admin->GetActive()->SetCursor(*wxSTANDARD_CURSOR);
}

//---------------------------------------------------
// class MyEventHandler
//---------------------------------------------------

BEGIN_EVENT_TABLE(MyEventHandler,wxEvtHandler)
  EVT_MOUSE_EVENTS( MyEventHandler::OnMouseEvent )
END_EVENT_TABLE()

MyEventHandler::MyEventHandler()
{
}

void MyEventHandler::OnMouseEvent(wxMouseEvent &event)
{
    wxCanvasObject* obj=(wxCanvasObject*)event.GetEventObject();

    if (!obj->GetDraggable())
        return;

    static double xprev;
    static double yprev;

    wxCanvasAdmin* adm=obj->GetAdmin();

    //new position of the mouse relative within the object
    double x = adm->DeviceToLogicalX(event.GetX());
    double y = adm->DeviceToLogicalY(event.GetY());

    if (event.LeftDown())
    {
        obj->CaptureMouse();
        if (obj->GetDragMode() != DRAG_REDRAW)
            obj->DragStart();
    }
    else if (event.LeftUp())
    {
        obj->ReleaseMouse();
        if (obj->GetDragMode() != DRAG_REDRAW)
            obj->DragEnd();
    }
    else if (obj->IsCapturedMouse())
    {
        if (obj->GetDragMode() != DRAG_REDRAW)
            obj->DragRelative(x-xprev,y-yprev);
        else
            obj->MoveRelative(x-xprev,y-yprev);
        adm->UpdateNow();
    }
    xprev=x;
    yprev=y;
    //well do something extra
    if (obj->IsCapturedMouse())
        obj->GetAdmin()->GetActive()->SetCursor(*wxHOURGLASS_CURSOR);
    else
        obj->GetAdmin()->GetActive()->SetCursor(*wxSTANDARD_CURSOR);
}

//------------------------------------------------
// class MyFrame
//------------------------------------------------

class MyApp;
class MyCanvas;

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(SPLIT_VERTICAL, MyFrame::SplitVertical)
  EVT_MENU(SPLIT_HORIZONTAL, MyFrame::SplitHorizontal)
  EVT_MENU(SPLIT_UNSPLIT, MyFrame::Unsplit)
  EVT_MENU(SPLIT_QUIT, MyFrame::Quit)
  EVT_MENU(SPLIT_SETMINSIZE, MyFrame::SetMinSize)

  EVT_UPDATE_UI(SPLIT_VERTICAL, MyFrame::UpdateUIVertical)
  EVT_UPDATE_UI(SPLIT_HORIZONTAL, MyFrame::UpdateUIHorizontal)
  EVT_UPDATE_UI(SPLIT_UNSPLIT, MyFrame::UpdateUIUnsplit)

  EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
  EVT_TIMER   (-1,       MyFrame::OnTimer)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame* frame, const wxString& title, const wxPoint& pos, const wxSize& size):
    wxFrame(frame, SPLITTER_FRAME, title, pos, size)
{
    m_eventhandler =new MyEventHandler();

    wxPathList pathList;
    pathList.Add(".");
    pathList.Add("..");

    wxString path = pathList.FindValidPath("pat36.bmp");
    gs_bmp36_mono.LoadFile(path, wxBITMAP_TYPE_BMP);
    wxMask* mask36 = new wxMask(gs_bmp36_mono, *wxBLACK);
    /* associate a monochrome mask with this bitmap */
    gs_bmp36_mono.SetMask(mask36);

    CreateStatusBar(2);

    // Make a menubar
    fileMenu = new wxMenu;
    fileMenu->Append(SPLIT_VERTICAL, "Split &Vertically\tCtrl-V", "Split vertically");
    fileMenu->Append(SPLIT_HORIZONTAL, "Split &Horizontally\tCtrl-H", "Split horizontally");
    fileMenu->Append(SPLIT_UNSPLIT, "&Unsplit", "Unsplit");
//    fileMenu->Append(SPLIT_UNSPLIT, "&Unsplit\tCtrl-U", "Unsplit");
    fileMenu->AppendSeparator();
    fileMenu->Append(SPLIT_SETMINSIZE, "Set &min size", "Set minimum pane size");
    fileMenu->AppendSeparator();
    fileMenu->Append(SPLIT_QUIT, "E&xit\tAlt-X", "Exit");
    fileMenu->Append( ID_ABOUT, "&About...");


    menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");

    SetMenuBar(menuBar);

    m_splitter = new MySplitterWindow(this, SPLITTER_WINDOW);

    m_canvas1 = new MyCanvas(&m_canvasadmin, m_splitter, CANVAS1, wxPoint(0, 0), wxSize(400, 400),wxHSCROLL|wxVSCROLL);
    m_canvas1->SetYaxis(TRUE);
    m_canvas1->SetMappingScroll(-300,-300,500,500,false);
    m_canvas1->SetScroll(-400,-400,600,600);
    m_canvas1->SetColour(wxColour(255, 255, 255) );
    m_canvas1->SetCursor(wxCursor(wxCURSOR_MAGNIFIER));

    m_canvas2 = new MyCanvas(&m_canvasadmin, m_splitter, CANVAS2, wxPoint(0, 0), wxSize(400, 400),wxHSCROLL|wxVSCROLL);
    m_canvas2->SetMappingScroll(-100,-100,500,500,false);
    m_canvas2->SetScroll(-400,-400,600,600);
    m_canvas2->SetColour( wxColour(187, 215, 243) );
    m_canvas2->Show(FALSE);

    m_canvasadmin.Append(m_canvas1);
    m_canvasadmin.Append(m_canvas2);
    m_canvasadmin.SetActive(m_canvas1);

    m_splitter->Initialize(m_canvas1);
    SetStatusText("Min pane size = 0", 1);

    int widths[] = { -1, 100 };
    SetStatusWidths( 2, widths );

    //root group always at 0,0
    m_datatree = new wxCanvasObjectGroup(0,0);
    m_datatree->DeleteContents( TRUE );
    m_datatree->SetAdmin(&m_canvasadmin);

    wxBitmap bitmap( smile_xpm );
    wxImage image( bitmap );

//    m_sm1 = new wxCanvasImage( image, 0,70,32,32 );
//    m_datatree->Append( m_sm1 );

    int i;
    for (i = 10; i < 300; i+=10)
    {
        wxCanvasRect *r = new wxCanvasRect( i,50,3,140 );
        r->SetBrush( *wxRED_BRUSH );
        m_datatree->Append( r );
    }


//    m_sm2 = new wxCanvasImage( image, 0,140,24,24 );
//    m_datatree->Append( m_sm2 );

    for (i = 15; i < 300; i+=10)
    {
        wxCanvasRect* rec= new wxCanvasRect( i,50,3,140 );
        rec->SetBrush(wxBrush(wxColour(0,10+i,2+i),wxSOLID));
        rec->SetDraggable(FALSE);
        m_datatree->Append( rec );
    }

/*
    wxButton *button = new wxButton( m_canvas1, -1, "Hello", wxPoint(80,50) );
    m_datatree->Append( new wxCanvasControl( button ) );

    m_datatree->Append( new wxCanvasText( "Hello", 180, 50,
                      wxGetApp().GetFontPath() + "/times.ttf", 20 ) );

    m_datatree->Append( new wxCanvasText( "How are you?", 180, 10,
                      wxGetApp().GetFontPath() + "/times.ttf", 8 ) );

    m_datatree->Append( new wxCanvasText( "How are you?", 180, 20,
                      wxGetApp().GetFontPath() + "/times.ttf", 10 ) );

    m_datatree->Append( new wxCanvasText( "How are you?", 180, 30,
                      wxGetApp().GetFontPath() + "/times.ttf", 12 ) );
    m_sm3 = new wxCanvasImage( image, 0,210,32,32 );
    m_datatree->Append( m_sm3 );
*/
    for (i = 10; i < 300; i+=10)
        m_datatree->Append( new wxCanvasLine( 10,-15,i,300 ) );
/*
    m_sm4 = new wxCanvasImage( image, 0,270,64,32 );
    m_sm4->SetDragMode(DRAG_RECTANGLE);
    m_datatree->Append( m_sm4 );
*/

//    m_canvas->Append( new wxCanvasLine( 10,-1500e6,50,300000e6, 0,255,0 ) );
//    m_canvas->Append( new wxCanvasLine( 10,-150000,50,300000, 0,255,0 ) );


    //make a group of wxCanvasObjects
    wxCanvasObjectGroup* group1 = new wxCanvasObjectGroup(0,0);

    wxCanvasLine* line = new wxCanvasLine( 10,-35,50,190);
    line->SetPen(wxPen(wxColour(255,161,5),5,wxDOT_DASH ));
    group1->Prepend( line );
    group1->Prepend( new wxCanvasImage( image, 4,38,32,32 ) );
    wxCanvasRect* rec3 = new wxCanvasRect(20,-20,50,170);
    rec3->SetBrush(wxBrush(wxColour(0,120,240),wxSOLID));
    rec3->SetPen(wxPen(wxColour(252,54,252 ),3,wxSOLID));
    group1->Prepend( rec3 );

    wxCanvasRect* rec2 = new wxCanvasRect(10,20,104,52);
    rec2->SetBrush(wxBrush(wxColour(0,240,240),wxSOLID));
    rec2->SetPen(wxPen(wxColour(210,40,50 ),1,wxSOLID));
    group1->Prepend( rec2 );



    wxPoint2DDouble* todraw2 = new wxPoint2DDouble[3];
    todraw2[0].m_x=230;
    todraw2[0].m_y=220;
    todraw2[1].m_x=300;
    todraw2[1].m_y=200;
    todraw2[2].m_x=300;
    todraw2[2].m_y=300;
    wxCanvasPolyline* poly2= new wxCanvasPolyline(3,todraw2);
    poly2->SetPen(wxPen(wxColour(200,0,64 ),4,wxDOT));
    m_datatree->Prepend( poly2 );



    //make another group of wxCanvasObjects
    wxCanvasObjectGroup* group2 = new wxCanvasObjectGroup(0,0);
    group2->Prepend( new wxCanvasImage( image, 60,38,52,32 ) );
    wxCanvasRect* rr = new wxCanvasRect(10,20,104,52,30);
    rr->SetBrush(wxBrush(wxColour(10,17,255),wxHORIZONTAL_HATCH ));
    rr->SetPen(wxPen(wxColour(9,115,64 ),4,wxSOLID));
    group2->Prepend( rr );


    //this a reference to group2 put into group1
    MywxCanvasObjectRef* m_subref = new MywxCanvasObjectRef(60,50, group2);
    m_subref->SetRotation(35);
    m_subref->SetRotation(0);
    group1->Prepend( m_subref );


    wxPoint2DDouble* todraw = new wxPoint2DDouble[5];
    todraw[0].m_x=-30;
    todraw[0].m_y=-20;
    todraw[1].m_x=100;
    todraw[1].m_y=0;
    todraw[2].m_x=100;
    todraw[2].m_y=100;
    todraw[3].m_x=50;
    todraw[3].m_y=150;
    todraw[4].m_x=0;
    todraw[4].m_y=100;

    wxCanvasPolygon* poly= new wxCanvasPolygon(5,todraw);
    poly->SetBrush(wxBrush(wxColour(100,17,255),wxCROSSDIAG_HATCH ));
    poly->SetPen(wxPen(wxColour(9,115,64 ),4,wxSOLID));
    group1->Prepend( poly );

    wxPoint2DDouble* todraw4 = new wxPoint2DDouble[4];

    todraw4[0].m_x=-50;
    todraw4[0].m_y=-30;
    todraw4[1].m_x=-50;
    todraw4[1].m_y=70;
    todraw4[2].m_x=150;
    todraw4[2].m_y=70;
    todraw4[3].m_x=150;
    todraw4[3].m_y=-30;

    wxCanvasPolygon* poly5= new wxCanvasPolygon(4,todraw4);
    poly5->SetBrush(wxBrush(wxColour(100,17,255),wxCROSSDIAG_HATCH ));
//    poly5->SetBrush(wxBrush(wxColour(100,17,255),wxSOLID ));
//    poly5->SetPen(wxPen(wxColour(9,115,64 ),1,wxSOLID));
    poly5->SetPen(wxPen(wxColour(9,115,64 ),4,wxSOLID));
    wxCanvasObjectGroup* group3 = new wxCanvasObjectGroup(0,0);
    group3->Prepend( poly5 );

    wxList* pointlist = new wxList();
    wxPoint2DDouble* point = new wxPoint2DDouble(0,0);
    pointlist->Append((wxObject*)point);
    point = new wxPoint2DDouble(-300,100);
    pointlist->Append((wxObject*)point);
    point = new wxPoint2DDouble(-100,100);
    pointlist->Append((wxObject*)point);
    point = new wxPoint2DDouble(-100,0);
    pointlist->Append((wxObject*)point);
    point = new wxPoint2DDouble(-200,50);
    pointlist->Append((wxObject*)point);

    wxCanvasPolygonL* poly15= new wxCanvasPolygonL(pointlist,TRUE);
    poly15->SetColour1(wxColour(250,78,216 ));
    poly15->SetColour2(*wxRED);
    poly15->SetBrush(wxBrush(gs_bmp36_mono));
    poly15->SetTransParent(TRUE);
    poly15->SetPen(wxPen(*wxRED,4,wxSOLID));
    group1->Prepend( poly15 );

    wxList* pointlist2 = new wxList();
    wxPoint2DDouble* point2 = new wxPoint2DDouble(-400,100);
    pointlist2->Append((wxObject*)point2);
    point2 = new wxPoint2DDouble(-400,200);
    pointlist2->Append((wxObject*)point2);
    point2 = new wxPoint2DDouble(0,200);
    pointlist2->Append((wxObject*)point2);
    point2 = new wxPoint2DDouble(0,100);
    pointlist2->Append((wxObject*)point2);
    point2 = new wxPoint2DDouble(-200,175);
    pointlist2->Append((wxObject*)point2);

    wxCanvasPolylineL* poly16= new wxCanvasPolylineL(pointlist2,TRUE);
    poly16->SetPen(wxPen(wxColour(9,115,64 ),4,wxSOLID));
    m_datatree->Prepend( poly16 );


    wxPoint2DDouble* todraw6 = new wxPoint2DDouble[5];
    todraw6[0].m_x=50;
    todraw6[0].m_y=305;
    todraw6[1].m_x=-200;
    todraw6[1].m_y=200;
    todraw6[2].m_x=0;
    todraw6[2].m_y=500;
    todraw6[3].m_x=300;
    todraw6[3].m_y=200;
    todraw6[4].m_x=-300;
    todraw6[4].m_y=-300;

    wxCanvasPolygon* poly17= new wxCanvasPolygon(5,todraw6,TRUE);
    poly17->SetBrush(wxBrush(wxColour(100,17,255),wxSOLID));
    poly17->SetPen(wxPen(wxColour(10,17,25),16,wxLONG_DASH  ));
    poly17->SetColour1(*wxGREEN);
    poly17->SetColour2(*wxRED);
    poly17->SetGradient(TRUE,wxPen(wxColour(0,0,0),0,wxSOLID),0);
    wxCanvasObjectRef* m_refc = new wxCanvasObjectRef(0,-200, poly17);
    m_refc->SetRotation(90);
    m_datatree->Prepend( m_refc );
    wxCanvasObjectRef* m_refd = new wxCanvasObjectRef(200,-50, poly17);
    m_refd->SetRotation(0);
    m_datatree->Append( m_refd );

    //now make two references to group1 into root group of the canvas
    m_ref = new MywxCanvasObjectRef(350,-200, group1);
    m_ref->SetRotation(25);
    //TODO if rotation is 0 scaling is weird
//    m_ref->SetScale(2,3.2);
    m_datatree->Append( m_ref );

    group3->Prepend( m_ref );


    //this a reference to group2 put into group1
    MywxCanvasObjectRef* subref2 = new MywxCanvasObjectRef(20,130, group2);
    subref2->SetRotation(15);
    group3->Prepend( subref2 );
    m_datatree->Prepend( subref2 );

    m_ref2 = new MywxCanvasObjectRef(80,450, group1);
    m_ref2->SetRotation(-35);
    m_ref2->SetDragMode(DRAG_RECTANGLE);
    m_datatree->Prepend( m_ref2 );

    wxCanvasCircle* cir = new  wxCanvasCircle( -100, -150, 100 );
    cir->SetBrush(wxBrush(wxColour(19,215,6),wxHORIZONTAL_HATCH ));
    cir->SetPen(wxPen(wxColour(198,3,105 ),30,wxSOLID));
    cir->SetDragMode(DRAG_REDRAW);
    m_datatree->Prepend( cir );

    wxCanvasEllipse* elp = new  wxCanvasEllipse( -100, 250, 100,300 );
    elp->SetBrush(wxBrush(wxColour(100,17,55),wxVERTICAL_HATCH ));
    elp->SetPen(wxPen(wxColour(2,255,6 ),10,wxDOT));
    m_datatree->Prepend( elp );

    wxCanvasEllipticArc* aelp = new  wxCanvasEllipticArc( -230, 250, 100,300, 30,270 );
    aelp->SetBrush(wxBrush(wxColour(100,17,155),wxSOLID ));
    aelp->SetPen(wxPen(wxColour(1,215,6 ),10,wxSOLID));
    m_datatree->Prepend( aelp );

    //HOW BAD DO THINGS GET
    int kk;
    for (kk=0;kk<100;kk++)
    {
//        MywxCanvasObjectRef* m_refx = new MywxCanvasObjectRef(180,50+kk*30, group1);
//        m_refx->SetRotation(-35);
//        m_datatree->Prepend( m_refx );
    }

/*
    m_log = new wxTextCtrl( this, -1, "", wxPoint(0,0), wxSize(100,100), wxTE_MULTILINE );
    wxLog *old_log = wxLog::SetActiveTarget( new wxLogTextCtrl( m_log ) );
    delete old_log;

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    topsizer->Add( m_canvas, 1, wxEXPAND );
    topsizer->Add( m_log, 0, wxEXPAND );

    SetAutoLayout( TRUE );
    SetSizer( topsizer );
*/

    //fancy background
    wxPoint2DDouble* todraw8 = new wxPoint2DDouble[4];
    todraw8[0].m_x=-350;
    todraw8[0].m_y=-350;
    todraw8[1].m_x=-350;
    todraw8[1].m_y=550;
    todraw8[2].m_x=550;
    todraw8[2].m_y=550;
    todraw8[3].m_x=550;
    todraw8[3].m_y=-350;

    wxCanvasPolygon* poly18= new wxCanvasPolygon(4,todraw8);
    poly18->SetPen(wxPen(wxColour(10,17,25),16,wxTRANSPARENT  ));
    poly18->SetColour1(wxColour(0,0,0));
    poly18->SetColour2(wxColour(0,255,255));
    poly18->SetGradient(TRUE,wxPen(wxColour(0,0,0),0,wxSOLID),0);

    wxCanvasObjectRef* m_refb = new wxCanvasObjectRef(200,0, poly18);
    m_refb->SetRotation(90);

    m_datatree->Prepend( m_refb );

/*
    wxCanvasCircle* cir = new  wxCanvasCircle( -100, -150, 100 );
    cir->SetBrush(wxBrush(wxColour(19,215,6),wxHORIZONTAL_HATCH ));
    cir->SetPen(wxPen(wxColour(198,3,105 ),30,wxSOLID));
    m_datatree->Prepend( cir );
*/
    m_datatree->SetAdmin(&m_canvasadmin);
    m_datatree->AppendEventHandler( m_eventhandler );
    m_canvas1->SetRoot(m_datatree);

    //wxCanvasObjectGroup* group3 = new wxCanvasObjectGroup(0,0);
   // group3->Prepend( cir );

    group3->SetAdmin(&m_canvasadmin);
    m_canvas2->SetRoot(group3);

    m_timer = new wxTimer( this );
    //m_timer->Start( 100, FALSE );
}

MyFrame::~MyFrame()
{
   m_datatree->RemoveLastEventHandler( FALSE );
   delete m_eventhandler;
   delete m_timer;
}

void MyFrame::Quit(wxCommandEvent& WXUNUSED(event) )
{
  Close(TRUE);
}

void MyFrame::SplitHorizontal(wxCommandEvent& WXUNUSED(event) )
{
  if ( m_splitter->IsSplit() )
    m_splitter->Unsplit();
  m_canvas1->Show(TRUE);
  m_canvas2->Show(TRUE);
  m_splitter->SplitHorizontally( m_canvas1, m_canvas2 );
  UpdatePosition();
}

void MyFrame::SplitVertical(wxCommandEvent& WXUNUSED(event) )
{
  if ( m_splitter->IsSplit() )
    m_splitter->Unsplit();
  m_canvas1->Show(TRUE);
  m_canvas2->Show(TRUE);
  m_splitter->SplitVertically( m_canvas1, m_canvas2 );
  UpdatePosition();
}

void MyFrame::Unsplit(wxCommandEvent& WXUNUSED(event) )
{
  if ( m_splitter->IsSplit() )
    m_splitter->Unsplit();
  SetStatusText("No splitter");
}

void MyFrame::SetMinSize(wxCommandEvent& WXUNUSED(event) )
{
  wxString str;
  str.Printf( _T("%d"), m_splitter->GetMinimumPaneSize());
  str = wxGetTextFromUser("Enter minimal size for panes:", "", str, this);
  if ( str.IsEmpty() )
    return;

  int minsize = wxStrtol( str, (wxChar**)NULL, 10 );
  m_splitter->SetMinimumPaneSize(minsize);
  str.Printf( _T("Min pane size = %d"), minsize);
  SetStatusText(str, 1);
}

void MyFrame::UpdateUIHorizontal(wxUpdateUIEvent& event)
{
  event.Enable( ( (!m_splitter->IsSplit()) || (m_splitter->GetSplitMode() != wxSPLIT_HORIZONTAL) ) );
}

void MyFrame::UpdateUIVertical(wxUpdateUIEvent& event)
{
  event.Enable( ( (!m_splitter->IsSplit()) || (m_splitter->GetSplitMode() != wxSPLIT_VERTICAL) ) );
}

void MyFrame::UpdateUIUnsplit(wxUpdateUIEvent& event)
{
  event.Enable( m_splitter->IsSplit() );
}

void MyFrame::UpdatePosition()
{
  wxString str;
  str.Printf( "Sash position = %d", m_splitter->GetSashPosition());
  SetStatusText(str);
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
    Close( TRUE );
}

void MyFrame::OnTimer( wxTimerEvent &WXUNUSED(event) )
{
    m_sm1->MoveRelative( 1, 0);
    m_sm2->MoveRelative( 1, 0);
    m_sm3->MoveRelative( 1, 0);
    m_sm4->MoveRelative( 2, 0);

//    m_ref->MoveRelative( 1, 0 );
    m_ref2->MoveRelative( 2, 0 );
    m_canvas1->UpdateNow();


    wxWakeUpIdle();
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
   (void)wxMessageBox( "wxCanvas demo\n"
                      "Robert Roebling (c) 1998,2000 \n Modified by Klaas Holwerda 2000",
                      "About wxCanvas Demo", wxICON_INFORMATION | wxOK );
}


//------------------------------------------------
// class MyCanvas
//------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(MyCanvas,wxCanvas)
    EVT_MOUSE_EVENTS (MyCanvas::OnMouseEvent)
END_EVENT_TABLE()

MyCanvas::MyCanvas(wxCanvasAdmin* admin, MySplitterWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxCanvas( admin, parent, id, position, size, style )
{
    m_parent=parent;
}

void MyCanvas::OnMouseEvent(wxMouseEvent& event)
{
    wxClientDC dc(this);
    PrepareDC(dc);

    wxPoint pos = event.GetPosition();

    m_mouse_worldx = DeviceToLogicalX( pos.x );
    m_mouse_worldy = DeviceToLogicalY( pos.y );

    wxString str;
    str.Printf( "Current mouse position: %f,%f", m_mouse_worldx, m_mouse_worldy );
    m_parent->SetStatusText( str );

    if (!event.m_controlDown && !GetCaptured())
    {
        if (event.LeftDown())
        {
            m_zoom_x1=m_zoom_x2=pos.x;
            m_zoom_y1=m_zoom_y2=pos.y;
        }
        if (event.RightDown())
        {
            SetMappingScroll(m_virtm_minX,m_virtm_minY,m_virtm_maxX,m_virtm_maxY,0);
            Update( 0,0, GetBufferWidth(), GetBufferHeight(), TRUE );
            UpdateNow();
        }
        if (event.LeftUp())
        {
            m_zoom_x2=pos.x;
            m_zoom_y2=pos.y;
            double x_virt_min=DeviceToLogicalX(m_zoom_x1);
            double y_virt_min=DeviceToLogicalY(m_zoom_y2);
            double x_virt_max=DeviceToLogicalX(m_zoom_x2);
            double y_virt_max=DeviceToLogicalY(m_zoom_y1);
            SetMappingScroll(x_virt_min,y_virt_min,x_virt_max,y_virt_max,0);
            Update( 0,0, GetBufferWidth(), GetBufferHeight(), TRUE );
            UpdateNow();
        }
        if (event.Dragging()&& event.m_leftDown)
        {
            dc.SetLogicalFunction(wxINVERT);
            dc.DrawRectangle(m_zoom_x1,m_zoom_y1,m_zoom_x2-m_zoom_x1,m_zoom_y2-m_zoom_y1 );
            m_zoom_x2=pos.x;
            m_zoom_y2=pos.y;
            dc.DrawRectangle(m_zoom_x1,m_zoom_y1,m_zoom_x2-m_zoom_x1,m_zoom_y2-m_zoom_y1 );
        }
    }
    else
    {
        wxCanvas::OnMouse(event);
    }
}


//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

// main program

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
  m_fontpath = getenv("TRUETYPE");
  m_fontpath = "c:/WINNT/Fonts/times.ttf";
  if ( !m_fontpath )
  {
      wxLogError("Please set env var TRUETYPE to the path where times.ttf lives.");

      return FALSE;

  }

#if wxUSE_LIBPNG
  wxImage::AddHandler( new wxPNGHandler );
#endif

  wxFrame *frame = new MyFrame((wxFrame *) NULL, "wxCanvas Example", wxPoint(0,0), wxSize(400,500));
  frame->Show( TRUE );

  SetTopWindow(frame);

  return TRUE;
}

