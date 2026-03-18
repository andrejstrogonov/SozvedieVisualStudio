QT += quick widgets sql

CONFIG += c++11

SOURCES += \
        main.cpp \
        database.cpp \
        objecttreemodel.cpp \
        specmodel.cpp

HEADERS += \
        database.h \
        objecttreemodel.h \
        specmodel.h

RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target