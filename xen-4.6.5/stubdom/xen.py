#!/usr/bin/python
import time
import os
import sys
import subprocess
import re

# open console device
cdev = open(sys.argv[2])

# unpause guest
ts_start = time.time()
ts_end = ts_start;
crashed = 0;
subprocess.Popen(["xl", "unpause", sys.argv[1]]) # start in background

f = open(sys.argv[3], "a")

# read guests output
while True:
    line = cdev.readline()
    # print "%s" % line
    if line == 'MiniML is up and running\r\n':
        ts_end = time.time()
        break
#    if line[:11] == 'Page fault ' or line[:19] == 'Segmentation fault ' or line[:25] == 'General protection fault ':
#        crashed = 1
#        break
    continue

ts_diff = ts_end - ts_start
print "Booted domain with ID %s in %s seconds" % (sys.argv[1], str(ts_diff))
# with open(sys.argv[3], "a") as f:
f.write("Booted domain with ID %s in %s seconds" % (sys.argv[1], str(ts_diff)))

while True:
    line = cdev.readline()
    print "%s" % line
    if re.match(r'Prediction time (\d+) microseconds\n', line):
        f.write(line)
        break
    
if crashed == 1:
    print "Domain CRASHED"
    # clean-up
    cdev.close()
    exit(1)

# output rest of boot process (trace boot has to be enabled!!!)
# while True:
#     line = cdev.readline()
#     if not line or line == '***\r\n':
#         break
#     sys.stdout.write(line)
#     continue

# clean-up
cdev.close()