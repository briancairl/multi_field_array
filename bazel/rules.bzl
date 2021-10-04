
def gtest(name, copts=[], linkopts=[], deps=[], **kwargs):
    '''
    A wrapper around cc_test for gtests
    Adds options to the compilation command.
    '''
    _GTEST_COPTS = [
        "-Iexternal/googletest/googletest/include",
        "-fsanitize=address",
        "-fsanitize-address-use-after-scope",
        "-DADDRESS_SANITIZER",
        "-g",
        "-fno-omit-frame-pointer",
        "-O0"
    ]

    _GTEST_LINKOPTS = [
        "-fsanitize=address",
        "-static-libasan"
    ]

    _GTEST_DEPS = [
        "@googletest//:gtest",
    ]

    native.cc_test(
        name=name,
        copts=_GTEST_COPTS + copts,
        deps=_GTEST_DEPS + deps,
        linkopts=_GTEST_LINKOPTS + linkopts,
        **kwargs
    )

def gbenchmark(name, copts=[], linkopts=[], deps=[], **kwargs):
    '''
    A wrapper around cc_test for gtests
    Adds options to the compilation command.
    '''
    _GBENCHMARK_COPTS = [
        "-Iexternal/googlebenchmark/googlebenchmark/include",
        "-O3",
        "-DNDEBUG"
    ]

    _GBENCHMARK_LINKOPTS = []

    _GBENCHMARK_DEPS = [
        "@googlebenchmark//:benchmark_main",
    ]

    native.cc_test(
        name=name,
        copts=_GBENCHMARK_COPTS + copts,
        deps=_GBENCHMARK_DEPS + deps,
        linkopts=_GBENCHMARK_LINKOPTS + linkopts,
        **kwargs
    )
