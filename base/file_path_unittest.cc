#include "base/file_path.h"
#include "base/file_util.h"
#include "base/dir_reader.h"
#include <gtest/gtest.h>

const char * kDirectory = "/data/note/201603181703265628/147245835137237700/hd/orig/";

namespace base {

TEST(FileUtilTest, test2) {
  base::FilePath b("my.ts");
  LOG(INFO) << "extension: " << b.Extension();
}

TEST(FileUtilTest, test) {
  base::FilePath orig_path(kDirectory);
  base::FilePath enc_path;
  base::FilePath dir_name = orig_path.DirName();
  LOG(INFO) << "dir_name: " << dir_name.value();
  LOG(INFO) << "new_name: " << dir_name.Append("enc").value();
  enc_path = dir_name.Append("enc");

  if (base::DirectoryExists(enc_path)) {
    DCHECK(base::DeleteFile(enc_path, true));
  }
  
  DCHECK(base::CreateDirectory(enc_path));
  DCHECK(base::SetPosixFilePermissions(enc_path, base::FILE_PERMISSION_MASK));

  base::DirReader dir_reader(orig_path.value().c_str());
  while (dir_reader.Next()) {
    std::string name = dir_reader.name();
    if (name != ".." && name != ".") {
      //LOG(INFO) << "name : " << name;
      LOG(INFO) << "Encrypt: " << orig_path.Append(name).value() << " -> " << enc_path.Append(name).value();
    }
  }

  EXPECT_TRUE(true);
}

TEST(FilePath, Basic) {
  FilePath path("/a/b/c/d.epub");

  LOG(INFO) << "base name: " << path.BaseName().value();
  LOG(INFO) << "exts name: " << path.Extension();
  LOG(INFO) << "dirs name: " << path.DirName().value();

//  FilePath new_path = (std::move(path.RemoveExtension())).
  LOG(INFO) << "----" << FilePath("/a/b/e.epub").ReplaceExtension(".pdf").value();
  EXPECT_TRUE(true);
}

} 
