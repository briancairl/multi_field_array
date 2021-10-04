workspace(name="multi_field_array")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# GTest/GMock
http_archive(
    name="googletest",
    url="https://github.com/google/googletest/archive/release-1.8.0.zip",
    sha256="f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf",
    strip_prefix="googletest-release-1.8.0",
    build_file="@//external:googletest.BUILD",
)

# Google Benchmark
http_archive(
    name="googlebenchmark",
    url="https://github.com/google/benchmark/archive/refs/tags/v1.6.0.zip",
    sha256="3da225763533aa179af8438e994842be5ca72e4a7fed4d7976dc66c8c4502f58",
    strip_prefix="benchmark-1.6.0",
    build_file="@//external:googlebenchmark.BUILD"
)
