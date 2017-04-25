## ======================================================================== ##
## Copyright 2009-2017 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

IF(NOT BUILD_TESTING)
  SET(BUILD_TESTING OFF CACHE BOOL "Build the testing tree.")
ENDIF()

INCLUDE(CTest)

IF (BUILD_TESTING)

  CMAKE_HOST_SYSTEM_INFORMATION(RESULT memory QUERY TOTAL_PHYSICAL_MEMORY)
  
  SET(EMBREE_TESTING_MODEL_DIR "${PROJECT_SOURCE_DIR}/models" CACHE PATH "Path to the folder containing the Embree models for regression testing.")
  SET(EMBREE_TESTING_INTENSIVE OFF CACHE BOOL "Turns intensive testing on.")
  SET(EMBREE_TESTING_MEMCHECK OFF CACHE BOOL "Turns on memory checking for some tests.")
  SET(EMBREE_TESTING_BENCHMARK OFF CACHE BOOL "Turns benchmarking on.")
  SET(EMBREE_TESTING_BENCHMARK_DATABASE "${PROJECT_BINARY_DIR}" CACHE PATH "Path to database for benchmarking.")
  SET(EMBREE_TESTING_PACKAGE OFF CACHE BOOL "Packages release as test.")
  SET(EMBREE_TESTING_KLOCWORK OFF CACHE BOOL "Runs Kocwork as test.")

  SET(EMBREE_TESTING_SDE OFF CACHE STRING "Uses SDE to run tests for specified CPU.")
  SET_PROPERTY(CACHE EMBREE_TESTING_SDE PROPERTY STRINGS OFF pnr nhm wsm snb ivb hsw bdw knl skl skx cnl)

  IF(   NOT EXISTS "${EMBREE_TESTING_MODEL_DIR}/embree-models-subdiv.txt"
     OR NOT EXISTS "${EMBREE_TESTING_MODEL_DIR}/embree-models-small-win32.txt"
     OR NOT EXISTS "${EMBREE_TESTING_MODEL_DIR}/embree-models-small-x64.txt"
     OR NOT EXISTS "${EMBREE_TESTING_MODEL_DIR}/embree-models-large.txt")
    MESSAGE(FATAL_ERROR "Invalid Embree testing model repository. Either disable BUILD_TESTING or properly set EMBREE_TESTING_MODEL_DIR.")
  ENDIF()
  
  FILE(READ "${EMBREE_TESTING_MODEL_DIR}/embree-models-subdiv.txt" models_subdiv)
  STRING(REGEX REPLACE "\n" ";" models_subdiv "${models_subdiv}")
  
  FILE(READ "${EMBREE_TESTING_MODEL_DIR}/embree-models-small-win32.txt" models_small_win32)
  STRING(REGEX REPLACE "\n" ";" models_small_win32 "${models_small_win32}")
  
  FILE(READ "${EMBREE_TESTING_MODEL_DIR}/embree-models-small-x64.txt" models_small_x64)
  STRING(REGEX REPLACE "\n" ";" models_small_x64 "${models_small_x64}")
  
  FILE(READ "${EMBREE_TESTING_MODEL_DIR}/embree-models-large.txt" models_large)
  STRING(REGEX REPLACE "\n" ";" models_large "${models_large}")

  IF (EMBREE_TESTING_INTENSIVE)
    SET(models ${models_small_x64})
    IF (${memory} GREATER 10000) 
      SET(models ${models} ${models_large})
    ENDIF()
  ELSE()
    SET(models ${models_small_win32})
  ENDIF()

  IF (WIN32)
    SET(MY_PROJECT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
  ELSE()
    SET(MY_PROJECT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  ENDIF()
  
  MACRO (ADD_EMBREE_MODEL_TEST name reference executable args model)
  
    ADD_TEST(NAME ${name}
             WORKING_DIRECTORY ${MY_PROJECT_BINARY_DIR}
             COMMAND python ${PROJECT_SOURCE_DIR}/scripts/invoke_test.py
                     --name ${name}
                     --modeldir ${EMBREE_TESTING_MODEL_DIR}
                     --reference ${reference}
                     --model ${model}
                     --sde ${EMBREE_TESTING_SDE}
                     --execute ${MY_PROJECT_BINARY_DIR}/${executable} ${args})
                     
    IF (EMBREE_ISPC_SUPPORT AND EMBREE_RAY_PACKETS)
      ADD_TEST(NAME ${name}_ispc
               WORKING_DIRECTORY ${MY_PROJECT_BINARY_DIR}
               COMMAND python ${PROJECT_SOURCE_DIR}/scripts/invoke_test.py
                       --name ${name}_ispc
                       --modeldir ${EMBREE_TESTING_MODEL_DIR}
                       --reference ${reference}
                       --model ${model}
                       --sde ${EMBREE_TESTING_SDE}
                       --execute ${MY_PROJECT_BINARY_DIR}/${executable}_ispc ${args})
    ENDIF()
  ENDMACRO()
  
  MACRO (ADD_EMBREE_MODELS_TEST name reference executable)
    FOREACH (model ${models})
      STRING(REGEX REPLACE "/" "_" modelname "${model}")
      STRING(REGEX REPLACE ".ecs" "" modelname "${modelname}")
      ADD_EMBREE_MODEL_TEST(${name}_${modelname} ${reference}_${modelname} ${executable} "${ARGN}" ${model})
    ENDFOREACH()
  ENDMACRO()

  MACRO (ADD_EMBREE_SUBDIV_MODELS_TEST name reference executable)
    FOREACH (model ${models_subdiv})
      STRING(REGEX REPLACE "/" "_" modelname "${model}")
      STRING(REGEX REPLACE ".ecs" "" modelname "${modelname}")
      ADD_EMBREE_MODEL_TEST(${name}_${modelname} ${reference}_${modelname} ${executable} "${ARGN}" ${model})
    ENDFOREACH()
  ENDMACRO()

  IF (EMBREE_TESTING_KLOCWORK)
    ADD_TEST(NAME Klocwork WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PROJECT_SOURCE_DIR}/scripts/klocwork.sh)
    SET_TESTS_PROPERTIES(Klocwork PROPERTIES TIMEOUT 1200)
  ENDIF()

ELSE()

  MACRO (ADD_EMBREE_MODEL_TEST name reference executable args model)
  ENDMACRO()

  MACRO (ADD_EMBREE_SUBDIV_MODELS_TEST name reference executable)
  ENDMACRO()

  MACRO (ADD_EMBREE_MODELS_TEST name)
  ENDMACRO()

ENDIF()

MACRO (ADD_EMBREE_TEST name)
  ADD_EMBREE_MODEL_TEST(${name} ${name} ${name} "" "default")
ENDMACRO()

MACRO (ADD_EMBREE_TEST2 name exe args)
  ADD_EMBREE_MODEL_TEST(${name} ${exe} ${exe} "${args}" "default")
ENDMACRO()

IF (EMBREE_TESTING_MEMCHECK)
  find_program( EMBREE_MEMORYCHECK_COMMAND valgrind )
  set( EMBREE_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --show-leak-kinds=definite --errors-for-leak-kinds=definite --error-exitcode=1" )

  FUNCTION(ADD_MEMCHECK_TEST name binary)
    set(memcheck_command "${EMBREE_MEMORYCHECK_COMMAND} ${EMBREE_MEMORYCHECK_COMMAND_OPTIONS}")
    separate_arguments(memcheck_command)
    add_test(NAME ${name} COMMAND ${memcheck_command} ${MY_PROJECT_BINARY_DIR}/${binary} ${ARGN})
  ENDFUNCTION()
ENDIF()
