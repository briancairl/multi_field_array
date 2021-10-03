/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */

// C++ Standard Library
#include <numeric>
#include <string>
#include <vector>

// GBenchmark
#include <benchmark/benchmark.h>

// MF
#include <mf/multi_field_array.hpp>


struct TwoFields
{
  float a;
  std::string b;
};

struct ManyFields
{
  float a;
  std::string b;
  int c, d, e, f;
};


//
// ITERATION BENCHMARKING
//


static void Iteration_TwoFields_MFA(benchmark::State& state)
{
  mf::multi_field_array<float, std::string> multi_field_array;
  multi_field_array.resize(1000000);

  for (auto _ : state)
  {
    auto view = multi_field_array.view<float, std::string>();

    const float sum = std::accumulate(
      view.begin(), view.end(), 0.f, [](float prev, const auto& tuples) { return prev + std::get<0>(tuples); });

    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(Iteration_TwoFields_MFA);


static void Iteration_TwoFields_Vec(benchmark::State& state)
{
  std::vector<TwoFields> vector_analogue;
  vector_analogue.resize(1000000);

  for (auto _ : state)
  {
    const float sum =
      std::accumulate(vector_analogue.begin(), vector_analogue.end(), 0.f, [](float prev, const auto& element) {
        return prev + element.a;
      });

    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(Iteration_TwoFields_Vec);


static void Iteration_ManyFields_MFA(benchmark::State& state)
{
  mf::multi_field_array<float, std::string, int, int, int, int> multi_field_array;
  multi_field_array.resize(1000000);

  for (auto _ : state)
  {
    auto view = multi_field_array.view<float, std::string>();

    const float sum = std::accumulate(
      view.begin(), view.end(), 0.f, [](float prev, const auto& tuples) { return prev + std::get<0>(tuples); });

    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(Iteration_ManyFields_MFA);


static void Iteration_ManyFields_Vec(benchmark::State& state)
{
  std::vector<ManyFields> vector_analogue;
  vector_analogue.resize(1000000);

  for (auto _ : state)
  {
    const float sum =
      std::accumulate(vector_analogue.begin(), vector_analogue.end(), 0.f, [](float prev, const auto& element) {
        return prev + element.a;
      });

    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(Iteration_ManyFields_Vec);


//
// ALLOCATION BENCHMARKING
//


static void Allocation_TwoFields_MFA(benchmark::State& state)
{
  for (auto _ : state)
  {
    mf::multi_field_array<float, std::string> multi_field_array;
    multi_field_array.resize(10000);
    benchmark::DoNotOptimize(multi_field_array);
  }
}
BENCHMARK(Allocation_TwoFields_MFA);


static void Allocation_TwoFields_Vec(benchmark::State& state)
{
  for (auto _ : state)
  {
    std::vector<TwoFields> vector_analogue;
    vector_analogue.resize(10000);
    benchmark::DoNotOptimize(vector_analogue);
  }
}
BENCHMARK(Allocation_TwoFields_Vec);


static void Allocation_ManyFields_MFA(benchmark::State& state)
{
  for (auto _ : state)
  {
    mf::multi_field_array<float, std::string, int, int, int, int> multi_field_array;
    multi_field_array.resize(10000);
    benchmark::DoNotOptimize(multi_field_array);
  }
}
BENCHMARK(Allocation_ManyFields_MFA);


static void Allocation_ManyFields_Vec(benchmark::State& state)
{
  for (auto _ : state)
  {
    std::vector<ManyFields> vector_analogue;
    vector_analogue.resize(10000);
    benchmark::DoNotOptimize(vector_analogue);
  }
}
BENCHMARK(Allocation_ManyFields_Vec);


BENCHMARK_MAIN();
