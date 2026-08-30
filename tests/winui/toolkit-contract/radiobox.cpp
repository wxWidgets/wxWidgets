#include "wx/radiobox.h"
#include "wx/statbox.h"
#include "contract.h"

static_assert(std::is_base_of<wxControl, wxRadioBox>::value, "control API");
static_assert(std::is_base_of<wxRadioBoxBase, wxRadioBox>::value, "item API");
static_assert(std::is_base_of<wxStaticBox, wxRadioBox>::value == !WX_EXPECT_WINUI,
              "Native RadioBox composition is not a cross-toolkit contract");
template class NativeHooksProbe<wxRadioBox>;

void RadioBoxPortableContract(wxRadioBox& radio)
{
    radio.SetSelection(0);
    radio.SetString(0, "item");
    radio.Enable(0, true);
    radio.Show(0, true);
    radio.SetLabel("group");
    (void)radio.GetSelection();
    (void)radio.GetCount();
}

#if !WX_EXPECT_WINUI
class NativeRadioBoxContract : public wxRadioBox
{
    void NativeComposition(wxDC& dc, const tagRECT& rect)
    {
        int top, other;
        GetBordersForSizer(&top, &other);
        (void)GetCompositeWindowParts();
        PaintBackground(dc, rect);
        PaintForeground(dc, rect);
    }
};
#endif
