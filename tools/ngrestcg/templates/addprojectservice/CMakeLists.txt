
##var servicesTmp $($services)
##until($($servicesTmp),)
##var serviceNs $($servicesTmp.!lasttoken/ /||$servicesTmp)
##var service $($serviceNs.!lasttoken/./||$serviceNs)
add_subdirectory($($service.!tolower))
if (APPLE) # cmake sets .so extension for modules under mac os x
    set_target_properties($($service.!tolower) PROPERTIES SUFFIX ".dylib")
endif()
if (MINGW) # cmake sets .so extension for modules under windows
    set_target_properties($($service.!tolower) PROPERTIES SUFFIX ".dll")
    link_directories("${NGREST_HOME}/bin")
endif()
##var servicesTmp $($servicesTmp.!cutlast/ /)
##done
