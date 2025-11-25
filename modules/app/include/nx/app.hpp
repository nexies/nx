//
// Created by nexie on 25.11.2025.
//

#ifndef NX_APP_HPP
#define NX_APP_HPP

#include "nx/core/Result.hpp"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#define nxTrace(...)    SPDLOG_LOGGER_TRACE(::spdlog::default_logger(), __VA_ARGS__)
#define nxDebug(...)    SPDLOG_LOGGER_DEBUG(::spdlog::default_logger(), __VA_ARGS__)
#define nxInfo(...)     SPDLOG_LOGGER_INFO(::spdlog::default_logger(), __VA_ARGS__)
#define nxWarning(...)  SPDLOG_LOGGER_WARN(::spdlog::default_logger(), __VA_ARGS__)
#define nxError(...)    SPDLOG_LOGGER_ERROR(::spdlog::default_logger(), __VA_ARGS__)
#define nxCritical(...) SPDLOG_LOGGER_CRITICAL(::spdlog::default_logger(), __VA_ARGS__)

#include <chrono>

namespace nx {
    using Clock         = std::chrono::high_resolution_clock;
    using Duration      = Clock::duration;
    using TimePoint     = Clock::time_point;

    // using Days    = std::chrono::days;
    using Hours         = std::chrono::hours;
    using Minutes       = std::chrono::minutes;
    using Seconds       = std::chrono::seconds;
    using Milliseconds  = std::chrono::milliseconds;
    using Microseconds  = std::chrono::microseconds;
    using Nanoseconds   = std::chrono::nanoseconds;

    using ThreadId          = size_t;
    using NativeThreadId    = std::thread::id;
    using TimerId           = size_t;

    std::string version ();
    std::string build_time_utc ();

/**
 *      // устарело, но идеи разумные
 *
 *      nx::Object - объект, зарегистрированный в приложении NX
 *          - Знает, в каком nx::Loop он находится (nx::Thread::current)
 *          - Умеет реагировать на события, делегированные ему из nx::Loop
 *          - Умеет создавать новые события, помещая их в nx::Loop
 *          - Умеет создавать и хранить соединения своих сигналов с другими nx::Object и с внешними классами/функциями
 *          - Умеет создавать события типа nx::SignalEvent, помещая их в nx::Loop соответствующий другому объекту nx::Object
 *          - Система дерева наследования?...
 *          - Обязан оставаться в том потоке, в котором был создан. Перемещаться между потоками только с помощью nx::moveToThread
 *
 *      nx::Thread - объект, глобальный в рамках одного потока.
 *          - Хранит в себе информацию о текущем потоке
 *          - Хранит очередь событий текущего потока
 *          - Позволяет запускать новые потоки, зарегистрированные в библиотеке NX
 *          - Позволяет переносить в себя и из себя объекты nx::Object с помощью nx::moveToThread
 *          - наследник nx::Object ! Знает, в каком nx::Thread был создан - соответственно строится дерево зависимостей nx::Thread
 *
 *      nx::Event
 *          - Quit - выход из текущего цикла ивентов
 *          - Exit - рекурсивно выйти из всех циклов ивентов в данном потоке
 *
 *      nx::Loop   - Объект, представляющий собой обработчик цикла ивентов, занимающий собой выделенный ему поток
 *          - Знает, в каком потоке был создан и вызван (nx::Thread::current)
 *          - При отсутствии событий в очереди, ждет поступления новых событий
 *          - Умеет обрабатывать событие exit - выходит из цикла
 *
 *      nx::App    - Объект-обертка вокруг nx::Loop, занимающий поток, в котором создан. Является основным циклом ивентов библиотеки NX.
 *          - Ловит системные сигналы
 *          - Отслеживает создание и состояния остальных циклов ивентов
 *          - Не может существовать в двух экземплярах (Singleton)
 *
 *
 *
 *
 *
 **/

}

#endif //NX_APP_HPP
