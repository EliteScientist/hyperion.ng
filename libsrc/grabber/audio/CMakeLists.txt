# Define the current source locations
SET( CURRENT_HEADER_DIR ${CMAKE_SOURCE_DIR}/include/grabber )
SET( CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libsrc/grabber/audio )


if (WIN32)
    FILE ( GLOB AUDIO_GRABBER_SOURCES "${CURRENT_HEADER_DIR}/Audio*Windows.h"  "${CURRENT_HEADER_DIR}/AudioGrabber.h"  "${CURRENT_HEADER_DIR}/AudioWrapper.h"  "${CURRENT_SOURCE_DIR}/*.h"  "${CURRENT_SOURCE_DIR}/*Windows.cpp"  "${CURRENT_SOURCE_DIR}/AudioGrabber.cpp"  "${CURRENT_SOURCE_DIR}/AudioWrapper.cpp")
elseif(${CMAKE_SYSTEM} MATCHES "Linux")
    FILE ( GLOB AUDIO_GRABBER_SOURCES "${CURRENT_HEADER_DIR}/Audio*Linux.h"  "${CURRENT_HEADER_DIR}/AudioGrabber.h"  "${CURRENT_HEADER_DIR}/AudioWrapper.h"  "${CURRENT_SOURCE_DIR}/*.h"  "${CURRENT_SOURCE_DIR}/*Linux.cpp"  "${CURRENT_SOURCE_DIR}/AudioGrabber.cpp"  "${CURRENT_SOURCE_DIR}/AudioWrapper.cpp")
elseif (APPLE)
    #TODO
    #FILE ( GLOB AUDIO_GRABBER_SOURCES "${CURRENT_HEADER_DIR}/Audio*Apple.h"  "${CURRENT_HEADER_DIR}/AudioGrabber.h"  "${CURRENT_HEADER_DIR}/AudioWrapper.h"  "${CURRENT_SOURCE_DIR}/*.h"  "${CURRENT_SOURCE_DIR}/*Apple.cpp"  "${CURRENT_SOURCE_DIR}/AudioGrabber.cpp"  "${CURRENT_SOURCE_DIR}/AudioWrapper.cpp")
endif()

add_library( audio-grabber ${AUDIO_GRABBER_SOURCES} )

target_link_libraries(audio-grabber
    hyperion
    ${QT_LIBRARIES}
)
