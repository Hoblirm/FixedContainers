#ifndef FLEX_CONFIG_H
#define FLEX_CONFIG_H

#ifndef FLEX_LIKELY
	#if defined(__GNUC__) && (__GNUC__ >= 3)
		#define FLEX_LIKELY(x)   __builtin_expect(!!(x), true)
		#define FLEX_UNLIKELY(x) __builtin_expect(!!(x), false) 
	#else
		#define FLEX_LIKELY(x)   (x)
		#define FLEX_UNLIKELY(x) (x)
	#endif
#endif

#endif //FLEX_CONFIG_H