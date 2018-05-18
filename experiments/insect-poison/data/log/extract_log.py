#!/usr/bin/bash
import matplotlib.pyplot as plt
import numpy as np

import sys

line_no = 0
BEGIN=6
af_stat = {}
duration=[]
'''
#with open('pydump-after-poison.data') as f:
with open('insecticide-only.data') as f:
  for line in f:
    if( BEGIN >= line_no):
      line_no = line_no + 1
      continue

    line = line.split(',')
    if line_no == 7:
      #line_no = line_no + 1
       #print line
       if line[0].strip() in af_stat.keys():
         af_stat[int(line[0].strip())].append([line[2].strip(), line[3].strip(), line[5].strip()])
       else:
         af_stat[int(line[0].strip())] = [[line[2].strip(), line[3].strip(), line[5].strip()]]
       
       #duration.append(float(line[5].strip()))
       #print line[5].strip()
       try:
         x = float(line[5].strip())
         duration.append(x)
         print x
       except ValueError:
         print "Tried to to convert %s to float line id %s " % (line[5].strip(), line[0].strip())
         sys.exit()

#x = 0
for i in af_stat:
#   if x < 50:
#   if(len(af_stat[i]) > 1):
   print"%s : %s" % (i, af_stat[i])
#     x = x + 1
'''

time = []
non_nlp = []
insect = []
poison = []
insecticide = []
line_no = 0
with open('pydump-percentage-af.data') as f:
  for line in f:
    if line_no == 0:
      line_no = line_no + 1
      continue
    line = [x.strip() for x in line.split(',') ]
    time.append(float(line[0]))
    non_nlp.append(float(line[1]))
    insect.append(float(line[2]))
    poison.append(float(line[3]))
    insecticide.append(float(line[4]))


plt.plot(time, insect)
plt.plot(time, poison)
plt.plot(time, insecticide)
plt.plot(time, non_nlp)
plt.legend(['Insect', 'Poison', 'Insecticide', 'Non-nlp'], loc='best') 
#plt.legend(['Insect', 'Poison', 'Insecticide'], loc='upper left') 
plt.ylabel('Percentage in AF')
plt.show('Time in AF(sec)')



