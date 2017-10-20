// exponential_distribution
#include <iostream>
#include <fstream>
#include <random>
/*
int main()
{
  //std::default_random_engine generator(std::random_device{}());
  int a = 2;
  std::default_random_engine generator(1);
  std::exponential_distribution<double> distribution(a);
  std::ofstream fout;
  fout.open("cdata.txt");
  //a= 10;

  for (int i=0; i<1000; i++)
  {
    double number = distribution(generator);
    fout<<number<<'\n';
    std::cerr << number << '\n';
  }

}
*/
// exponential_distribution::param
#include <iostream>
#include <random>

int main()
{
  //std::default_random_engine generator(std::random_device{}());
  std::default_random_engine generator();
  std::exponential_distribution<double> d1(1);
  std::exponential_distribution<double> d2(d1.param());
  auto ptr = &d1;
  std::cout << *ptr << '\n';
  //std::exponential_distribution<double>::param_type new_lambda(0.2);


  // print two independent values:
  std::cout << d1(generator) << std::endl;
  std::cout << d2(generator) << std::endl;
  //std::exponential_distribution<double>
  return 0;
}
