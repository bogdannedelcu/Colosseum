// GlibcCompatShim.cpp — link-time compatibility shim for the UE bundled toolchain.
//
// AirLib (ColosseumLib.a) is built with the host clang-18 / libc++ against the host glibc
// (Ubuntu 26.04, glibc >= 2.30), whose std::condition_variable::wait_for lowers to
// pthread_cond_clockwait (added in glibc 2.30). But Unreal links the final plugin .so with
// its own bundled toolchain whose sysroot is Rocky Linux 8 (glibc 2.28) — which does NOT
// export pthread_cond_clockwait, so the link fails with "undefined symbol".
//
// This unit *defines* the symbol so the static link resolves against the old sysroot, while
// at runtime it forwards to the real glibc implementation (present on the host, glibc 2.43)
// via dlsym(RTLD_NEXT). If the running glibc happens to lack it, we fall back to
// pthread_cond_timedwait (which honours the abstime; only the clock id is dropped).
//
// Keep this Linux-only and out of the way on every other platform.
#if defined(__linux__)

#include <pthread.h>
#include <dlfcn.h>
#include <time.h>

extern "C" int pthread_cond_clockwait(pthread_cond_t* cond,
                                      pthread_mutex_t* mutex,
                                      clockid_t clock_id,
                                      const struct timespec* abstime)
{
    using clockwait_fn = int (*)(pthread_cond_t*, pthread_mutex_t*, clockid_t, const struct timespec*);
    static clockwait_fn real = reinterpret_cast<clockwait_fn>(dlsym(RTLD_NEXT, "pthread_cond_clockwait"));
    if (real)
        return real(cond, mutex, clock_id, abstime);
    // Fallback for a runtime glibc that lacks clockwait: honour the deadline, drop the clock id.
    return pthread_cond_timedwait(cond, mutex, abstime);
}

#endif // __linux__
