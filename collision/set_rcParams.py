import matplotlib.pyplot as plt

# apply this so all the numbers are in the same font


def set_tick_font():
    """
    Sets the font of the axes to use stix for the tick labels so that anything that is a number is stix
    """
    ax = plt.gca()
    plt.setp(ax.get_xticklabels(), fontfamily="STIXGeneral")
    plt.setp(ax.get_yticklabels(), fontfamily="STIXGeneral")


def set_rcParams(presentation=False):
    # line color scheme
    plt.style.use("seaborn-v0_8-colorblind")
    # overall figure configuation
    plt.rcParams["figure.frameon"] = False
    if not presentation:
        plt.rcParams["figure.figsize"] = 3.15, 3.1
    # font size configuration
    font = 20 if presentation else 12
    # font family settings
    # arial for all text and stix for math
    plt.rcParams["mathtext.fontset"] = "stix"
    plt.rcParams["font.family"] = "sans-serif"
    plt.rcParams["font.sans-serif"] = "arial"
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
    plt.rcParams["lines.markersize"] = 8 if presentation else 6
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
