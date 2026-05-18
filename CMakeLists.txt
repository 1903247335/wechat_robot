cmake_minimum_required(VERSION 3.16)

project(wx VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg：不要把 -DCMAKE_TOOLCHAIN_FILE=... 写在本文件里。
# 应在 Qt Creator → 项目 → CMake →「Initial CMake parameters」里添加，例如：
#   -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake
#   -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic
#   -DWX_WITH_PROTOBUF=ON

# 默认关闭：无 Protobuf 时仍可配置工程。
option(WX_WITH_PROTOBUF "生成并链接 proto/message.proto（需本机已安装与 MinGW 匹配的 protobuf）" ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)

set(PROTO_SRCS "")
if(WX_WITH_PROTOBUF)
    find_package(Protobuf REQUIRED)
    protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/proto/message.proto")
    message(STATUS "WX_WITH_PROTOBUF: 已启用；生成 .cc/.h 多半在「首次构建」后才会出现在磁盘上。")
    message(STATUS "  PROTO_SRCS = ${PROTO_SRCS}")
    message(STATUS "  PROTO_HDRS = ${PROTO_HDRS}")





endif()

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appwx
    main.cpp
    ${PROTO_SRCS}
)

add_dependencies(appwx copy_proto_files)#执行copy_proto_files任务

qt_add_qml_module(appwx
    URI wx
    QML_FILES
        Main.qml
        component/MyButton.qml
        component/MyPopup.qml
    SOURCES server/cppserver.h server/cppserver.cpp
    SOURCES modules/searcher.h modules/searcher.cpp
    SOURCES modules/injector.h modules/injector.cpp
    SOURCES modules/starter.h modules/starter.cpp
    SOURCES global.h
    SOURCES tools/tools.h tools/tools.cpp
    SOURCES logging.h
    SOURCES logging.cpp
)

if(WX_WITH_PROTOBUF)
    #给我复制proto的 不用管
    set(DEST_DIR "E:/c/wxdll32/wxdll32")
    file(TO_NATIVE_PATH "E:/c/wxdll32/wxdll32" NATIVE_DEST)

    add_custom_target(copy_proto_files ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${NATIVE_DEST}"
        # 先删除旧文件，防止被占用导致的 copy_if_different 失败
        COMMAND ${CMAKE_COMMAND} -E remove -f "${NATIVE_DEST}/message.pb.cc"
        COMMAND ${CMAKE_COMMAND} -E remove -f "${NATIVE_DEST}/message.pb.h"
        # 执行复制
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/message.pb.cc" "${NATIVE_DEST}/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/message.pb.h" "${NATIVE_DEST}/"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/message.pb.cc"
        COMMENT "Forcing sync of proto files..."
    )


    target_compile_definitions(appwx PRIVATE WX_WITH_PROTOBUF=1)
    target_include_directories(appwx PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/proto
    )
    if(TARGET protobuf::libprotobuf)
        target_link_libraries(appwx PRIVATE protobuf::libprotobuf)
    elseif(TARGET Protobuf::libprotobuf)
        target_link_libraries(appwx PRIVATE Protobuf::libprotobuf)
    else()
        target_link_libraries(appwx PRIVATE ${Protobuf_LIBRARIES})
        target_include_directories(appwx PRIVATE ${Protobuf_INCLUDE_DIRS})
    endif()
endif()

set_target_properties(appwx PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appwx
    PRIVATE Qt6::Quick Qt6::QuickControls2
)

# libabseil_dll.dll needs matching libwinpthread-1.dll. copy_if_different fails if source missing;
# use a small -P script with fallbacks (debug/bin vs bin).


include(GNUInstallDirs)
install(TARGETS appwx
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)



