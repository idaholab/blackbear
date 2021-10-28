#!/usr/bin/env python3
# /*************************************************/
# /*           DO NOT MODIFY THIS HEADER           */
# /*                                               */
# /*                 BlackBear                     */
# /*                                               */
# /*    (c) 2020 Battelle Energy Alliance, LLC     */
# /*            ALL RIGHTS RESERVED                */
# /*                                               */
# /*   Prepared by Battelle Energy Alliance, LLC   */
# /*     Under Contract No. DE-AC07-05ID14517      */
# /*     With the U. S. Department of Energy       */
# /*                                               */
# /*     See COPYRIGHT for full restrictions       */
# /*************************************************/
"""Create all plots for asr_validation/wald2017b assessment case."""
from glob import glob
import itertools as it
import re
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Plot Settings
plt.style.use('../../../../../scripts/plot_style.mplstyle')

# Global Constants
AXES = ["x", "y", "z"]
SEQ = range(1, 7)
TIME_DIV = 86400
MEAN_DIV = 0.48

# plotting constants
COLOR_LINES = ["r-", "b-", "g-"]
COLOR_LINES_DASH = ["r--", "b--", "g--"]
SUBSCRIPTS = ["xx", "yy", "zz"]
AXES_LIST = ['dispx', 'dispy', 'dispz']

def extract_name(fp):
    """Extract specimen name and axis from the name of the csv file."""
    clean = re.sub(r"\./figure[0-9]{2}/", "", fp)
    clean = clean.replace('.csv', '')
    split = clean.split('_')
    return (split[0], split[1].replace('p', ''))


def define_dfs(*args):
    """Return dataframe containing stacked data of all csv files."""
    df_settings = {'header': 0, 'names': ['x', 'y']}
    df_array = []
    for fp in args:
        df = pd.read_csv(fp, **df_settings)
        df = df.assign(**{'specimen': extract_name(fp)[0],
                          'axis': extract_name(fp)[1]})
        df_array.append(df)
    return pd.concat(df_array)

def define_dataframes(*args):
    """Return a list of pandas dataframes."""
    return [pd.read_csv(fp) for fp in args]


def sum_cols(df, col1, col2):
    """Compute absolute value of two columns from a dataframe."""
    return abs(df[col1]) + abs(df[col2])


def make_disp_sum_func(df):
    """Return a function initialized with a dataframe."""

    def sum_abs_cols(axis, num):
        """Sum the columns specified by axis and num."""
        return sum_cols(df, f"disp_{axis}_p{num}_pos", f"disp_{axis}_p{num}_neg")

    return sum_abs_cols


def disp_dict(func):
    """Return dictionary of key:values for all axes and numbers."""
    return {f"disp{ax}_p{i}": func(ax, i) for ax in AXES for i in SEQ}


def compute_displacements(df):
    disp_func = make_disp_sum_func(df)
    new_df = df.assign(**disp_dict(disp_func))
    return new_df


def disp_group(idx):
    """Utility function used by groupby to group disp{x,y,z} values."""
    return idx.split("_")[0]


def get_displacement_mean(df):
    """Return dataframe with each row being an axis of displacement."""
    cols_wanted = [f'disp{i}_p{j}' for i in AXES for j in SEQ]
    dat = df[cols_wanted].transpose()
    # print(dat)
    dat_mean = dat.groupby(by=disp_group).apply(lambda x: x.mean())
    return dat_mean


def row_to_list(df, identifier):
    """Return row of dataframe as list where index == identifier."""
    return df.filter(like=identifier, axis=0).squeeze()


def simplify(df, identifier):
    """Return a dataframe with two columns: [time, disp{x|y|z}]."""
    df_mean = get_displacement_mean(df)
    # print(df_mean)
    temp = {'time': df.time,
            identifier: row_to_list(df_mean, identifier)}
    return pd.DataFrame(temp)


