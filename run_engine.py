#!/usr/bin/env python

import glob
import os

for f in sorted(glob.glob('*.ini')):
    print 'Running ' + f + '...'
    os.system('./engine ' + f)
