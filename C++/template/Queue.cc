template <class T> template <class Iter>
void Queue<T>::assign(Iter beg, Iter end)
{
  destroy();
  copy_elems(beg, end);
}

template<class Type> template <class Iter>
void Queue<Type>::copy_elems(It beg, It end)
{
  while(beg != end)
  {
    push(*beg);
    ++beg;
  }
}
