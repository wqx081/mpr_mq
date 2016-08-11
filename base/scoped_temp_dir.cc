#include "base/scoped_temp_dir.h"
#include "base/file_util.h"

#include <glog/logging.h>

namespace base {

ScopedTempDir::ScopedTempDir() {
}

ScopedTempDir::~ScopedTempDir() {
  if (!path_.empty() && !Delete())
    DLOG(WARNING) << "Could not delete temp dir in dtor.";
}

bool ScopedTempDir::CreateUniqueTempDir() {
  if (!path_.empty())
    return false;

  // This "scoped_dir" prefix is only used on Windows and serves as a template
  // for the unique name.
  if (!CreateNewTempDirectory(FILE_PATH_LITERAL("scoped_dir"),
                                         &path_))
    return false;

  return true;
}

bool ScopedTempDir::CreateUniqueTempDirUnderPath(const FilePath& base_path) {
  if (!path_.empty())
    return false;

  // If |base_path| does not exist, create it.
  if (!CreateDirectory(base_path))
    return false;

  // Create a new, uniquely named directory under |base_path|.
  if (!CreateTemporaryDirInDir(
          base_path,
          FILE_PATH_LITERAL("scoped_dir_"),
          &path_))
    return false;

  return true;
}

bool ScopedTempDir::Set(const FilePath& path) {
  if (!path_.empty())
    return false;

  if (!DirectoryExists(path) &&
      !CreateDirectory(path))
    return false;

  path_ = path;
  return true;
}

bool ScopedTempDir::Delete() {
  if (path_.empty())
    return false;

  bool ret = DeleteFile(path_, true);
  if (ret) {
    // We only clear the path if deleted the directory.
    path_.clear();
  }

  return ret;
}

FilePath ScopedTempDir::Take() {
  FilePath ret = path_;
  path_ = FilePath();
  return ret;
}

bool ScopedTempDir::IsValid() const {
  return !path_.empty() && DirectoryExists(path_);
}

}  // namespace base
