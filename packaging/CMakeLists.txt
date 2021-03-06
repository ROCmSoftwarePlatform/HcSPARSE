cmake_minimum_required(VERSION 2.8)

# HIP_PATH
IF(NOT DEFINED $ENV{HIP_PATH})
  SET(HIP_PATH "/opt/rocm/hip")
ELSE()
  SET(HIP_PATH $ENV{HIP_PATH})
ENDIF()

SET(INSTALL_OPT $ENV{INSTALL_OPT})
SET(HIP_SUPPORT $ENV{HIP_SUPPORT})

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake  "${HIP_PATH}/cmake")
EXECUTE_PROCESS(COMMAND ${HIP_PATH}/bin/hipconfig -P OUTPUT_VARIABLE PLATFORM)

IF (${PLATFORM} MATCHES "hcc")
  IF(DEFINED INSTALL_OPT)
    SET( lib_so_link ${CMAKE_INSTALL_PREFIX}/../lib/libhcsparse.so)
    SET( hcc_so_target ${CMAKE_INSTALL_PREFIX}/lib/libhcsparse.so)
    execute_process(COMMAND ln -s ${hcc_so_target} ${lib_so_link})

    IF(${HIP_SUPPORT} MATCHES "on")
      SET( lib_so_link ${CMAKE_INSTALL_PREFIX}/../lib/libhipsparse.so)
      SET( hip_so_target ${CMAKE_INSTALL_PREFIX}/lib/libhipsparse.so)
      execute_process(COMMAND ln -s ${hip_so_target} ${lib_so_link})
      SET( lib_so_link ${CMAKE_INSTALL_PREFIX}/lib/libhipsparse.so)
      execute_process(COMMAND ln -s ${hip_so_target} ${lib_so_link})
    ENDIF()
  ENDIF()
ENDIF()
