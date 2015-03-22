#ifndef HTTPSERV_PARAM_REPOSITORY_H_
#define HTTPSERV_PARAM_REPOSITORY_H_
#include <string>
#include <unordered_map>
#include <pthread.h>
namespace MicroAd
{
namespace Utils
{
enum ParamType {BOOL_T, INT32_T, INT64_T, STRING_T };
enum ValSrc {DEFAULT, COMMAND, WEB};
struct ParamInfo
{
  ParamType paramType_;
  std::string name_;
  std::string desc_;
  bool mutable_;
  ValSrc valSrc_;
  void *ptr_;
  ParamInfo(ParamType pt, const char* name, const char* desc, bool alter, void *ptr):
    paramType_(pt), name_(name), desc_(desc), mutable_(alter), valSrc_(DEFAULT), \
    ptr_(ptr) {}
};
class ParamRepository
{
public:
  static ParamRepository* Instance();
  int32_t CreateInt32(const char* name, const char* val, const char* desc, bool alter, void* ptr);
  int64_t CreateInt64(const char* name, const char* val, const char* desc, bool alter, void* ptr);
  std::string CreateString(const char* name, const char* val, const char* desc, bool alter, void* ptr);
  bool CreateBool(const char* name, const char* val, const char* desc, bool alter, void* ptr);
  bool SetValue(const char* name, const char* val, const ValSrc vs);
private:
  ParamRepository(){}
  ParamRepository(const ParamRepository&);
  void operator=(const ParamRepository&);
  std::unordered_map<std::string, ParamInfo> paramPool_;
  static pthread_mutex_t lock_;
  static ParamRepository* instance_;
};
#define DECLARE_INT32(name) extern int32_t name;
#define DEFINE_INT32(name, value, alterable, desc) \
  int32_t name = ParamRepository::Instance()->CreateInt32(#name, value, desc, alterable, &name);
#define INT32_PARAM(name) name


}
}
#endif
