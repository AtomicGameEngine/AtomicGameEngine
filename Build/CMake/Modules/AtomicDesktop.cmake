
include(AtomicCommon)

include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/Poco/Foundation/include)

add_definitions( -DATOMIC_NAVIGATION -DATOMIC_TBUI -DATOMIC_FILEWATCHER -DPOCO_NO_AUTOMATIC_LIBS -DPOCO_STATIC )

set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} LibCpuId SQLite)

# Check whether the CEF submodule is available
if (EXISTS ${CMAKE_SOURCE_DIR}/Submodules/CEF)
    #Check if CEF got pulled
    file(GLOB RESULT ${CMAKE_SOURCE_DIR}/Submodules/CEF)
    list(LENGTH ${RESULT} RES_LEN)
    if(RES_LEN EQUAL 0)
     message(STATUS "Trying to initalise CEF submodule")

     set(PATH_STRING $ENV{PATH})
     string(REPLACE ":" ";" PATHLIST ${PATH_STRING})

     set(GIT_FOUND false)
     foreach(cpath ${PATHLIST})
      if (EXISTS ${cpath}/git)
       set(GIT_FOUND true)
       break()
      endif()
     endforeach(cpath)

     # DIR is empty, so init the submodule and checkout master
     if(GIT_FOUND)
      execute_process(COMMAND git submodule update --init --remote)

      set(ATOMIC_WEBVIEW TRUE)
      add_definitions( -DATOMIC_WEBVIEW )
      include("AtomicWebView")
     else ()
      message(STATUS "Could not find git in your Path please install git to enable WebView")
     endif(GIT_FOUND)

    else ()
     set(ATOMIC_WEBVIEW TRUE)
     add_definitions( -DATOMIC_WEBVIEW )
     include("AtomicWebView")
    endif()
endif()

include(AtomicNET)
