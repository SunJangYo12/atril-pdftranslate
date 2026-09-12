#include <gio/gio.h>

#if defined (__ELF__) && ( __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6))
# define SECTION __attribute__ ((section (".gresource.ev_previewer"), aligned (8)))
#else
# define SECTION
#endif

#ifdef _MSC_VER
static const SECTION union { const guint8 data[645]; const double alignment; void * const ptr;}  ev_previewer_resource_data = { {
  0107, 0126, 0141, 0162, 0151, 0141, 0156, 0164, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 
  0030, 0000, 0000, 0000, 0344, 0000, 0000, 0000, 0000, 0000, 0000, 0050, 0007, 0000, 0000, 0000, 
  0000, 0000, 0000, 0000, 0001, 0000, 0000, 0000, 0002, 0000, 0000, 0000, 0003, 0000, 0000, 0000, 
  0005, 0000, 0000, 0000, 0006, 0000, 0000, 0000, 0007, 0000, 0000, 0000, 0031, 0047, 0334, 0342, 
  0004, 0000, 0000, 0000, 0344, 0000, 0000, 0000, 0015, 0000, 0166, 0000, 0370, 0000, 0000, 0000, 
  0103, 0002, 0000, 0000, 0113, 0120, 0220, 0013, 0002, 0000, 0000, 0000, 0103, 0002, 0000, 0000, 
  0004, 0000, 0114, 0000, 0110, 0002, 0000, 0000, 0114, 0002, 0000, 0000, 0324, 0265, 0002, 0000, 
  0377, 0377, 0377, 0377, 0114, 0002, 0000, 0000, 0001, 0000, 0114, 0000, 0120, 0002, 0000, 0000, 
  0124, 0002, 0000, 0000, 0254, 0375, 0211, 0161, 0005, 0000, 0000, 0000, 0124, 0002, 0000, 0000, 
  0006, 0000, 0114, 0000, 0134, 0002, 0000, 0000, 0140, 0002, 0000, 0000, 0301, 0104, 0303, 0351, 
  0006, 0000, 0000, 0000, 0140, 0002, 0000, 0000, 0003, 0000, 0114, 0000, 0144, 0002, 0000, 0000, 
  0150, 0002, 0000, 0000, 0201, 0321, 0040, 0031, 0001, 0000, 0000, 0000, 0150, 0002, 0000, 0000, 
  0005, 0000, 0114, 0000, 0160, 0002, 0000, 0000, 0164, 0002, 0000, 0000, 0324, 0045, 0343, 0073, 
  0003, 0000, 0000, 0000, 0164, 0002, 0000, 0000, 0012, 0000, 0114, 0000, 0200, 0002, 0000, 0000, 
  0204, 0002, 0000, 0000, 0160, 0162, 0145, 0166, 0151, 0145, 0167, 0145, 0162, 0056, 0170, 0155, 
  0154, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0352, 0004, 0000, 0000, 0001, 0000, 0000, 0000, 
  0170, 0332, 0205, 0224, 0137, 0117, 0203, 0060, 0024, 0305, 0337, 0367, 0051, 0010, 0037, 0300, 
  0351, 0073, 0143, 0061, 0213, 0063, 0306, 0250, 0104, 0215, 0113, 0366, 0126, 0331, 0215, 0066, 
  0051, 0275, 0330, 0226, 0155, 0037, 0337, 0376, 0301, 0001, 0345, 0202, 0217, 0234, 0163, 0176, 
  0207, 0333, 0162, 0103, 0266, 0076, 0127, 0042, 0071, 0202, 0322, 0034, 0345, 0052, 0275, 0271, 
  0272, 0116, 0327, 0371, 0042, 0153, 0170, 0236, 0031, 0104, 0361, 0311, 0124, 0042, 0131, 0005, 
  0253, 0264, 0120, 0160, 0344, 0160, 0172, 0017, 0142, 0032, 0154, 0156, 0240, 0152, 0375, 0173, 
  0364, 0011, 0154, 0164, 0301, 0276, 0040, 0115, 0130, 0151, 0174, 0143, 0244, 0057, 0011, 0360, 
  0031, 0316, 0046, 0206, 0056, 0232, 0005, 0064, 0324, 0114, 0061, 0203, 0152, 0014, 0273, 0310, 
  0033, 0010, 0050, 0255, 0333, 0341, 0003, 0165, 0276, 0340, 0303, 0236, 0151, 0313, 0315, 0216, 
  0037, 0314, 0167, 0127, 0060, 0120, 0047, 0241, 0341, 0314, 0175, 0221, 0106, 0366, 0210, 0325, 
  0203, 0034, 0022, 0255, 0066, 0015, 0274, 0064, 0146, 0114, 0070, 0161, 0032, 0171, 0005, 0015, 
  0004, 0024, 0344, 0177, 0356, 0063, 0174, 0345, 0102, 0161, 0331, 0153, 0030, 0250, 0026, 0132, 
  0266, 0253, 0221, 0147, 0254, 0054, 0355, 0075, 0373, 0262, 0266, 0141, 0313, 0005, 0154, 0004, 
  0152, 0330, 0161, 0171, 0300, 0323, 0255, 0013, 0164, 0115, 0221, 0353, 0312, 0306, 0025, 0165, 
  0004, 0325, 0164, 0114, 0106, 0061, 0111, 0307, 0266, 0130, 0206, 0335, 0373, 0333, 0210, 0170, 
  0244, 0330, 0247, 0153, 0012, 0321, 0350, 0210, 0164, 0022, 0035, 0176, 0342, 0162, 0224, 0366, 
  0032, 0035, 0277, 0373, 0151, 0130, 0057, 0031, 0036, 0311, 0344, 0306, 0050, 0341, 0355, 0250, 
  0374, 0242, 0323, 0330, 0143, 0115, 0214, 0037, 0304, 0051, 0200, 0072, 0102, 0253, 0116, 0217, 
  0106, 0276, 0247, 0063, 0346, 0100, 0352, 0175, 0075, 0147, 0016, 0335, 0203, 0102, 0222, 0164, 
  0206, 0337, 0127, 0373, 0073, 0133, 0374, 0002, 0140, 0112, 0276, 0100, 0000, 0050, 0165, 0165, 
  0141, 0171, 0051, 0157, 0162, 0147, 0057, 0000, 0005, 0000, 0000, 0000, 0057, 0000, 0000, 0000, 
  0001, 0000, 0000, 0000, 0141, 0164, 0162, 0151, 0154, 0057, 0000, 0000, 0006, 0000, 0000, 0000, 
  0165, 0151, 0057, 0000, 0000, 0000, 0000, 0000, 0155, 0141, 0164, 0145, 0057, 0000, 0000, 0000, 
  0003, 0000, 0000, 0000, 0160, 0162, 0145, 0166, 0151, 0145, 0167, 0145, 0162, 0057, 0000, 0000, 
  0004, 0000, 0000, 0000
} };
#else /* _MSC_VER */
static const SECTION union { const guint8 data[645]; const double alignment; void * const ptr;}  ev_previewer_resource_data = {
  "\107\126\141\162\151\141\156\164\000\000\000\000\000\000\000\000"
  "\030\000\000\000\344\000\000\000\000\000\000\050\007\000\000\000"
  "\000\000\000\000\001\000\000\000\002\000\000\000\003\000\000\000"
  "\005\000\000\000\006\000\000\000\007\000\000\000\031\047\334\342"
  "\004\000\000\000\344\000\000\000\015\000\166\000\370\000\000\000"
  "\103\002\000\000\113\120\220\013\002\000\000\000\103\002\000\000"
  "\004\000\114\000\110\002\000\000\114\002\000\000\324\265\002\000"
  "\377\377\377\377\114\002\000\000\001\000\114\000\120\002\000\000"
  "\124\002\000\000\254\375\211\161\005\000\000\000\124\002\000\000"
  "\006\000\114\000\134\002\000\000\140\002\000\000\301\104\303\351"
  "\006\000\000\000\140\002\000\000\003\000\114\000\144\002\000\000"
  "\150\002\000\000\201\321\040\031\001\000\000\000\150\002\000\000"
  "\005\000\114\000\160\002\000\000\164\002\000\000\324\045\343\073"
  "\003\000\000\000\164\002\000\000\012\000\114\000\200\002\000\000"
  "\204\002\000\000\160\162\145\166\151\145\167\145\162\056\170\155"
  "\154\000\000\000\000\000\000\000\352\004\000\000\001\000\000\000"
  "\170\332\205\224\137\117\203\060\024\305\337\367\051\010\037\300"
  "\351\073\143\061\213\063\306\250\104\215\113\366\126\331\215\066"
  "\051\275\330\226\155\037\337\376\301\001\345\202\217\234\163\176"
  "\207\333\162\103\266\076\127\042\071\202\322\034\345\052\275\271"
  "\272\116\327\371\042\153\170\236\031\104\361\311\124\042\131\005"
  "\253\264\120\160\344\160\172\017\142\032\154\156\240\152\375\173"
  "\364\011\154\164\301\276\040\115\130\151\174\143\244\057\011\360"
  "\031\316\046\206\056\232\005\064\324\114\061\203\152\014\273\310"
  "\033\010\050\255\333\341\003\165\276\340\303\236\151\313\315\216"
  "\037\314\167\127\060\120\047\241\341\314\175\221\106\366\210\325"
  "\203\034\022\255\066\015\274\064\146\114\070\161\032\171\005\015"
  "\004\024\344\177\356\063\174\345\102\161\331\153\030\250\026\132"
  "\266\253\221\147\254\054\355\075\373\262\266\141\313\005\154\004"
  "\152\330\161\171\300\323\255\013\164\115\221\353\312\306\025\165"
  "\004\325\164\114\106\061\111\307\266\130\206\335\373\333\210\170"
  "\244\330\247\153\012\321\350\210\164\022\035\176\342\162\224\366"
  "\032\035\277\373\151\130\057\031\036\311\344\306\050\341\355\250"
  "\374\242\323\330\143\115\214\037\304\051\200\072\102\253\116\217"
  "\106\276\247\063\346\100\352\175\075\147\016\335\203\102\222\164"
  "\206\337\127\373\073\133\374\002\140\112\276\100\000\050\165\165"
  "\141\171\051\157\162\147\057\000\005\000\000\000\057\000\000\000"
  "\001\000\000\000\141\164\162\151\154\057\000\000\006\000\000\000"
  "\165\151\057\000\000\000\000\000\155\141\164\145\057\000\000\000"
  "\003\000\000\000\160\162\145\166\151\145\167\145\162\057\000\000"
  "\004\000\000\000" };
