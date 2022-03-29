Functional features
===================

Python support
--------------

Python integration
******************

.. doxygenfile:: python_integration.hpp
   :sections: detaileddescription

.. doxygenvariable:: mcmc::util::is_python_initialized

.. doxygenfunction:: mcmc::util::initialize_python

.. doxygenfunction:: mcmc::util::finalize_python


Integrating external python modules
***********************************

.. doxygenvariable:: mcmc::util::python_modules_path

.. doxygenfunction:: mcmc::util::get_python_modules_path

.. doxygenfunction:: mcmc::util::set_python_modules_path

Utils
-----

Random number generation
************************

.. doxygenfile:: random.hpp
   :sections: detaileddescription

.. doxygenvariable:: mcmc::util::rd

.. doxygenvariable:: mcmc::util::gen

.. doxygenfunction:: mcmc::util::set_random_seed

Linspace intervals
******************

.. doxygenfunction:: mcmc::util::linspace

Sign function
*************

.. doxygenfunction:: mcmc::util::sign(const T value)