# Install script for directory: D:/RV College of Engineering/6th Semester/Compiler Design/Lab EL/llvm-project/llvm/tools/opt-viewer

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/RV College of Engineering/6th Semester/Compiler Design/Lab EL/llvm-project/llvm/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "opt-viewer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/opt-viewer" TYPE PROGRAM FILES "D:/RV College of Engineering/6th Semester/Compiler Design/Lab EL/llvm-project/llvm/tools/opt-viewer/opt-diff.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "opt-viewer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/opt-viewer" TYPE PROGRAM FILES "D:/RV College of Engineering/6th Semester/Compiler Design/Lab EL/llvm-project/llvm/tools/opt-viewer/opt-stats.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "opt-viewer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/opt-viewer" TYPE PROGRAM FILES "D:/RV College of Engineering/6th Semester/Compiler Design/Lab EL/llvm-project/llvm/tools/opt-viewer/opt-viewer.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "opt-viewer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/opt-viewer" TYPE PROGRAM FILES "D:/RV College of Engineering/6th Semester/Compiler Design/Lab EL/llvm-project/llvm/tools/opt-viewer/optpmap.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "opt-viewer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/opt-viewer" TYPE PROGRAM FILES "D:/RV College of Engineering/6th Semester/Compiler Design/Lab EL/llvm-project/llvm/tools/opt-viewer/optrecord.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "opt-viewer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/opt-viewer" TYPE PROGRAM FILES "D:/RV College of Engineering/6th Semester/Compiler Design/Lab EL/llvm-project/llvm/tools/opt-viewer/style.css")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/RV College of Engineering/6th Semester/Compiler Design/Lab EL/llvm-project/llvm/out/build/x64-Debug/tools/opt-viewer/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
