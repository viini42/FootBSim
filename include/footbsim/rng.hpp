#pragma once

#include <cstdint>
#include <optional>
#include <random>

namespace footbsim
{

  // Thin wrapper around std::mt19937_64 so every probability roll in the
  // engine goes through one place -- lets tests seed for determinism.
  class Rng
  {
  public:
    explicit Rng(std::optional<std::uint64_t> seed = std::nullopt);

    double Uniform01();
    bool Bernoulli(double probability);
    double Normal(double mean, double stddev);

  private:
    std::mt19937_64 m_engine;
  };

} // namespace footbsim
