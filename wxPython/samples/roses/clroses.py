#----------------------------------------------------------------------------
# Name:         clroses.py
# Purpose:      Class definitions for Roses interactive display programs.
#
# Author:       Ric Werme
# WWW:          http://WermeNH.com/roses
#
# Created:      June 2007
# CVS-ID:       $Id$
# Copyright:    Public Domain, please give credit where credit is due.
# License:      Sorry, no EULA.
#----------------------------------------------------------------------------

# This is yet another incarnation of an old graphics hack based around
# misdrawing an analytic geometry curve called a rose.  The basic form is
# simply the polar coordinate function r = cos(a * theta).  "a" is the
# "order" of the rose, a zero value degenerates to r = 1, a circle.  While
# this program is happy to draw that, much more interesting things happen when
# one or more of the following is in effect:

# 1) The "delta theta" between points is large enough to distort the curve,
#    e.g. 90 degrees will draw a square, slightly less will be interesting.

# 2) The order of the rose is too large to draw it accurately.

# 3) Vectors are drawn at less than full speed.

# 4) The program is stepping through different patterns on its own.

# While you will be able to predict some aspects of the generated patterns,
# a lot of what there is to be found is found at random!

# The rose class has all the knowledge to implement generating vector data for
# roses and handles all the timing issues.  It does not have the user interface
# for changing all the drawing parameters.  It offers a "vision" of what an
# ideal Roses program should be, however, callers are welcome to assert their
# independence, override defaults, ignore features, etc.

from math import sin, cos, pi

# Rose class knows about:
# > Generating points and vectors (returning data as a list of points)
# > Starting a new rose (e.g. telling user to erase old vectors)
# > Stepping from one pattern to the next.