def main():
    """Script Driver Function."""
    ###################################################################
    #                 Experimental Data                               #
    ###################################################################
    a1_000 = pd.read_csv('./data/asr_experiment.csv')

    a1_001a_px, a1_001a_py, a1_001a_pz, a1_001b_px, a1_001b_py, a1_001b_pz, a1_002_px, a1_002_py, a1_002_pz, a1_003_px, a1_003_py, a1_003_pz = define_dataframes(
        "./data/A1-uniaxial/a1-001a_px.csv",
        "./data/A1-uniaxial/a1-001a_py.csv",
        "./data/A1-uniaxial/a1-001a_pz.csv",
        "./data/A1-uniaxial/a1-001b_px.csv",
        "./data/A1-uniaxial/a1-001b_py.csv",
        "./data/A1-uniaxial/a1-001b_pz.csv",
        "./data/A1-uniaxial/a1-002_px.csv",
        "./data/A1-uniaxial/a1-002_py.csv",
        "./data/A1-uniaxial/a1-002_pz.csv",
        "./data/A1-uniaxial/a1-003_px.csv",
        "./data/A1-uniaxial/a1-003_py.csv",
        "./data/A1-uniaxial/a1-003_pz.csv",
    )


    a1_101a_px, a1_101a_py, a1_101a_pz, a1_101b_px, a1_101b_py, a1_101b_pz, a1_102_px, a1_102_py, a1_102_pz, a1_103_px, a1_103_py, a1_103_pz, a1_202_px, a1_202_py, a1_202_pz, a1_303_px, a1_303_py, a1_303_pz = define_dataframes(
        "./data/A1-biaxial/a1-101a_px.csv",
        "./data/A1-biaxial/a1-101a_py.csv",
        "./data/A1-biaxial/a1-101a_pz.csv",
        "./data/A1-biaxial/a1-101b_px.csv",
        "./data/A1-biaxial/a1-101b_py.csv",
        "./data/A1-biaxial/a1-101b_pz.csv",
        "./data/A1-biaxial/a1-102a_px.csv",
        "./data/A1-biaxial/a1-102a_py.csv",
        "./data/A1-biaxial/a1-102a_pz.csv",
        "./data/A1-biaxial/a1-103_px.csv",
        "./data/A1-biaxial/a1-103_py.csv",
        "./data/A1-biaxial/a1-103_pz.csv",
        "./data/A1-biaxial/a1-202_px.csv",
        "./data/A1-biaxial/a1-202_py.csv",
        "./data/A1-biaxial/a1-202_pz.csv",
        "./data/A1-biaxial/a1-303_px.csv",
        "./data/A1-biaxial/a1-303_py.csv",
        "./data/A1-biaxial/a1-303_pz.csv",
    )

    a1_111a_px, a1_111a_py, a1_111a_pz, a1_111b_px, a1_111b_py, a1_111b_pz, a1_211_px, a1_211_py, a1_211_pz, a1_222_px, a1_222_py, a1_222_pz, a1_321_px, a1_321_py, a1_321_pz, a1_331_px, a1_331_py, a1_331_pz = define_dataframes(
        "./data/A1-triaxial/a1-111a_px.csv",
        "./data/A1-triaxial/a1-111a_py.csv",
        "./data/A1-triaxial/a1-111a_pz.csv",
        "./data/A1-triaxial/a1-111b_px.csv",
        "./data/A1-triaxial/a1-111b_py.csv",
        "./data/A1-triaxial/a1-111b_pz.csv",
        "./data/A1-triaxial/a1-211_px.csv",
        "./data/A1-triaxial/a1-211_py.csv",
        "./data/A1-triaxial/a1-211_pz.csv",
        "./data/A1-triaxial/a1-222_px.csv",
        "./data/A1-triaxial/a1-222_py.csv",
        "./data/A1-triaxial/a1-222_pz.csv",
        "./data/A1-triaxial/a1-321a_px.csv",
        "./data/A1-triaxial/a1-321a_py.csv",
        "./data/A1-triaxial/a1-321a_pz.csv",
        "./data/A1-triaxial/a1-331_px.csv",
        "./data/A1-triaxial/a1-331_py.csv",
        "./data/A1-triaxial/a1-331_pz.csv",
    )

    a3_102_L1_px, a3_102_L1_py, a3_102_L1_pz, a3_102_px, a3_102_py, a3_102_pz, a3_202_L2_px, a3_202_L2_py, a3_202_L2_pz, a3_202_L3_px, a3_202_L3_py, a3_202_L3_pz = define_dataframes(
        "./data/A3-biaxial/a3-102-L1_px.csv",
        "./data/A3-biaxial/a3-102-L1_py.csv",
        "./data/A3-biaxial/a3-102-L1_pz.csv",
        "./data/A3-biaxial/a3-102c_px.csv",
        "./data/A3-biaxial/a3-102c_py.csv",
        "./data/A3-biaxial/a3-102c_pz.csv",
        "./data/A3-biaxial/a3-202-L2_px.csv",
        "./data/A3-biaxial/a3-202-L2_py.csv",
        "./data/A3-biaxial/a3-202-L2_pz.csv",
        "./data/A3-biaxial/a3-202-L3_px.csv",
        "./data/A3-biaxial/a3-202-L3_py.csv",
        "./data/A3-biaxial/a3-202-L3_pz.csv",
    )

    ###################################################################
    #                Simulation Outputs                               #
    ###################################################################

    case_A1_000b, case_A1_001, case_A1_002, case_A1_003, case_A1_101, case_A1_102, case_A1_103, case_A1_202, case_A1_303, case_A1_111, case_A1_222, case_A1_211, case_A1_331, case_A1_321,case_A3_102, case_A3_102_L1, case_A3_202_L2, case_A3_202_L3 = define_dataframes(
        "../A1-000b/gold/A1-000b_out.csv",
        "../A1-001a/gold/A1-001a_out.csv",
        "../A1-002/gold/A1-002_out.csv",
        "../A1-003/gold/A1-003_out.csv",
        "../A1-101a/gold/A1-101a_out.csv",
        "../A1-102a/gold/A1-102a_out.csv",
        "../A1-103/gold/A1-103_out.csv",
        "../A1-202/gold/A1-202_out.csv",
        "../A1-303/gold/A1-303_out.csv",
        "../A1-111a/gold/A1-111a_out.csv",
        "../A1-222/gold/A1-222_out.csv",
        "../A1-211/gold/A1-211_out.csv",
        "../A1-331/gold/A1-331_out.csv",
        "../A1-321a/gold/A1-321a_out.csv",
        "../A3-102c/gold/A3-102c_out.csv",
        "../A3-102-L1/gold/A3-102-L1_out.csv",
        "../A3-202-L2/gold/A3-202-L2_out.csv",
        "../A3-202-L3/gold/A3-202-L3_out.csv",
    )

    # Add new displacement columns
    case_A1_000b = compute_displacements(case_A1_000b)
    case_A1_001 = compute_displacements(case_A1_001)
    case_A1_002 = compute_displacements(case_A1_002)
    case_A1_003 = compute_displacements(case_A1_003)

    case_A1_101 = compute_displacements(case_A1_101)
    case_A1_102 = compute_displacements(case_A1_102)
    case_A1_103 = compute_displacements(case_A1_103)
    case_A1_202 = compute_displacements(case_A1_202)
    case_A1_303 = compute_displacements(case_A1_303)

    case_A1_111 = compute_displacements(case_A1_111)
    case_A1_222 = compute_displacements(case_A1_222)
    case_A1_211 = compute_displacements(case_A1_211)
    case_A1_331 = compute_displacements(case_A1_331)
    case_A1_321 = compute_displacements(case_A1_321)

    case_A3_102 = compute_displacements(case_A3_102)
    case_A3_102_L1 = compute_displacements(case_A3_102_L1)
    case_A3_202_L2 = compute_displacements(case_A3_202_L2)
    case_A3_202_L3 = compute_displacements(case_A3_202_L3)

    ###################################################################
    #                             Plots                               #
    ###################################################################
    #############
    # Figure 01 #
    #############

    ax = plt.figure(1)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_000b, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'{sub}-direction (Simulation)',
            markersize=5,
            linewidth=3
        )

    plt.plot(a1_000['x'], a1_000['y'], 'k*',label='Experiment', markersize = 5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-000.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 02 #
    #############
    ax = plt.figure(2)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_001, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'{sub}-direction (Simulation)',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_001a_px.x, a1_001a_px.y, 'r*', label='x-direction (Experiment)', markersize=5)
    plt.plot(a1_001a_py.x, a1_001a_py.y, 'b*', label='y-direction (Experiment)', markersize=5)
    plt.plot(a1_001a_pz.x, a1_001a_pz.y, 'g*', label='z-direction (Experiment)', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-001a.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 03 #
    #############
    ax = plt.figure(3)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_002, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_002_px.x, a1_002_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_002_py.x, a1_002_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_002_pz.x, a1_002_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-002.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 04 #
    #############
    ax = plt.figure(4)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_003, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_003_px.x, a1_003_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_003_py.x, a1_003_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_003_pz.x, a1_003_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-003.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 05 #
    #############
    ax = plt.figure(5)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat2 = simplify(case_A1_101, ax)
        plt.plot(
            tmp_dat2.time / TIME_DIV,
            tmp_dat2[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_101a_px.x, a1_101a_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_101a_py.x, a1_101a_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_101a_pz.x, a1_101a_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-101a.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 06 #
    #############
    ax = plt.figure(6)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_101, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_101b_px.x, a1_101b_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_101b_py.x, a1_101b_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_101b_pz.x, a1_101b_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-101b.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 07 #
    #############
    ax = plt.figure(7)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_102, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_102_px.x, a1_102_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_102_py.x, a1_102_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_102_pz.x, a1_102_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-102a.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 08 #
    #############
    ax = plt.figure(8)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_103, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_103_px.x, a1_103_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_103_py.x, a1_103_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_103_pz.x, a1_103_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-103.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 09 #
    #############
    ax = plt.figure(9)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_202, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_202_px.x, a1_202_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_202_py.x, a1_202_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_202_pz.x, a1_202_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-202.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 10 #
    #############
    ax = plt.figure(10)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_303, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_303_px.x, a1_303_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_303_py.x, a1_303_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_303_pz.x, a1_303_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-303.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 11 #
    #############
    ax = plt.figure(11)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_111, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_111a_px.x, a1_111a_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_111a_py.x, a1_111a_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_111a_pz.x, a1_111a_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-111a.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 12 #
    #############
    ax = plt.figure(12)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat2 = simplify(case_A1_111, ax)
        plt.plot(
            tmp_dat2.time / TIME_DIV,
            tmp_dat2[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_111b_px.x, a1_111b_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_111b_py.x, a1_111b_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_111b_pz.x, a1_111b_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-111b.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 13 #
    #############
    ax = plt.figure(13)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_211, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_211_px.x, a1_211_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_211_py.x, a1_211_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_211_pz.x, a1_211_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-211.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 14 #
    #############
    ax = plt.figure(14)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat2 = simplify(case_A1_222, ax)
        plt.plot(
            tmp_dat2.time / TIME_DIV,
            tmp_dat2[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_222_px.x, a1_222_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_222_py.x, a1_222_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_222_pz.x, a1_222_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-222.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 15 #
    #############
    ax = plt.figure(15)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat2 = simplify(case_A1_331, ax)
        plt.plot(
            tmp_dat2.time / TIME_DIV,
            tmp_dat2[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_331_px.x, a1_331_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_331_py.x, a1_331_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_331_pz.x, a1_331_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-331.png", bbox_inches='tight')
    plt.close()
    #############
    # Figure 16 #
    #############
    ax = plt.figure(16)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A1_321, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a1_321_px.x, a1_321_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a1_321_py.x, a1_321_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a1_321_pz.x, a1_321_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A1-321a.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 17 #
    #############
    ax = plt.figure(17)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A3_102, ax)
        plt.plot(
            (tmp_dat.time - tmp_dat.time[0]) / TIME_DIV + 27,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a3_102_px.x, a3_102_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a3_102_py.x, a3_102_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a3_102_pz.x, a3_102_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A3-102.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 18 #
    #############
    ax = plt.figure(18)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A3_102_L1, ax)
        plt.plot(
            (tmp_dat.time - tmp_dat.time[0]) / TIME_DIV + 27,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a3_102_L1_px.x, a3_102_L1_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a3_102_L1_py.x, a3_102_L1_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a3_102_L1_pz.x, a3_102_L1_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A3-102-L1.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 19 #
    #############
    ax = plt.figure(19)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A3_202_L2, ax)
        plt.plot(
            (tmp_dat.time - tmp_dat.time[0]) / TIME_DIV + 27,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a3_202_L2_px.x, a3_202_L2_px.y, 'r*', label='Experiment-x', markersize=5)
    plt.plot(a3_202_L2_py.x, a3_202_L2_py.y, 'b*', label='Experiment-y', markersize=5)
    plt.plot(a3_202_L2_pz.x, a3_202_L2_pz.y, 'g*', label='Experiment-z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A3-202-L2.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 20 #
    #############
    ax = plt.figure(20)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, AXES):
        tmp_dat = simplify(case_A3_202_L3, ax)
        plt.plot(
            (tmp_dat.time - tmp_dat.time[0]) / TIME_DIV + 27,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'Simulation-{sub}',
            markersize=5,
            linewidth=3
        )
    plt.plot(a3_202_L3_px.x, a3_202_L3_px.y, 'r*', label='x', markersize=5)
    plt.plot(a3_202_L3_py.x, a3_202_L3_py.y, 'b*', label='y', markersize=5)
    plt.plot(a3_202_L3_pz.x, a3_202_L3_pz.y, 'g*', label='z', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("A3-202-L3.png", bbox_inches='tight')
    plt.close()

    #############
    # Figure 21 #
    #############
    # temperature and humidity ptofile
    case_A1_002 = pd.read_csv('../A1-002/gold/A1-002_out.csv') # pick any output file

    asr_concrete_temp = pd.read_csv('../temperature_history.csv')
    asr_concrete_rh = pd.read_csv('../humidity_history.csv')

    fig, (ax1, ax2) = plt.subplots(2, figsize=(7, 6))
    #Drop points up to approximately 28 days
    ax1.plot(case_A1_002.iloc[1:]['time']/86400, case_A1_002.iloc[1:]['temp'], 'r-',label='Average response from simulation', markersize = 5, linewidth=3)
    ax1.plot(asr_concrete_temp['time']/86400, asr_concrete_temp['temp'], 'b+',label='Experimental conditions ', markersize = 6, markeredgewidth=2)
    ax1.set_xlim(0,)
    ax1.set_ylim(0,)
    ax1.set_ylabel('Temperature ($^o$C)')

    ax2.plot(asr_concrete_rh['time']/86400, asr_concrete_rh['humidity']*100, 'b+',label='Experimental conditions', markersize = 6, markeredgewidth=2)
    ax2.plot(case_A1_002.iloc[1:]['time']/86400, case_A1_002.iloc[1:]['humidity']*100, 'r-', label='Average response from simulation', markersize = 5, linewidth=3)
    ax2.set_xlim(0,)
    ax2.set_ylim(0,)
    ax2.set_ylabel('Relative Humidity (\%)')

    plt.xlabel('Time (days)')
    fig.tight_layout()

    ax1.legend(bbox_to_anchor=(0.5, 0), loc='lower center', frameon=False )
    fig.savefig("temp_rh_history.png", bbox_inches='tight')
    plt.close()


if __name__ == "__main__":
    main()
