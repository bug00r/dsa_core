#ifndef DSA_PORTAL_DEFS_H
#define DSA_PORTAL_DEFS_H


#ifndef EXTERN_BLOB
	#define EXTERN_BLOB(name,suffix) \
		extern unsigned char _binary_ ## name ## _ ## suffix ## _start; \
		extern unsigned char _binary_ ## name ## _ ## suffix ## _size
#endif

#define UNUSED(x) (void)(x)

#endif