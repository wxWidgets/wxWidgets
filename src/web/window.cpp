// Copyright (c) 2007 John Wilmes

#include "wx/window.h"
#include "wx/app.h"

IMPLEMENT_ABSTRACT_CLASS(wxWindowWeb, wxWindowBase)

wxWindow* wxWindowBase::DoFindFocus() {
    //TODO
    // cycle through top-level windows until we find one with focus
    return wxTheApp->GetTopWindow();
}

wxWindow* wxWindowBase::GetCapture() {
    //TODO - a better way to do this?
    return DoFindFocus();
}

wxWindowWeb::wxWindowWeb() {
}

wxWindowWeb::wxWindowWeb(wxWindow* parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name) {
    Create(parent, id, pos, size, style, name);
}

wxWindowWeb::~wxWindowWeb() {
}

bool wxWindowWeb::Create(wxWindow* parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name) {
    CreateBase(parent, id, pos, size, style, wxDefaultValidator, name);
    m_flushPending = false;
    return true;
}

bool wxWindowWeb::Reparent(wxWindowBase *newParent) {
    if (GetParent() == newParent) {
        return false;
    }
    SetParent(newParent);
    return true;
}

void wxWindowWeb::SetFocus()
{
    //TODO
}

bool wxWindowWeb::Show(bool show)
{
    //TODO
    return true;
}

void wxWindowWeb::Clear() {
    //TODO
}

void wxWindowWeb::Raise()
{
    //TODO
}

void wxWindowWeb::Lower()
{
    //TODO
}

void wxWindowWeb::Freeze() {
    //TODO
}

void wxWindowWeb::Thaw() {
    //TODO
}

void wxWindowWeb::SetLabel(const wxString& WXUNUSED(label))
{
    //TODO
}

WXWidget wxWindowWeb::GetHandle() const {
    //TODO
    return 0;
}

wxString wxWindowWeb::GetLabel() const
{
    //TODO
    return wxEmptyString;
}

int wxWindowWeb::GetScrollPos(int orient) const {
    //TODO
    return 0;
}

int wxWindowWeb::GetScrollRange(int orient) const {
    //TODO
    return 0;
}

int wxWindowWeb::GetScrollThumb(int orient) const {
    //TODO
    return 0;
}

void wxWindowWeb::DoCaptureMouse()
{
    //TODO
}

void wxWindowWeb::DoReleaseMouse()
{
    //TODO
}

bool wxWindowWeb::SetFont(const wxFont& font)
{
    //TODO
    return true;
}

bool wxWindowWeb::SetCursor(const wxCursor& cursor)
{
    //TODO
    return true;
}

void wxWindowWeb::SetScrollbar(int orient, int pos, int thumbVisible,
                               int range, bool refresh) {
    //TODO
}

void wxWindowWeb::SetScrollPos(int orient, int pos, bool refresh) {
    //TODO
}

void wxWindowWeb::WarpPointer (int x, int y)
{
    //TODO
}

void wxWindowWeb::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    //TODO
}

void wxWindowWeb::DoGetSize(int *x, int *y) const
{
    //TODO
   *x = 0;
   *y = 0; 
}

void wxWindowWeb::DoGetPosition(int *x, int *y) const
{
    //TODO
    *x = 0;
    *y = 0;
}

void wxWindowWeb::DoScreenToClient(int *x, int *y) const
{
    //TODO
    *x = 0;
    *y = 0;
}

void wxWindowWeb::DoClientToScreen(int *x, int *y) const
{
    //TODO
    *x = 0;
    *x = 0;
}


void wxWindowWeb::DoGetClientSize(int *x, int *y) const
{
    //TODO
    *x = 0;
    *x = 0;
}

void wxWindowWeb::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    //TODO
}

void wxWindowWeb::DoSetClientSize(int width, int height)
{
    //TODO
}

void wxWindowWeb::DoMoveWindow(int x, int y, int width, int height)
{
    //TODO
}

int wxWindowWeb::GetCharHeight() const
{
    //TODO
    return 0;
}

int wxWindowWeb::GetCharWidth() const
{
    //TODO
    return 0;
}

void wxWindowWeb::GetTextExtent(const wxString& string,
                                int *x, int *y,
                                int *descent, int *externalLeading,
                                const wxFont *theFont) const
{
    //TODO
    *x = 0;
    *y = 0;
}

void wxWindowWeb::Refresh(bool eraseBack, const wxRect *rect)
{
    //TODO
}

void wxWindowWeb::Update()
{
    //TODO
}

void wxWindowWeb::EvalInClient(const wxString& cmd) {
    m_evalBuffer += cmd;
    if (!m_flushPending) {
        m_flushPending = true;
        wxTheApp->RequestFlush(this);
    }
}

wxString wxWindowWeb::FlushClientEvalBuffer() {
    wxRect r = GetScreenRect();
    m_flushPending = false;
    wxString ret;
    ret.Printf("(new WindowContext(App, %d, %d, %d, %d, %d)).evaluate(\"%s\")", GetId(), r.x, r.y, r.width, r.height, m_evalBuffer);
    m_evalBuffer.Empty();
    return ret;
}
