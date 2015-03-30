#include <iostream>
#include <vector>

void Comb(std::vector<std::vector<int> >& result, std::vector<int>& nums,
          size_t start, std::vector<int>& temp)
{
  if(start >= nums.size())
  {
    result.push_back(temp);
  } else
  {
    Comb(result, nums, start+1, temp);
    temp.push_back(nums[start]);
    Comb(result, nums, start+1, temp);
    temp.pop_back();
  }
}

int main(int argc, char *argv[])
{
  int v4[] = {1, 2, 3, 4};
  std::vector<int> nums(v4, v4+sizeof(v4)/sizeof(int)), temp;
  std::vector<std::vector<int> > result;
  Comb(result, nums, 0, temp);
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
