///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __guiframe__
#define __guiframe__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/radiobut.h>
#include <wx/bmpbuttn.h>
#include <wx/statbmp.h>
#include <wx/gauge.h>
#include <wx/slider.h>
#include <wx/tglbtn.h>
#include <wx/hyperlink.h>
#include <wx/spinctrl.h>
#include <wx/spinbutt.h>
#include <wx/scrolbar.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/checklst.h>
#include <wx/listbox.h>
#include <wx/radiobox.h>
#include <wx/statbox.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include "wx/animate.h"
#include "wx/collpane.h"
#include <wx/textctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/clrpicker.h>
#include <wx/fontpicker.h>
#include <wx/filepicker.h>
#include <wx/calctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/dirctrl.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/bmpcbox.h>
#include "customcombo.h"
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class GUIFrame
///////////////////////////////////////////////////////////////////////////////
class GUIFrame : public wxFrame
{
	private:

	protected:
		enum
		{
			idMenuOpen = 1000,
			idMenuQuit,
			idMenuCapFullScreen,
			idMenuCapRect,
			idMenuEndCapRect,
			idMenuCapAll,
			idMenuAbout,
		};

		wxMenuBar* mbar;
		wxMenu* fileMenu;
		wxMenu* captureMenu;
		wxMenu* helpMenu;
		wxNotebook* m_notebook1;
		wxPanel* m_panel1;
		wxButton* m_button1;
		wxStaticText* m_staticText1;
		wxCheckBox* m_checkBox1;
		wxCheckBox* m_checkBox2;
		wxRadioButton* m_radioBtn1;
		wxRadioButton* m_radioBtn2;
		wxBitmapButton* m_bpButton1;
		wxStaticBitmap* m_bitmap1;
		wxGauge* m_gauge1;
		wxSlider* m_slider1;
		wxToggleButton* m_toggleBtn1;
		wxToggleButton* m_toggleBtn2;
		wxHyperlinkCtrl* m_hyperlink1;
		wxSpinCtrl* m_spinCtrl1;
		wxSpinButton* m_spinBtn1;
		wxScrollBar* m_scrollBar1;
		wxPanel* m_panel2;
		wxCheckListBox* m_checkList1;
		wxListBox* m_listBox1;
		wxRadioBox* m_radioBox1;
		wxStaticBox * m_staticBox1;
		wxTreeCtrl* m_treeCtrl1;
		wxListCtrl* m_listCtrl1;
		wxAnimationCtrl * m_animationCtrl1;
		wxCollapsiblePane *m_collPane1;
		wxCollapsiblePane *m_collPane2;
		wxPanel* m_panel3;
		wxTextCtrl* m_textCtrl1;

		wxTextCtrl* m_textCtrl2;
		wxRichTextCtrl* m_richText1;
		wxPanel* m_panel4;
		wxColourPickerCtrl* m_colourPicker1;
		wxFontPickerCtrl* m_fontPicker1;
		wxFilePickerCtrl* m_filePicker1;

		wxCalendarCtrl* m_calendar1;
		wxDatePickerCtrl* m_datePicker1;
		wxGenericDirCtrl* m_genericDirCtrl1;
		wxDirPickerCtrl* m_dirPicker1;
		wxPanel* m_panel5;
		wxChoice* m_choice1;
		wxComboBox* m_comboBox1;


		wxBitmapComboBox * m_bmpComboBox1;
		wxPenStyleComboBox * m_ownerDrawnComboBox1;


		wxComboCtrl * m_comboCtrl1;
		wxComboCtrl * m_comboCtrl2;
		wxStatusBar* statusBar;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void OnSeeScreenshots( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCaptureFullScreen( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCaptureRect( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEndCaptureRect( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCaptureAllControls( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnNotebookPageChanged( wxNotebookEvent& event ){ event.Skip(); }
		virtual void OnNotebookPageChanging( wxNotebookEvent& event ){ event.Skip(); }


	public:
		GUIFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("wxWidgets Control Screenshot Generator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~GUIFrame();

};

#endif //__guiframe__
