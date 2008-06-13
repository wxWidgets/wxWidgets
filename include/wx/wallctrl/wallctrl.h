// ----------------------------------------------------------------------------
// wxWallCtrl: the Wall Control
// Mokhtar M. Khorshid
// ----------------------------------------------------------------------------

#include "wx/glcanvas.h"

#include "wx/string.h"
#include "wx/Wallctrl/WallCtrlDataSource.h"
#include "wx/Wallctrl/WallCtrlSurface.h"

// TODO: this one is for testing only
#include "wx/wallctrl/wallctrlplanesurface.h"

// For some reason wxVector< > does not compile, so for the meanwhile we'll use this vector
template <class T>
struct TempVector
{
	unsigned size()
	{
		return m_size;
	}
	int m_size;
};

class WXDLLEXPORT wxWallCtrl : public wxGLCanvas
{
public:
	// Constructors
	wxWallCtrl(wxWindow *parent, wxWallCtrlSurface *surface,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0, const wxString & name = _T("WallCtrl") )
		:wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name ), m_surface(surface)
	{
		m_init= false;
	}

	wxWallCtrl(wxWindow *parent, wxWallCtrlSurface *surface, const wxWallCtrl *other,
		wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
		const wxString& name )
		: wxGLCanvas(parent, other->GetContext(), id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name),m_surface(surface)
	{
		m_init=false;
	}

	// Render the control (and all its items)
	void Render()
	{
		SetCurrent();
		m_surface->Render(GetSize());
		SwapBuffers();
	}

	void OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
	{
		SetFocus();
	}

	void OnPaint( wxPaintEvent& WXUNUSED(event) )
	{
		Render();
	}

	void OnSize(wxSizeEvent& event)
	{
		// this is also necessary to update the context on some platforms
		wxGLCanvas::OnSize(event);
	}

	void OnEraseBackground(wxEraseEvent& WXUNUSED(event))
	{
		// Do nothing, to avoid flashing.
	}

	// TODO: This is only temp for testing
	void OnKeyDown(wxKeyEvent &event)
	{
		// TODO: wxDynamicCast should be used instead here
		//wxWallCtrlPlaneSurface * temp = wxDynamicCast(m_surface, wxWallCtrlPlaneSurface);
		wxWallCtrlPlaneSurface * temp = dynamic_cast<wxWallCtrlPlaneSurface *> (m_surface);
		if (!temp)
			return;
		switch (event.GetKeyCode())
		{
		case 'A'://WXK_LEFT:
			temp->MoveLeft(0.1);
			break;
		case 'D':
			temp->MoveRight(0.1);
			break;
		case 'W':
			temp->MoveIn(0.1);
			break;
		case 'S':
			temp->MoveOut(0.1);
			break;
		}
	
	}

	private:
		bool m_init;
		
		// TODO: Used for testing only
		wxBitmap tempTexture;


		// This is the data source that will supply the bitmaps
		wxWallCtrlDataSource * m_dataSource;

		// This will point to the single surface that is currently selected
		wxWallCtrlSurface * m_surface;

		DECLARE_EVENT_TABLE();

};

