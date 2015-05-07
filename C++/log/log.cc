//if is_debug and if need detail is everywhere, we should wrap it around to make it clear

class Logger
{
public:
  enum type {DETAIL_T=1, DEBUG_T=2};
  Logger(bool debug, bool detail): type_(0), isCommit_(false)
  {
    if(debug)
    {
      type_ ^= debug;
      debug_.reserve(1024*10);
    }
    if(detail)
    {
      type_ ^= detail;
    }
  }
  void Write(int level, const char* formate, ...);
  void Commit();
  bool IsDebug();
  bool IsDetail();
private:
  //no copy
  Logger(const Logger&);
  void operator=(const Logger&);
  int type_;
  string detail_;
  string debug_;
};
//usage
Logger log(debug, detail);
log.Write(DETAIL_T, "hello, world");
log.Write(DEBUG_T, "this is the %dst debug", 1);
log.Write(DEBUG_T|DETAIL_T, "detail and debug at the same time, %s", "hello world" );
log.Commit();

if(log.IsDebug())
 {
   //do not check cache
 }
