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
#include "wx/wallctrl/wxWallCtrlNavigation.h"

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

	void SetNavigation (wxWallCtrlNavigation * navigation)
	{
		// TODO: See if we need to pop the old event handlers. But we should only pop navigation handlers anyway
	//	PopEventHandler();

		// Push the navigation as the current event handler
		PushEventHandler(navigation);
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

