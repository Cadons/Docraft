#pragma once

#include <filesystem>
#include <fstream>
#include <random>
#include <string>

namespace docraft::test::utils {
  // Reserves a unique temp file path using exclusive creation (std::ios::noreplace),
  // avoiding the predictable-name / TOCTOU race of temp_directory_path() + fixed filename.
  inline std::filesystem::path make_unique_temp_path(const std::string& suffix)
  {
    static std::mt19937_64 rng{std::random_device{}()};
    std::uniform_int_distribution<uint64_t> dist;

    for (;;)
    {
      const auto candidate = std::filesystem::temp_directory_path() /
        ("docraft_test_" + std::to_string(dist(rng)) + suffix);
      if (std::ofstream file(candidate, std::ios::out | std::ios::noreplace); file.is_open())
      {
        return candidate;
      }
    }
  }
} // namespace docraft::test::utils
