/////////////////////////////////////////////////////////////////////////////
// Name:        controls.cpp
// Purpose:     Controls wxWindows sample
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling, Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "controls.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/spinbutt.h"
#include "wx/notebook.h"
#include "wx/imaglist.h"
#include "wx/spinbutt.h"
#include "wx/clipbrd.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #define USE_XPM
#endif

#ifdef USE_XPM
    #include "mondrian.xpm"
    #include "icons/choice.xpm"
    #include "icons/combo.xpm"
    #include "icons/list.xpm"
    #include "icons/radio.xpm"
    #include "icons/text.xpm"
    #include "icons/gauge.xpm"
#endif

//----------------------------------------------------------------------
// class definitions
//----------------------------------------------------------------------

class MyApp: public wxApp
{
  public:
    bool OnInit(void);
};

class MyPanel: public wxPanel
{
  public:

    MyPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~MyPanel();

    void OnSize( wxSizeEvent& event );
    void OnListBox( wxCommandEvent &event );
    void OnListBoxDoubleClick( wxCommandEvent &event );
    void OnListBoxButtons( wxCommandEvent &event );
    void OnChoice( wxCommandEvent &event );
    void OnChoiceButtons( wxCommandEvent &event );
    void OnCombo( wxCommandEvent &event );
    void OnComboButtons( wxCommandEvent &event );
    void OnRadio( wxCommandEvent &event );
    void OnRadioButtons( wxCommandEvent &event );
    void OnSetFont( wxCommandEvent &event );
    void OnPageChanged( wxNotebookEvent &event );
    void OnSliderUpdate( wxCommandEvent &event );
    void OnSpinUpdate( wxSpinEvent &event );
    void OnPasteFromClipboard( wxCommandEvent &event );
    void OnCopyToClipboard( wxCommandEvent &event );

    wxListBox     *m_listbox;
    wxChoice      *m_choice;
    wxComboBox    *m_combo;
    wxRadioBox    *m_radio;
    wxGauge       *m_gauge;
    wxSlider      *m_slider;
    wxButton      *m_fontButton;
    wxSpinButton  *m_spinbutton;
    wxTextCtrl    *m_spintext;
    wxTextCtrl    *m_multitext;
    wxCheckBox    *m_checkbox;

    wxTextCtrl    *m_text;
    wxNotebook    *m_notebook;

  DECLARE_EVENT_TABLE()
};

class MyFrame: public wxFrame
{
  public:

    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);

  public:

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    bool OnClose(void) { return TRUE; }

  DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------
// main()
//----------------------------------------------------------------------

IMPLEMENT_APP	(MyApp)

//----------------------------------------------------------------------
// MyApp
//----------------------------------------------------------------------

const   int MINIMAL_QUIT   = 100;
const   int MINIMAL_TEXT   = 101;
const   int MINIMAL_ABOUT  = 102;

bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame((wxFrame *) NULL,
                               "Controls wxWindows App",
                               50, 50, 530, 420);

  // Give it an icon
  // The wxICON() macros loads an icon from a resource under Windows
  // and uses an #included XPM image under GTK+ and Motif

  frame->SetIcon( wxICON(mondrian) );

  wxMenu *file_menu = new wxMenu;

  file_menu->Append(MINIMAL_ABOUT, "&About");
  file_menu->Append(MINIMAL_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  frame->SetMenuBar(menu_bar);

  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

//----------------------------------------------------------------------
// MyPanel
//----------------------------------------------------------------------

const int  ID_NOTEBOOK          = 1000;

const int  ID_LISTBOX           = 130;
const int  ID_LISTBOX_SEL_NUM   = 131;
const int  ID_LISTBOX_SEL_STR   = 132;
const int  ID_LISTBOX_CLEAR     = 133;
const int  ID_LISTBOX_APPEND    = 134;
const int  ID_LISTBOX_DELETE    = 135;
const int  ID_LISTBOX_FONT      = 136;
const int  ID_LISTBOX_ENABLE    = 137;

const int  ID_CHOICE            = 120;
const int  ID_CHOICE_SEL_NUM    = 121;
const int  ID_CHOICE_SEL_STR    = 122;
const int  ID_CHOICE_CLEAR      = 123;
const int  ID_CHOICE_APPEND     = 124;
const int  ID_CHOICE_DELETE     = 125;
const int  ID_CHOICE_FONT       = 126;
const int  ID_CHOICE_ENABLE     = 127;

const int  ID_COMBO             = 140;
const int  ID_COMBO_SEL_NUM     = 141;
const int  ID_COMBO_SEL_STR     = 142;
const int  ID_COMBO_CLEAR       = 143;
const int  ID_COMBO_APPEND      = 144;
const int  ID_COMBO_DELETE      = 145;
const int  ID_COMBO_FONT        = 146;
const int  ID_COMBO_ENABLE      = 147;

const int  ID_TEXT              = 150;
const int  ID_PASTE_TEXT        = 151;
const int  ID_COPY_TEXT         = 152;

const int  ID_RADIOBOX          = 160;
const int  ID_RADIOBOX_SEL_NUM  = 161;
const int  ID_RADIOBOX_SEL_STR  = 162;
const int  ID_RADIOBOX_FONT     = 163;
const int  ID_RADIOBOX_ENABLE   = 164;

const int  ID_RADIOBUTTON_1     = 166;
const int  ID_RADIOBUTTON_2     = 167;

const int  ID_SET_FONT          = 170;

const int  ID_GAUGE             = 180;
const int  ID_SLIDER            = 181;

const int  ID_SPIN              = 182;


BEGIN_EVENT_TABLE(MyPanel, wxPanel)
  EVT_SIZE      (                         MyPanel::OnSize)
  EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK,  MyPanel::OnPageChanged)
  EVT_LISTBOX   (ID_LISTBOX,              MyPanel::OnListBox)
  EVT_LISTBOX_DCLICK(ID_LISTBOX,          MyPanel::OnListBoxDoubleClick)
  EVT_BUTTON    (ID_LISTBOX_SEL_NUM,      MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_SEL_STR,      MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_CLEAR,        MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_APPEND,       MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_DELETE,       MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_FONT,         MyPanel::OnListBoxButtons)
  EVT_CHECKBOX  (ID_LISTBOX_ENABLE,       MyPanel::OnListBoxButtons)
  EVT_CHOICE    (ID_CHOICE,               MyPanel::OnChoice)
  EVT_BUTTON    (ID_CHOICE_SEL_NUM,       MyPanel::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_SEL_STR,       MyPanel::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_CLEAR,         MyPanel::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_APPEND,        MyPanel::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_DELETE,        MyPanel::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_FONT,          MyPanel::OnChoiceButtons)
  EVT_CHECKBOX  (ID_CHOICE_ENABLE,        MyPanel::OnChoiceButtons)
  EVT_CHOICE    (ID_COMBO,                MyPanel::OnCombo)
  EVT_BUTTON    (ID_COMBO_SEL_NUM,        MyPanel::OnComboButtons)
  EVT_BUTTON    (ID_COMBO_SEL_STR,        MyPanel::OnComboButtons)
  EVT_BUTTON    (ID_COMBO_CLEAR,          MyPanel::OnComboButtons)
  EVT_BUTTON    (ID_COMBO_APPEND,         MyPanel::OnComboButtons)
  EVT_BUTTON    (ID_COMBO_DELETE,         MyPanel::OnComboButtons)
  EVT_BUTTON    (ID_COMBO_FONT,           MyPanel::OnComboButtons)
  EVT_CHECKBOX  (ID_COMBO_ENABLE,         MyPanel::OnComboButtons)
  EVT_RADIOBOX  (ID_RADIOBOX,             MyPanel::OnRadio)
  EVT_BUTTON    (ID_RADIOBOX_SEL_NUM,     MyPanel::OnRadioButtons)
  EVT_BUTTON    (ID_RADIOBOX_SEL_STR,     MyPanel::OnRadioButtons)
  EVT_BUTTON    (ID_RADIOBOX_FONT,        MyPanel::OnRadioButtons)
  EVT_CHECKBOX  (ID_RADIOBOX_ENABLE,      MyPanel::OnRadioButtons)
  EVT_BUTTON    (ID_SET_FONT,             MyPanel::OnSetFont)
  EVT_SLIDER    (ID_SLIDER,               MyPanel::OnSliderUpdate)
  EVT_SPIN      (ID_SPIN,                 MyPanel::OnSpinUpdate)
  EVT_BUTTON    (ID_PASTE_TEXT,           MyPanel::OnPasteFromClipboard)
  EVT_BUTTON    (ID_COPY_TEXT,            MyPanel::OnCopyToClipboard)
