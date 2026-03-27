include_guard(GLOBAL)

set (__NX_LOG_SCOPE_CURRENT "")
set (__NX_LOG_SCOPE_STACK "")

function(nx_push_log_scope SCOPE)
    if(DEFINED __NX_LOG_SCOPE_CURRENT)
        list(APPEND __NX_LOG_SCOPE_STACK "${__NX_LOG_SCOPE_CURRENT}")
    else()
        list(APPEND __NX_LOG_SCOPE_STACK "")
    endif()

    set(__NX_LOG_SCOPE_CURRENT "${SCOPE}" PARENT_SCOPE)
    set(__NX_LOG_SCOPE_STACK "${__NX_LOG_SCOPE_STACK}" PARENT_SCOPE)
endfunction()

function(nx_pop_log_scope)
    list(LENGTH __NX_LOG_SCOPE_STACK _len)
    if(_len GREATER 0)
        math(EXPR _last "${_len} - 1")
        list(GET __NX_LOG_SCOPE_STACK ${_last} _prev_scope)
        list(REMOVE_AT __NX_LOG_SCOPE_STACK ${_last})

        set(__NX_LOG_SCOPE_CURRENT "${_prev_scope}" PARENT_SCOPE)
        set(__NX_LOG_SCOPE_STACK "${__NX_LOG_SCOPE_STACK}" PARENT_SCOPE)
    else()
        set(__NX_LOG_SCOPE_CURRENT "" PARENT_SCOPE)
    endif()
endfunction()

function(nx_log)
    set(_levels STATUS WARNING AUTHOR_WARNING SEND_ERROR FATAL_ERROR DEPRECATION NOTICE VERBOSE DEBUG TRACE)

    # Первый аргумент — возможно уровень
    list(GET ARGV 0 _first)

    # Проверяем, это уровень или нет
    list(FIND _levels "${_first}" _idx)

    if(_idx GREATER -1)
        # Есть уровень
        set(_level "${_first}")
        list(REMOVE_AT ARGV 0)
    else()
        # По умолчанию
        set(_level "")
    endif()

    list(LENGTH __NX_LOG_SCOPE_STACK _depth)
    string(REPEAT "  " "${_depth}" _indent)

    # Формируем сообщение
    if(DEFINED __NX_LOG_SCOPE_CURRENT)
        message(${_level} "${_indent}[${__NX_LOG_SCOPE_CURRENT}] -- ${ARGV}")
    else()
        message(${_level} "${ARGV}")
    endif()
endfunction()