workspace(name="mf")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# GTest/GMock
http_archive(
    name="googletest",
    url="https://github.com/google/googletest/archive/release-1.8.0.zip",
    sha256="f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf",
    strip_prefix="googletest-release-1.8.0",
    build_file="@//external:googletest.BUILD",
)
