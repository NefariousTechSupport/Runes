######################################################################
# Automatically generated by qmake (3.1) Wed Oct 4 21:46:10 2023
######################################################################

TEMPLATE = app
TARGET = Runes
INCLUDEPATH += ./include
QT += widgets gui

CONFIG(debug, debug|release) {
    DEFINES += DEBUG
    CONFIG += console
    copyfiles.commands = $$quote(xcopy ".\Resources" ".\release\Resources" /y /s /i)
}
CONFIG(debug, debug|release) {
    copyfiles.commands = $$quote(xcopy ".\Resources" ".\release\Resources" /y /s /i)
}


QMAKE_EXTRA_TARGETS += copyfiles echotest
POST_TARGETDEPS += copyfiles echotest

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += include/Constants.hpp \
           include/debug.hpp \
           include/ESkylandersGame.hpp \
           include/kTfbSpyroTag_DecoID.hpp \
           include/kTfbSpyroTag_HatType.hpp \
           include/kTfbSpyroTag_ToyType.hpp \
           include/kTfbSpyroTag_VillainType.hpp \
           include/PortalAlgos.hpp \
           include/PortalTag.hpp \
           include/RfidTag.hpp \
           include/toydata.hpp \
           include/3rd_party/crc.h \
           include/3rd_party/md5.h \
           include/3rd_party/rijndael.h \
           include/PortalAlgos.hpp \
           include/3rd_party/md5.h \
           include/3rd_party/rijndael.h \
           include/PortalTag.hpp \
           include/toydata.hpp \
           include/Constants.hpp \
           include/3rd_party/crc.h \
           include/RfidTag.hpp \
           include/RunesWidget.hpp
SOURCES += source/debug.cpp \
           source/kTfbSpyroTag_HatType.cpp \
           source/main.cpp \
           source/PortalAlgos.cpp \
           source/PortalTag.cpp \
           source/RfidTag.cpp \
           source/toydata.cpp \
           source/3rd_party/crc.cpp \
           source/3rd_party/md5.cpp \
           source/3rd_party/rijndael.cpp \
           source/RunesWidget.cpp
