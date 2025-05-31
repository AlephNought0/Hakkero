
#ifndef HAKKERO_EXPORT_H
#define HAKKERO_EXPORT_H

#ifdef HAKKERO_STATIC_DEFINE
#  define HAKKERO_EXPORT
#  define HAKKERO_NO_EXPORT
#else
#  ifndef HAKKERO_EXPORT
#    ifdef Hakkero_EXPORTS
        /* We are building this library */
#      define HAKKERO_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define HAKKERO_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef HAKKERO_NO_EXPORT
#    define HAKKERO_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef HAKKERO_DEPRECATED
#  define HAKKERO_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef HAKKERO_DEPRECATED_EXPORT
#  define HAKKERO_DEPRECATED_EXPORT HAKKERO_EXPORT HAKKERO_DEPRECATED
#endif

#ifndef HAKKERO_DEPRECATED_NO_EXPORT
#  define HAKKERO_DEPRECATED_NO_EXPORT HAKKERO_NO_EXPORT HAKKERO_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef HAKKERO_NO_DEPRECATED
#    define HAKKERO_NO_DEPRECATED
#  endif
#endif

#endif /* HAKKERO_EXPORT_H */
