///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "bitmaps/play.xpm"
#include "bitmaps/stop.xpm"


#include "guiframe.h"

///////////////////////////////////////////////////////////////////////////

GUIFrame::GUIFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	mbar = new wxMenuBar( 0 );
	fileMenu = new wxMenu();
	wxMenuItem* m_menuSeeScr;
	m_menuSeeScr = new wxMenuItem( fileMenu, wxID_ZOOM_IN, wxString( _("&Open screenshots folder...") ) + wxT('\t') + wxT("Ctrl+O"), _("Opens the directory where the screenshots are saved."), wxITEM_NORMAL );
	fileMenu->Append( m_menuSeeScr );
	
	fileMenu->AppendSeparator();
	
	wxMenuItem* m_menuFileQuit;
	m_menuFileQuit = new wxMenuItem( fileMenu, wxID_EXIT, wxString( _("&Quit") ) + wxT('\t') + wxT("Alt+F4"), _("Quits the application."), wxITEM_NORMAL );
	fileMenu->Append( m_menuFileQuit );
	
	mbar->Append( fileMenu, _("&File") );
	
	captureMenu = new wxMenu();
	wxMenuItem* m_menuCapFullScreen;
	m_menuCapFullScreen = new wxMenuItem( captureMenu, idMenuCapFullScreen, wxString( _("&Full Screen") ) + wxT('\t') + wxT("Ctrl+Alt+F"), _("Takes a screenshot of the entire screen."), wxITEM_NORMAL );
	captureMenu->Append( m_menuCapFullScreen );
	
	wxMenuItem* m_menuCapRect;
	m_menuCapRect = new wxMenuItem( captureMenu, idMenuCapRect, wxString( _("Regions<Begin>") ) + wxT('\t') + wxT("Ctrl+Alt+R"), _("Manually specify rectangular regions for the screenshots."), wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuCapRect->SetBitmaps( wxICON( play ) );
	#elif defined( __WXGTK__ )
	m_menuCapRect->SetBitmap( wxICON( play ) );
	#endif
	captureMenu->Append( m_menuCapRect );
	
	wxMenuItem* m_menuEndCapRect;
	m_menuEndCapRect = new wxMenuItem( captureMenu, idMenuEndCapRect, wxString( _("Regions<End>") ) + wxT('\t') + wxT("Ctrl+Alt+E"), _("Stop manually generating screenshots."), wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuEndCapRect->SetBitmaps( wxICON( stop ) );
	#elif defined( __WXGTK__ )
	m_menuEndCapRect->SetBitmap( wxICON( stop ) );
	#endif
	captureMenu->Append( m_menuEndCapRect );
	m_menuEndCapRect->Enable( false );
	
	wxMenuItem* m_menuCapAll;
	m_menuCapAll = new wxMenuItem( captureMenu, idMenuCapAll, wxString( _("Capture All") ) + wxT('\t') + wxT("Ctrl+Alt+A"), _("Takes screenshots for all controls automatically."), wxITEM_NORMAL );
	captureMenu->Append( m_menuCapAll );
	
	mbar->Append( captureMenu, _("&Capture") );
	
	helpMenu = new wxMenu();
	wxMenuItem* m_menuHelpAbout;
	m_menuHelpAbout = new wxMenuItem( helpMenu, wxID_ABOUT, wxString( _("&About...") ) + wxT('\t') + wxT("F1"), _("Shows info about this application."), wxITEM_NORMAL );
	helpMenu->Append( m_menuHelpAbout );
	
	mbar->Append( helpMenu, _("&Help") );
	
	this->SetMenuBar( mbar );
	
	wxBoxSizer* bSizer0;
	bSizer0 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	m_button1 = new wxButton( m_panel1, wxID_ANY, _("wxButton"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_button1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, _("wxStaticText"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_checkBox1 = new wxCheckBox( m_panel1, wxID_ANY, _("Checked"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox1->SetValue(true);
	
	m_checkBox1->SetToolTip( _("wxCheckBox") );
	
	fgSizer1->Add( m_checkBox1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_checkBox2 = new wxCheckBox( m_panel1, wxID_ANY, _("Unchecked"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_checkBox2->SetToolTip( _("wxCheckBox") );
	
	fgSizer1->Add( m_checkBox2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 20 );
	
	m_radioBtn1 = new wxRadioButton( m_panel1, wxID_ANY, _("Checked"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_radioBtn1->SetValue( true ); 
	m_radioBtn1->SetToolTip( _("wxRadioButton") );
	
	fgSizer1->Add( m_radioBtn1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_radioBtn2 = new wxRadioButton( m_panel1, wxID_ANY, _("Unchecked"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtn2->SetToolTip( _("wxRadioButton") );
	
	fgSizer1->Add( m_radioBtn2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_bpButton1 = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( wxT("bitmaps/wxwin32x32.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpButton1->SetToolTip( _("wxBitmapButton") );
	
	m_bpButton1->SetToolTip( _("wxBitmapButton") );
	
	fgSizer1->Add( m_bpButton1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_bitmap1 = new wxStaticBitmap( m_panel1, wxID_ANY, wxBitmap( wxT("bitmaps/wxwin32x32.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	m_bitmap1->SetToolTip( _("wxStaticBitmap") );
	
	fgSizer1->Add( m_bitmap1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_gauge1 = new wxGauge( m_panel1, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL, wxDefaultValidator, wxT("_Gauge") );
	m_gauge1->SetValue( 50 ); 
	m_gauge1->SetToolTip( _("wxGauge") );
	
	fgSizer1->Add( m_gauge1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 20 );
	
	m_slider1 = new wxSlider( m_panel1, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	m_slider1->SetToolTip( _("wxSlider") );
	
	fgSizer1->Add( m_slider1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 20 );
	
	m_toggleBtn1 = new wxToggleButton( m_panel1, wxID_ANY, _("Untoggled"), wxDefaultPosition, wxDefaultSize, 0 );
	m_toggleBtn1->SetToolTip( _("wxToggleButton") );
	
	fgSizer1->Add( m_toggleBtn1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_toggleBtn2 = new wxToggleButton( m_panel1, wxID_ANY, _("Toggled"), wxDefaultPosition, wxDefaultSize, 0 );
	m_toggleBtn2->SetValue( true ); 
	m_toggleBtn2->SetToolTip( _("wxToggleButton") );
	
	fgSizer1->Add( m_toggleBtn2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 20 );
	
	m_hyperlink1 = new wxHyperlinkCtrl( m_panel1, wxID_ANY, _("www.wxwidgets.org"), wxT("http://www.wxwidgets.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	m_hyperlink1->SetToolTip( _("wxHyperlinkCtrl") );
	
	fgSizer1->Add( m_hyperlink1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 20 );
	
	m_spinCtrl1 = new wxSpinCtrl( m_panel1, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	m_spinCtrl1->SetToolTip( _("wxSpinCtrl") );
	
	fgSizer1->Add( m_spinCtrl1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_spinBtn1 = new wxSpinButton( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_spinBtn1->SetToolTip( _("wxSpinButton") );
	
	fgSizer1->Add( m_spinBtn1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_scrollBar1 = new wxScrollBar( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL );
	m_scrollBar1->SetToolTip( _("wxScrollBar") );
	
	fgSizer1->Add( m_scrollBar1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 20 );
	
	m_panel1->SetSizer( fgSizer1 );
	m_panel1->Layout();
	fgSizer1->Fit( m_panel1 );
	m_notebook1->AddPage( m_panel1, _("Tiny Controls"), true );
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_checkList1Choices[] = { _("wxCheckListBox"), _("Item1"), _("Item2") };
	int m_checkList1NChoices = sizeof( m_checkList1Choices ) / sizeof( wxString );
	m_checkList1 = new wxCheckListBox( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList1NChoices, m_checkList1Choices, 0 );
	fgSizer2->Add( m_checkList1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_listBox1 = new wxListBox( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_listBox1->Append( _("wxListBox") );
	m_listBox1->Append( _("Item1") );
	m_listBox1->Append( _("Item2") );
	fgSizer2->Add( m_listBox1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	wxString m_radioBox1Choices[] = { _("Item1"), _("Item2") };
	int m_radioBox1NChoices = sizeof( m_radioBox1Choices ) / sizeof( wxString );
	m_radioBox1 = new wxRadioBox( m_panel2, wxID_ANY, _("wxRadioBox"), wxDefaultPosition, wxDefaultSize, m_radioBox1NChoices, m_radioBox1Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox1->SetSelection( 0 );
	fgSizer2->Add( m_radioBox1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_staticBox1 = new wxStaticBox(m_panel2,wxID_ANY, _("wxStaticBox"), wxDefaultPosition, wxDefaultSize, 0, _("_StaticBox"));
	
	
	fgSizer2->Add( m_staticBox1, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 20 );
	
	m_treeCtrl1 = new wxTreeCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxSUNKEN_BORDER );
	fgSizer2->Add( m_treeCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_listCtrl1 = new wxListCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0|wxSUNKEN_BORDER );
	m_listCtrl1->SetToolTip( _("wxListCtrl") );
	
	fgSizer2->Add( m_listCtrl1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 20 );
	
	m_animationCtrl1 = new wxAnimationCtrl(m_panel2, wxID_ANY);
	m_animationCtrl1->SetToolTip(_("wxAnimationCtrl"));
	if (m_animationCtrl1->LoadFile(wxT("bitmaps/throbber.gif")))
	m_animationCtrl1->Play();
	fgSizer2->Add( m_animationCtrl1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_collPane1 = new wxCollapsiblePane(m_panel2, -1, wxT("Collapsed"));
	wxWindow *win = m_collPane1->GetPane();
	m_collPane1->SetToolTip(_("wxCollapsiblePane"));
	
	wxBoxSizer * collbSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText* m_collSText = new wxStaticText(win, -1, wxT("You can place"));
	wxButton* m_collBut = new wxButton(win, -1, wxT("anything"));
	wxTextCtrl* m_collText = new wxTextCtrl(win, -1, wxT("inside a wxCollapsiblePane"));
	collbSizer->Add( m_collSText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0 );
	collbSizer->Add( m_collBut, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0 );
	collbSizer->Add( m_collText, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0 );
	win->SetSizer(collbSizer);
	fgSizer2->Add( m_collPane1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_collPane2 = new wxCollapsiblePane(m_panel2, -1, wxT("Expanded"));
	wxWindow *win2 = m_collPane2->GetPane();
	m_collPane2->SetToolTip(_("wxCollapsiblePane"));
	
	wxBoxSizer * collbSizer2 = new wxBoxSizer(wxVERTICAL);
	wxStaticText* m_collSText2 = new wxStaticText(win2, -1, wxT("You can place"));
	wxButton* m_collBut2 = new wxButton(win2, -1, wxT("anything"));
	wxTextCtrl* m_collText2 = new wxTextCtrl(win2, -1, wxT("inside a wxCollapsiblePane"));
	collbSizer2->Add( m_collSText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0 );
	collbSizer2->Add( m_collBut2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0 );
	collbSizer2->Add( m_collText2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0 );
	win2->SetSizer(collbSizer2);
	
	m_collPane2->Expand();
	fgSizer2->Add( m_collPane2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_panel2->SetSizer( fgSizer2 );
	m_panel2->Layout();
	fgSizer2->Fit( m_panel2 );
	m_notebook1->AddPage( m_panel2, _("Multiple choice Controls"), false );
	m_panel3 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl1 = new wxTextCtrl( m_panel3, wxID_ANY, _("Single-line"), wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl1->SetToolTip( _("wxTextCtrl") );
	
	bSizer2->Add( m_textCtrl1, 0, wxTOP|wxRIGHT|wxLEFT, 20 );
	
	
	bSizer2->Add( 0, 10, 0, 0, 5 );
	
	m_textCtrl2 = new wxTextCtrl( m_panel3, wxID_ANY, _("Multi-line\nLine1\nLine2\n"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_textCtrl2->SetToolTip( _("wxTextCtrl") );
	
	bSizer2->Add( m_textCtrl2, 0, wxBOTTOM|wxRIGHT|wxLEFT, 20 );
	
	m_richText1 = new wxRichTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
	m_richText1->SetToolTip( _("wxRichTextCtrl") );
	m_richText1->SetMinSize( wxSize( 200,200 ) );
	
	bSizer2->Add( m_richText1, 0, wxALL, 20 );
	
	m_panel3->SetSizer( bSizer2 );
	m_panel3->Layout();
	bSizer2->Fit( m_panel3 );
	m_notebook1->AddPage( m_panel3, _("[Rich]Text Controls"), false );
	m_panel4 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 10, 10 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_colourPicker1 = new wxColourPickerCtrl( m_panel4, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_colourPicker1->SetToolTip( _("wxColourPickerCtrl") );
	
	fgSizer5->Add( m_colourPicker1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_fontPicker1 = new wxFontPickerCtrl( m_panel4, wxID_ANY, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE );
	m_fontPicker1->SetMaxPointSize( 100 ); 
	m_fontPicker1->SetToolTip( _("wxFontPickerCtrl") );
	
	fgSizer5->Add( m_fontPicker1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 20 );
	
	m_filePicker1 = new wxFilePickerCtrl( m_panel4, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE, wxDefaultValidator, wxT("_FilePickerCtrl") );
	m_filePicker1->SetToolTip( _("wxFilePickerCtrl") );
	
	fgSizer5->Add( m_filePicker1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 20 );
	
	
	fgSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_calendar1 = new wxCalendarCtrl( m_panel4, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxCAL_SHOW_HOLIDAYS );
	m_calendar1->SetToolTip( _("wxCalendarCtrl") );
	
	fgSizer5->Add( m_calendar1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_datePicker1 = new wxDatePickerCtrl( m_panel4, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT );
	m_datePicker1->SetToolTip( _("wxDatePickerCtrl") );
	
	fgSizer5->Add( m_datePicker1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_genericDirCtrl1 = new wxGenericDirCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER, wxEmptyString, 0, wxT("_GenericDirCtrl") );
	
	m_genericDirCtrl1->ShowHidden( false );
	m_genericDirCtrl1->SetToolTip( _("wxGenericDirCtrl") );
	m_genericDirCtrl1->SetMinSize( wxSize( -1,150 ) );
	
	fgSizer5->Add( m_genericDirCtrl1, 1, wxEXPAND|wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_dirPicker1 = new wxDirPickerCtrl( m_panel4, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE, wxDefaultValidator, wxT("_DirPickerCtrl") );
	m_dirPicker1->SetToolTip( _("wxDirPickerCtrl") );
	
	fgSizer5->Add( m_dirPicker1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_panel4->SetSizer( fgSizer5 );
	m_panel4->Layout();
	fgSizer5->Fit( m_panel4 );
	m_notebook1->AddPage( m_panel4, _("Picker Controls"), false );
	m_panel5 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_choice1Choices[] = { _("wxChoice"), _("Item1"), _("Item2"), _("Item3"), _("Item4") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	fgSizer4->Add( m_choice1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_comboBox1 = new wxComboBox( m_panel5, wxID_ANY, _("wxComboBox"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBox1->Append( _("wxComboBox") );
	m_comboBox1->Append( _("Item1") );
	m_comboBox1->Append( _("Item2") );
	m_comboBox1->Append( _("Item3") );
	m_comboBox1->Append( _("Item4") );
	fgSizer4->Add( m_comboBox1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	
	fgSizer4->Add( 0, 120, 1, wxEXPAND, 5 );
	
	
	fgSizer4->Add( 0, 120, 1, wxEXPAND, 5 );
	
	m_bmpComboBox1 = new wxBitmapComboBox(m_panel5, wxID_ANY,_("Item1"));
	m_bmpComboBox1->Append(_("Item1"), wxBitmap(_T("bitmaps/bell.png"),wxBITMAP_TYPE_PNG));
	m_bmpComboBox1->Append(_("Item2"), wxBitmap(_T("bitmaps/sound.png"),wxBITMAP_TYPE_PNG));
	m_bmpComboBox1->Append(_("Item3"), wxBitmap(_T("bitmaps/bell.png"),wxBITMAP_TYPE_PNG));
	m_bmpComboBox1->Append(_("Item4"), wxBitmap(_T("bitmaps/sound.png"),wxBITMAP_TYPE_PNG));
	m_bmpComboBox1->SetToolTip(_("wxBitmapComboBox"));
	fgSizer4->Add( m_bmpComboBox1, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 20 );
	
	m_ownerDrawnComboBox1 = PenStyleComboBox::CreateSample(m_panel5);
	    m_ownerDrawnComboBox1->SetToolTip(_("wxOwnerDrawnComboBox"));
	fgSizer4->Add( m_ownerDrawnComboBox1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 20 );
	
	
	fgSizer4->Add( 0, 90, 1, wxEXPAND, 5 );
	
	
	fgSizer4->Add( 0, 90, 1, wxEXPAND, 5 );
	
	m_comboCtrl1 = new wxComboCtrl(m_panel5,wxID_ANY,wxEmptyString);
	// first of all, set the popup control!
	ListViewComboPopup* popupList = new ListViewComboPopup();
	m_comboCtrl1->SetPopupControl(popupList);
	m_comboCtrl1->SetPopupMaxHeight(80);
	
	m_comboCtrl1->SetText(wxT("wxComboCtrl"));
	m_comboCtrl1->SetToolTip(_("wxComboCtrl"));
	
	// Populate using wxListView methods
	popupList->InsertItem(popupList->GetItemCount(),wxT("wxComboCtrl"));
	popupList->InsertItem(popupList->GetItemCount(),wxT("with"));
	popupList->InsertItem(popupList->GetItemCount(),wxT("wxListView"));
	popupList->InsertItem(popupList->GetItemCount(),wxT("popup"));
	popupList->InsertItem(popupList->GetItemCount(),wxT("Item1"));
	popupList->InsertItem(popupList->GetItemCount(),wxT("Item2"));
	popupList->InsertItem(popupList->GetItemCount(),wxT("Item3"));
	
	popupList->Select(0, true);
	fgSizer4->Add( m_comboCtrl1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 20 );
	
	m_comboCtrl2 = new wxComboCtrl(m_panel5,wxID_ANY,wxEmptyString);
	// first of all, set the popup control!
	TreeCtrlComboPopup* popupTree = new TreeCtrlComboPopup();
	m_comboCtrl2->SetPopupControl(popupTree);
	m_comboCtrl2->SetPopupMaxHeight(80);
	
	m_comboCtrl2->SetText(wxT("wxComboCtrl"));
	m_comboCtrl2->SetToolTip(_("wxComboCtrl"));
	
	//Add a root and some nodes using wxTreeCtrl methods
	wxTreeItemId root = popupTree->AddRoot(_("wxComboCtrl"));
	
	popupTree->AppendItem(root, _("with"));
	popupTree->AppendItem(root, _("wxTreeCtrl"));
	
	wxTreeItemId node2 = popupTree->AppendItem(root, _("popout"));
	popupTree->AppendItem(node2, _("Node1"));
	popupTree->AppendItem(node2, _("Node2"));
	
	popupTree->ExpandAll();
	fgSizer4->Add( m_comboCtrl2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 20 );
	
	m_panel5->SetSizer( fgSizer4 );
	m_panel5->Layout();
	fgSizer4->Fit( m_panel5 );
	m_notebook1->AddPage( m_panel5, _("Drop-down Controls"), false );
	
	bSizer0->Add( m_notebook1, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer0 );
	this->Layout();
	bSizer0->Fit( this );
	statusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( GUIFrame::OnClose ) );
	this->Connect( m_menuSeeScr->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnSeeScreenshots ) );
	this->Connect( m_menuFileQuit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnQuit ) );
	this->Connect( m_menuCapFullScreen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnCaptureFullScreen ) );
	this->Connect( m_menuCapRect->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnCaptureRect ) );
	this->Connect( m_menuEndCapRect->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnEndCaptureRect ) );
	this->Connect( m_menuCapAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnCaptureAllControls ) );
	this->Connect( m_menuHelpAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnAbout ) );
	m_notebook1->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( GUIFrame::OnNotebookPageChanged ), NULL, this );
	m_notebook1->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, wxNotebookEventHandler( GUIFrame::OnNotebookPageChanging ), NULL, this );
}

GUIFrame::~GUIFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( GUIFrame::OnClose ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnSeeScreenshots ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnQuit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnCaptureFullScreen ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnCaptureRect ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnEndCaptureRect ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnCaptureAllControls ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnAbout ) );
	m_notebook1->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( GUIFrame::OnNotebookPageChanged ), NULL, this );
	m_notebook1->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, wxNotebookEventHandler( GUIFrame::OnNotebookPageChanging ), NULL, this );
}
