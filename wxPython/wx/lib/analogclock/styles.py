# AnalogClock constants
#   E. A. Tacao <e.a.tacao |at| estadao.com.br>
#   http://j.domaindlx.com/elements28/wxpython/
#   15 Fev 2006, 22:00 GMT-03:00
# Distributed under the wxWidgets license.

# Style options that control the general clock appearance,
# chosen via SetClockStyle.
SHOW_QUARTERS_TICKS = 1
SHOW_HOURS_TICKS    = 2
SHOW_MINUTES_TICKS  = 4
ROTATE_TICKS        = 8
SHOW_HOURS_HAND     = 16
SHOW_MINUTES_HAND   = 32
SHOW_SECONDS_HAND   = 64
SHOW_SHADOWS        = 128
OVERLAP_TICKS       = 256

DEFAULT_CLOCK_STYLE = SHOW_HOURS_TICKS|SHOW_MINUTES_TICKS| \
                      SHOW_HOURS_HAND|SHOW_MINUTES_HAND|SHOW_SECONDS_HAND| \
                      SHOW_SHADOWS|ROTATE_TICKS


# Style options that control the appearance of tick marks,
# chosen via SetTickStyle.
TICKS_NONE          = 1
TICKS_SQUARE        = 2
TICKS_CIRCLE        = 4
TICKS_POLY          = 8
TICKS_DECIMAL       = 16
TICKS_ROMAN         = 32
TICKS_BINARY        = 64
TICKS_HEX           = 128


# Constants that may be used as 'target' keyword value in
# the various Get/Set methods.
HOUR                = 1
MINUTE              = 2
SECOND              = 4

ALL                 = HOUR|MINUTE|SECOND


#
##
### eof
