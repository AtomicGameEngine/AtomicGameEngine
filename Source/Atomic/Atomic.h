

#ifndef ATOMIC_API_H
#define ATOMIC_API_H

#ifdef _MSC_VER

#pragma warning(disable: 4251)
#pragma warning(disable: 4275)

#ifdef ATOMIC_STATIC_DEFINE
#  define ATOMIC_API
#  define ATOMIC_NO_EXPORT
#else
#  ifndef ATOMIC_API
#    ifdef ATOMIC_EXPORTS
        /* We are building this library */
#      define ATOMIC_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define ATOMIC_API __declspec(dllimport)
#    endif
#  endif

#  ifndef ATOMIC_NO_EXPORT
#    define ATOMIC_NO_EXPORT 
#  endif
#endif

#ifndef ATOMIC_DEPRECATED
#  define ATOMIC_DEPRECATED __declspec(deprecated)
#endif

#ifndef ATOMIC_DEPRECATED_EXPORT
#  define ATOMIC_DEPRECATED_EXPORT ATOMIC_API ATOMIC_DEPRECATED
#endif

#ifndef ATOMIC_DEPRECATED_NO_EXPORT
#  define ATOMIC_DEPRECATED_NO_EXPORT ATOMIC_NO_EXPORT ATOMIC_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define ATOMIC_NO_DEPRECATED
#endif

#else

#ifdef ATOMIC_STATIC_DEFINE
#ifndef ATOMIC_API
#  define ATOMIC_API
#endif
#  define ATOMIC_NO_EXPORT
#else
#  ifndef ATOMIC_API
#    ifdef ATOMIC_EXPORTS
        /* We are building this library */
#      define ATOMIC_API __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define ATOMIC_API __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef ATOMIC_NO_EXPORT
#    define ATOMIC_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef ATOMIC_DEPRECATED
#  define ATOMIC_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef ATOMIC_DEPRECATED_EXPORT
#  define ATOMIC_DEPRECATED_EXPORT ATOMIC_API ATOMIC_DEPRECATED
#endif

#ifndef ATOMIC_DEPRECATED_NO_EXPORT
#  define ATOMIC_DEPRECATED_NO_EXPORT ATOMIC_NO_EXPORT ATOMIC_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define ATOMIC_NO_DEPRECATED
#endif

#endif

#endif

