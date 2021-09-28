TEST_COPTS = ["-fsanitize=address", "-fsanitize-address-use-after-scope", "-DADDRESS_SANITIZER", "-g", "-fno-omit-frame-pointer", "-O0"]
TEST_LINKOPTS = ["-fsanitize=address", "-static-libasan"]

cc_test(
  name="multi_field_array",
  timeout = "short",
  srcs=["multi_field_array.cpp"],
  copts = TEST_COPTS,
  linkopts = TEST_LINKOPTS,
  deps=[
    "//:mf",
    "@googletest//:gtest",
  ],
  visibility=["//visibility:public"],
)

cc_test(
  name="zip_iterator",
  timeout = "short",
  srcs=["zip_iterator.cpp"],
  copts = TEST_COPTS,
  linkopts = TEST_LINKOPTS,
  deps=[
    "//:mf",
    "@googletest//:gtest",
  ],
  visibility=["//visibility:public"],
)
