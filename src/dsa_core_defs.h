#ifndef DSA_PORTAL_DEFS_H
#define DSA_PORTAL_DEFS_H

#ifndef EXTERN_BLOB
	#define EXTERN_BLOB(name,suffix) \
		extern unsigned char _binary_ ## name ## _ ## suffix ## _start; \
		extern unsigned char _binary_ ## name ## _ ## suffix ## _size
#endif

#ifndef UNUSED
	#define UNUSED(x) (void)(x)
#endif

#ifndef DEBUG_LOG_ARGS
	#if debug != 0
		#define DEBUG_LOG_ARGS(fmt, ...) printf(" %s:%s:%i => ", __FILE__, __func__, __LINE__);printf((fmt), __VA_ARGS__)
	#else
		#define DEBUG_LOG_ARGS(fmt, ...)
	#endif
#endif

#ifndef DEBUG_LOG
	#if debug != 0
		#define DEBUG_LOG(msg) printf(" %s:%s:%i => ", __FILE__, __func__, __LINE__);printf((msg))
	#else
		#define DEBUG_LOG(msg)
	#endif
#endif

#endif