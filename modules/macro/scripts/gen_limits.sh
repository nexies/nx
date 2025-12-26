#!/usr/bin/env bash

LIMIT=1024
OUT_DIR=include/nx/macro/detail/generated

format_nx_while()
{
  local i="$1"
  local n=((i+1))

  printf '#define _nx_while_%d(c, o, r, ...)  _nx_logic_if(c(%d, __VA_ARGS__))(_nx_while_%d(c, o, r, o(%d, __VA_ARGS__)), r(%d, __VA_ARGS__))\n' \
          "$i" "$n" "$n" "$n" "$n"
}

format_nx_inc()
{
  local i="$1"
  local n=((i+1))

  if[[ $i -lt 0]]; then
    return
  fi

  printf "#define _nx_numeric_inc_%d %d\n" \
    "$i" "$n"
}

format_nx_dec()
{
  local i="$1"
  local n=((i-1))

  if[[ $n -lt 0]]; then
    return
  fi

  printf "#define _nx_numeric_dec_%d %d\n" \
    "$i" "$n"
}

format_nx_bool()
{
  local i = "$1"
  local n = 0

  if [[ $i -lt 0]]; then
    return
  elif [[ $i -eq 0]]; then
    n=0
  elif [[ $i -gt 0]]; then
    n=1
  else
    return
  fi

  printf "#define _nx_logic_bool_%d %d\n" \
    "$i" "$n"
}

format_nx_is_max()
{
  local i = "$1"
  local n = 0

  if [[ $i -ge $LIMIT ]]; then
    n = 1
  fi

  printf "#define _nx_limits_is_max_%d %d\n" \
    "$i" "$n"
}


generate_with_format()
{
  local format = "$1"
  local min = "$2"
  local max = "$3"
  local output = "$4"
}

generate_nx_while() {
    local limit="$2"
    local output="$1/while_$limit.hpp"


    : > "$output"

    for((i=0; i<limit; ++i)); do
      local n=$((i+1))
#      printf '#define _nx_while_%d(c, o, r, ...)  _nx_logic_if(c(%d, __VA_ARGS__))(_nx_while_%d(c, o, r, o(%d, __VA_ARGS__)), r(%d, __VA_ARGS__))\n' \
#        "$i" "$n" "$n" "$n" "$n" >> "$output"
      format_nx_while "$i" >> "$output"
    done

    printf '\n\n#define NX_LIMITS_WHILE %d\n' \
      "$i" >> "$output"
}


while [[ $# -gt 0 ]]; do
    case "$1" in
        --limit)
            LIMIT="$2"
            shift 2
            ;;
        --dir)
            OUT_DIR="$2"
            shift 2
            ;;
        *)
            echo "Неизвестный параметр: $1"
            echo "Использование: $0 --limit N [--output file]"
            exit 1
            ;;
    esac
done


generate_nx_while "${OUT_DIR}" "$LIMIT"