#endif /* !_MSC_VER */

static GStaticResource static_resource = { ev_previewer_resource_data.data, sizeof (ev_previewer_resource_data.data) - 1 /* nul terminator */, NULL, NULL, NULL };

G_MODULE_EXPORT
GResource *ev_previewer_get_resource (void);
GResource *ev_previewer_get_resource (void)
{
  return g_static_resource_get_resource (&static_resource);
}
/*
  If G_HAS_CONSTRUCTORS is true then the compiler support *both* constructors and
  destructors, in a usable way, including e.g. on library unload. If not you're on
  your own.

  Some compilers need #pragma to handle this, which does not work with macros,
  so the way you need to use this is (for constructors):

  #ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
  #pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(my_constructor)
  #endif
  G_DEFINE_CONSTRUCTOR(my_constructor)
  static void my_constructor(void) {
   ...
  }

*/

#ifndef __GTK_DOC_IGNORE__

#if  __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR(_func) static void __attribute__((constructor)) _func (void);
#define G_DEFINE_DESTRUCTOR(_func) static void __attribute__((destructor)) _func (void);

#elif defined (_MSC_VER) && (_MSC_VER >= 1500)
/* Visual studio 2008 and later has _Pragma */

#include <stdlib.h>

#define G_HAS_CONSTRUCTORS 1

