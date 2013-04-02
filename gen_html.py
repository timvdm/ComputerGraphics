#!/usr/bin/env python

import glob

print '<table>'
print '  <tr>'
print '    <th>Reference</th><th>Experiment</th><th>Config file</th>'
print '  </tr>'


for f in sorted(glob.glob('*.ini')):
    base = f[:-4]
    print '  <tr>'
    print '    <td><img width="50%" src="' + base + '.png"></td><td><img width="50%" src="' + base + '.bmp"></td><td><a href="' + base + '.ini">' + base + '.ini</a></td>'
    print '  </tr>'




print '</table>'
