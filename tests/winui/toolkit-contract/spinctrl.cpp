#include "wx/spinctrl.h"
#include "wx/spinbutt.h"
#include "contract.h"

static_assert(std::is_base_of<wxSpinButton, wxSpinCtrl>::value == !WX_EXPECT_WINUI,
              "Only the MSW integer control is a native spin button with buddy");
static_assert(std::is_base_of<wxSpinCtrlBase, wxSpinCtrl>::value == WX_EXPECT_WINUI,
              "WinUI integer control uses the portable spin control base");
static_assert(std::is_base_of<wxSpinCtrlBase, wxSpinCtrlDouble>::value,
              "Both floating-point controls implement the portable base");

template<typename T, typename = void>
struct HasBuddyWindowProc : std::false_type {};
template<typename T>
struct HasBuddyWindowProc<T,
    std::void_t<decltype(std::declval<T&>().GetBuddyWndProc())>> : std::true_type {};
static_assert(HasBuddyWindowProc<wxSpinCtrl>::value == !WX_EXPECT_WINUI,
              "Native buddy access is not available on a XAML NumberBox");
template class NativeHooksProbe<wxSpinCtrl>;
template class NativeHooksProbe<wxSpinCtrlDouble>;

void SpinCtrlPortableContract(wxSpinCtrl& integer, wxSpinCtrlDouble& real)
{
    integer.SetValue(2);
    integer.SetValue("2");
    integer.SetRange(0, 20);
    integer.SetSelection(0, -1);
    (void)integer.SetBase(10);
    (void)integer.GetTextValue();
    real.SetRange(0, 20);
    real.SetIncrement(0.5);
    real.SetDigits(2);
    real.SetValue(2.5);
}

#if WX_EXPECT_WINUI
    #ifndef wxHAS_NATIVE_SPINCTRLDOUBLE
        #error WinUI must select its NumberBox floating-point implementation
    #endif
void SpinCtrlSnapContract(wxSpinCtrl& integer)
{
    integer.SetSnapToTicks(true);
    (void)integer.GetSnapToTicks();
}
#else
    #ifdef wxHAS_NATIVE_SPINCTRLDOUBLE
        #error Ordinary MSW currently uses the generic floating-point control
    #endif
class NativeSpinCtrlContract : public wxSpinCtrl
{
    void NativeBuddy()
    {
        NormalizeValue();
        (void)GetBuddyWndProc();
        (void)GetSpinForTextCtrl(m_hwndBuddy);
        (void)ProcessTextCommand(0, 0);
    }
};
#endif
