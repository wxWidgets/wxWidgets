# wxWinUI sample

This sample is the first wxWinUI vertical slice. It requires a CMake build with
`wxBUILD_TOOLKIT=winui`, MSVC, and the Windows App SDK runtime installed on the
machine running the executable.

Implemented in this milestone:

- `wxBUILD_TOOLKIT=winui` with distinct `wxwinuiu...` library names.
- Public `__WXWINUI__` and `wxUSE_WINUI3` build switch.
- Windows App SDK bootstrapper initialization for unpackaged applications.
- `wxWinUIXamlHost`, a `wxWindow` that hosts a WinUI 3 XAML island.
- WinUI-backed `wxStaticText`, `wxButton`, `wxTextCtrl`, `wxCheckBox`,
  `wxChoice`, `wxRadioButton`, `wxSlider`, and `wxStaticLine`.

Still out of scope:

- Building the full `widgets` sample as native WinUI controls.
- MinGW and non-MSVC builds.