class rose:
    "Defines everything needed for drawing a rose with timers."

    # The following data is accessible by callers, but there are set
    # methods for most everything and various method calls to client methods
    # to display current values.    
    style = 100		# Angular distance along curve between points
    sincr = -1		# Amount to increment style by in auto mode
    petals = 2		# Lobes on the rose (even values have 2X lobes)
    pincr = 1		# Amount to increment petals by in auto mode
    nvec = 399		# Number of vectors to draw the rose
    minvec = 0		# Minimum number acceptable in automatic mode
    maxvec = 3600	# Maximum number acceptable in automatic mode
    skipvec = 0		# Don't draw this many at the start (cheap animations)
    drawvec = 3600	# Draw only this many (cheap animations)
    step = 20		# Number of vectors to draw each clock tick
    draw_delay = 50	# Time between roselet calls to watch pattern draw
    wait_delay = 2000	# Time between roses in automatic mode

    # Other variables that the application shouldn't access.
    verbose = 0		# No good way to set this at the moment.
    nextpt = 0		# Next position to draw on next clock tick
    
    # Internal states:
    INT_IDLE, INT_DRAW, INT_SEARCH, INT_WAIT, INT_RESIZE = range(5)
    int_state = INT_IDLE

    # Command states
    CMD_STOP, CMD_GO = range(2)
    cmd_state = CMD_STOP
    
    # Return full rose line (a tuple of (x, y) tuples).  Not used by interactive
    # clients but still useful for command line and batch clients.
    # This is the "purest" code and doesn't require the App* methods defined
    # by the caller.
    def rose(self, style, petals, vectors):
        self.nvec = vectors
        self.make_tables(vectors)
        line = [(1.0, 0.0)]
        for i in range (1, vectors):
            theta = (style * i) % vectors
            r = self.cos_table[(petals * theta) % vectors]
            line.append((r * self.cos_table[theta], r * self.sin_table[theta]))
        line.append((1.0, 0.0))
        return line

    # Generate vectors for the next chunk of rose.
    
    # This is not meant to be called from an external module, as it is closely
    # coupled to parameters set up within the class and limits set up by
    # restart().  Restart() initializes all data this needs to start drawing a
    # pattern, and clock() calls this to compute the next batch of points and
    # hear if that is the last batch.  We maintain all data we need to draw each
    # batch after the first.  theta should be 2.0*pi * style*i/self.nvec
    # radians, but we deal in terms of the lookup table so it's just the index
    # that refers to the same spot.
    def roselet(self):
        line = []
        stop = self.nextpt + self.step
        keep_running = True
        if stop >= self.endpt:
            stop = self.endpt
            keep_running = False
        for i in range (self.nextpt, stop + 1):
            theta = (self.style * i) % self.nvec
            r = self.cos_table[(self.petals * theta) % self.nvec]
            line.append((r * self.cos_table[theta], r * self.sin_table[theta]))
        self.nextpt = stop
        return line, keep_running

    # Generate sine and cosine lookup tables.  We could create data for just
    # 1/4 of a circle, at least if vectors was a multiple of 4, and share a
    # table for both sine and cosine, but memory is cheaper than it was in
    # PDP-11 days.  OTOH, small, shared tables would be more cache friendly,
    # but if we were that concerned, this would be in C.
    def make_tables(self, vectors):
        self.sin_table = [sin(2.0 * pi * i / vectors) for i in range(vectors)]
        self.cos_table = [cos(2.0 * pi * i / vectors) for i in range(vectors)]
            
    # Rescale (x,y) data to match our window.  Note the negative scaling in the
    # Y direction, this compensates for Y moving down the screen, but up on
    # graph paper.
    def rescale(self, line, offset, scale):
        for i in range(len(line)):
            line[i] = (line[i][0] *   scale  + offset[0],
                       line[i][1] * (-scale) + offset[1])
        return line

    # Euler's Method for computing the greatest common divisor.  Knuth's
    # "The Art of Computer Programming" vol.2 is the standard reference,
    # but the web has several good ones too.  Basically this sheds factors
    # that aren't in the GCD and returns when there's nothing left to shed.
    # N.B. Call with a >= b.
    def gcd(self, a, b):
        while b != 0:
            a, b = b, a % b
        return a

    # Erase any old vectors and start drawing a new rose.  When the program
    # starts, the sine and cosine tables don't exist, build them here.  (Of
    # course, if an __init__() method is added, move the call there.
    # If we're in automatic mode, check to see if the new pattern has neither
    # too few or too many vectors and skip it if so.  Skip by setting up for
    # a one tick wait to let us get back to the main loop so the user can
    # update parameters or stop.
    def restart(self):
        if self.verbose:
            print 'restart: int_state', self.int_state, 'cmd_state', self.cmd_state
        try:
            tmp = self.sin_table[0]
        except:
            self.make_tables(self.nvec)

        new_state = self.INT_DRAW
        self.takesvec = self.nvec / self.gcd(self.nvec, self.style)
        if not self.takesvec & 1 and self.petals & 1:
            self.takesvec /= 2
        if self.cmd_state == self.CMD_GO:
            if self.minvec > self.takesvec or self.maxvec < self.takesvec:
                new_state = self.INT_SEARCH
        self.AppSetTakesVec(self.takesvec)
        self.AppClear()
        self.nextpt = self.skipvec
        self.endpt = min(self.takesvec, self.skipvec + self.drawvec)
        old_state, self.int_state = self.int_state, new_state
        if old_state == self.INT_IDLE:	# Clock not running
            self.clock()
        elif old_state == self.INT_WAIT:	# May be long delay, restart
            self.AppCancelTimer()
            self.clock()
        else:
            return 1		# If called by clock(), return and start clock
        return 0		# We're in INT_IDLE or INT_WAIT, clock running

    # Called from App.  Recompute the center and scale values for the subsequent pattern.
    # Force us into INT_RESIZE state if not already there so that in 100 ms we'll start
    # to draw something to give an idea of the new size.
    def resize(self, size, delay):
        xsize, ysize = size
        self.center = (xsize / 2, ysize / 2)
        self.scale = min(xsize, ysize) / 2.1
        self.repaint(delay)
        
    # Called from App or above.  From App, called with small delay because
    # some window managers will produce a flood of expose events or call us
    # before initialization is done.
    def repaint(self, delay):
        if self.int_state != self.INT_RESIZE:
            # print 'repaint after', delay
            self.int_state = self.INT_RESIZE
            self.AppCancelTimer()
            self.AppAfter(delay, self.clock)
            
    # Method that returns the next style and petal values for automatic
    # mode and remembers them internally.  Keep things scaled in the
    # range [0:nvec) because there's little reason to exceed that.
    def next(self):
        self.style += self.sincr
        self.petals += self.pincr
        if self.style <= 0 or self.petals < 0:
            self.style, self.petals = \
                        abs(self.petals) + 1, abs(self.style)
        if self.style >= self.nvec:
            self.style %= self.nvec	# Don't bother defending against 0
        if self.petals >= self.nvec:
            self.petals %= self.nvec
        self.AppSetParam(self.style, self.petals, self.nvec)

    # Resume pattern drawing with the next one to display.
    def resume(self):
        self.next()
        return self.restart()

    # Go/Stop button.
    def cmd_go_stop(self):
        if self.cmd_state == self.CMD_STOP:
            self.cmd_state = self.CMD_GO
            self.resume()		# Draw next pattern
        elif self.cmd_state == self.CMD_GO:
            self.cmd_state = self.CMD_STOP
        self.update_labels()

    # Centralize button naming to share with initialization.
    # Leave colors to the application (assuming it cares), we can't guess
    # what's available.
    def update_labels(self):
        if self.cmd_state == self.CMD_STOP:
            self.AppCmdLabels(('Go', 'Redraw', 'Backward', 'Forward'))
        else:		# Must be in state CMD_GO
            self.AppCmdLabels(('Stop', 'Redraw', 'Reverse', 'Skip'))

    # Redraw/Redraw button
    def cmd_redraw(self):
        self.restart()		# Redraw current pattern

    # Backward/Reverse button
    # Useful for when you see an interesting pattern and want
    # to go back to it.  If running, just change direction.  If stopped, back
    # up one step.  The resume code handles the step, then we change the
    # incrementers back to what they were. (Unless resume changed them too.)
    def cmd_backward(self):
        self.sincr = -self.sincr
        self.pincr = -self.pincr
        if self.cmd_state == self.CMD_STOP:
            self.resume();
            self.sincr = -self.sincr	# Go forward again
            self.pincr = -self.pincr
        else:
            self.AppSetIncrs(self.sincr, self.pincr)
        
    # Forward/Skip button.  CMD_STOP & CMD_GO both just call resume.
    def cmd_step(self):
        self.resume()		# Draw next pattern

    # Handler called on each timer event.  This handles the metered drawing
    # of a rose and the delays between them.  It also registers for the next
    # timer event unless we're idle (rose is done and the delay between
    # roses is 0.)
    def clock(self):
        if self.int_state == self.INT_IDLE:
            # print 'clock called in idle state'
            delay = 0
        elif self.int_state == self.INT_DRAW:
            line, run = self.roselet()
            self.AppCreateLine(self.rescale(line, self.center, self.scale))
            if run:
                delay = self.draw_delay
            else:
                if self.cmd_state == self.CMD_GO:
                    self.int_state = self.INT_WAIT
                    delay = self.wait_delay
                else:
                    self.int_state = self.INT_IDLE
                    delay = 0
        elif self.int_state == self.INT_SEARCH:
            delay = self.resume()	# May call us to start drawing
            if self.int_state == self.INT_SEARCH:
                delay = self.draw_delay	# but not if searching.
        elif self.int_state == self.INT_WAIT:
            if self.cmd_state == self.CMD_GO:
                delay = self.resume()	# Calls us to start drawing
            else:
                self.int_state = self.INT_IDLE
                delay = 0
        elif self.int_state == self.INT_RESIZE:	# Waiting for resize event stream to settle
            self.AppSetParam(self.style, self.petals, self.nvec)
            self.AppSetIncrs(self.sincr, self.pincr)
            delay = self.restart()	# Calls us to start drawing
        
        if delay == 0:
            if self.verbose:
                print 'clock: going idle from state', self.int_state
        else:
            self.AppAfter(delay, self.clock)

    # Methods to allow App to change the parameters on the screen.
    # These expect to be called when the associated paramenter changes,
    # but work reasonably well if several are called at once.  (E.g.
    # tkroses.py groups them into things that affect the visual display
    # and warrant a new start, and things that just change and don't affect
    # the ultimate pattern.  All parameters within a group are updated
    # at once even if the value hasn't changed.

    # We restrict the style and petals parameters to the range [0: nvec)
    # since numbers outside of that range aren't interesting. We don't
    # immediately update the value in the application, we probably should.
    
    # NW control window - key parameters
    def SetStyle(self, value):
        self.style = value % self.nvec
        self.restart()

    def SetSincr(self, value):
        self.sincr = value

    def SetPetals(self, value):
        self.petals = value % self.nvec
        self.restart()

    def SetPincr(self, value):
        self.pincr = value


    # SW control window - vectors
    def SetVectors(self, value):
        self.nvec = value
        self.style %= value
        self.petals %= value
        self.AppSetParam(self.style, self.petals, self.nvec)
        self.make_tables(value)
        self.restart()

    def SetMinVec(self, value):
        if self.maxvec >= value and self.nvec >= value:
            self.minvec = value

    def SetMaxVec(self, value):
        if self.minvec < value:
            self.maxvec = value

    def SetSkipFirst(self, value):
        self.skipvec = value
        self.restart()

    def SetDrawOnly(self, value):
        self.drawvec = value
        self.restart()


    # SE control window - timings
    def SetStep(self, value):
        self.step = value

    def SetDrawDelay(self, value):
        self.draw_delay = value

    def SetWaitDelay(self, value):
        self.wait_delay = value

    # Method for client to use to have us supply our defaults.
    def SupplyControlValues(self):
        self.update_labels()
        self.AppSetParam(self.style, self.petals, self.nvec)
        self.AppSetIncrs(self.sincr, self.pincr)
        self.AppSetVectors(self.nvec, self.minvec, self.maxvec,
                           self.skipvec, self.drawvec)
        self.AppSetTiming(self.step, self.draw_delay, self.wait_delay)
