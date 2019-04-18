#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np

"""
This script generates plots
"""

plt.rcParams.update({'font.size':10})
data1 = np.genfromtxt('elas_plas_LSH_out.csv', delimiter=',', names=True)
data2 = np.genfromtxt('elas_plas_LSH_neml_out.csv', delimiter=',', names=True)

fig = plt.figure()
ax = fig.add_subplot(111)
ax.grid(color='dimgray', linestyle='--', linewidth=1, alpha=0.5)
plt.ticklabel_format(style='plain', axis='y', scilimits=(0,0))
ax.plot(data1['tot_strain_yy']*100.0, data1['vonmises'], linestyle='-', marker='x', color='red', linewidth=1, label='BlackBear')
ax.plot(data2['tot_strain_yy']*100.0, data2['vonmises'], linestyle='-', marker='o', fillstyle='none', color='blue', linewidth=1, label='NEML')
ax.set_xlabel('Strain ($\%$)')
ax.set_ylabel('Stress (MPa)')
ax.legend(loc='lower right')
fig.suptitle('Stress vs Strain', x=0.5)
plt.savefig('plot_stress_strain.pdf', format='pdf', bbox_inches='tight', pad_inches=0.5)
plt.show(fig)
