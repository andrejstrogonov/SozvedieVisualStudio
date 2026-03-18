cmake_minimum_required(VERSION 3.16)

project(SozvedieVisualStudio)

# Устанавливаем стандарт C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем автоматическую обработку Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Ищем Qt 6.10.2
set(Qt6_DIR "C:/Qt/6.10.2/msvc2022_64/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick Sql Widgets Gui)

# Перечисляем исходники проекта (только .cpp файлы)
add_executable(SozvedieVisualStudio
    SozvedieVisualStudio.cpp
    mainwindow.cpp
    numericdelegate.cpp
    specmodel.cpp
    treemodel.cpp
    database.cpp
)

# Линкуем Qt компоненты
target_link_libraries(SozvedieVisualStudio
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::Sql
    Qt6::Widgets
    Qt6::Gui
)

