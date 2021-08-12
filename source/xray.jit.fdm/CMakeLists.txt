include(${CMAKE_CURRENT_SOURCE_DIR}/../max-sdk-base/script/max-pretarget.cmake)


#############################################################
# MAX EXTERNAL
#############################################################


include_directories( 
	"${C74_INCLUDES}"
)

file(GLOB SOURCE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h *.c)

add_library( 
	${PROJECT_NAME} 
	MODULE
	${SOURCE_FILES}
)


include(${CMAKE_CURRENT_SOURCE_DIR}/../max-sdk-base/script/max-posttarget.cmake)
