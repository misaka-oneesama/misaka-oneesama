SOURCES += \
    $$PWD/Source/main.cpp \
    $$PWD/Source/Global.cpp \
    \
    $$PWD/Source/Core/ConfigManager.cpp \
    $$PWD/Source/Core/ThreadId.cpp \
    $$PWD/Source/Core/IpcProcess.cpp \
    $$PWD/Source/Core/Debugger.cpp \
    $$PWD/Source/Core/BotManager.cpp \
    $$PWD/Source/Core/DiscordEventHandler.cpp \
    \
    $$PWD/Source/Server/Server.cpp \
    $$PWD/Source/Server/RequestMapper.cpp

HEADERS += \
    $$PWD/Source/Global.hpp \
    \
    $$PWD/Source/Core/ConfigManager.hpp \
    $$PWD/Source/Core/ThreadId.hpp \
    $$PWD/Source/Core/IpcProcess.hpp \
    $$PWD/Source/Core/Debugger.hpp \
    $$PWD/Source/Core/BotManager.hpp \
    $$PWD/Source/Core/DiscordEventHandler.hpp \
    \
    $$PWD/Source/Server/Server.hpp \
    $$PWD/Source/Server/RequestMapper.hpp
