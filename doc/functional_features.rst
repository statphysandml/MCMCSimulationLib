Functional Features
===================

Python support
--------------

Python integration
******************

.. doxygenfile:: python_integration.hpp
   :sections: detaileddescription

.. doxygenvariable:: mcmc::util::python_integration::g_is_python_initialized

.. doxygenfunction:: mcmc::util::initialize_python

.. doxygenfunction:: mcmc::util::finalize_python


Integrating external Python modules
***********************************

.. doxygenvariable:: mcmc::util::python_integration::g_python_modules_path

.. doxygenfunction:: mcmc::util::python_integration::get_python_modules_path

.. doxygenfunction:: mcmc::util::python_integration::set_python_modules_path

Utils
-----

Random number generation
************************

.. doxygenfile:: random.hpp
   :sections: detaileddescription

.. doxygenvariable:: mcmc::util::random::g_rd

.. doxygenvariable:: mcmc::util::random::g_gen

.. doxygenfunction:: mcmc::util::random::set_random_seed

Linspace intervals
******************

.. doxygenfunction:: mcmc::util::linspace

Sign function
*************

.. doxygenfunction:: mcmc::util::sign(const T value)