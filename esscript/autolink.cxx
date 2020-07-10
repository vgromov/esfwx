#ifndef ES_MONOBUILD

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# if __CODEGEARC__ < 0x0710
#   ifdef ES_PACKAGE
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc22.dylib"
#     else
#       pragma link "escore_ecc22.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc22.a"
#       else
#         pragma link "escore_ecc22.a"
#       endif
#     else
#       pragma link "escore_ecc22.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0720
#   ifdef ES_PACKAGE
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc23.dylib"
#     else
#       pragma link "escore_ecc23.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc23.a"
#       else          
#         pragma link "escore_ecc23.a"
#       endif
#     else
#       pragma link "escore_ecc23.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0730
#   ifdef ES_PACKAGE
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc24.dylib"
#     else
#       pragma link "escore_ecc24.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc24.a"
#       else          
#         pragma link "escore_ecc24.a"
#       endif
#     else
#       pragma link "escore_ecc24.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0740
#   ifdef ES_PACKAGE
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc25.dylib"
#     else
#       pragma link "escore_ecc25.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc25.a"
#       else          
#         pragma link "escore_ecc25.a"
#       endif
#     else
#       pragma link "escore_ecc25.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0750
#   ifdef ES_PACKAGE
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc26.dylib"
#     else
#       pragma link "escore_ecc26.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc26.a"
#       else
#         pragma link "escore_ecc26.a"
#       endif
#     else
#       pragma link "escore_ecc26.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0760
#   ifdef ES_PACKAGE
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc27.dylib"
#     else
#       pragma link "escore_ecc27.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc27.a"
#       else
#         pragma link "escore_ecc27.a"
#       endif
#     else
#       pragma link "escore_ecc27.lib"
#     endif
#   endif
# else
#   error Autolink is not supported!
# endif
#endif

#endif // ES_MONOBUILD