END_EVENT_TABLE()

MyPanel::MyPanel( wxFrame *frame, int x, int y, int w, int h ) :
  wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) )
{
//  SetBackgroundColour("cadet blue");

  m_text = new wxTextCtrl( this, -1, "This is the log window.\n", wxPoint(0,50), wxSize(100,50), wxTE_MULTILINE );
//  m_text->SetBackgroundColour("wheat");

  m_notebook = new wxNotebook( this, ID_NOTEBOOK, wxPoint(0,0), wxSize(200,150) );

  wxString choices[] =
  {
    "This",
    "is one of my",
    "really",
    "wonderful",
    "examples."
  };

#ifdef USE_XPM
  // image ids
  enum
  {
    Image_List, Image_Choice, Image_Combo, Image_Text, Image_Radio, Image_Gauge, Image_Max
  };

  // fill the image list
  wxImageList *imagelist = new wxImageList(32, 32);

  imagelist-> Add( wxBitmap( list_xpm ));
  imagelist-> Add( wxBitmap( choice_xpm ));
  imagelist-> Add( wxBitmap( combo_xpm ));
  imagelist-> Add( wxBitmap( text_xpm ));
  imagelist-> Add( wxBitmap( radio_xpm ));
  imagelist-> Add( wxBitmap( gauge_xpm ));
  m_notebook->SetImageList(imagelist);
#elif defined(__WXMSW__)
  // load images from resources
  enum
  {
    Image_List, Image_Choice, Image_Combo, Image_Text, Image_Radio, Image_Gauge, Image_Max
  };
  wxImageList *imagelist = new wxImageList(32, 32, FALSE, Image_Max);

  static const char *s_iconNames[Image_Max] =
  {
      "list", "choice", "combo", "text", "radio", "gauge"
  };

  for ( size_t n = 0; n < Image_Max; n++ )
  {
      wxBitmap bmp(s_iconNames[n]);
      if ( !bmp.Ok() || (imagelist->Add(bmp) == -1) )
      {
          wxLogWarning("Couldn't load the image '%s' for the notebook page %d.",
                       s_iconNames[n], n);
      }
  }

  m_notebook->SetImageList(imagelist);
#else

// No images for now
#define    Image_List -1
#define    Image_Choice -1
#define    Image_Combo -1
#define    Image_Text -1
#define    Image_Radio -1
#define    Image_Gauge -1
#define    Image_Max -1

#endif

  wxButton *button = (wxButton*)NULL;

//  m_notebook->SetBackgroundColour("cadet blue");

  wxPanel *panel = (wxPanel*) NULL;

  panel = new wxPanel(m_notebook);
//  panel->SetBackgroundColour("cadet blue");
//  panel->SetForegroundColour("blue");
  m_listbox = new wxListBox( panel, ID_LISTBOX, wxPoint(10,10), wxSize(120,70), 5, choices );
//  m_listbox->SetBackgroundColour("wheat");
  (void)new wxButton( panel, ID_LISTBOX_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(140,30) );
  (void)new wxButton( panel, ID_LISTBOX_SEL_STR, "Select 'This'", wxPoint(340,30), wxSize(140,30) );
  (void)new wxButton( panel, ID_LISTBOX_CLEAR, "Clear", wxPoint(180,80), wxSize(140,30) );
  (void)new wxButton( panel, ID_LISTBOX_APPEND, "Append 'Hi!'", wxPoint(340,80), wxSize(140,30) );
  (void)new wxButton( panel, ID_LISTBOX_DELETE, "Delete selected item", wxPoint(180,130), wxSize(140,30) );
  button = new wxButton( panel, ID_LISTBOX_FONT, "Set Italic font", wxPoint(340,130), wxSize(140,30) );
//  button->SetForegroundColour( "red" );
  m_checkbox = new wxCheckBox( panel, ID_LISTBOX_ENABLE, "Disable", wxPoint(20,130), wxSize(140,30) );
  m_checkbox->SetValue(FALSE);
  m_notebook->AddPage(panel, "wxList", TRUE, Image_List);

  panel = new wxPanel(m_notebook);
//  panel->SetBackgroundColour("cadet blue");
//  panel->SetForegroundColour("blue");
  m_choice = new wxChoice( panel, ID_CHOICE, wxPoint(10,10), wxSize(120,-1), 5, choices );
//  m_choice->SetBackgroundColour("wheat");
  (void)new wxButton( panel, ID_CHOICE_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(140,30) );
  (void)new wxButton( panel, ID_CHOICE_SEL_STR, "Select 'This'", wxPoint(340,30), wxSize(140,30) );
  (void)new wxButton( panel, ID_CHOICE_CLEAR, "Clear", wxPoint(180,80), wxSize(140,30) );
  (void)new wxButton( panel, ID_CHOICE_APPEND, "Append 'Hi!'", wxPoint(340,80), wxSize(140,30) );
  (void)new wxButton( panel, ID_CHOICE_DELETE, "Delete selected item", wxPoint(180,130), wxSize(140,30) );
  (void)new wxButton( panel, ID_CHOICE_FONT, "Set Italic font", wxPoint(340,130), wxSize(140,30) );
  (void)new wxCheckBox( panel, ID_CHOICE_ENABLE, "Disable", wxPoint(20,130), wxSize(140,30) );
  m_notebook->AddPage(panel, "wxChoice", FALSE, Image_Choice);

  panel = new wxPanel(m_notebook);
//  panel->SetBackgroundColour("cadet blue");
//  panel->SetForegroundColour("blue");
  m_combo = new wxComboBox( panel, ID_COMBO, "This", wxPoint(10,10), wxSize(120,-1), 5, choices );
//  m_combo->SetBackgroundColour("wheat");
  (void)new wxButton( panel, ID_COMBO_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(140,30) );
  (void)new wxButton( panel, ID_COMBO_SEL_STR, "Select 'This'", wxPoint(340,30), wxSize(140,30) );
  (void)new wxButton( panel, ID_COMBO_CLEAR, "Clear", wxPoint(180,80), wxSize(140,30) );
  (void)new wxButton( panel, ID_COMBO_APPEND, "Append 'Hi!'", wxPoint(340,80), wxSize(140,30) );
  (void)new wxButton( panel, ID_COMBO_DELETE, "Delete selected item", wxPoint(180,130), wxSize(140,30) );
  (void)new wxButton( panel, ID_COMBO_FONT, "Set Italic font", wxPoint(340,130), wxSize(140,30) );
  (void)new wxCheckBox( panel, ID_COMBO_ENABLE, "Disable", wxPoint(20,130), wxSize(140,30) );
  m_notebook->AddPage(panel, "wxComboBox", FALSE, Image_Combo);

  panel = new wxPanel(m_notebook);
//  panel->SetBackgroundColour("cadet blue");
//  panel->SetForegroundColour("blue");
  wxTextCtrl *tc = new wxTextCtrl( panel, ID_TEXT, "Write text here.", wxPoint(10,10), wxSize(320,28));
  (*tc) << " More text.";
//  tc->SetBackgroundColour("wheat");
  m_multitext = new wxTextCtrl( panel, ID_TEXT, "And here.", wxPoint(10,50), wxSize(320,160), wxTE_MULTILINE  );
  (*m_multitext) << " More text.";
  m_multitext->SetBackgroundColour("wheat");
  (void)new wxStaticBox( panel, -1, "wxClipboard", wxPoint(345,50), wxSize(160,145) );
  (void)new wxButton( panel, ID_COPY_TEXT, "Copy line 1", wxPoint(370,80), wxSize(110,30) );
  (void)new wxButton( panel, ID_PASTE_TEXT, "Paste text", wxPoint(370,140), wxSize(110,30) );
  m_notebook->AddPage(panel, "wxTextCtrl" , FALSE, Image_Text);

  wxString choices2[] =
  {
    "Wonderful",
    "examples.",
  };

  panel = new wxPanel(m_notebook);
//  panel->SetBackgroundColour("cadet blue");
//  panel->SetForegroundColour("blue");
  m_radio = new wxRadioBox( panel, ID_RADIOBOX, "That", wxPoint(10,160), wxSize(-1,-1), 2, choices2, 1, wxRA_SPECIFY_ROWS );
//  m_radio->SetBackgroundColour("wheat");
  m_radio = new wxRadioBox( panel, ID_RADIOBOX, "This", wxPoint(10,10), wxSize(-1,-1), 5, choices, 1, wxRA_SPECIFY_COLS );
//  m_radio->SetBackgroundColour("wheat");
  (void)new wxButton( panel, ID_RADIOBOX_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(140,30) );
  (void)new wxButton( panel, ID_RADIOBOX_SEL_STR, "Select 'This'", wxPoint(180,80), wxSize(140,30) );
  m_fontButton = new wxButton( panel, ID_SET_FONT, "Set more Italic font", wxPoint(340,30), wxSize(140,30) );
//  m_fontButton->SetForegroundColour("blue");
  (void)new wxButton( panel, ID_RADIOBOX_FONT, "Set Italic font", wxPoint(340,80), wxSize(140,30) );
  (void)new wxCheckBox( panel, ID_RADIOBOX_ENABLE, "Disable", wxPoint(340,130), wxSize(140,30) );
  wxRadioButton *rb = new wxRadioButton( panel, ID_RADIOBUTTON_1, "Radiobutton1", wxPoint(210,170), wxSize(110,30) );
  rb->SetValue( FALSE );
  (void)new wxRadioButton( panel, ID_RADIOBUTTON_2, "Radiobutton2", wxPoint(340,170), wxSize(110,30) );
  m_notebook->AddPage(panel, "wxRadioBox", FALSE, Image_Radio);

  panel = new wxPanel(m_notebook);
//  panel->SetBackgroundColour("cadet blue");
//  panel->SetForegroundColour("blue");
  (void)new wxStaticBox( panel, -1, "wxGauge and wxSlider", wxPoint(10,10), wxSize(180,130) );
  m_gauge = new wxGauge( panel, -1, 200, wxPoint(18,50), wxSize(155, 30) );
//  m_gauge->SetBackgroundColour("wheat");
  m_slider = new wxSlider( panel, ID_SLIDER, 0, 0, 200, wxPoint(18,90), wxSize(155,-1) );
//  m_slider->SetBackgroundColour("wheat");
  (void)new wxStaticBox( panel, -1, "Explanation", wxPoint(200,10), wxSize(290,130) );
#ifdef __WXMOTIF__
  // No wrapping text in wxStaticText yet :-(
  (void)new wxStaticText( panel, -1,
    "Drag the slider!",
    wxPoint(208,30),
    wxSize(210, -1)
    );
#else
  (void)new wxStaticText( panel, -1,
    "In order see the gauge (aka progress bar)\n"
    "control do something you have to drag the\n"
    "handle of the slider to the right.\n"
    "\n"
    "This is also supposed to demonstrate how\n"
    "to use static controls.\n",
    wxPoint(208,25),
    wxSize(210, 110)
    );
#endif
  m_spintext = new wxTextCtrl( panel, -1, "0", wxPoint(20,160), wxSize(80,-1) );
//  m_spintext->SetBackgroundColour("wheat");
  m_spinbutton = new wxSpinButton( panel, ID_SPIN, wxPoint(103,159), wxSize(-1,-1) );
//  m_spinbutton->SetBackgroundColour("wheat");
  m_spinbutton->SetRange(0,100);

  m_notebook->AddPage(panel, "wxGauge", FALSE, Image_Gauge);
}

