import numpy as np


def polynomial_model(x, lamda, sigma, h=0):
    return 0.25 * lamda * np.power(x, 4.0) + 0.5 * sigma * np.power(x, 2.0) + h * x


def get_energy_of_dat(dat, sigma_imag):
    return (dat["StateReal"] + dat["StateImag"] * 1j).apply(lambda x: polynomial_model(x, 1.0, 1.0 + sigma_imag * 1.0j, 0.0))


def get_energy_of_dat_with_sigma_imag(data):
    dat = data.groupby("sigma_imag").apply(lambda x: polynomial_model(x["StateReal"] + x["StateImag"] * 1.0j, 1.0, 1.0 + x["Sigma_imag"] * 1.0j, 0.0))
    dat.index = dat.index.droplevel(0)
    return dat


def get_energy_of_real_dat(dat, sigma_imag):
    return dat["Mean"].apply(lambda x: polynomial_model(x, 1.0, 1.0 + sigma_imag * 1.0j, 0.0))


def get_complex2nd_moment(dat):
    return (dat["StateReal"] + dat["StateImag"] * 1j).apply(lambda x: np.power(x, 2.0))


def get_2nd_moment(dat):
    return dat["Mean"].apply(lambda x: np.power(x, 2.0))


def compute_reweighted_complex_polynomial_model_complex_2nd_moment(data):
    sigma_imag_target = 5.0
    dat = (get_complex2nd_moment(data) * np.exp(-1.0 * (get_energy_of_dat(data, sigma_imag_target) - get_energy_of_dat_with_sigma_imag(data)))).values
    data.insert(len(data.columns), "ReweightedComplexPolynomialModelComplex2ndMomentReal", np.real(dat))
    data.insert(len(data.columns), "ReweightedComplexPolynomialModelComplex2ndMomentImag", np.imag(dat))
    data.insert(len(data.columns), "ReweightedComplexPolynomialModelComplex2ndMomentAbs", np.abs(dat))


def compute_reweighted_complex_polynomial_model_phase_factor(data):
    sigma_imag_target = 5.0
    dat = np.exp(-1.0 * (get_energy_of_dat(data, sigma_imag_target) - get_energy_of_dat_with_sigma_imag(data))).values
    data.insert(len(data.columns), "ReweightedComplexPolynomialModelPhaseFactorReal", np.real(dat))
    data.insert(len(data.columns), "ReweightedComplexPolynomialModelPhaseFactorImag", np.imag(dat))
    data.insert(len(data.columns), "ReweightedComplexPolynomialModelPhaseFactorAbs", np.abs(dat))


def compute_reweighted_polynomial_model_2nd_moment(data):
    sigma_imag_target = 5.0
    dat = (get_2nd_moment(data) * np.exp(-1.0 * (get_energy_of_real_dat(data, sigma_imag_target) - get_energy_of_real_dat(data, 0.0)))).values
    data.insert(len(data.columns), "ReweightedPolynomialModel2ndMomentReal", np.real(dat))
    data.insert(len(data.columns), "ReweightedPolynomialModel2ndMomentImag", np.imag(dat))
    data.insert(len(data.columns), "ReweightedPolynomialModel2ndMomentAbs", np.abs(dat))


def compute_reweighted_polynomial_model_phase_factor(data):
    sigma_imag_target = 5.0
    dat = np.exp(-1.0 * (get_energy_of_real_dat(data, sigma_imag_target) - get_energy_of_real_dat(data, 0.0))).values
    data.insert(len(data.columns), "ReweightedPolynomialModelPhaseFactorReal", np.real(dat))
    data.insert(len(data.columns), "ReweightedPolynomialModelPhaseFactorImag", np.imag(dat))
    data.insert(len(data.columns), "ReweightedPolynomialModelPhaseFactorAbs", np.abs(dat))


''' Not used at the moment '''


def get_reweighted_complex2nd_moment(dat_a, sigma_imag_a, sigma_imag_b):
    observable = (get_complex2nd_moment(dat_a) * np.exp(-1.0 * (get_energy_of_dat(dat_a, sigma_imag_b) - get_energy_of_dat(dat_a, sigma_imag_a)))).mean()
    phase_factor = np.exp(-1.0 * (get_energy_of_dat(dat_a, sigma_imag_b) - get_energy_of_dat(dat_a, sigma_imag_a))).apply("mean")
    return observable / phase_factor, phase_factor

def get_reweighted_2nd_moment(dat_a, sigma_imag_a, sigma_imag_b):
    observable = (get_2nd_moment(dat_a) * np.exp(-1.0 * (get_energy_of_real_dat(dat_a, sigma_imag_b) - get_energy_of_real_dat(dat_a, sigma_imag_a)))).mean()
    phase_factor = np.exp(-1.0 * (get_energy_of_real_dat(dat_a, sigma_imag_b) - get_energy_of_real_dat(dat_a, sigma_imag_a))).apply("mean")
    return observable / phase_factor, phase_factor
