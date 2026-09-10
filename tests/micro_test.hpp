#pragma once

// A deliberately tiny, dependency-free test harness -- avoids pulling in a
// third-party framework (and the network access that requires) for a v1
// project this small. Register tests with FOOTBSIM_TEST, assert with
// FOOTBSIM_CHECK, and run everything from a single main() via RunAll().

#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace footbsim::test
{

  struct Test
  {
    std::string name;
    std::function<void()> fn;
  };

  inline std::vector<Test>& Registry()
  {
    static std::vector<Test> tests;
    return tests;
  }

  struct Registrar
  {
    Registrar(std::string name, std::function<void()> fn)
    {
      Registry().push_back({ std::move(name), std::move(fn) });
    }
  };

  struct AssertionFailure
  {
    std::string message;
  };

  inline int RunAll()
  {
    int failed = 0;
    for (const Test& t : Registry())
    {
      try
      {
        t.fn();
        std::cout << "[PASS] " << t.name << "\n";
      }
      catch (const AssertionFailure& e)
      {
        std::cout << "[FAIL] " << t.name << ": " << e.message << "\n";
        ++failed;
      }
      catch (const std::exception& e)
      {
        std::cout << "[FAIL] " << t.name << " (exception): " << e.what() << "\n";
        ++failed;
      }
    }
    std::cout << (Registry().size() - static_cast<std::size_t>(failed)) << "/" << Registry().size()
              << " tests passed\n";
    return failed == 0 ? 0 : 1;
  }

} // namespace footbsim::test

#define FOOTBSIM_TEST(name)                                                                        \
  static void name();                                                                              \
  static const ::footbsim::test::Registrar registrar_##name(#name, name); /* NOLINT */             \
  static void name()

#define FOOTBSIM_CHECK(cond)                                                                       \
  do                                                                                               \
  {                                                                                                \
    if (!(cond))                                                                                   \
    {                                                                                              \
      throw ::footbsim::test::AssertionFailure{                                                    \
        std::string("CHECK failed: " #cond " at " __FILE__ ":") + std::to_string(__LINE__)         \
      };                                                                                           \
    }                                                                                              \
  } while (0)
