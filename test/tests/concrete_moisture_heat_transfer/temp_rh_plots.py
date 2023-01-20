#!/usr/bin/env python

import pandas
import mooseutils
import matplotlib.pyplot as plt

f1 = plt.figure(1, figsize=(6,4.5))
ax = plt.gca()
data = mooseutils.VectorPostprocessorReader('maqbeth_1d_csv/out_profiles_*.csv')

t_times = [19.5, 30.5, 41.8, 61.3, 125.0, 194.6]
rh_times = [33.4, 58.4, 66.7, 77.9, 152.8, 194.5]

#T = data('T', time = 0)
#print(T)
#print(data.data.keys().values)

#Plot a line for every output time:
#for time in data.data.data.keys().values:
#  data.data.data[time].plot(ax=ax,x='x',y='T')

#Plot lines for selected interpolated output times:
for t_hr in t_times:
  t = t_hr * 3600
  data.update(time=t)
  data.data.data.plot(ax=ax,x='id',y='T',label=str(t_hr)+' hr')

plt.xlabel('Radial Position (m)')
plt.ylabel('Temperature ($\degree$C)')

plt.savefig('temp_plot.pdf')

f2 = plt.figure(2, figsize=(6,4.5))
ax = plt.gca()

for t_hr in rh_times:
  t = t_hr * 3600
  data.update(time=t)
  data.data.data.plot(ax=ax,x='id',y='rh',label=str(t_hr)+' hr')

plt.xlabel('Radial Position (m)')
plt.ylabel('$h_r$')

plt.savefig('rh_plot.pdf')
