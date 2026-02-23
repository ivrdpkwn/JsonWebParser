cmake_minimum_required(VERSION 3.16)

# 项目名称
project(JsonWebParser)

# 使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug模式更容易调试
set(CMAKE_BUILD_TYPE Debug)

# 可执行文件名称
set(EXECUTABLE_NAME JsonWebParser)

# include目录
include_directories(
    src
    src/server 
    src/controller
    src/service
    src/utils
    third_party
)

# 收集所有源文件
set(SOURCES

    src/main.cpp

    src/server/HttpServer.cpp

    src/controller/ParseController.cpp

    src/service/JsonService.cpp

    src/utils/FileUtil.cpp

)

# 创建可执行文件
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Linux需要 pthread
target_link_libraries(${EXECUTABLE_NAME}
    pthread
)

# 让程序运行目录在项目根目录
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
