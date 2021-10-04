load("@multi_field_array//:bazel/rules.bzl", "gbenchmark")

gbenchmark(
  name="multi_field_array",
  timeout = "short",
  srcs=["multi_field_array.cpp"],
  deps=["//:mf",],
  visibility=["//visibility:public"],
)
