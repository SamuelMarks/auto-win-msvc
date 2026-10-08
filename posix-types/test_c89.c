#if defined(_MSC_VER)
typedef unsigned __int64 fsblkcnt_t;
#elif defined(__GNUC__) || defined(__clang__)
__extension__ typedef unsigned long long fsblkcnt_t;
#else
typedef unsigned long fsblkcnt_t;
#endif
