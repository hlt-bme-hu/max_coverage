cat "$1" | tr "\r" "\t" | sed "s/^\(\t[^\t]\+\)*\t\([^\t]\+\)/\2\n/" | \
grep @ | cut -f3 -d" "

echo -n "Maximum cover(s): "

python -c "
from __future__ import print_function
import re
best = 0
best_str = ''
bests = set()
is_best = False
for line in open('$1', 'r'):
    line = line.strip()
    if re.match(r'^\d+/\d+$', line):
        b = int(line.split('/')[0])
        if b == best:
            is_best = True
        elif b > best:
            best = b
            best_str = line
            bests = set()
            is_best = True
        else:
            is_best = False
    elif re.match(r'^\d+(\t\d+)*$', line) and is_best:
        bests.add(line)
print(best_str)
for solution in sorted(bests):
    print(solution)
"
