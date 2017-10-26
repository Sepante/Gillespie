#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
using std::vector;

template <typename T>
T remove_at(std::vector<T>&v, typename std::vector<T>::size_type n)
{
  if( v.size()>=1)
  {
    T ans = std::move_if_noexcept(v[n]);
    v[n] = std::move_if_noexcept(v.back());
    v.pop_back();
    return ans;
  }
  else
    std::cout << "no element to delete from" << '\n';
}

int main()
{
  std::vector<int> R2;
  R2 = {1,2,3};
  //vec.push_back(1);

  int i = 0;

  remove_at(R2, i);
  remove_at(R2, i);
  remove_at(R2, i);
  remove_at(R2, i);
  //std::vector<int>::iterator it;
//  auto it = std::find(R2.begin(), R2.end(), 3);
  //std::cout << *it << '\n';
  for (size_t j = 0; j < R2.size(); j++) {
    std::cout << R2[j] << '\n';
  }
}
