#include "base/file_enumerator.h"

#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include <glog/logging.h>


namespace base {

FileEnumerator::FileInfo::FileInfo() {
  memset(&stat_, 0, sizeof(stat_));
}

bool FileEnumerator::FileInfo::IsDirectory() const {
  return S_ISDIR(stat_.st_mode);
}

FilePath FileEnumerator::FileInfo::GetName() const {
  return filename_;
}

int64_t FileEnumerator::FileInfo::GetSize() const {
  return stat_.st_size;
}

Time FileEnumerator::FileInfo::GetLastModifiedTime() const {
  return Time::FromTimeT(stat_.st_mtime);
}

FileEnumerator::FileInfo::~FileInfo() {
}


FileEnumerator::FileEnumerator(const FilePath& root_path,
		               bool recursive,
			       int file_type) 
  : current_directory_entry_(0),
    root_path_(root_path),
    recursive_(recursive),
    file_type_(file_type) {
  DCHECK(!(recursive && (INCLUDE_DOT_DOT & file_type_)));
  pending_paths_.push(root_path);
}

FileEnumerator::FileEnumerator(const FilePath& root_path,
		               bool recursive,
			       int file_type,
			       const FilePath::StringType& pattern)
  : current_directory_entry_(0),
    root_path_(root_path),
    recursive_(recursive),
    file_type_(file_type),
    pattern_(root_path.Append(pattern).value()) {
  DCHECK(!(recursive && (INCLUDE_DOT_DOT & file_type_)));
  if (pattern.empty()) {
    pattern_ = FilePath::StringType();
  }  
  pending_paths_.push(root_path);
}

FileEnumerator::~FileEnumerator() {
}

FilePath FileEnumerator::Next() {
  ++current_directory_entry_;

  while (current_directory_entry_ >= directory_entries_.size()) {
    if (pending_paths_.empty()) {
      return FilePath();
    }

    root_path_ = pending_paths_.top();
    root_path_ = root_path_.StripTrailingSeparators();
    pending_paths_.pop();

    std::vector<FileInfo> entries;
    if (!ReadDirectory(&entries, root_path_, file_type_ & SHOW_SYM_LINKS))
      continue;

    directory_entries_.clear();
    current_directory_entry_ = 0;
    for (std::vector<FileInfo>::const_iterator i = entries.begin();
	 i != entries.end(); ++i) {
      FilePath full_path = root_path_.Append(i->filename_);
      if (ShouldSkip(full_path))
        continue;

      if (pattern_.size() &&
          fnmatch(pattern_.c_str(), full_path.value().c_str(),
		  FNM_NOESCAPE)) {
        continue;
      }

      if (recursive_ && S_ISDIR(i->stat_.st_mode)) {
        pending_paths_.push(full_path);
      }

      if ((S_ISDIR(i->stat_.st_mode) && (file_type_ & DIRECTORIES)) ||
          (!S_ISDIR(i->stat_.st_mode) && (file_type_ & FILES))) {
        directory_entries_.push_back(*i);
      }
    }
  }
  return root_path_.Append(
    directory_entries_[current_directory_entry_].filename_);

}

FileEnumerator::FileInfo FileEnumerator::GetInfo() const {
  return directory_entries_[current_directory_entry_];
}

bool FileEnumerator::ShouldSkip(const FilePath& path) {
  FilePath::StringType basename = path.BaseName().value();
  return basename == "." ||
         (basename == ".." && !(INCLUDE_DOT_DOT & file_type_));
}

bool FileEnumerator::ReadDirectory(std::vector<FileInfo>* entries,
		                   const FilePath& source,
				   bool show_links) {
  DIR* dir = opendir(source.value().c_str());
  if (!dir)
    return false;
  
  struct dirent dent_buf;
  struct dirent* dent;
  
  while (readdir_r(dir, &dent_buf, &dent) == 0 && dent) {
    FileInfo info;
    info.filename_ = FilePath(dent->d_name);

    FilePath full_name = source.Append(dent->d_name);
    int ret;
    if (show_links) {
      ret = lstat(full_name.value().c_str(), &info.stat_);
    } else {
      ret = stat(full_name.value().c_str(), &info.stat_);
    }

    if (ret < 0) {
      if (!(errno == ENOENT && !show_links)) {
        
      }
      memset(&info.stat_, 0, sizeof(info.stat_));
    }
    entries->push_back(info);
  }
  closedir(dir);
  return true;
}

} // namespace base
