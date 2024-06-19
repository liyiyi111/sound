cmake_minimum_required(VERSION 3.5)

project(DigConfe VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TRANS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/translate")

set(TS_FILES
    ${TRANS_DIR}/DigConfe_zh_CN.ts
    ${TRANS_DIR}/DigConfe_en.ts
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Svg WebEngineWidgets Xml PdfWidgets
                                Network
                                LinguistTools )


file(GLOB_RECURSE HEADERS "*.h")

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE FORMS	  "*.ui")

find_package(fmt CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)


add_executable(DigConfe
    ${HEADERS}
    ${SOURCES}
    ${FORMS}
    pic/image.qrc
)

target_link_libraries(DigConfe
    PRIVATE
    Qt::WebEngineWidgets
    Qt::Xml
    Qt::PdfWidgets
    Qt::Network
    fmt::fmt
    range-v3::range-v3
    asio::asio
)

qt_add_lupdate(DigConfe TS_FILES ${TS_FILES})

qt_add_lrelease(DigConfe
    TS_FILES ${TS_FILES}
    QM_FILES_OUTPUT_VARIABLE qm_files)

qt_add_resources(DigConfe
    PREFIX "/"
    FILES "en_UN.qm"
          "zh_CN.qm"
           "style.css"
)

#TODO 增加自动cmd指令执行
# find_program(LUPDATE_EXECUTABLE lupdate)
# find_program(LRELEASE_EXECUTABLE  lrelease)

# foreach(_ts_file ${TS_FILES})
#         execute_process(
#                 COMMAND ${LUPDATE_EXECUTABLE} -recursive ${CMAKE_SOURCE_DIR} -ts ${_ts_file}
#                 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#         )
#         execute_process(
#                 COMMAND ${LRELEASE_EXECUTABLE} ${_ts_file}
#                 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#         )
# endforeach()


