#if ES_OS == ES_OS_WINDOWS
# if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#   if defined(_WIN64)
#     pragma link "winmm.a"
#       ifdef EsDynamicLibrary_SUPPORTED
#         pragma link "version64.a"
#       endif
#   else
#     pragma link "winmm.lib"
#       ifdef EsDynamicLibrary_SUPPORTED
#         pragma link "version.lib"
#       endif
#   endif
# elif ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS
#   if defined(_WIN64)
#     pragma comment(lib, "winmm.a")
#       ifdef EsDynamicLibrary_SUPPORTED
#         pragma comment(lib, "version64.a")
#       endif
#   else
#     pragma comment(lib, "winmm.lib")
#       ifdef EsDynamicLibrary_SUPPORTED
#         pragma comment(lib, "version.lib")
#       endif
#   endif
# else
#   error Autolink is not supported!
# endif
#endif