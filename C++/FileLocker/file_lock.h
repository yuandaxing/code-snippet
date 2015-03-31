#ifndefine FILE_LOCKER_H_
#define FILE_LOCKER_H_
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
namespace MicroAd
{
namespace Utils
{
class FileLocker
{
public:
  explicit FileLocker(const std::string& filePath);
  /* 0 success, 1 be hold by other process, 2 other failure*/
  int TryLock();
  bool Open();
  bool UnLock();
  bool TruncWrite(const std::string& content);
  bool Read(std::string& result);
  ~FileLocker();
private:
  //no copy
  FileLocker(const FileLocker&);
  void operator=(const FileLocker&);
  const std::string path_;
  int file_;
};

FileLocker::FileLocker(const std::string& filePath): path_(filePath): file_(-1){}
bool FileLocker::Open()
{
  file_ = open(lock_file.c_str(), O_RDWR | O_CREAT, lock_mode);
  if (fd < 0)
  {
    std::cout << "open lock file error";
  }
  return fd == 0;
}

int FileLocker::TryLock()
{
  if(-1 == file_)
  {
    return 2;
  }
  struct flock file_lock;
  file_lock.l_type = F_WRLCK;
  file_lock.l_start = 0;
  file_lock.l_whence = SEEK_SET;
  int ret = fcntl(fd, F_SETLK, &file_lock);
  if(ret != 0)
  {
    if(errno == EAGAIN || errno == EACCES)
    {
      return 1;
    }else
    {
      return 2;
    }
  }
  return 0;
}
bool FileLocker::UnLock()
{
  if(fd < 0)
  {
    return false;
  }
  struct flock file_lock;
  file_lock.l_type = F_UNLCK;
  file_lock.l_start = 0;
  file_lock.l_whence = SEEK_SET;
  return 0 == fcntl(fd, F_SETLK, &file_lock);
}
bool FileLocker::TruncWrite(const std::string& content)
{
  if(fd < 0)
  {
    return false;
  }
  if(ftruncate(fd, 0) != 0)
  {
    return -1;
  }
  if(write(fd, content.c_str(), content.size()) != content.size())
  {
    return -1;
  }
  return 0;
}
}
}

#endif
