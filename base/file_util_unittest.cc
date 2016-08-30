#include "base/file_util.h"
#include "base/file_path.h"
#include "base/dir_reader.h"

#include <gtest/gtest.h>

namespace base {

const char * kDirectory = "/data/note/201603181703265628/147245835137237700/hd/orig/";

TEST(FileUtilTest, test) {
  base::FilePath orig_path(kDirectory);
  base::FilePath dir_name = orig_path.DirName();
  LOG(INFO) << "dir_name: " << dir_name;
  EXPECT_TRUE(true);
}

}
