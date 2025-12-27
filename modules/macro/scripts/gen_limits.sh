#!/usr/bin/env bash

LIMIT=1024
OUT_DIR=include/nx/macro/detail/generated

format_nx_while()
{
  local i="$1"
  local n=$((i+1))

#  printf ' # define _nx_while_%d(c, o, r, ...)  _nx_logic_if(c(%d, __VA_ARGS__))(_nx_while_%d(c, o, r, o(%d, __VA_ARGS__)), r(%d, __VA_ARGS__))\n' \
#          "$i" "$n" "$n" "$n" "$n"
#
  printf ' # define _nx_while_%d(c, o, r, ...) _nx_while_%d_c(c(%d, __VA_ARGS__), c, o, r, __VA_ARGS__)\n' \
         "$i" "$i" "$n"
  printf ' # define _nx_while_%d_c(p, c, o, r, ...) _nx_logic_if(p)(_nx_while_%d(c, o, r, o(%d, __VA_ARGS__)), r(%d, __VA_ARGS__))\n' \
         "$i" "$n" "$n" "$n"
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

  printf " # define _nx_numeric_is_max_%d %d\n" \
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
  local c=1

  if(( n <= 1 )); then
    c=0
  fi

  if [[ $i -eq 0 ]]; then
    printf ' # define _nx_sequence_0(...)\n'
  else
    printf ' # define _nx_sequence_%d(c, h, m, ...) _nx_sequence_%d(%d, h, m, __VA_ARGS__)  h(c, _nx_apply(m, %d, __VA_ARGS__))\n' \
      "$i" "$n" "$c" "$i"
  fi
}

format_nx_choose()
{
  local i="$1"
  local n=$((i-1))

  if (( i == 0 )); then
    printf ' # define _nx_choose_0(c, ...) c\n'
    return
  fi

  printf ' # define _nx_choose_%d(c, ...) _nx_choose_%d(__VA_ARGS__) \n' \
    "$i" "$n"
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

generate_powers ()
{
  local name="$1"
  local limit="$2"
  local base=128

  if (( limit <= base )); then
    generate "$name" "$limit"
    return
  fi

  local cur=$base
  while (( cur <= limit )); do
    generate "$name" "$cur"
    cur=$((cur * 2))
  done

  if (( cur / 2 != limit )); then
    generate "$name" "$limit"
  fi
}

is_power_of_two() {
    local n="$1"

    # n > 0 И (n & (n-1)) == 0
    (( n > 0 )) && (( (n & (n-1)) == 0 ))
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

# Проверка, что LIMIT — целое число
if ! [[ "$LIMIT" =~ ^[0-9]+$ ]]; then
    echo "Error: NX_MODULE_MACRO_LIMITS_MAX_NUMBER has to be an integer number, not '$LIMIT'" >&2
    exit 1
fi

# Проверка, что LIMIT — степень двойки
if ! is_power_of_two "$LIMIT"; then
    echo "Error: NX_MODULE_MACRO_LIMITS_MAX_NUMBER should be a power of two (2^n), not $LIMIT" >&2
    exit 1
fi

if (( LIMIT <= 64 )); then
  exit 0
fi

generate_powers "while" "$LIMIT"
generate_powers "bool" "$LIMIT"
generate_powers "inc" "$LIMIT"
generate_powers "dec" "$LIMIT"
generate_powers "is_max" "$LIMIT"
generate_powers "arg_tk" "$LIMIT"
generate_powers "sequence" "$LIMIT"
generate_powers "choose" "$LIMIT"