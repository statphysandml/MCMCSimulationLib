//
// Created by lukas on 28.02.20.
//
#include "../../include/execution/python_integration.hpp"

#ifdef RUN_WITH_PYTHON_BACKEND

namespace mcmc {
    namespace util {

        // Project dependent parameters
        bool is_python_initialized = false;

        // Project dependent parameter
        std::string python_modules_path = "";

        std::string get_python_modules_path() {
            return python_modules_path;
        }

        void set_python_modules_path(const std::string python_modules_path_) {
            python_modules_path = python_modules_path_;
            std::cout << " -- Setting python scripts path to " << python_modules_path_ << " --" << std::endl;
        }

        void initialize_python(const std::string python_modules_path_, const bool fma_develop, const bool executing_from_python) {
            if(is_python_initialized)
                return;
            if(!executing_from_python)
                py::initialize_interpreter();
            if(python_modules_path_ != "")
            {
                set_python_modules_path(python_modules_path_);
                py::exec(("import sys; sys.path.append('" + get_python_modules_path() + "')").c_str());
            }
            // py::exec("import sys; print('Python system path:',  sys.path)");
            
            py::exec("from pystatplottools.pdf_env.loading_figure_mode import loading_figure_mode");
            if(fma_develop)
                py::exec("fma, plt = loading_figure_mode(develop=True)");
            else
                py::exec("fma, plt = loading_figure_mode(develop=False)");
            is_python_initialized = true;
        }

        void finalize_python() {
            py::finalize_interpreter();
            is_python_initialized = false;
        }
    }
}

#endif
