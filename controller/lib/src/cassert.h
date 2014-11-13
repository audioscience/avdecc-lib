
#ifndef _CASSERT_H
#define _CASSERT_H

#ifndef compile_time_assert
#define compile_time_assert(cond, msg) \
    typedef char msg[(cond) ? 1 : -1]
#endif

/* The following macro is often used in conjunction with compile_time_assert().
 * While a better location is found for its definitions, we will leave it here.
 */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#endif /* _CASSERT_H */
