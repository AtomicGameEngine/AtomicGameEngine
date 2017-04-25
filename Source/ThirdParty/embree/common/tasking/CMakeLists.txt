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

IF (TASKING_INTERNAL)
  ADD_LIBRARY(tasking STATIC taskschedulerinternal.cpp)
ENDIF()

IF (TASKING_TBB)
  ADD_LIBRARY(tasking STATIC taskschedulertbb.cpp)
  TARGET_LINK_LIBRARIES(tasking sys ${TBB_LIBRARIES})
ENDIF()

IF (TASKING_PPL)
  ADD_LIBRARY(tasking STATIC taskschedulerppl.cpp)
  TARGET_LINK_LIBRARIES(tasking sys ${PPL_LIBRARIES})
ENDIF()

SET_PROPERTY(TARGET tasking PROPERTY FOLDER common)
