import matplotlib.pyplot as plt
import seaborn as sns
from cycler import cycler

# apply this so all the numbers are in the same font


def set_tick_font():
    """
    Sets the font of the axes to use stix for the tick labels so that anything that is a number is stix
    """
    ax = plt.gca()
    plt.setp(ax.get_xticklabels(), fontfamily="STIXGeneral")
    plt.setp(ax.get_yticklabels(), fontfamily="STIXGeneral")


def set_rcParams(
    presentation=False, save_format="svg", legend_loc="upper left", set_figsize=True,
    set_cycler=False
):
    # line color scheme
    plt.style.use("seaborn-v0_8-colorblind")
    colorblind_palette = sns.color_palette("colorblind")
    if set_cycler:
      cyc_lines = cycler(linestyle=["-", "--", "-.", ":"])
      cyc_color = cycler(color=colorblind_palette[:4])
      cyc_alpha = cycler(alpha=[1] * 4)
      cyc_marker = cycler(marker=["o", "v", "s", "D"])
      plt.rcParams["axes.prop_cycle"] = cycler(cyc_lines + cyc_color + cyc_alpha)
    else:
      cyc_color = cycler(color=colorblind_palette)
      plt.rcParams["axes.prop_cycle"] = cycler(cyc_color)

    plt.rcParams["legend.loc"] = legend_loc
    plt.rcParams["savefig.format"] = save_format
    plt.rcParams["savefig.bbox"] = "tight"

    # overall figure configuation
    plt.rcParams["figure.frameon"] = False
    if not presentation and set_figsize:
        plt.rcParams["figure.figsize"] = 3.15, 3.1
    # font size configuration
    font = 18 if presentation else 12
    # font family settings
    # arial for all text and stix for math
    plt.rcParams["mathtext.fontset"] = "stix"
    plt.rcParams["font.family"] = "STIXGeneral"
    plt.rcParams["axes.formatter.use_mathtext"] = True

    plt.rcParams["axes.titlesize"] = font
    plt.rcParams["axes.labelsize"] = font
    plt.rcParams["axes.titlesize"] = font
    plt.rcParams["axes.labelsize"] = font
    # legend configuration
    plt.rcParams["legend.frameon"] = False
    plt.rcParams["legend.fontsize"] = font - 2
    # plotting line parameters
    plt.rcParams["lines.linewidth"] = 2
    plt.rcParams["lines.markersize"] = 6
    plt.rcParams["errorbar.capsize"] = 5
    # axes
    linewidth = 2.5 if presentation else 1.5
    plt.rcParams["axes.linewidth"] = linewidth
    plt.rcParams["axes.labelpad"] = 4 if presentation else 2

    plt.rcParams["xtick.direction"] = "in"
    plt.rcParams["xtick.labelsize"] = font - 1

    plt.rcParams["ytick.direction"] = "in"
    plt.rcParams["ytick.labelsize"] = font - 1

    plt.rcParams["xtick.major.size"] = 9 if presentation else 4.5
    plt.rcParams["xtick.major.width"] = linewidth / 2

    plt.rcParams["ytick.major.size"] = 9 if presentation else 4.5
    plt.rcParams["ytick.major.width"] = linewidth / 2

    plt.rcParams["ytick.minor.visible"] = True
    plt.rcParams["ytick.minor.size"] = 5 if presentation else 2.5
    plt.rcParams["ytick.minor.width"] = linewidth / 2

    plt.rcParams["xtick.minor.visible"] = True
    plt.rcParams["xtick.minor.size"] = 5 if presentation else 2.5
    plt.rcParams["xtick.minor.width"] = linewidth / 2

    plt.rcParams["xtick.top"] = True
    plt.rcParams["xtick.bottom"] = True

    plt.rcParams["ytick.left"] = True
    plt.rcParams["ytick.right"] = True
