cmake_minimum_required(VERSION 3.16)

include(FetchContent)

project(Runes VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Set up yaml-cpp
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 0.8.0
)
FetchContent_MakeAvailable(yaml-cpp)

# Set up fmtlib
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

# Set up hidapi
FetchContent_Declare(hidapi
    URL      https://github.com/libusb/hidapi/archive/d6b2a974608dec3b76fb1e36c189f22b9cf3650c.zip
    URL_HASH SHA256=e72b71904604238de9e04e4b456acbb30cb2c65fe96797043e572aff39cb8939
)
FetchContent_GetProperties(hidapi)
if(NOT hidapi_POPULATED)
    FetchContent_Populate(hidapi)
	set(HIDAPI_WITH_LIBUSB FALSE) # surely will be used only on Linux
	set(BUILD_SHARED_LIBS FALSE)  # HIDAPI as static library on all platforms
    add_subdirectory(${hidapi_SOURCE_DIR} ${hidapi_BINARY_DIR})
endif()

qt_standard_project_setup()

qt_add_executable(Runes
	source/3rd_party/crc.cpp
	source/3rd_party/crc.h
	source/3rd_party/md5.cpp
	source/3rd_party/md5.h
	source/3rd_party/rijndael.cpp
	source/3rd_party/rijndael.h
	source/Constants.cpp
	source/Constants.hpp
	source/debug.cpp
	source/debug.hpp
	source/EElementType.hpp
	source/ESkylandersGame.hpp
	source/FigureTabWidget.cpp
	source/FigureTabWidget.hpp
	source/hardware/HardwareIds.hpp
	source/hardware/HardwareInterface.cpp
	source/hardware/HardwareInterface.hpp
	source/hardware/HidUsbInterface.cpp
	source/hardware/HidUsbInterface.hpp
	source/hardware/PortalDriver.cpp
	source/hardware/PortalDriver.hpp
	source/hardware/PortalType.hpp
	source/HeroicsNames.cpp
	source/HeroicsNames.hpp
	source/kTfbSpyroTag_DecoID.hpp
	source/kTfbSpyroTag_HatType.cpp
	source/kTfbSpyroTag_HatType.hpp
	source/kTfbSpyroTag_ToyType.hpp
	source/kTfbSpyroTag_TrinketType.cpp
	source/kTfbSpyroTag_TrinketType.hpp
	source/kTfbSpyroTag_VillainType.hpp
	source/main.cpp
	source/PortalAlgos.cpp
	source/PortalAlgos.hpp
	source/PortalDebuggerWidget.cpp
	source/PortalDebuggerWidget.hpp
	source/PortalTag.cpp
	source/PortalTag.hpp
	source/PortalTagAsserts.inc
	source/RfidTag.cpp
	source/RfidTag.hpp
	source/RunesDebug.hpp
	source/RunesMainWidget.cpp
	source/RunesMainWidget.hpp
	source/toydata.cpp
	source/toydata.hpp
)

# Retrieve the absolute path to qmake and then use that path to find
# the windeployqt executable
get_target_property(QMAKE_EXE Qt6::qmake IMPORTED_LOCATION)
get_filename_component(QT_BIN_DIR "${QMAKE_EXE}" DIRECTORY)

if (MSVC)
	# Required compile options for MSVC with Qt
	target_compile_options(Runes PUBLIC -Zc:rvalueCast -Zc:inline -Zc:strictStrings -Zc:throwingNew -permissive- -Zc:__cplusplus -Zc:externConstexpr -utf-8 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -W4)

	# Set it as Win32 app to hide console window
	#set_property(TARGET Runes PROPERTY WIN32_EXECUTABLE true)

	# Run windeployqt
	find_program(WINDEPLOYQT_ENV_SETUP qtenv2.bat HINTS "${QT_BIN_DIR}")
	find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

	add_custom_command(TARGET Runes
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E env "${WINDEPLOYQT_ENV_SETUP}" && "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:Runes>"
	)
endif()

if (WIN32)
	target_link_libraries(Runes PRIVATE "Hid.dll")
endif()

# Copy resources directory
add_custom_command(
	TARGET Runes
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Resources" "$<TARGET_FILE_DIR:Runes>/Resources"
)

# This is a cursed way to include the hidapi.h file but ignore it
target_include_directories(Runes PRIVATE ${hidapi_SOURCE_DIR}/hidapi)

target_include_directories(Runes PRIVATE
	source
	source/3rd_party
	source/hardware
)

# Link libraries
target_link_libraries(Runes PRIVATE
	Qt6::Core
	Qt6::Widgets
	yaml-cpp::yaml-cpp
	fmt::fmt
	hidapi::hidapi
)