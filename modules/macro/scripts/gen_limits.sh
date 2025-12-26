#!/usr/bin/env bash

LIMIT=1024
OUT_DIR=include/nx/macro/detail/generated

format_nx_while()
{
  local i="$1"
  local n=$((i+1))

  printf ' # define _nx_while_%d(c, o, r, ...)  _nx_logic_if(c(%d, __VA_ARGS__))(_nx_while_%d(c, o, r, o(%d, __VA_ARGS__)), r(%d, __VA_ARGS__))\n' \
          "$i" "$n" "$n" "$n" "$n"
}

format_nx_inc()
{
  local i="$1"
  local n=$((i+1))

  if [[ $i -lt 0 ]]; then
    return
  fi

  printf " # define _nx_numeric_inc_%d %d\n" \
    "$i" "$n"
}

format_nx_dec()
{
  local i="$1"
  local n=$((i-1))

  if [[ $n -lt 0 ]]; then
    return
  fi

  printf " # define _nx_numeric_dec_%d %d\n" \
    "$i" "$n"
}

format_nx_bool()
{
  local i="$1"
  local n=0

  if [[ $i -lt 0 ]]; then
    return
  elif [[ $i -eq 0 ]]; then
    n=0
  elif [[ $i -gt 0 ]]; then
    n=1
  else
    return
  fi

  printf " # define _nx_logic_bool_%d %d\n" \
    "$i" "$n"
}

format_nx_is_max()
{
  local i="$1"
  local n=0

  if [[ $i -ge $LIMIT ]]; then
    n=1
  fi

  printf " # define _nx_limits_is_max_%d %d\n" \
    "$i" "$n"
}

format_nx_arg_tk()
{
  local i="$1"

  printf ' # define _nx_arg_tk_%d _nx_arg_tk_%d_(\n' "$i" "$i"
  printf ' # define _nx_arg_tk_%d_(...) (%d, _nx_append_args(__VA_ARGS__))\n' "$i" "$i"
}

format_nx_sequence()
{
  local i="$1"
  local n=$((i-1))

  if [[ $i -eq 0 ]]; then
    printf ' # define _nx_sequence_0(...)\n'
  else
    printf ' # define _nx_sequence_%d(c, h, m, ...) _nx_sequence_%d(0, h, m, __VA_ARGS__)  h(c, _nx_apply(m, %d, __VA_ARGS__))\n' \
      "$i" "$n" "$i"
  fi
}

generate_with_format()
{
  local format="$1"
  local min="$2"
  local max="$3"

  for((i=min; i<=max; ++i)); do
    $format "$i"
  done
}

generate_main_ifndef()
{
  local name="$1"
  local upper_name="${name^^}"
  local limit="$2"

  {
    printf '#ifndef NX_LIMITS_%s_%d_HPP\n' "$upper_name" "$limit"
    printf '#define NX_LIMITS_%s_%d_HPP\n' "$upper_name" "$limit"
    printf '\n'
  }
}

generate_private_include()
{
  local name="$1"

  printf '// This file is auto-generated\n'
  printf '// You can set NX_MODULE_MACRO_LIMITS_MAX_NUMBER variable to your desired value in CMakeFile\n\n'

  printf '\n'
  printf '#ifndef NX_MACRO_PRIVATE_INCLUDE\n'
  printf '    #error "This file was not meant to be included directly. Include <nx/macro/detail/%s_limit.hpp> instead"\n' "$name"
  printf '#endif\n'
  printf '\n'
}

generate_limit_define()
{
  local name="$1"
  local upper_name="${name^^}"
  local limit="$2"

  printf '\n'
  printf ' # define NX_LIMITS_%s %d\n' "$upper_name" "$limit"
  printf '\n'

}

generate()
{
  local name="$1"
  local format="format_nx_${name}"
  local upper_name="${name^^}"

  local limit="$2"
  local filename="${OUT_DIR}/${name}_${limit}.hpp"

  : > "$filename"

  {
    generate_main_ifndef "$name" "$limit"
    generate_private_include "$name"
    generate_with_format "$format" 0 "$limit"
    generate_limit_define "$name" "$limit"

    printf '#endif' # main ifndef
  } >> "$filename"
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

generate "while" "$LIMIT"
generate "bool" "$LIMIT"
generate "inc" "$LIMIT"
generate "dec" "$LIMIT"
generate "is_max" "$LIMIT"
generate "arg_tk" "$LIMIT"
generate "sequence" "$LIMIT"


