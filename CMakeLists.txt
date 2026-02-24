cmake_minimum_required(VERSION 3.16)

# 项目名称
project(JsonWebParser VERSION 0.2 LANGUAGES CXX)

# 使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Debug模式更容易调试
set(CMAKE_BUILD_TYPE Debug)

# 可执行文件名称
set(EXECUTABLE_NAME JsonWebParser)

# ========================
# 输出目录（可执行文件、库文件等）
# ========================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ========================
# include目录
# ========================
set(INCLUDE_DIRS
    src
    src/server 
    src/controller
    src/service
    src/utils
    src/config
    src/middleware
    src/model
    third_party
)

# ========================
# 源文件
# ========================
set(SOURCES
    src/main.cpp
    src/server/HttpServer.cpp
    src/controller/ParseController.cpp
    src/service/JsonService.cpp
    src/utils/FileUtil.cpp
    src/utils/Logger.cpp
    src/config/config.cpp
    src/model/ApiResponse.cpp
    src/middleware/LoggerMiddleware.cpp
    src/middleware/ErrorMiddleware.cpp
)

# ========================
# 创建可执行文件（必须在 link 之前）
# ========================
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# ========================
# include 绑定到 target（现代CMake推荐）
# ========================
target_include_directories(${EXECUTABLE_NAME}
    PRIVATE
    ${INCLUDE_DIRS}
)

# ========================
# 链接 pthread（必须在 add_executable 之后）
# ========================
target_link_libraries(${EXECUTABLE_NAME}
    PRIVATE
    pthread
)