/* cltu_debug.h */

#ifndef __CLTU_DEBUG_H__
#define __CLTU_DEBUG_H__


#define PRINT_HLI_CONFIG    0
#define PRINT_LLI_CONFIG    0


#define T(fmt, args...) printk(KERN_DEBUG "cltu_core:" fmt, ## args)
#define E(fmt, args...) printk(KERN_NOTICE "cltu_core:" fmt, ## args)


#define FN __FUNCTION__
#define KE KERN_ERR
#define KI KERN_INFO
#define KW KERN_WARNING
/*#define KD KERN_DEBUG*/
#define KD KI

#ifndef LOCAL_DEBUG
#define LOCAL_DEBUG 0
#endif

#define DBG_FIRED(lvl)  (cltu_debug>=(lvl) || LOCAL_DEBUG>=(lvl))

#define dbg(lvl,format, arg...)						\
	do {								\
		if( DBG_FIRED(lvl) && printk_ratelimit())		\
			printk (KD "%s: " format "\n", FN , ## arg);	\
	} while(0)

#define err(format, arg...) printk(KE "ERROR:%s: " format "\n", FN , ## arg)
#define info(format, arg...) printk(KI "%s: " format "\n", FN , ## arg)
#define warn(format, arg...) printk(KW "%s: " format "\n", FN , ## arg)


#ifdef CONFIG_KERNEL_ASSERTS
/* kgdb stuff */
#define assert(p) KERNEL_ASSERT(#p, p)
#else
#define assert(p) do {	\
	if (!(p)) {	\
		printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",	\
		       __FILE__, __LINE__, #p);			\
		BUG();	\
	}		\
} while (0)
#endif


#define MARK printk(KERN_INFO "%s:%d %s MARK\n", \
             __FILE__, __LINE__, __FUNCTION__ )

#define DMARK dbg(1, "%s:%d %s DMARK\n", __FILE__, __LINE__, __FUNCTION__ )

#endif /* __CLTU_DEBUG_H__ */