/* We do some weird things to avoid the constructors being optimized
 * away on VS2015 if WholeProgramOptimization is enabled. First we
 * make a reference to the array from the wrapper to make sure its
 * references. Then we use a pragma to make sure the wrapper function
 * symbol is always included at the link stage. Also, the symbols
 * need to be extern (but not dllexport), even though they are not
 * really used from another object file.
 */

/* We need to account for differences between the mangling of symbols
 * for x86 and x64/ARM/ARM64 programs, as symbols on x86 are prefixed
 * with an underscore but symbols on x64/ARM/ARM64 are not.
 */
#ifdef _M_IX86
#define G_MSVC_SYMBOL_PREFIX "_"
#else
#define G_MSVC_SYMBOL_PREFIX ""
#endif

#define G_DEFINE_CONSTRUCTOR(_func) G_MSVC_CTOR (_func, G_MSVC_SYMBOL_PREFIX)
#define G_DEFINE_DESTRUCTOR(_func) G_MSVC_DTOR (_func, G_MSVC_SYMBOL_PREFIX)

#define G_MSVC_CTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _wrapper(void) { _func(); g_slist_find (NULL,  _array ## _func); return 0; } \
  __pragma(comment(linker,"/include:" _sym_prefix # _func "_wrapper")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _wrapper;

#define G_MSVC_DTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _constructor(void) { atexit (_func); g_slist_find (NULL,  _array ## _func); return 0; } \
   __pragma(comment(linker,"/include:" _sym_prefix # _func "_constructor")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _constructor;

#elif defined (_MSC_VER)

#define G_HAS_CONSTRUCTORS 1

/* Pre Visual studio 2008 must use #pragma section */
#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  section(".CRT$XCU",read)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void); \
  static int _func ## _wrapper(void) { _func(); return 0; } \
  __declspec(allocate(".CRT$XCU")) static int (*p)(void) = _func ## _wrapper;

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  section(".CRT$XCU",read)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void); \
  static int _func ## _constructor(void) { atexit (_func); return 0; } \
  __declspec(allocate(".CRT$XCU")) static int (* _array ## _func)(void) = _func ## _constructor;

#elif defined(__SUNPRO_C)

/* This is not tested, but i believe it should work, based on:
 * http://opensource.apple.com/source/OpenSSL098/OpenSSL098-35/src/fips/fips_premain.c
 */

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  init(_func)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void);

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  fini(_func)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void);

#else

/* constructors not supported for this compiler */

#endif

#endif /* __GTK_DOC_IGNORE__ */

#ifdef G_HAS_CONSTRUCTORS

#ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(resource_constructor)
#endif
G_DEFINE_CONSTRUCTOR(resource_constructor)
#ifdef G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(resource_destructor)
#endif
G_DEFINE_DESTRUCTOR(resource_destructor)

#else
#warning "Constructor not supported on this compiler, linking in resources will not work"
#endif

static void resource_constructor (void)
{
  g_static_resource_init (&static_resource);
}

static void resource_destructor (void)
{
  g_static_resource_fini (&static_resource);
}
