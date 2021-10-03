load("@mf//:bazel/rules.bzl", "gtest")

gtest(
  name="multi_field_array",
  timeout = "short",
  srcs=["multi_field_array.cpp"],
  deps=["//:mf",],
  visibility=["//visibility:public"],
)

gtest(
  name="zip_iterator",
  timeout = "short",
  srcs=["zip_iterator.cpp"],
  deps=["//:mf",],
  visibility=["//visibility:public"],
)
