PROJECT(logcat-mini)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

# Compiler and linker flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -ffunction-sections -fdata-sections -fomit-frame-pointer -flto")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -flto")

AUX_SOURCE_DIRECTORY(. SRC_LIST)
ADD_EXECUTABLE(logcat ${SRC_LIST})

# Strip the binary
add_custom_command(TARGET logcat POST_BUILD
                   COMMAND ${CMAKE_STRIP} $<TARGET_FILE:logcat>)

install(TARGETS logcat
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
