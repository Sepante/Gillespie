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



Vertex_Num vert_num = 1024;
//Vertex_Num vert_num = 10;
Network society(vert_num);

float cnct_prob = (float)4/(float)vert_num;
float r = 1, p = 0.25, q = 20;
int duration = 1000;
float t = 0;
int infect_num;
int lambda;
//std::vector<Edge> I2;
//std::vector<Edge> I3;
std::vector<int> R2;
std::vector<int> R3;


void init_states()
{
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
	//I2 = {};
	//I3 = {};
	R2 = {};
	R3 = {};
	R2.push_back(seed);
	R3.push_back(seed);
	/*for ( Vertex vi : make_iterator_range( adjacent_vertices(seed, society) ) )
	for ( Edge vi : make_iterator_range( out_edges(seed, society) ) )
	{
		I2.push_back(vi);
		I3.push_back(vi);
	}*/
}

void recover(int v, Transfer dis)
{
		if(dis == 2)
		{
		society[R2[v]].turn_R(dis);
		remove_at(R2, v);
		}

		else if(dis == 3)
		{
		society[R3[v]].turn_R(dis);
		remove_at(R3, v);
		}
		std::cout << "infect_num: " << infect_num << '\n';


/*
	for ( Edge e : make_iterator_range( out_edges(v, society) ) )
	{
		auto it = std::find(R2.begin(), R2.end(), e);
		remove_at(R2, it);
		std::cout << vi << '\n';
	}*/
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
			}
		}


	}

int main()
{
  clock_t begin = clock();
	//I2.reserve(vert_num*4);
	//I3.reserve(vert_num*4);
	R2.reserve(vert_num);
	R3.reserve(vert_num);
  std::default_random_engine gen(std::random_device{}());
  //std::default_random_engine gen;
  std::ofstream fout;
	fout.open("cdata.txt");
  float dt;
  std::exponential_distribution<float> exp(1);
	std::uniform_real_distribution<float> choice_maker(0.0,1.0);


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

	Edge_Num edge_num = num_edges(society);
	init_states();

  for (size_t i = 0; t<duration && (R2.size() + R3.size() )>0 ; i++)
  {
		lambda = edge_num + R2.size() + R3.size();
    dt = exp(gen);
    //fout<<dt<<'\n';
		t += (float) dt/lambda;

		auto choice = choice_maker(gen);
		auto alpha = (float) r * R2.size() / (q* edge_num + r * R2.size() + r * R3.size() );
		auto beta = (float) (r * R2.size() + r * R3.size() ) / (q* edge_num + r * R2.size() + r * R3.size() ) ;
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

		else
		{
			fout <<(float) t << '\n';
			auto e = random_edge(society, gen);

			Transfer dis;

			if (dice(0.5))
				dis = dis_one;
			else
				dis = dis_two;
			infect (e, dis);

		}
		//active sites:
		/*
		std::cout << '\n';
		for (size_t i = 0; i < R2.size(); i++)
			std::cout << i << " R2: " << R2[i] << ": " << society[R2[i]].health << '\n';

		std::cout << "******" << '\n';
		for (size_t i = 0; i < R3.size(); i++)
			std::cout << i << " R3: " << R3[i] << ": " << society[R3[i]].health << '\n';
			*/
  }

	std::cout << "t: " << t << '\n';
	clock_t end = clock();
	float elapsed_secs = float(end - begin) / CLOCKS_PER_SEC;
	std::cout << "time: " << elapsed_secs << '\n';

  std::cout << "END" << '\n';
}
