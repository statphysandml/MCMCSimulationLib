# from distribution1D import Distribution1D
#
# from plotting_environment.loading_figure_mode import loading_figure_mode
# fma = loading_figure_mode("saving")
#
# import numpy as np
# import pandas as pd
# import json as json
# import matplotlib.pyplot as plt
#
#
# # directory = "IpamSeries"
# # filename = "metropolis_distribution"
#
# output_dir = "/home/lukas/Lattice_Git/PROJECTS/LUKAS/BoltzmannMachine/plots/cubic_gaussian_model_n/"
#
#
# def trajectory_1d_ipam_plot(directory, filename, xlabel, ylabel, num=100):
#     df = pd.read_csv('../data/' + directory + '/' + filename + '.dat', header=0, delimiter="\t", index_col=False)
#     df.drop(df.columns[len(df.columns) - 1], axis=1, inplace=True)
#     # df.Config = df.Config.apply(lambda x: np.float32(x.split()))
#     print("Mean", df.Mean.mean(), "Std", df.Mean.std())
#
#     t = np.arange(0, len(df) * 0.02, 0.02)[:num]
#     y = df.Mean.values[:num + 1]
#
#     y = np.repeat(y, 2)
#     y = y[1:-1]
#     t = np.repeat(t, 2)
#     # t[1::2] -= 0.0001
#
#     fig, ax = fma.newfig(1, ratio=0.34)
#     ax.plot(t, y, linewidth=1, c="darkblue")
# #     from colorlineplot import colorline
# #     lc = colorline(t, y, linewidth=1, cmap='ocean')
# #     plt.colorbar(lc)
#     ax.set_xlim(t.min(), t.max())
#     ax.set_ylim(y.min(), y.max())
#     ax.set_xlabel(xlabel)
#     ax.set_ylabel(ylabel)
#
#     plt.tight_layout()
#     fma.savefig(output_dir, filename)
#     plt.close()
#
# # directory = "IpamSeries"
# # filename = "cle_trajectory"
#
#
# def trajectory_2d_ipam_plot(directory, filename, xlabel, ylabel, zlabel, num=1000):
#     df = pd.read_csv('../data/' + directory + '/' + filename + '.dat', header=0, delimiter="\t", index_col=False)
#     df.drop(df.columns[len(df.columns) - 1], axis=1, inplace=True)
#     df.ComplexConfig = df.ComplexConfig.apply(lambda x: np.float32(x.split()))
#     print("Mean", df.Mean.mean(), "Std", df.Mean.std())
#     data = np.stack(df.ComplexConfig.values, axis=0)
#
#     fig, ax = fma.newfig(0.8)
#     from colorlineplot import colorline
#
#     x = data[:num, 0]
#     y = data[:num, 1]
#
#     # t[1::2] -= 0.0001
#
#     lc = colorline(x, y, linewidth=1, cmap='nipy_spectral')
#     cbar = plt.colorbar(lc)
#     cbar.ax.set_ylabel(zlabel)
#     ax.set_xlim(x.min(), x.max())
#     ax.set_ylim(y.min(), y.max())
#     ax.set_xlabel(xlabel)
#     ax.set_ylabel(ylabel)
#
#     plt.tight_layout()
#     fma.savefig(output_dir, filename)
#     plt.close()
#
#
# # directory = "IpamSeries"
# # filename = "metropolis_trajectory"
#
#
# def distribution_1d_ipam_plot(directory, filename, xlabel):
#     df = pd.read_csv('../data/' + directory + '/' + filename + '.dat', header=0, delimiter="\t", index_col=False)
#     df.drop(df.columns[len(df.columns) - 1], axis=1, inplace=True)
#     # df.Config = df.Config.apply(lambda x: np.float32(x.split()))
#     # df.Mean.mean(), df.Mean.std()
#     print("Mean", df.Mean.mean(), "Std", df.Mean.std())
#
#     dist1d = Distribution1D(data=pd.concat([df], keys="dat"))
#     dist1d.compute_histograms(["Mean"], nbins=100, kind="probability_dist")
#
#     fig, ax = fma.newfig(0.54, ratio=0.8)
#     histo = dist1d.histograms["d"]["Mean"]
#     dist1d.plot_histogram(ax, histo["hist"], histo["rel_bins"])
#     ax.set_xlabel(xlabel)
#     ax.set_ylabel("Probability")
#
#     plt.tight_layout()
#
#     fma.savefig(output_dir,
#                 filename)
#     plt.close()
#
#
# def distribution_2d_ipam_plot(directory, filename, xlabel, ylabel, range_min_x=None, range_max_x=None, range_min_y=None, range_max_y=None):
#     df = pd.read_csv('../data/' + directory + '/' + filename + '.dat', header=0, delimiter="\t", index_col=False)
#     df.drop(df.columns[len(df.columns) - 1], axis=1, inplace=True)
#     df.ComplexConfig = df.ComplexConfig.apply(lambda x: np.float32(x.split()))
#     print("Mean", df.Mean.mean(), "Std", df.Mean.std())
#     data = np.stack(df.ComplexConfig.values, axis=0)
#
#     from distribution2D import Distribution2D
#     dist2d = Distribution2D(data=pd.concat([pd.DataFrame(data=data)], keys=["dat"]))
#
#     binned_statistics = dist2d.compute_binned_statistics(
#         x_index=0,
#         y_index=1,
#         range_min_x=range_min_x,
#         range_max_x=range_max_x,
#         range_min_y=range_min_y,
#         range_max_y=range_max_y,
#         nbins_x=100,
#         nbins_y=100,
#         columns=[0],
#         transform='lin',
#         statistic='count'
#     )
#
#     linearized_statistics = Distribution2D.linearize_binned_statistics(x_index=0, y_index=1,
#                                                                        binned_statistics=binned_statistics)
#     row_value = linearized_statistics.index.unique(0)[0]
#
#     # from surfaceplot import Surface2D
#     #
#     z_index = '0_ext'
#     sum = linearized_statistics[z_index].sum()
#     linearized_statistics[z_index] = linearized_statistics[z_index].apply(lambda x: x / sum)
#     linearized_statistics.columns = [0, 1, 'Probability']
#     # print(row_value)
#     #
#     # contour2D = Surface2D(
#     #     data=linearized_statistics.loc[row_value],
#     #     compute_x_func=lambda x: x[0],
#     #     compute_y_func=lambda x: x[1],
#     #     z_index="Probability"
#     # )
#     #
#     # import matplotlib.pyplot as plt
#     #
#     # fig, ax = fma.surfacenewfig(1.4)
#     # lev_min, lev_max = Surface2D.get_min_max_of_index(linearized_statistics, "Probability")
#     # # norm, levs = Contour2D.get_exp_norm_and_levs(lev_min=lev_min, lev_max=lev_max, lev_num=40)
#     # norm, levs = Surface2D.get_lin_norm_and_levs(lev_min=lev_min, lev_max=lev_max, lev_num=40)
#     #
#     # contour2D.surface(
#     #     fig=fig,
#     #     ax=ax,
#     #     levs=levs,
#     #     norm=norm,
#     #     # cax=cbar_ax,
#     #     x_label=xlabel,
#     #     y_label=ylabel,
#     #     z_label="Probability",
#     #     # x_ticks_visible=False,
#     #     # z_ticks=[-1.4, -1, -0.5, 0, 0.5, 1, 1.4],
#     #     # z_tick_labels=['$-1.4$', '$-1$', '$-0.5$', '$0$', '$0.5$',
#     #     #               '$1$', '$1.4$']
#     #     # z_ticks=[1e-3, 3e-3, 1e-2, 3e-2, 1e-1, 3e-1, 1],
#     #     # z_tick_labels=['$10^{-3}$', '$3\\times 10^{-3}$', '$10^{-2}$', '$3 \\times 10^{-2}$', '$10^{-1}$',
#     #     #             '$3\\times 10^{-1}$', '$1$']
#     # )
#     #
#     # plt.tight_layout()
#     #
#     # fma.savefig(output_dir, filename + "_surface")
#     # plt.close()
#
#     from contour2D import Contour2D
#     contour2D = Contour2D(
#         data=linearized_statistics.loc[row_value],
#         compute_x_func=lambda x: x[0],
#         compute_y_func=lambda x: x[1],
#         z_index="Probability"
#     )
#
#     import matplotlib.pyplot as plt
#
#     fig, ax = fma.newfig(1.4)
#     lev_min, lev_max = Contour2D.get_min_max_of_index(linearized_statistics, "Probability")
#     # norm, levs = Contour2D.get_exp_norm_and_levs(lev_min=lev_min, lev_max=lev_max, lev_num=40)
#     norm, levs = Contour2D.get_lin_norm_and_levs(lev_min=lev_min, lev_max=lev_max, lev_num=40)
#
#     contour2D.contourf(
#         fig=fig,
#         ax=ax,
#         levs=levs,
#         norm=norm,
#         # cax=cbar_ax,
#         x_label=xlabel,
#         y_label=ylabel,
#         z_label="Probability",
#         # x_ticks_visible=False,
#         # z_ticks=[-1.4, -1, -0.5, 0, 0.5, 1, 1.4],
#         # z_tick_labels=['$-1.4$', '$-1$', '$-0.5$', '$0$', '$0.5$',
#         #               '$1$', '$1.4$']
#         # z_ticks=[1e-3, 3e-3, 1e-2, 3e-2, 1e-1, 3e-1, 1],
#         # z_tick_labels=['$10^{-3}$', '$3\\times 10^{-3}$', '$10^{-2}$', '$3 \\times 10^{-2}$', '$10^{-1}$',
#         #             '$3\\times 10^{-1}$', '$1$']
#     )
#
#     plt.tight_layout()
#
#     fma.savefig(output_dir, filename + "cle_contour")
#     plt.close()
#
#

from ppd_examples.mock_data import load_multivariate_mock_data
data = load_multivariate_mock_data()

if __name__ == '__main__':
    pass
#
#
#
#
