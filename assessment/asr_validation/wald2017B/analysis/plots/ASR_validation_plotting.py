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
import pandas as pd
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
SUBSCRIPTS = ["xx", "yy", "zz"]
AXES_LIST = ['dispx', 'dispy', 'dispz']


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
    dat_mean = dat.groupby(by=disp_group).apply(lambda x: x.mean())
    return dat_mean


def row_to_list(df, identifier):
    """Return row of dataframe as list where index == identifier."""
    return df.filter(like=identifier, axis=0).squeeze()


def simplify(df, identifier):
    """Return a dataframe with two columns: [time, disp{x|y|z}]."""
    df_mean = get_displacement_mean(df)
    temp = {'time': df.time,
            identifier: row_to_list(df_mean, identifier)}
    return pd.DataFrame(temp)


def main():
    """Script Driver Function."""
    # Experiment Data
    experiment, temp, rh, rebar2 = define_dataframes(
        "./data/asr_experiment.csv",
        "./data/ASR_validation_temperature1.csv",
        "./data/ASR_humidity.csv",
        "./data/asr_experiment_rebarz.csv"
    )

    # Simulation Data
    updated_out, refined_out, time01_out, time02_out, damage_out = define_dataframes(
        "./data/asr_concrete_block_calibration_creep_updated_out.csv",
        "./data/asr_concrete_block_calibration_creep_updated_refine_out.csv",
        "./data/asr_concrete_block_calibration_creep_updated_refine_time1_out.csv",
        "./data/asr_concrete_block_calibration_creep_updated_refine_time2_out.csv",
        "./data/asr_concrete_block_validation_rebar_creep_damage_updated_refine_out.csv",
    )

    # Add new displacement columns
    updated_out = compute_displacements(updated_out)
    refined_out = compute_displacements(refined_out)
    time01_out = compute_displacements(time01_out)
    time02_out = compute_displacements(time02_out)
    damage_out = compute_displacements(damage_out)

    ###################################################################
    #                             Plots                               #
    ###################################################################

    #############
    # Figure 01 #
    #############
    ax = plt.figure(1)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, SUBSCRIPTS):
        temp_dat = simplify(refined_out, ax)
        plt.plot(
            temp_dat.time / TIME_DIV,
            temp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f"$\epsilon_{{ {sub} }}$",
            markersize=5,
            linewidth=3,
        )

    plt.plot(experiment.x, experiment.y, "k*", label="Experiment", markersize=5)

    plt.xlabel("Time (days)")
    plt.ylabel("Axial Expansion (%)")
    plt.legend(frameon=False)
    plt.savefig("./figures/conc_calibration3.png", bbox_inches='tight')

    #############
    # Figure 02 #
    #############
    ax = plt.figure(2)
    for idx, df in enumerate([refined_out, time01_out, time02_out]):
        tmp_dat = simplify(df, 'dispx')
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat.dispx / MEAN_DIV * 100,
            COLOR_LINES[idx],
            label=f'Set-{idx}',
            markersize=5,
            linewidth=3
        )
    plt.ylim(0,)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("./figures/plain_conc_sets3.png", bbox_inches='tight')

    #############
    # Figure 03 #
    #############
    ax = plt.figure(3)
    for ax, cl, sub in zip(AXES_LIST, COLOR_LINES, SUBSCRIPTS):
        tmp_dat = simplify(damage_out, ax)
        plt.plot(
            tmp_dat.time / TIME_DIV,
            tmp_dat[ax] / MEAN_DIV * 100,
            cl,
            label=f'$\epsilon_{{ {sub} }}$',
            markersize=5,
            linewidth=3
        )
    plt.plot(rebar2.x, rebar2.y, 'k*', label='Experiment', markersize=5)
    plt.xlabel('Time (days)')
    plt.ylabel('Axial Expansion (%)')
    plt.legend(frameon=False)
    plt.savefig("./figures/uniaxial_rebar3.png", bbox_inches='tight')

    #############
    # Figure 04 #
    #############
    fig, (ax1, ax2) = plt.subplots(2, figsize=(10, 15))
    refined_out = refined_out.iloc[1:]  # Drop points up to approximately 28 days
    ax1.plot(
        refined_out.time / TIME_DIV,
        refined_out.temp,
        'r-',
        label='Average response from simulation',
        markersize=5,
        linewidth=3
    )

    ax1.plot(
        temp.time / TIME_DIV,
        temp.temp,
        'b+',
        label='Experimental conditions ',
        markersize=6,
        markeredgewidth=2
    )

    ax2.plot(
        rh.time_sec / TIME_DIV,
        rh.rh * 100,
        'b+',
        label='Experimental conditions',
        markersize=6,
        markeredgewidth=2
    )

    ax2.plot(
        refined_out.time / TIME_DIV,
        refined_out.humidity * 100,
        'r-',
        label='Average response from simulation',
        markersize=5,
        linewidth=3
    )

    ax1.set_xlim(0,)
    ax1.set_ylim(0,)
    ax1.set_ylabel('Temperature ($^o$C)')
    ax2.set_xlim(0,)
    ax2.set_ylim(0,)
    ax2.set_ylabel('Relative Humidity (%)')
    plt.xlabel('Time (days)')
    fig.tight_layout()
    ax1.legend(bbox_to_anchor=(0.5, 0), loc='lower center', frameon=False)
    fig.savefig("./figures/temp_rh_history3.png", bbox_inches='tight')


if __name__ == "__main__":
    main()
