#include <iostream>
#include <vector>
#include <algorithm>
void Perm(std::vector<std::vector<int> >& result, std::vector<int> seq, size_t start)
{
  if(start == seq.size())
  {
    result.push_back(seq);
  } else
  {
    for(size_t i = start; i != seq.size(); i++)
    {
      std::swap(seq[i], seq[start]);
      Perm(result, seq, start+1);
      std::swap(seq[i], seq[start]);
    }
  }
}

int main(int argc, char *argv[])
{
  int v4[] = {0, 1, 2, 3};
  std::vector<int> seq(v4, v4+sizeof(v4)/sizeof(int));
  std::vector<std::vector<int> > result;
  Perm(result, seq, 0);
  for(size_t i = 0; i != result.size(); i++)
  {
    for(size_t j = 0; j != result[i].size(); j++)
    {
      std::cout << result[i][j] << "\t";
    }
    std::cout << std::endl;
  }
  return 0;
}
