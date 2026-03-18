cmake_minimum_required(VERSION 3.16)

project(SozvedieVisualStudio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем автоматическую обработку Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ищем Qt
set(Qt6_DIR "C:/Qt/6.10.2/msvc2022_64/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick Sql Gui)

# Создаем исполняемый файл
add_executable(SozvedieVisualStudio
    SozvedieVisualStudio.cpp
    SozvedieVisualStudio.h
    database.h
    database.cpp
    numericdelegate.h
    numericdelegate.cpp
    mainwindow.h
    mainwindow.cpp
    
)

# Линкуем Qt
target_link_libraries(SozvedieVisualStudio
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::Sql
    Qt6::Gui
)
