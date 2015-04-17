#include "file_lock.h"
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>

namespace MicroAd
{
namespace Utils
{

FileLocker::FileLocker(const std::string& filePath): path_(filePath),   \
                                                     fd_(-1){}
bool FileLocker::Open()
{
  //fcntl.h
  const int lock_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  fd_ = open(path_.c_str(), O_RDWR | O_CREAT, lock_mode);
  if (fd_ < 0)
  {
    std::cout << "open lock file error";
  }
  return fd_ > 0;
}

int FileLocker::TryLock()
{
  if(0 > fd_)
  {
    return 2;
  }
  struct flock file_lock;
  file_lock.l_type = F_WRLCK;
  file_lock.l_start = 0;
  file_lock.l_whence = SEEK_SET;
  int ret = fcntl(fd_, F_SETLK, &file_lock);
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
  if(fd_ < 0)
  {
    return false;
  }
  struct flock file_lock;
  file_lock.l_type = F_UNLCK;
  file_lock.l_start = 0;
  file_lock.l_whence = SEEK_SET;
  return 0 == fcntl(fd_, F_SETLK, &file_lock);
}

FileLocker::~FileLocker()
{
  if(fd_ > 0)
  {
    ::close(fd_);
    fd_ = -1;
  }
}

bool FileLocker::TruncWrite(const std::string& content)
{
  if(fd_ < 0)
  {
    return false;
  }
  if(ftruncate(fd_, 0) != 0)
  {
    return false;
  }
  if(write(fd_, content.c_str(), content.size()) != static_cast<int>(content.size()))
  {
    return false;
  }
  return true;
}


bool FileLocker::Read(std::string& result)
{
  if(fd_ < 0)
  {
    return false;
  }
  char buf[10];
  int ret = 0;
  while((ret = ::read(fd_, buf, 10)) > 0)
  {
    result.append(buf, ret);
  }
  return ret >= 0;
}

}
}
