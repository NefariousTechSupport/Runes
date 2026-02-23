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
