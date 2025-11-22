//
// Created by nexie on 21.11.2025.
//

#ifndef SIGNAL_DEFS_HPP
#define SIGNAL_DEFS_HPP

///
///  vision:
///
/// 1)
///     declaration:
///         void signalName(arg1, arg2, ...)
///         {   NX_SIGNAL_DEF(arg1, arg2, ...); }
///     usage:
///         signalName(1, 2, 3);
///
///  2)
///     declaration:
///         NX_SIGNAL(signalName, args1, args2, ...);
///     usage:
///         nx::Object::emit(signalName, args1, args2m ...);
///
///  3) would be cool:
///     NX_SIGNAL(signalName, arg_type1, arg_type2, ...);
///
///     signalName(1, 2, 3);
///
///
///
///     NX_SIGNAL(signalName, arg_type1, arg_type2, arg_type3) -->
///
///     void signalName (arg_type1 _arg1, arg_type2 _arg2, arg_type3 _arg3)

// #define __NX_ARGS_COUNT_SEQ \
//     63, 62, 61, 60, \
//     59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
//     49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
//     39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
//     29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
//     19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
//     9, 8, 7, 6, 5, 4, 3, 2, 1, 0



#endif //SIGNAL_DEFS_HPP
