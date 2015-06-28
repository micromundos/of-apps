
if(NOT OPENCV_HIGHGUI_FOUND)
    message(FATAL_ERROR "OpenCV compiled without support for highgui, cannot compile samples!")
endif()

add_executable( detect_from_file detect_from_file.cpp)
target_link_libraries( detect_from_file chilitags_static )
target_link_libraries( detect_from_file ${OpenCV_LIBS} )
target_link_libraries( detect_from_file oscpack ${LIBS})

add_executable( osc_dump OscDump.cpp)
target_link_libraries( osc_dump chilitags_static )
target_link_libraries( osc_dump ${OpenCV_LIBS} )
target_link_libraries( osc_dump oscpack ${LIBS})