void MyPanel::OnPasteFromClipboard( wxCommandEvent &WXUNUSED(event) )
{
#ifdef __WXGTK__

  if (!wxTheClipboard->Open())
  {
     *m_text << "Error opening the clipboard." << "\n";

     return;
  }
  else
  {
     *m_text << "Successfully opened the clipboard." << "\n";
  }

  wxTextDataObject *data = new wxTextDataObject();

  if (wxTheClipboard->GetData( data ))
  {
     *m_text << "Successfully retrieved data from the clipboard." << "\n";
     *m_multitext << data->GetText() << "\n";
  }
  else
  {
     *m_text << "Error getting data from the clipboard." << "\n";
  }

  wxTheClipboard->Close();

  *m_text << "Closed the clipboard." << "\n";

  delete data;

#endif
}

void MyPanel::OnCopyToClipboard( wxCommandEvent &WXUNUSED(event) )
{
#ifdef __WXGTK__

  wxString text( m_multitext->GetLineText(0) );

  if (text.IsEmpty()) return;

  if (!wxTheClipboard->Open())
  {
     *m_text << "Error opening the clipboard." << "\n";

     return;
  }
  else
  {
     *m_text << "Successfully opened the clipboard." << "\n";
  }

  wxTextDataObject *data = new wxTextDataObject( text );
  wxDataBroker *broker = new wxDataBroker();
  broker->Add( data );

  if (!wxTheClipboard->SetData( broker ))
  {
     *m_text << "Error while copying to the clipboard." << "\n";
  }
  else
  {
     *m_text << "Successfully copied data to the clipboard." << "\n";
  }

  wxTheClipboard->Close();

  *m_text << "Closed the clipboard." << "\n";

#endif
}

