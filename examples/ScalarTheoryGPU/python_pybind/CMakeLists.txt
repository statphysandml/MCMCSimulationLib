 pybind11_add_module(scalartheorygpu_python scalartheorygpu_python.cpp ../src/scalar_theory_gpu_helper.cu)

 enable_language(CUDA)
# if(NOT DEFINED CMAKE_CUDA_STANDARD)
#  set(CMAKE_CUDA_STANDARD 11)
#  set(CMAKE_CUDA_STANDARD_REQUIRED ON)
# endif()

 set_target_properties(scalartheorygpu_python PROPERTIES
         # CUDA_SEPARABLE_COMPILATION ON
         CUDA_STANDARD 14
         CUDA_RESOLVE_DEVICE_SYMBOLS ON)
#
# set_property(TARGET scalartheorygpu_python
#         PROPERTY
#         BUILD_RPATH ${CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES})

 if(RUN_WITH_PYTHON_BACKEND)
    target_compile_definitions(scalartheorygpu_python PUBLIC PYTHON_BACKEND)
     # target_link_libraries(scalartheory_python PRIVATE ${target_link_libraries_python_appendix})
 endif()

 target_compile_definitions(scalartheorygpu_python PUBLIC -DGPU)

 target_link_libraries(scalartheorygpu_python PUBLIC mcmcsimulationlib::mcmcsimulationlib paramhelper::paramhelper cudart)
 set_target_properties(scalartheorygpu_python PROPERTIES OUTPUT_NAME scalartheorygpusimulation)

## set(PYBIND11_CPP_STANDARD -std=c++11)
#
#add_library(dummycu STATIC
#        ../src/scalar_theory_gpu_helper.cu
#        )
#
#set_target_properties(dummycu PROPERTIES
#        POSITION_INDEPENDENT_CODE ON
#        CUDA_VISIBILITY_PRESET "hidden"
#        CUDA_STANDARD 14
#        CUDA_RESOLVE_DEVICE_SYMBOLS ON
#        CUDA_SEPARABLE_COMPILATION ON
#        )
#
#set_property(TARGET dummycu
#        PROPERTY
#        BUILD_RPATH ${CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES})
#
#if(RUN_WITH_PYTHON_BACKEND)
#   target_compile_definitions(dummycu PUBLIC PYTHON_BACKEND)
#   # target_link_libraries(scalartheory_python PRIVATE ${target_link_libraries_python_appendix})
#endif()
#target_compile_definitions(dummycu PUBLIC -DGPU)
#
#target_link_libraries(dummycu PUBLIC mcmcsimulationlib::mcmcsimulationlib paramhelper::paramhelper)
#
#enable_language(CUDA)
#
#add_library(scalartheorygpu_python MODULE
#        ScalarTheoryGPU_python.cpp
#        )
##set_target_properties(dummycu PROPERTIES
##        POSITION_INDEPENDENT_CODE ON
##        CUDA_VISIBILITY_PRESET "hidden"
##        CUDA_STANDARD 14
##        CUDA_SEPARABLE_COMPILATION ON
##        )
#set_target_properties(scalartheorygpu_python PROPERTIES
#        CXX_VISIBILITY_PRESET "hidden"
#        INTERPROCEDURAL_OPTIMIZATION TRUE
#        PREFIX "${PYTHON_MODULE_PREFIX}"
#        SUFFIX "${PYTHON_MODULE_EXTENSION}"
#        CUDA_RESOLVE_DEVICE_SYMBOLS ON)
#
#set_property(TARGET scalartheorygpu_python
#        PROPERTY
#        BUILD_RPATH ${CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES})
#
#
#if(RUN_WITH_PYTHON_BACKEND)
#   target_compile_definitions(scalartheorygpu_python PUBLIC PYTHON_BACKEND)
#   # target_link_libraries(scalartheory_python PRIVATE ${target_link_libraries_python_appendix})
#endif()
#target_compile_definitions(scalartheorygpu_python PUBLIC -DGPU)
#
#target_link_libraries(scalartheorygpu_python PRIVATE dummycu)
#target_link_libraries(scalartheorygpu_python PRIVATE pybind11::module)
#target_link_libraries(scalartheorygpu_python PUBLIC mcmcsimulationlib::mcmcsimulationlib paramhelper::paramhelper)
#set_target_properties(scalartheorygpu_python PROPERTIES OUTPUT_NAME scalartheorygpusimulation)
