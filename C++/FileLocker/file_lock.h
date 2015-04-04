#ifndef FILE_LOCKER_H_
#define FILE_LOCKER_H_
#include <string>

namespace MicroAd
{
namespace Utils
{
class FileLocker
{
public:
  explicit FileLocker(const std::string& filePath);
  bool Open();
  bool TruncWrite(const std::string& content);
  bool Read(std::string& result);
  /* 0 success, 1 be hold by other process, 2 other failure*/
  int TryLock();
  bool UnLock();
  ~FileLocker();
private:
  //no copy
  FileLocker(const FileLocker&);
  void operator=(const FileLocker&);
  const std::string path_;
  int fd_;
};
}
}

#endif