void MyPanel::OnSize( wxSizeEvent& WXUNUSED(event) )
{
  int x = 0;
  int y = 0;
  GetClientSize( &x, &y );

  if (m_notebook) m_notebook->SetSize( 2, 2, x-4, y*2/3-4 );
  if (m_text) m_text->SetSize( 2, y*2/3+2, x-4, y/3-4 );
}

void MyPanel::OnPageChanged( wxNotebookEvent &event )
{
  *m_text << "Notebook selection is " << event.GetSelection() << "\n";
}

void MyPanel::OnListBox( wxCommandEvent &event )
{
  m_text->WriteText( "ListBox selection string is: " );
  m_text->WriteText( event.GetString() );
  m_text->WriteText( "\n" );
}

void MyPanel::OnListBoxDoubleClick( wxCommandEvent &event )
{
  m_text->WriteText( "ListBox double click string is: " );
  m_text->WriteText( event.GetString() );
  m_text->WriteText( "\n" );
}

void MyPanel::OnListBoxButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_LISTBOX_ENABLE:
    {
      m_text->WriteText("Checkbox clicked.\n");
      m_listbox->Enable( event.GetInt() == 0 );
      break;
    }
    case ID_LISTBOX_SEL_NUM:
    {
      m_listbox->SetSelection( 2 );
      break;
    }
    case ID_LISTBOX_SEL_STR:
    {
      m_listbox->SetStringSelection( "This" );
      break;
    }
    case ID_LISTBOX_CLEAR:
    {
      m_listbox->Clear();
      break;
    }
    case ID_LISTBOX_APPEND:
    {
      m_listbox->Append( "Hi!" );
      break;
    }
    case ID_LISTBOX_DELETE:
    {
      int idx = m_listbox->GetSelection();
      m_listbox->Delete( idx );
      break;
    }
    case ID_LISTBOX_FONT:
    {
      m_listbox->SetFont( *wxITALIC_FONT );
      m_checkbox->SetFont( *wxITALIC_FONT );
      break;
    }
  }
}

