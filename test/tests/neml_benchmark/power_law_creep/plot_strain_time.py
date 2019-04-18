#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np

"""
This script generates plots
"""

plt.rcParams.update({'font.size':10})
data1 = np.genfromtxt('nafems_test5a_lim_out.csv', delimiter=',', names=True)
data2 = np.genfromtxt('nafems_test5a_lim_neml_out.csv', delimiter=',', names=True)

fig = plt.figure()
ax = fig.add_subplot(111)
ax.grid(color='dimgray', linestyle='--', linewidth=1, alpha=0.5)
plt.ticklabel_format(style='plain', axis='y', scilimits=(0,0))
ax.plot(data1['time'], data1['eps_crp_xx']*100.0, linestyle='-', marker='s', markersize=3, color='blue', linewidth=1, label='$\epsilon_{xx}$ (BlackBear)')
ax.plot(data1['time'], data1['eps_crp_yy']*100.0, linestyle='-', marker='s', markersize=3, color='red', linewidth=1, label='$\epsilon_{yy}$ (BlackBear)')
ax.plot(data2['time'], data2['tot_strain_xx']*100.0, linestyle='-', fillstyle='none', marker='o', color='blue', linewidth=1, label='$\epsilon_{xx}$ (NEML)')
ax.plot(data2['time'], data2['tot_strain_yy']*100.0, linestyle='-', marker='o', fillstyle='none', color='red', linewidth=1, label='$\epsilon_{yy}$ (NEML)')
ax.set_xlabel('Time (hr)')
ax.set_ylabel('Strain ($\%$)')
ax.legend(loc='best')
fig.suptitle('Strain vs Time', x=0.5)
plt.savefig('plot_strain_time.pdf', format='pdf', bbox_inches='tight', pad_inches=0.5)
plt.show(fig)
