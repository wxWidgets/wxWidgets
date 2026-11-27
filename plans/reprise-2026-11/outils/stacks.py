"""Aggregate cdb 'kc' stack samples. Usage: python -I stacks.py samples.txt [top]

Prints, for each function, in how many samples it appears anywhere on the
stack (inclusive) and how many times it is the innermost frame (self).
"""
import re
import sys
from collections import Counter

path = sys.argv[1]
top = int(sys.argv[2]) if len(sys.argv) > 2 else 45

samples = []
cur = None
for line in open(path, encoding="utf-8", errors="replace"):
    line = line.rstrip("\n")
    if line.startswith("=== sample"):
        cur = []
        samples.append(cur)
        continue
    if cur is None:
        continue
    # kc lines look like:  "00 module!Function" or "module!Function"
    m = re.match(r"^\s*(?:[0-9a-f]{2}\s+)?([\w\.]+![^\s].*)$", line)
    if m and "!" in m.group(1) and not line.lstrip().startswith(("Call Site", "#")):
        cur.append(m.group(1).strip())

samples = [s for s in samples if s]
print("usable samples:", len(samples))
incl, selfc = Counter(), Counter()
for s in samples:
    selfc[s[0]] += 1
    for f in set(s):
        incl[f] += 1

def short(f):
    f = re.sub(r"wxwinui\w+_vc_x64_custom", "wxcore", f)
    f = re.sub(r"wxbase\w+_vc_x64_custom", "wxbase", f)
    return f[:130]

print("\n-- innermost frame (self)")
for f, n in selfc.most_common(25):
    print("%3d  %s" % (n, short(f)))
print("\n-- on the stack (inclusive), wx frames only")
shown = 0
for f, n in incl.most_common():
    if not re.search(r"^wx(winui|base)|showcase|widgets", f):
        continue
    print("%3d  %s" % (n, short(f)))
    shown += 1
    if shown >= top:
        break
