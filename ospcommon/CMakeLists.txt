## ======================================================================== ##
## Copyright 2009-2016 Intel Corporation                                    ##
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

IF (NOT (TARGET ospray_common))
  SET(CMAKE_THREAD_PREFER_PTHREAD TRUE)
  SET(THREADS_PREFER_PTHREAD_FLAG TRUE)
  FIND_PACKAGE(Threads REQUIRED)
  SET(LINK_LIBS ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
  
  IF (WIN32)
    LIST(APPEND LINK_LIBS ws2_32)
  ENDIF()
  
  ADD_LIBRARY(ospray_common SHARED
    common.cpp
    FileName.cpp
    sysinfo.cpp
    malloc.cpp
    library.cpp
    thread.cpp
    vec.cpp
    array3D/Array3D.cpp

    AffineSpace.h
    box.h
    constants.h
    intrinsics.h
    LinearSpace.h
    math.h
    platform.h
    Quaternion.h
    RefCount.h
    vec.h
    )
  TARGET_LINK_LIBRARIES(
    ospray_common ${LINK_LIBS}
    )
ENDIF()
