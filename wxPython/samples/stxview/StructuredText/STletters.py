import string

try:
    del string
    import locale
    locale.setlocale(locale.LC_ALL,"")
except:
    pass    

import string

letters     = string.letters
punctuations = string.punctuation 

lettpunc    = letters + punctuations
