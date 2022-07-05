#pragma once

#ifndef C_FS_THREAD_LOCK
#define C_FS_THREAD_LOCK

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <sys/errno.h>

// ==== MUTEX ====

typedef pthread_mutex_t __cfs_lock_t;

#define __cfs_lock_init(lock, type) ({ \
    int result = 0; \
    pthread_mutexattr_t attr; \
    if ((result = pthread_mutexattr_init(&attr)) == 0) { \
        if ((result = pthread_mutexattr_settype(&attr, type)) == 0) { \
            if ((result = pthread_mutex_init(lock, &attr)) == 0) { \
                result = pthread_mutexattr_destroy(&attr) == EINVAL ? EINVAL : 0; \
            } \
        } \
    } \
    result; \
})

#define __cfs_lock_lock(lock) pthread_mutex_lock(lock)
#define __cfs_lock_unlock(lock) pthread_mutex_unlock(lock)

#define __cfs_lock_lock_handled(lock) ({ \
    int _lock_result = 0; \
    if (__cfs_lock_lock(lock) == EINVAL) { \
        set_alloc_errno_msg(MUTEX_LOCK_LOCK, strerror(EINVAL)); \
        _lock_result = -1; \
    } \
    _lock_result; \
})

#define __cfs_lock_unlock_handled(lock) ({ \
    int _unlock_result = 0; \
    if ((_unlock_result = __cfs_lock_unlock(lock)) != 0) { \
        set_alloc_errno_msg(MUTEX_LOCK_UNLOCK, strerror(_unlock_result)); \
        _unlock_result = -1; \
    } \
    _unlock_result; \
})

// ==== RWLOCK ====

typedef pthread_rwlock_t __cfs_rwlock_t;

#define __cfs_rwlock_init(lock, pshared) ({ \
    int result = 0; \
    pthread_rwlockattr_t attr; \
    if ((result = pthread_rwlockattr_init(&attr)) == 0) { \
        if ((result = pthread_rwlockattr_setpshared(&attr, pshared)) == 0) { \
            if ((result = pthread_rwlock_init(lock, &attr)) == 0) { \
                result = pthread_rwlockattr_destroy(&attr) == EINVAL ? EINVAL : 0; \
            } \
        } \
    } \
    result; \
})

#define __cfs_rwlock_rdlock(lock) pthread_rwlock_rdlock(lock)
#define __cfs_rwlock_wrlock(lock) pthread_rwlock_wrlock(lock)
#define __cfs_rwlock_unlock(lock) pthread_rwlock_unlock(lock)

#define __cfs_rwlock_rdlock_handled(lock) ({ \
    int _lock_result = 0; \
    if (__cfs_rwlock_rdlock(lock) == EINVAL) { \
        set_alloc_errno_msg(RWLOCK_WRLOCK_LOCK, strerror(EINVAL)); \
        _lock_result = -1; \
    } \
    _lock_result; \
})

#define __cfs_rwlock_wrlock_handled(lock) ({ \
    int _lock_result = 0; \
    if (__cfs_rwlock_wrlock(lock) == EINVAL) { \
        set_alloc_errno_msg(RWLOCK_RDLOCK_LOCK, strerror(EINVAL)); \
        _lock_result = -1; \
    } \
    _lock_result; \
})

#define __cfs_rwlock_unlock_handled(lock) ({ \
    int _unlock_result = 0; \
    if ((_unlock_result = __cfs_rwlock_unlock(lock)) != 0) { \
        set_alloc_errno_msg(RWLOCK_LOCK_UNLOCK, strerror(_unlock_result)); \
        _unlock_result = -1; \
    } \
    _unlock_result; \
})

#ifdef __cplusplus
};
#endif

#endif //C_FS_THREAD_LOCK