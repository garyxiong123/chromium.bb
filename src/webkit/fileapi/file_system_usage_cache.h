// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBKIT_FILEAPI_FILE_SYSTEM_USAGE_CACHE_H_
#define WEBKIT_FILEAPI_FILE_SYSTEM_USAGE_CACHE_H_

#include <map>

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "base/memory/weak_ptr.h"
#include "base/platform_file.h"
#include "base/sequenced_task_runner.h"
#include "base/timer.h"
#include "webkit/storage/webkit_storage_export.h"

namespace fileapi {

class WEBKIT_STORAGE_EXPORT_PRIVATE FileSystemUsageCache {
 public:
  explicit FileSystemUsageCache(base::SequencedTaskRunner* task_runner);
  ~FileSystemUsageCache();

  // Gets the size described in the .usage file even if dirty > 0 or
  // is_valid == false.  Returns less than zero if the .usage file is not
  // available.
  int64 GetUsage(const base::FilePath& usage_file_path);

  // Gets the dirty count in the .usage file.
  // Returns less than zero if the .usage file is not available.
  int32 GetDirty(const base::FilePath& usage_file_path);

  // Increments or decrements the "dirty" entry in the .usage file.
  // Returns false if no .usage is available.
  bool IncrementDirty(const base::FilePath& usage_file_path);
  bool DecrementDirty(const base::FilePath& usage_file_path);

  // Notifies quota system that it needs to recalculate the usage cache of the
  // origin.  Returns false if no .usage is available.
  bool Invalidate(const base::FilePath& usage_file_path);
  bool IsValid(const base::FilePath& usage_file_path);

  // Updates the size described in the .usage file.
  bool UpdateUsage(const base::FilePath& usage_file_path, int64 fs_usage);

  // Updates the size described in the .usage file by delta with keeping dirty
  // even if dirty > 0.
  bool AtomicUpdateUsageByDelta(const base::FilePath& usage_file_path,
                                int64 delta);

  bool Exists(const base::FilePath& usage_file_path);
  bool Delete(const base::FilePath& usage_file_path);

  void CloseCacheFiles();

  static const base::FilePath::CharType kUsageFileName[];
  static const char kUsageFileHeader[];
  static const int kUsageFileSize;
  static const int kUsageFileHeaderSize;

 private:
  typedef std::map<base::FilePath, base::PlatformFile> CacheFiles;

  // Read the size, validity and the "dirty" entry described in the .usage file.
  // Returns less than zero if no .usage file is available.
  int64 Read(const base::FilePath& usage_file_path,
             bool* is_valid,
             uint32* dirty);

  bool Write(const base::FilePath& usage_file_path,
             bool is_valid,
             uint32 dirty,
             int64 fs_usage);

  bool GetPlatformFile(const base::FilePath& file_path,
                       base::PlatformFile* file);

  bool ReadBytes(const base::FilePath& file_path,
                 char* buffer,
                 int64 buffer_size);
  bool WriteBytes(const base::FilePath& file_path,
                  const char* buffer,
                  int64 buffer_size);
  bool FlushFile(const base::FilePath& file_path);
  void ScheduleCloseTimer();

  bool HasCacheFileHandle(const base::FilePath& file_path);

  bool CalledOnValidThread();

  base::OneShotTimer<FileSystemUsageCache> timer_;
  std::map<base::FilePath, base::PlatformFile> cache_files_;
  base::WeakPtrFactory<FileSystemUsageCache> weak_factory_;

  scoped_refptr<base::SequencedTaskRunner> task_runner_;

  DISALLOW_COPY_AND_ASSIGN(FileSystemUsageCache);
};

}  // namespace fileapi

#endif  // WEBKIT_FILEAPI_FILE_SYSTEM_USAGE_CACHE_H_
