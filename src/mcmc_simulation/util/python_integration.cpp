#include <mcmc/mcmc_simulation/util/python_integration.hpp>

#ifdef PYTHON_BACKEND

namespace mcmc::util {
    // Project dependent parameters
    bool python_integration::g_is_python_initialized = false;

    // Project dependent parameter
    std::string python_integration::g_python_modules_path = "";

    std::string python_integration::get_python_modules_path() {
        return g_python_modules_path;
    }

    void set_python_modules_path(const std::string &python_modules_path) {
        python_integration::g_python_modules_path = python_modules_path;
        std::cout << " -- Setting python scripts path to '" << python_modules_path << "' --" << std::endl;
    }

    void initialize_python(const std::string &python_modules_path, const bool fma_develop, const bool executing_from_python) {
        if(python_integration::g_is_python_initialized)
            return;
        if(!executing_from_python)
            py::initialize_interpreter();
        if(python_modules_path != "")
        {
            set_python_modules_path(python_modules_path);
            py::exec(("import sys; sys.path.append('" + python_integration::get_python_modules_path() + "')").c_str());
        }
        // py::exec("import sys; print('Python system path:',  sys.path)");
        
        py::exec("from pystatplottools.pdf_env.loading_figure_mode import loading_figure_mode");
        if(fma_develop)
            py::exec("fma, plt = loading_figure_mode(develop=True)");
        else
            py::exec("fma, plt = loading_figure_mode(develop=False)");
        python_integration::g_is_python_initialized = true;
    }

    void finalize_python() {
        py::finalize_interpreter();
        python_integration::g_is_python_initialized = false;
    }
}

#endif
