INCLUDEPATH += $$PWD/Source

SOURCES += \
    $$PWD/Source/main.cpp \
    $$PWD/Source/Global.cpp \
    \
    $$PWD/Source/Core/Debugger.cpp \
    $$PWD/Source/Core/ConfigManager.cpp \
    $$PWD/Source/Core/ThreadId.cpp \
    $$PWD/Source/Core/IpcProcess.cpp \
    \
    $$PWD/Source/Server/Server.cpp \
    $$PWD/Source/Server/RequestMapper.cpp \
    \
    $$PWD/Source/Bot/BotManager.cpp \
    $$PWD/Source/Bot/DiscordEventHandler.cpp

HEADERS += \
    $$PWD/Source/Global.hpp \
    \
    $$PWD/Source/Core/Debugger.hpp \
    $$PWD/Source/Core/ConfigManager.hpp \
    $$PWD/Source/Core/ThreadId.hpp \
    $$PWD/Source/Core/IpcProcess.hpp \
    \
    $$PWD/Source/Server/Server.hpp \
    $$PWD/Source/Server/RequestMapper.hpp \
    \
    $$PWD/Source/Bot/BotManager.hpp \
    $$PWD/Source/Bot/DiscordEventHandler.hpp
