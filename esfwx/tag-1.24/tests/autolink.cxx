#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# if __CODEGEARC__ < 0x0720
#   ifdef USEPACKAGES
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc23.dylib"
#       pragma link "bplescore_ecc23.dylib"
#       pragma link "bplescomm_ecc23.dylib"
#       pragma link "bplesmath_ecc23.dylib"
#       pragma link "bplesscript_ecc23.dylib"
#     else
#       pragma link "escore_ecc23.bpi"
#       pragma link "escore_ecc23.bpi"
#       pragma link "escomm_ecc23.bpi"
#       pragma link "esmath_ecc23.bpi"
#       pragma link "esscript_ecc23.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc23.a"
#         pragma link "libescomm_ecc23.a"
#         pragma link "libesmath_ecc23.a"
#         pragma link "libesscript_ecc23.a"
#       else
#         pragma link "escore_ecc23.a"
#         pragma link "escomm_ecc23.a"
#         pragma link "esmath_ecc23.a"
#         pragma link "esscript_ecc23.a"
#       endif
#     else
#       pragma link "escore_ecc23.lib"
#       pragma link "escomm_ecc23.lib"
#       pragma link "esmath_ecc23.lib"
#       pragma link "esscript_ecc23.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0730
#   ifdef USEPACKAGES
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc24.dylib"
#       pragma link "bplescore_ecc24.dylib"
#       pragma link "bplescomm_ecc24.dylib"
#       pragma link "bplesmath_ecc24.dylib"
#       pragma link "bplesscript_ecc24.dylib"
#     else
#       pragma link "escore_ecc24.bpi"
#       pragma link "escore_ecc24.bpi"
#       pragma link "escomm_ecc24.bpi"
#       pragma link "esmath_ecc24.bpi"
#       pragma link "esscript_ecc24.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc24.a"
#         pragma link "libescomm_ecc24.a"
#         pragma link "libesmath_ecc24.a"
#         pragma link "libesscript_ecc24.a"
#       else
#         pragma link "escore_ecc24.a"
#         pragma link "escomm_ecc24.a"
#         pragma link "esmath_ecc24.a"
#         pragma link "esscript_ecc24.a"
#       endif
#     else
#       pragma link "escore_ecc24.lib"
#       pragma link "escomm_ecc24.lib"
#       pragma link "esmath_ecc24.lib"
#       pragma link "esscript_ecc24.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0740
#   ifdef USEPACKAGES
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc25.dylib"
#       pragma link "bplescore_ecc25.dylib"
#       pragma link "bplescomm_ecc25.dylib"
#       pragma link "bplesmath_ecc25.dylib"
#       pragma link "bplesscript_ecc25.dylib"
#     else
#       pragma link "escore_ecc25.bpi"
#       pragma link "escore_ecc25.bpi"
#       pragma link "escomm_ecc25.bpi"
#       pragma link "esmath_ecc25.bpi"
#       pragma link "esscript_ecc25.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc25.a"
#         pragma link "libescomm_ecc25.a"
#         pragma link "libesmath_ecc25.a"
#         pragma link "libesscript_ecc25.a"
#       else
#         pragma link "escore_ecc25.a"
#         pragma link "escomm_ecc25.a"
#         pragma link "esmath_ecc25.a"
#         pragma link "esscript_ecc25.a"
#       endif
#     else
#       pragma link "escore_ecc25.lib"
#       pragma link "escomm_ecc25.lib"
#       pragma link "esmath_ecc25.lib"
#       pragma link "esscript_ecc25.lib"
#     endif
#   endif
# else
#   error Autolink is not supported!
# endif
#endif
