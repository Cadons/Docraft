#include <fstream>
#include <vector>

#include <gtest/gtest.h>

#include "docraft/utils/docraft_file_utils.h"

using docraft::utils::DocraftFileUtils;

TEST(DocraftFileUtilsTest, WriteTempFileReturnsNulloptForNullData)
{
    EXPECT_EQ(DocraftFileUtils::write_temp_file(nullptr, 10), std::nullopt);
}

TEST(DocraftFileUtilsTest, WriteTempFileReturnsNulloptForZeroSize)
{
    const unsigned char data[] = {1, 2, 3};
    EXPECT_EQ(DocraftFileUtils::write_temp_file(data, 0), std::nullopt);
}

TEST(DocraftFileUtilsTest, WriteTempFileWritesExactBytesToAFreshUniqueFile)
{
    const std::vector<unsigned char> data{'D', 'o', 'c', 'r', 'a', 'f', 't'};

    const auto path = DocraftFileUtils::write_temp_file(data.data(), data.size());
    ASSERT_TRUE(path.has_value());
    ASSERT_TRUE(std::filesystem::exists(*path));

    std::ifstream in(*path, std::ios::binary);
    const std::vector<char> contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    ASSERT_EQ(contents.size(), data.size());
    EXPECT_TRUE(std::equal(contents.begin(), contents.end(), data.begin()));
    // Windows can't delete a file while a handle to it is still open (unlike
    // POSIX, where unlinking an open file just removes the directory entry) --
    // close the read handle before removing, or remove_file() below silently
    // no-ops there and the file is still on disk afterwards.
    in.close();

    DocraftFileUtils::remove_file(*path);
    EXPECT_FALSE(std::filesystem::exists(*path));
}

TEST(DocraftFileUtilsTest, WriteTempFileProducesDistinctPathsAcrossCalls)
{
    const unsigned char data[] = {42};

    const auto first = DocraftFileUtils::write_temp_file(data, 1);
    const auto second = DocraftFileUtils::write_temp_file(data, 1);
    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    EXPECT_NE(*first, *second);

    DocraftFileUtils::remove_file(*first);
    DocraftFileUtils::remove_file(*second);
}

TEST(DocraftFileUtilsTest, RemoveFileIsSafeOnAMissingPath)
{
    DocraftFileUtils::remove_file(std::filesystem::temp_directory_path() / "docraft_never_created.tmp");
}
