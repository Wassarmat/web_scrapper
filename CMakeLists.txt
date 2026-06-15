cmake_minimum_required(VERSION 3.19)
set(CMAKE_TOOLCHAIN_FILE "/home/abyraeg/tools/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(web_scrapper LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(CURL REQUIRED)

qt_standard_project_setup()

qt_add_executable(web_scrapper
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    core/WebScrapper.h
    core/WebScrapper.cpp
    core/HtmlParser.h
    core/DataSaver.h
    models/ScrapedData.h
    utils/NetworkHelper.h
    utils/Logger.h
    models/ScrapedData.cpp
    core/HtmlParser.cpp
    core/DataSaver.cpp
)

set(SOURCES
    Source_Files/main.cpp
    Source_Files/core/WebScrapper.cpp
    Source_Files/models/ScrapedData.cpp
)
target_include_directories(web_scrapper PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)



target_link_libraries(web_scrapper
    PRIVATE
        Qt::Core
        Qt::Widgets
        CURL::libcurl
)

install(TARGETS web_scrapper
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET web_scrapper
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})