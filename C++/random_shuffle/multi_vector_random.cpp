/*
 * author: yuandx
 * create: 2015-07-09
 * email: yuandx@mvad.com
 */
#include <vector>
#include "../public/common.h"
#include <algorithm>
#include <list>
#include <iostream>

using std::list;
using std::vector;
using micro_ad::utils::Random;
typedef vector<int> PostList;

class RandomPicker
{
public:
  void Prepare(int randCount);
  int Next();
  RandomPicker(const vector<const PostList*>& lists);
  
private:
  void Move(int32_t i);
  bool ValidPos();
  uint32_t Size();
private:
  Random rand_;
  vector<const PostList*> lists_;
  uint32_t group_size_, list_idx_, cur_idx_;
};

RandomPicker::RandomPicker(const vector<const PostList*>& lists):
  lists_(lists), group_size_(0), list_idx_(0), cur_idx_(0)
{
}

uint32_t RandomPicker::Size()
{
  uint32_t size = 0;
  for (vector<const PostList*>::iterator it = lists_.begin(); it != lists_.end(); ++it)
  {
    size += (*it)->size();
  }
  return size;
}
void RandomPicker::Prepare(int randCount)
{
  int32_t total_size = Size();
  group_size_ = total_size / randCount;
  uint32_t size = lists_.size();
  std::swap(lists_[size - 1], lists_[rand_.Next() % size]);
  list_idx_ = 0;
  cur_idx_ = 0;
}

void RandomPicker::Move(int i)
{
  cur_idx_ += i;
  while (list_idx_ < lists_.size() && cur_idx_ >= lists_[list_idx_]->size())
  {
    cur_idx_ -= lists_[list_idx_]->size();
    list_idx_++;
  }
}

bool RandomPicker::ValidPos()
{
  return list_idx_ < lists_.size() && cur_idx_ < lists_[list_idx_]->size();
}

int RandomPicker::Next()
{
  uint32_t step = rand_.Next() % group_size_;
  Move(step);
  int ret = lists_[list_idx_]->at(cur_idx_);
  Move(group_size_ - step);
  return ret;
}

list<vector<int> > g_vec_store;
PostList* Store(PostList& pl)
{
  g_vec_store.push_back(PostList());
  g_vec_store.back().swap(pl);
  return &(g_vec_store.back());
}
void CreatePostList(int (*beg)[100], int (*end)[100], vector<const PostList*>& result)
{
  while (beg != end)
  {
    PostList tmp;
    int* pos = *beg;
    while ((*pos) != 0)
    {
      tmp.push_back(*pos);
      ++pos;
    }
    result.push_back(Store(tmp));
    ++beg;
  }
}

using std::cout;
using std::endl;
int main(int argc, char *argv[])
{
  int A1[5][100] = {{1, 2, 4, 5, 6, 7}, {2, 3, 4, 6, 9}, {1, 9, 10, 17}, {5, 7, 9, 20}, {100}};
  vector<const PostList*> vec_pl;
  CreatePostList(A1, A1+5, vec_pl);
  RandomPicker rp(vec_pl);
  rp.Prepare(5);
  for (int i = 0; i != 5; i++)
  {
    std::cout << rp.Next() << "\t";
  }
  std::cout << std::endl;
  return 0;
}



