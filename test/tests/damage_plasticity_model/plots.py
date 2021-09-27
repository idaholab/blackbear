import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import matplotlib.image as mpimg
from matplotlib.ticker import (MultipleLocator, FormatStrFormatter, AutoMinorLocator)
from matplotlib.offsetbox import TextArea, DrawingArea, OffsetImage, AnnotationBbox, AnchoredOffsetbox
from numpy import *
import numpy as np
import pandas as pd
import math

uni_ten = pd.read_csv('./test/tests/damage_plasticity_model/uniaxial_tension_out.csv')
uni_cmp = pd.read_csv('./test/tests/damage_plasticity_model/uniaxial_compression_out.csv')
uni_cmp_dila30 = pd.read_csv('./test/tests/damage_plasticity_model/dilatancy_out.csv')
uni_shr = pd.read_csv('./test/tests/damage_plasticity_model/shear_test_out.csv')
uni_msh_sens = pd.read_csv('./test/tests/damage_plasticity_model/uniaxial_tension_2elem_out.csv')

fig, sub_fig = plt.subplots(2,3, sharex=False, sharey=False, figsize = (12,8))
plt.subplots_adjust(left=0.08, bottom=0.1, right=0.95, top=0.95, wspace = 0.25, hspace = 0.35)
[i, j] = [0, 0]
sub_fig[i][j].set_title('Uniaxial Tensile Test', fontsize=11)
sub_fig[i][j].plot(uni_ten['e_xx']*1000, uni_ten['s_xx'], color = 'black', linestyle ='solid',   linewidth=1.0)
sub_fig[i][j].set_xlabel("Tensile Strain [mm/m]")
sub_fig[i][j].set_ylabel("Tensile Stress [MPa]")

[i, j] = [1, 0]
sub_fig[i][j].set_title('Uniaxial Tensile \n Mesh Sensitivity Test', fontsize=11)
sub_fig[i][j].plot(uni_ten['displacement_x']*1000, -1*uni_ten['react_x'], color = 'black', linestyle ='solid',   linewidth=1.0)
sub_fig[i][j].plot(uni_msh_sens['displacement_x']*1000, -1*uni_msh_sens['react_x'], color = 'red' , linestyle ='solid',   linewidth=1.0)
sub_fig[i][j].set_xlabel("Displacement [mm]")
sub_fig[i][j].set_ylabel("Force [N]")

[i, j] = [0, 1]
sub_fig[i][j].set_title('Uniaxial Compression Test', fontsize=11)
sub_fig[i][j].plot(-1*uni_cmp['e_xx']*1000, -1*uni_cmp['s_xx'], color = 'black', linestyle ='solid',   linewidth=1.0)
sub_fig[i][j].set_xlabel("Comrpessive Strain [mm/m]")
sub_fig[i][j].set_ylabel("Comrpessive Stress [MPa]")

[i, j] = [1, 1]
sub_fig[i][j].set_title('Uniaxial Compression \n Dilatancy Test', fontsize=11)
sub_fig[i][j].plot(uni_cmp['volumetric_strain']*1000, -1*uni_cmp['s_xx'], color = 'black', linestyle ='solid',   linewidth=1.0)
sub_fig[i][j].plot(uni_cmp_dila30['volumetric_strain']*1000, -1*uni_cmp_dila30['s_xx'], color = 'red' , linestyle ='solid',   linewidth=1.0)
sub_fig[i][j].set_xlabel("Volumetric Strain [mm/m]")
sub_fig[i][j].set_ylabel("Comrpessive Stress [MPa]")

[i, j] = [0, 2]
sub_fig[i][j].set_title('Shear Test', fontsize=11)
sub_fig[i][j].plot((uni_shr['e_xy']+uni_shr['ep_xy'])*1000, uni_shr['s_xy'], color = 'black', linestyle ='solid',   linewidth=1.0)
sub_fig[i][j].set_xlabel("Shear Strain [mm/m]")
sub_fig[i][j].set_ylabel("Shear Stress [MPa]")

for i in range(2):
    for j in range(3):
            sub_fig[i][j].grid(color="grey", ls = '--', lw = 0.5)
            sub_fig[i][j].legend(loc='upper right', facecolor = 'white', framealpha = 0.4, edgecolor ='none')

plt.show()
