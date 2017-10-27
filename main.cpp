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
typedef adjacency_list<vecS, vecS, bidirectionalS, SIR, Interaction> Network;
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



Vertex_Num vert_num = 40000;
//Vertex_Num vert_num = 10;
Network society(vert_num);

float cnct_prob = (float)4/(float)vert_num;
float r = 1, p = 0.25, q = 2;
int duration;
int durationc = 1000;
float t = 0;
int  runNum = 1;
int infect_num;
int lambda;
//std::vector<Edge> I2;
//std::vector<Edge> I3;
std::vector<std::vector<Edge> > I(2);
std::vector<int> R2;
std::vector<int> R3;


void init_states()
{
	duration = durationc;
	for( Vertex vd : make_iterator_range( vertices(society) ) )
	{
		society[vd].health = 1;
		society[vd].future = 1;
	}
	infect_num = 1;
	//int seed = rand() % vert_num;
	int seed = 0;
	Vertex v = vertex(seed, society);
	society[v].health = 6;
	society[v].future = 6;
	//society_origin = Network(society);
	I[0] = {};
	I[1] = {};
	R2 = {};
	R3 = {};
	R2.push_back(seed);
	R3.push_back(seed);
	//for ( Vertex vi : make_iterator_range( adjacent_vertices(seed, society) ) )
	for ( Edge vi : make_iterator_range( out_edges(seed, society) ) )
	{
		I[0].push_back(vi);
		I[1].push_back(vi);
	}
}

void recover(int v, Transfer dis)
{
	int vert;
	if(dis == 2)
	{
		vert = R2[v];
		society[R2[v]].turn_R(dis);
		remove_at(R2, v);
	}

	else if(dis == 3)
	{
		vert = R3[v];
		society[R3[v]].turn_R(dis);
		remove_at(R3, v);
	}


		//std::cout << vert << '\n';

	for ( Edge ei : make_iterator_range( out_edges(vert, society) ) )
	{
		//std::cout << ei << '\n';
		if( society[target(ei, society)].demand() % dis == 0 )
		{
			//std::cout << "in_state" << '\n';
			//std::cout << I[dis - 2].size() << '\n';
			auto it = std::find(I[dis - 2].begin(), I[dis - 2].end(), ei);
			//std::cout << *it << '\n';

			if (it != I[dis - 2].end())
			{
			  // swap the one to be removed with the last element
			  // and remove the item at the end of the container
			  // to prevent moving all items after 'ei' by one
			  std::swap(*it, I[dis - 2].back());
			  I[dis - 2].pop_back();
			}
			//std::cout << I[dis - 2].size() << '\n';
			//std::cout << "****" << '\n';

		}
	}


}

	void infect(Edge e, Transfer dis)
	{
		bool change = false;
		if ( society[source(e, society)].supply() % dis == 0 && society[target(e, society)].demand() % dis == 0 )
		{
			if (society[target(e, society)].health != 1)
				change = true;
			else if (dice(0.25) )
			{
					change = true;
					infect_num++;
			}

			if (change)
			{
				int target_v = target(e, society);
				//std::cout << "targ: " << target(e, society) << '\n';
				if (dis == dis_one)
				{
					R2.push_back(target(e, society));

				}
				else if (dis == dis_two)
				{
					R3.push_back(target(e, society));

				}

				society[target(e, society)].future *= dis;
				society[target(e, society)].update();
				//std::cout << "original:  " << society[target(e, society)].health << '\n';
				//std::cout << "duplicate: " << society[target_v].health << '\n';

				for ( Edge ei : make_iterator_range( out_edges(target_v, society) ) )
				{
					if( society[target(ei, society)].demand() % dis == 0 )
					{
							I[dis - 2].push_back(ei);
					}
				}
				for ( Edge ei : make_iterator_range( in_edges(target_v, society) ) )
				{
					if( society[source(ei, society)].supply() % dis == 0 )
					{
						//std::cout << "in style: " << '\n';
						//std::cout << I[dis - 2].size() << '\n';
						auto it = std::find(I[dis - 2].begin(), I[dis - 2].end(), ei);
						//for (size_t i = 0; i < I[dis - 2].size(); i++) {
//							std::cout << I[dis - 2][i] << '\n';
						//}

						if (it != I[dis - 2].end())
						{
							// swap the one to be removed with the last element
							// and remove the item at the end of the container
							// to prevent moving all items after 'ei' by one
							std::swap(*it, I[dis - 2].back());
							I[dis - 2].pop_back();
						}
						//std::cout << I[dis - 2].size() << '\n';
						//std::cout << "****" << '\n';
					}

				}


			}
		}


	}

