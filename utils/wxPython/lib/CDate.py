# Name:         CDate.py
# Purpose:      Date and Calendar classes
#
# Author:       Lorne White (email: lwhite1@planet.eon.net)
#
# Created:
# Version       0.2 1999/11/08
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import time

Month = {2: 'February', 3: 'March', None: 0, 'July': 7, 11:
	 'November', 'December': 12, 'June': 6, 'January': 1, 'September': 9,
	 'August': 8, 'March': 3, 'November': 11, 'April': 4, 12: 'December',
	 'May': 5, 10: 'October', 9: 'September', 8: 'August', 7: 'July', 6:
	 'June', 5: 'May', 4: 'April', 'October': 10, 'February': 2, 1:
	 'January', 0: None}

# Number of days per month (except for February in leap years)
mdays = [0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

# Full and abbreviated names of weekdays
day_name = [ 'Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday']
day_abbr = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', ]

# Return number of leap years in range [y1, y2)
# Assume y1 <= y2 and no funny (non-leap century) years

def leapdays(y1, y2):
    return (y2+3)/4 - (y1+3)/4
	
# Return 1 for leap years, 0 for non-leap years
def isleap(year):
    return year % 4 == 0 and (year % 100 <> 0 or year % 400 == 0)

def FillDate(val):
    s = str(val)
    if len(s) < 2:
        s = '0' + s
    return s

	
def julianDay(year, month, day):
    b = 0L
    year, month, day = long(year), long(month), long(day)
    if month > 12L:
        year = year + month/12L
        month = month%12
    elif month < 1L:
        month = -month
        year = year - month/12L - 1L
        month = 12L - month%12L
    if year > 0L:
        yearCorr = 0L
    else:
        yearCorr = 3L
    if month < 3L:
        year = year - 1L
        month = month + 12L
    if year*10000L + month*100L + day > 15821014L:
        b = 2L - year/100L + year/400L
    return (1461L*year - yearCorr)/4L + 306001L*(month + 1L)/10000L + day + 1720994L + b

	
def TodayDay():
    date = time.localtime(time.time())
    year = date[0]
    month = date[1]
    day = date[2]
    julian = julianDay(year, month, day)
    daywk = dayOfWeek(julian)
    daywk = day_name[daywk]
    return(daywk)

def FormatDay(value):
    date = FromFormat(value)
    daywk = DateCalc.dayOfWeek(date)
    daywk = day_name[daywk]
    return(daywk)

def FromJulian(julian):
    julian = long(julian)
    if (julian < 2299160L):
        b = julian + 1525L
    else:
        alpha = (4L*julian - 7468861L)/146097L
        b = julian + 1526L + alpha - alpha/4L
    c = (20L*b - 2442L)/7305L
    d = 1461L*c/4L
    e = 10000L*(b - d)/306001L
    day = int(b - d - 306001L*e/10000L)
    if e < 14L:
        month = int(e - 1L)
    else:
        month = int(e - 13L)
    if month > 2:
        year = c - 4716L
    else:
        year = c - 4715L
    year = int(year)
    return year, month, day

def dayOfWeek(julian):
    return int((julian + 1L)%7L)

def daysPerMonth(month, year):
    ndays = mdays[month] + (month == 2 and isleap(year))
    return ndays

class now:
    def __init__(self):        
        self.date = time.localtime(time.time())
        self.year = self.date[0]
        self.month = self.date[1]
        self.day = self.date[2]
		
class Date:
    def __init__(self, year, month, day):        
        self.julian = julianDay(year, month, day)
        self.month = month
        self.year = year
        self.day_of_week = dayOfWeek(self.julian)
        self.days_in_month = daysPerMonth(self.month, self.year)

