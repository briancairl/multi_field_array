load("@multi_field_array//:bazel/rules.bzl", "gtest")

gtest(
  name="tuple_for_each",
  timeout = "short",
  srcs=["tuple_for_each.cpp"],
  deps=["//:support"],
  visibility=["//visibility:public"],
)

gtest(
  name="apply_placement_new",
  timeout = "short",
  srcs=["apply_placement_new.cpp"],
  deps=["//:support"],
  visibility=["//visibility:public"],
)