int main()
{
	//srand(time(0));

  clock_t begin = clock();
	std::default_random_engine gen(std::random_device{}());
	//std::default_random_engine gen(121);
	std::ofstream fout;
	fout.open("cdata.txt");
	float dt;
	std::exponential_distribution<float> exp(1);
	std::uniform_real_distribution<float> choice_maker(0.0,1.0);
	I[0].reserve(vert_num*4);
	I[1].reserve(vert_num*4);
	R2.reserve(vert_num);
	R3.reserve(vert_num);


	float cnct_prob = (float)4/(float)vert_num;

  //Erdos Renyi constructor.

	for (size_t i = 0; i < vert_num; i++)
	{
		for (size_t j = i+1; j < vert_num; j++)
		{
			if ( dice(cnct_prob) )
			{
				add_edge(i, j, society);
				add_edge(j, i, society);
			}
		}
	}
	std::cout << "Erdos runnin" << '\n';

		for (size_t x=0; x<=0; x++)
		{
			for (size_t run = 0; run < runNum; run++)
			{
			Edge_Num edge_num = num_edges(society);
			init_states();

		  for (size_t i = 0; t<duration && (R2.size() + R3.size() )>0 ; i++)
		  {
				lambda = I[0].size() + I[1].size() + R2.size() + R3.size();
		    dt = exp(gen);
		    //fout<<dt<<'\n';
				t += (float) dt/lambda;

				auto choice = choice_maker(gen);
				auto alpha = (float) r * R2.size() / (q* (I[0].size() + I[1].size()) + r * R2.size() + r * R3.size() );
				auto beta = (float) (r * R2.size() + r * R3.size() ) / (q* (I[0].size() + I[1].size()) + r * R2.size() + r * R3.size() ) ;
				auto gamma = (float) (r * R2.size() + r * R3.size() + q* (I[0].size()) ) / (q* (I[0].size() + I[1].size()) + r * R2.size() + r * R3.size() ) ;
				if (choice < alpha )
				{
					Transfer dis = dis_one;
					int locator = int (choice_maker(gen) * R2.size());
					recover( locator , dis );

				}
				else if ( choice < beta )
				{
					Transfer dis = dis_two;
					int locator = int (choice_maker(gen) * R3.size());
					recover( locator , dis );
				}

				else if ( choice < gamma )
				{
					fout <<(float) t << '\n';

					int locator = int (choice_maker(gen) * I[0].size());
					auto e = I[0][locator];
					Transfer dis = dis_one;
					infect (e, dis);
				}
				else
				{
					fout <<(float) t << '\n';

					int locator = int (choice_maker(gen) * I[1].size());
					auto e = I[1][locator];
					Transfer dis = dis_two;
					infect (e, dis);
				}
				std::cout << "infect_num: " << infect_num << '\n';
				//active sites:
				/*
				std::cout << '\n';
				for (size_t i = 0; i < R2.size(); i++)
					std::cout << i << " R2: " << R2[i] << ": " << society[R2[i]].health << '\n';
					*/

				//std::cout << "******" << '\n';
				//for (size_t i = 0; i < I[0].size(); i++)
					//std::cout << I[0] << '\n';

				}
			}
		}
	std::cout << "t: " << t << '\n';
	clock_t end = clock();
	float elapsed_secs = float(end - begin) / CLOCKS_PER_SEC;
	std::cout << "time: " << elapsed_secs << '\n';
	std::cout << "size: " << I[0].size() << '\n';
	std::cout << "size: " << I[1].size() << '\n';
	std::cout << num_edges(society) << '\n';
  std::cout << "END" << '\n';
}
