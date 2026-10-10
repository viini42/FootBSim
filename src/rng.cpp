#include "footbsim/rng.hpp"

#include <algorithm>

namespace
{

  std::uint64_t MakeSeed(std::optional<std::uint64_t> seed)
  {
    if (seed)
    {
      return *seed;
    }
    std::random_device rd;
    return (static_cast<std::uint64_t>(rd()) << 32) ^ static_cast<std::uint64_t>(rd());
  }

} // namespace

footbsim::Rng::Rng(std::optional<std::uint64_t> seed) : m_engine{ MakeSeed(seed) } {}

double footbsim::Rng::Uniform01()
{
  std::uniform_real_distribution<double> dist{ 0.0, 1.0 };
  return dist(m_engine);
}

bool footbsim::Rng::Bernoulli(double probability)
{
  std::bernoulli_distribution dist{ std::clamp(probability, 0.0, 1.0) };
  return dist(m_engine);
}

double footbsim::Rng::Normal(double mean, double stddev)
{
  std::normal_distribution<double> dist{ mean, std::max(stddev, 0.0) };
  return dist(m_engine);
}
