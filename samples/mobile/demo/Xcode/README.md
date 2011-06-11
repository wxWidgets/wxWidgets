mobiledemo Xcode project
========================

How to run
----------

1. Go to project -> 'mobiledemo', click on 'Build Settings', enter 'flags' into a search field
2. Set value for 'Linking' -> 'Other Linker Flags' to the output of `wx-config --libs` (however, omit the `-arch ***` flag)
3. Set value for 'GCC 4.2' -> 'Other C Flags' to the output of `wx-config --cflags`
	3.1. At the time of writing, the flags should also include `-I$(SRCROOT)/../../../../include`
4. Cross your fingers and build+run.

Requirements
------------

* OS X 10.6 (might work with others)
* Xcode 4 (might be configured to work with others)

Notes
-----

* Web API omitted:
	* src/mobile/generic/mo_webctrl_h.cpp
	* src/mobile/web/iehtmlwin.cpp
	* src/mobile/web/wxactivex.cpp
	* include/wx/mobile/webctrl.h
	* include/wx/mobile/generic/webctrl.h
	* include/wx/mobile/web/iehtmlwin.h
	* include/wx/mobile/web/wxactivex.h
