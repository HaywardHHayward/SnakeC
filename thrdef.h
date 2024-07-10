#ifndef SNAKE_THRDEF_H
#define SNAKE_THRDEF_H

#ifdef __has_include

#if __has_include(<pthread.h>)

#include <pthread.h>

typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;

typedef void* thread_ret;

#define thread_create(thr, fun, arg) pthread_create(thr, NULL, fun, arg)
#define thread_join(thr, arg) pthread_join(thr, arg)
#define thread_detach(thr) pthread_detach(thr)

#define mutex_init(mut) pthread_mutex_init(mut, NULL)
#define mutex_lock(mut) pthread_mutex_lock(mut)
#define mutex_unlock(mut) pthread_mutex_unlock(mut)


#elif defined(!__STDC_NO_THREADS__)

#include <threads.h>

typedef thrd_t thread_t;
typedef mtx_t mutex_t;
typedef cnd_t cond_t;

typedef thrd_start_t thread_ret;

#define thread_create(thr, fun, arg) thrd_create(thr, fun, arg)
#define thread_join(thr, arg) thrd_join(thr, arg)
#define thread_detach(thr) thrd_detach(thr)

#define mutex_init(mut) mtx_init(mut, mtx_plain)
#define mutex_lock(mut) mtx_lock(mut)
#define mutex_unlock(mut) mtx_unlock(mut)

#else
#error Must have pthread compatible library or the threads C standard library.
#endif
#else

#ifdef __STDC_NO_THREADS__

#include <pthread.h>

typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;

typedef void* thread_ret;

#define thread_create(thr, fun, arg) pthread_create(thr, NULL, fun, arg)
#define thread_join(thr, arg) pthread_join(thr, arg)
#define thread_detach(thr) pthread_detach(thr)

#define mutex_init(mut) pthread_mutex_init(mut, NULL)
#define mutex_lock(mut) pthread_mutex_lock(mut)
#define mutex_unlock(mut) pthread_mutex_unlock(mut)

#else

#include <threads.h>

typedef thrd_t thread_t;
typedef mtx_t mutex_t;
typedef cnd_t cond_t;

typedef thrd_start_t thread_ret;

#define thread_create(thr, fun, arg) thrd_create(thr, fun, arg)
#define thread_join(thr, arg) thrd_join(thr, arg)
#define thread_detach(thr) thrd_detach(thr)

#define mutex_init(mut) mtx_init(mut, mtx_plain)
#define mutex_lock(mut) mtx_lock(mut)
#define mutex_unlock(mut) mtx_unlock(mut)

#endif
#endif
#endif //SNAKE_THRDEF_H
