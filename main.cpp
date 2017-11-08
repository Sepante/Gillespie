#include <iostream>
#include <algorithm>
#include <random>
#include <fstream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edge_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/random.hpp>
#include <boost/config.hpp>

#include "SIR.h"
#include "global.h"

using namespace boost;


class Interaction
{
public:
	int trans = 0;
};
//typedef adjacency_list<listS, vecS, undirectedS, SIR, Interaction> Network;
//typedef adjacency_list<listS, vecS, undirectedS, SIR> Network;

typedef adjacency_list<vecS, vecS, undirectedS, SIR, Interaction> Network;
//typedef adjacency_list<vecS, vecS, bidirectionalS, SIR, Interaction> Network;

typedef graph_traits<Network>::edges_size_type Edge_Num;
typedef graph_traits<Network>::vertices_size_type Vertex_Num;
typedef graph_traits<Network>::vertex_iterator Vertex_iter;
typedef graph_traits<Network>::vertex_descriptor Vertex;
typedef graph_traits<Network>::edge_descriptor Edge;
typedef graph_traits<Network>::edge_iterator Edge_iter;

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


Vertex_Num vert_num;
Network society(vert_num);

//float cnct_prob = (float)4/(float)vert_num;
float r = 1, p = 0.25, q = 1.5;
float rate_difference;
int duration;
int durationc = 1000000;
float t = 0;
int  runNum = 1000;
//int  runNum = 100;
int infect_num;
int lambda;

std::vector<std::vector<Edge> > I(2);
std::vector<std::vector<Edge> > first_I(2);
std::vector<std::vector<int> > R(2);


void init_states()
{
	t = 0;
	duration = durationc;
	for( Vertex vd : make_iterator_range( vertices(society) ) )
	{
		society[vd].health = 1;
		society[vd].future = 1;
	}
	infect_num = 1;
	int seed = rand() % vert_num;
	//int seed = 0;
	Vertex v = vertex(seed, society);
	society[v].health = 6;
	society[v].future = 6;
	//society[v].health = 2;
	//society[v].future = 2;
	//society_origin = Network(society);
	first_I[0] = {};
	first_I[1] = {};
	I[0] = {};
	I[1] = {};
	R[0] = {};
	R[1] = {};
	R[0].push_back(seed);
	R[1].push_back(seed);
	//for ( Vertex vi : make_iterator_range( adjacent_vertices(seed, society) ) )
	for ( Edge vi : make_iterator_range( out_edges(seed, society) ) )
	{
		//I[0].push_back(vi);
		//I[1].push_back(vi);
		first_I[0].push_back(vi);
		first_I[1].push_back(vi);
	}
}

void recover(int v, Transfer dis)
{
	int vert;
	vert = R[dis - 2][v];
	society[R[dis - 2][v]].turn_R(dis);
	remove_at(R[dis - 2], v);
		//std::cout << vert << '\n';

	for ( Edge ei : make_iterator_range( out_edges(vert, society) ) )
	{
		//std::cout << ei << '\n';
		if( society[target(ei, society)].demand() % dis == 0 ) //if this edge was in action list
		{
			if (society[target(ei, society)].health != 1)
			{
				auto it = std::find(I[dis - 2].begin(), I[dis - 2].end(), ei);

				if (it != I[dis - 2].end())
				{
					// swap the one to be removed with the last element
					// and remove the item at the end of the container
					// to prevent moving all items after 'ei' by one
					std::swap(*it, I[dis - 2].back());
					I[dis - 2].pop_back();
				}
			}
			else //target is in state S
			{
				auto it = std::find(first_I[dis - 2].begin(), first_I[dis - 2].end(), ei);

				if (it != first_I[dis - 2].end())
				{
					// swap the one to be removed with the last element
					// and remove the item at the end of the container
					// to prevent moving all items after 'ei' by one
					std::swap(*it, first_I[dis - 2].back());
					first_I[dis - 2].pop_back();
				}
			}

		}
	}


}

void infect(Edge e, Transfer dis, bool first_infect)
{
	//bool first_infect = false;
	if ( society[source(e, society)].supply() % dis == 0 && society[target(e, society)].demand() % dis == 0 )
	{
		if ( first_infect )
		{
			//change for SIS
			infect_num++;
		}
			//needs to get fixed.
		if (true)
		{
			int target_v = target(e, society);
			//std::cout << "targ: " << target(e, society) << '\n';
			R[dis - 2].push_back(target(e, society));
			society[target(e, society)].future *= dis;
			society[target(e, society)].update();
			//the node can transmit the disease now.
			for ( Edge ei : make_iterator_range( out_edges(target_v, society) ) )
			{
				if( society[target(ei, society)].demand() % dis == 0 )
					if (society[target(ei, society)].health != 1)
						I[dis - 2].push_back(ei);
					else //the neighbor is S.
						first_I[dis - 2].push_back(ei);
			}
			//the node won't accept the disease anymore.
			for ( Edge ei : make_iterator_range( in_edges(target_v, society) ) )
			{
				if( society[source(ei, society)].supply() % dis == 0 )
				{
					if( !first_infect )
					{
						auto it = std::find(I[dis - 2].begin(), I[dis - 2].end(), ei);
						if (it != I[dis - 2].end())
						{
							std::swap(*it, I[dis - 2].back());
							I[dis - 2].pop_back();
						}
					}
					else //first_infect
					{
						auto it = std::find(first_I[dis - 2].begin(), first_I[dis - 2].end(), ei);
						if (it != first_I[dis - 2].end())
						{
							std::swap(*it, first_I[dis - 2].back());
							first_I[dis - 2].pop_back();
						}
					}

				}

			}
			if(first_infect)
			{
				auto other_dis = ( (dis+1) % 2 ) + 2;
				for ( Edge ei : make_iterator_range( in_edges(target_v, society) ) )
				{
					//cut every item from this guy in the first_I vector and paste it to the I vector.
					if( society[source(ei, society)].supply() % other_dis == 0 )
					{
						auto it = std::find(first_I[other_dis - 2].begin(), first_I[other_dis - 2].end(), ei);
						if (it != first_I[other_dis - 2].end())
						{
							std::swap(*it, first_I[other_dis - 2].back());
							first_I[other_dis - 2].pop_back();

							I[other_dis - 2].push_back(ei);
						}
						//std::cout << I[dis - 2].size() << '\n';
						//std::cout << "****" << '\n';
					}


				}
			}
		}
	}
}

void cons_Erdos(int n)
{
	float cnct_prob = (float)4/(float)n;
	society.clear();
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = i+1; j < n; j++)
		{
			if ( dice(cnct_prob) )
			{
				add_edge(i, j, society);
				//add_edge(j, i, society);
			}
		}
	}
	//std::cout << "Erdos running" << '\n';
}

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
	std::vector<int> n_set={16384};
	std::vector<float> p_set={0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
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
