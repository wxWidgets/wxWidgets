#include "wx/spinbutt.h"
#include "contract.h"

static_assert(std::is_base_of<wxSpinButtonBase, wxSpinButton>::value,
              "Both spin buttons implement the portable base");
template class NativeHooksProbe<wxSpinButton>;

void SpinButtonPortableContract(wxSpinButton& button)
{
    button.SetRange(0, 20);
    button.SetValue(2);
    button.SetIncrement(1);
    (void)button.GetValue();
    (void)button.GetIncrement();
    (void)button.AcceptsFocus();
}

#if !WX_EXPECT_WINUI
class NativeSpinButtonContract : public wxSpinButton
{
    void NativeRange() { NormalizeValue(); }
};
#endif