void MyPanel::OnChoice( wxCommandEvent &event )
{
  m_text->WriteText( "Choice selection string is: " );
  m_text->WriteText( event.GetString() );
  m_text->WriteText( "\n" );
}

void MyPanel::OnChoiceButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_CHOICE_ENABLE:
    {
      m_choice->Enable( event.GetInt() == 0 );
      break;
    }
    case ID_CHOICE_SEL_NUM:
    {
      m_choice->SetSelection( 2 );
      break;
    }
    case ID_CHOICE_SEL_STR:
    {
      m_choice->SetStringSelection( "This" );
      break;
    }
    case ID_CHOICE_CLEAR:
    {
      m_choice->Clear();
      break;
    }
    case ID_CHOICE_APPEND:
    {
      m_choice->Append( "Hi!" );
      break;
    }
    case ID_CHOICE_DELETE:
    {
      int idx = m_choice->GetSelection();
      m_choice->Delete( idx );
      break;
    }
    case ID_CHOICE_FONT:
    {
      m_choice->SetFont( *wxITALIC_FONT );
      break;
    }
  }
}

void MyPanel::OnCombo( wxCommandEvent &event )
{
  m_text->WriteText( "ComboBox selection string is: " );
  m_text->WriteText( event.GetString() );
  m_text->WriteText( "\n" );
}

