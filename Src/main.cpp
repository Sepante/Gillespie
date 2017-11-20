#include <iostream>
#include <algorithm>
#include <random>
#include <fstream>

#include "SIR.h"
#include "global.h"
#include "graphfuncs.h"

using namespace boost;

//float cnct_prob = (float)4/(float)vert_num;
float r = 1, p = 0.25, q = 1.5;
float rate_difference;
//int duration;
//int durationc = 1000000;
float t = 0;
int  runNum = 1000;
//int  runNum = 100;
int infect_num;
int lambda;

int main()
{
	//srand(time(0));

  clock_t begin = clock();
	std::default_random_engine gen(std::random_device{}());
	//std::default_random_engine gen(121);
	std::ofstream fout;
	std::ofstream tout;
	fout.open("cdata.txt");
	//tout.open("timed_data.txt");


	//std::vector<int> n_set={128, 256, 512,1024, 2048, 4096, 8192, 16384};
	std::vector<int> n_set={512};
	std::vector<float> p_set={0.1, 0.2, 0.3, 0.4, 0.5};
	//std::vector<float> p_set={0.8, 0.9, 1};
	std::vector<float> q_set={0.1 ,0.5, 0.8, 1};
	std::vector<float> r_set={0.1 ,0.5, 0.8, 1};
	q_set={1.5};
	//p_set={0.375};
	//q_set={1};
	//p_set={0.25};
	r_set={1};

	fout<<n_set.size()<<"\n";
	fout<<p_set.size()<<"\n";
	fout<<q_set.size()<<"\n";
	fout<<r_set.size()<<"\n";
	fout<<runNum<<"\n";

	//tout << runNum <<'\n';

	for(int nindex=0; nindex<=n_set.size()-1; nindex++)
		fout<<n_set[nindex]<<"\n";
	for(int pindex=0; pindex<=p_set.size()-1; pindex++)
		fout<<p_set[pindex]<<"\n";
	for(int qindex=0; qindex<=q_set.size()-1; qindex++)
		fout<<q_set[qindex]<<"\n";
	for(int rindex=0; rindex<=r_set.size()-1; rindex++)
		fout<<r_set[rindex]<<"\n";

	float dt;
	std::exponential_distribution<float> exp(1);
	std::uniform_real_distribution<float> choice_maker(0.0,1.0);
	I[0].reserve(vert_num*4);
	I[1].reserve(vert_num*4);
	R[0].reserve(vert_num);
	R[1].reserve(vert_num);


	//float cnct_prob = (float)4/(float)vert_num;
	int count = 1;

	for(int nindex=0; nindex<=n_set.size()-1; nindex++)
	{
		vert_num = n_set[nindex];
		//Erdos Renyi constructor.
		//cnct_prob = (float)4/(float)vert_num;
		cons_Erdos(vert_num);
		for (size_t pindex = 0; pindex < p_set.size(); pindex++)
		{
			p = p_set[pindex];
			for (size_t qindex = 0; qindex < q_set.size(); qindex++)
			{
				//rate_difference = p / q;
				q = q_set[qindex];
			/*
			for(int nindex=0; nindex<=n_set.size()-1; nindex++)
			{
				vert_num = n_set[nindex];
				//Erdos Renyi constructor.
				//cnct_prob = (float)4/(float)vert_num;
				cons_Erdos(vert_num);
				*/
				for (size_t run = 0; run < runNum; run++)
				{
					Edge_Num edge_num = num_edges(society);
					init_states();

				  //for (size_t i = 0; t<duration && (R[0].size() + R[1].size() )>0 ; i++)
					for (size_t i = 0; (R[0].size() + R[1].size() )>0 ; i++)
				  {
						lambda = I[0].size() + I[1].size() + R[0].size() + R[1].size();
				    dt = exp(gen);
				    //fout<<dt<<'\n';
						t += (float) dt/lambda;
						auto p0chance = p* (first_I[0].size() );
						auto p1chance = p* (first_I[1].size() );
						auto q0chance = q* I[0].size();
						auto q1chance = q* I[1].size();
						auto r0chance = r * R[0].size();
						auto r1chance = r * R[1].size();
						auto chance_sum = q0chance + q1chance + p0chance + p1chance + r0chance + r1chance;

						auto choice = choice_maker(gen);
						auto alpha = (float) r0chance / ( chance_sum );
						auto beta = (float) ( r0chance + r1chance ) / ( chance_sum ) ;
						auto gamma = (float) ( r0chance + r1chance + q0chance ) / ( chance_sum ) ;
						auto jhi = float ( r0chance + r1chance + q0chance + q1chance ) / ( chance_sum ) ;
						auto leo = float ( r0chance + r1chance + q0chance + q1chance + p0chance ) / ( chance_sum ) ;
						if (choice < alpha )
						{
							Transfer dis = dis_one;
							int locator = int (choice_maker(gen) * R[0].size());
							recover( locator , dis );
						}
						else if ( choice < beta )
						{
							Transfer dis = dis_two;
							int locator = int (choice_maker(gen) * R[1].size());
							recover( locator , dis );
						}

						else if ( choice < gamma )
						{
							//tout <<(float) t << '\n';

							int locator = int (choice_maker(gen) * I[0].size());
							auto e = I[0][locator];
							Transfer dis = dis_one;
							infect (e, dis, 0);
						}
						else if ( choice < jhi )
						{
							//tout <<(float) t << '\n';

							int locator = int (choice_maker(gen) * I[1].size());
							auto e = I[1][locator];
							Transfer dis = dis_two;
							infect (e, dis, 0);
						}
						else if (choice < leo)
						{
							int locator = int (choice_maker(gen) * first_I[0].size());
							auto e = first_I[0][locator];
							Transfer dis = dis_one;
							infect (e, dis, 1);
						}
						else
						{
							int locator = int (choice_maker(gen) * first_I[1].size());
							auto e = first_I[1][locator];
							Transfer dis = dis_two;
							infect (e, dis, 1);
						}

					}
					//tout << -1 << '\n';
					fout << infect_num << '\n';
					if(run % 200 == 0)
						std::cout << "n: " << vert_num << ", p: " << p << ", q: " << q << ", run: " << run << std::endl;
						//std::cout << run << '\n';
				}
			}
		}
	}
	std::cout << "t: " << t << '\n';
	clock_t end = clock();
	float elapsed_secs = float(end - begin) / CLOCKS_PER_SEC;
	std::cout << "time: " << elapsed_secs << '\n';
}
