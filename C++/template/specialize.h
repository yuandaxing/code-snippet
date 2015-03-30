#ifndef CPLUSPLUS_H_
#define CPLUSPLUS_H_
template <typename V>
struct RebindType
{
  static FieldType::type GetFieldType();
  static V unknown();
};
template<>
struct RebindType<bool>
{
  typedef bool Type;
  typedef BoolValues ListType;
  static FieldType::type GetFieldType()
  {
    return FieldType::BOOL_TYPE;
  }
  static Type unknown()
  {
    return false;
  }
};

#endif
