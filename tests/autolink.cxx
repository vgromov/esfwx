#ifndef ES_MONOBUILD

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# if __CODEGEARC__ >= 0x0730
#   if __CODEGEARC__ < 0x0740
#     ifdef USEPACKAGES
#       pragma link "escore_ecc25.bpi"
#       pragma link "escomm_ecc25.bpi"
#       pragma link "esmath_ecc25.bpi"
#       pragma link "esscript_ecc25.bpi"
#     else
#       if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
            (ES_OS != ES_OS_WINDOWS)
#         if ES_OS == ES_OS_ANDROID
#           pragma link "libescore_ecc25.a"
#           pragma link "libescomm_ecc25.a"
#           pragma link "libesmath_ecc25.a"
#           pragma link "libesscript_ecc25.a"
#         else
#           pragma link "escore_ecc25.a"
#           pragma link "escomm_ecc25.a"
#           pragma link "esmath_ecc25.a"
#           pragma link "esscript_ecc25.a"
#         endif
#       else
#         pragma link "escore_ecc25.lib"
#         pragma link "escomm_ecc25.lib"
#         pragma link "esmath_ecc25.lib"
#         pragma link "esscript_ecc25.lib"
#       endif
#     endif
#   elif __CODEGEARC__ < 0x0750
#     ifdef USEPACKAGES
#       pragma link "escore_ecc26.bpi"
#       pragma link "escomm_ecc26.bpi"
#       pragma link "esmath_ecc26.bpi"
#       pragma link "esscript_ecc26.bpi"
#     else
#       if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
            (ES_OS != ES_OS_WINDOWS)
#         if ES_OS == ES_OS_ANDROID
#           pragma link "libescore_ecc26.a"
#           pragma link "libescomm_ecc26.a"
#           pragma link "libesmath_ecc26.a"
#           pragma link "libesscript_ecc26.a"
#         else
#           pragma link "escore_ecc26.a"
#           pragma link "escomm_ecc26.a"
#           pragma link "esmath_ecc26.a"
#           pragma link "esscript_ecc26.a"
#         endif
#       else
#         pragma link "escore_ecc26.lib"
#         pragma link "escomm_ecc26.lib"
#         pragma link "esmath_ecc26.lib"
#         pragma link "esscript_ecc26.lib"
#       endif
#     endif
#   else
#     error Autolink is not supported!
#   endif
# else
#  error Autolink is not supported!
# endif
#endif

#endif //< ES_MONOBUILD