void MyPanel::OnComboButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_COMBO_ENABLE:
    {
      m_combo->Enable( event.GetInt() == 0 );
      break;
    }
    case ID_COMBO_SEL_NUM:
    {
      m_combo->SetSelection( 2 );
      break;
    }
    case ID_COMBO_SEL_STR:
    {
      m_combo->SetStringSelection( "This" );
      break;
    }
    case ID_COMBO_CLEAR:
    {
      m_combo->Clear();
      break;
    }
    case ID_COMBO_APPEND:
    {
      m_combo->Append( "Hi!" );
      break;
    }
    case ID_COMBO_DELETE:
    {
      int idx = m_combo->GetSelection();
      m_combo->Delete( idx );
      break;
    }
    case ID_COMBO_FONT:
    {
      m_combo->SetFont( *wxITALIC_FONT );
      break;
    }
  }
}

void MyPanel::OnRadio( wxCommandEvent &event )
{
  m_text->WriteText( "RadioBox selection string is: " );
  m_text->WriteText( event.GetString() );
  m_text->WriteText( "\n" );
}

void MyPanel::OnRadioButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_RADIOBOX_ENABLE:
    {
      m_radio->Enable( event.GetInt() == 0 );
      break;
    }
    case ID_RADIOBOX_SEL_NUM:
    {
      m_radio->SetSelection( 2 );
      break;
    }
    case ID_RADIOBOX_SEL_STR:
    {
      m_radio->SetStringSelection( "This" );
      break;
    }
    case ID_RADIOBOX_FONT:
    {
      m_radio->SetFont( *wxITALIC_FONT );
      break;
    }
  }
}

void MyPanel::OnSetFont( wxCommandEvent &WXUNUSED(event) )
{
  m_fontButton->SetFont( *wxITALIC_FONT );
  m_text->SetFont( *wxITALIC_FONT );
}

void MyPanel::OnSliderUpdate( wxCommandEvent &WXUNUSED(event) )
{
  m_gauge->SetValue( m_slider->GetValue() );
}

void MyPanel::OnSpinUpdate( wxSpinEvent &event )
{
  wxString value;
  value.sprintf( "%d", (int)event.GetPosition() );
  m_spintext->SetValue( value );
}

MyPanel::~MyPanel()
{
  delete m_notebook->GetImageList();
}

//----------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(MINIMAL_QUIT,   MyFrame::OnQuit)
    EVT_MENU(MINIMAL_ABOUT,  MyFrame::OnAbout)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
  (void)new MyPanel( this, 10, 10, 300, 100 );
}

void MyFrame::OnQuit (wxCommandEvent& WXUNUSED(event) )
{
  Close(TRUE);
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
  wxMessageDialog dialog(this, "This is a control sample", "About Controls", wxOK );
  dialog.ShowModal();
}
