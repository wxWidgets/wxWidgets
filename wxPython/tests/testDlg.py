## import all of the wxPython GUI package
from wxPython.wx import *


## Create a new frame class, derived from the wxPython Frame.
class Dialog(wxDialog):

	def __init__(self, parent, title):
		# First, call the base class' __init__ method to create the frame
		wxDialog.__init__( self, parent, -1, title, wxDefaultPosition, wxDefaultSize )

                wxButton(self, wxID_OK, "OK", (10, 10))
                wxButton(self, wxID_CANCEL, "Cancel", (50,50))
		self.Centre( wxBOTH )


	# This method is called automatically when the CLOSE event is
	# sent to this window
	#def OnCloseWindow(self, event):
	#	self.Destroy()

	#def OnCloseMe(self, event):
		#self.Close(true)


def main():
	# Every wxWindows application must have a class derived from wxApp
	class App(wxApp):

		# wxWindows calls this method to initialize the application
		def OnInit(self):

			# Create an instance of our customized Frame class
			dialog = Dialog( NULL, 'test' )
			dialog.ShowModal()
                        print "got here"
                        dialog.Destroy()

			# Tell wxWindows that this is our main window
			# Return a success flag
			return true

	app = App(0)	 # Create an instance of the application class
	app.MainLoop()	 # Tell it to start processing events



if __name__ == '__main__':
        main()
