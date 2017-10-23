#include <iostream>
#include <random>
#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/properties.hpp>
#include <boost/config.hpp>
#include "SIR.h"
#include "global.h"
using namespace boost;

//typedef adjacency_list<listS, vecS, undirectedS, SIR, Interaction> Network;
//typedef adjacency_list<listS, vecS, undirectedS, SIR> Network;
typedef adjacency_list<vecS, vecS, undirectedS, SIR> Network;
typedef graph_traits<Network>::edges_size_type Edge_Num;
typedef graph_traits<Network>::vertices_size_type Vertex_Num;
typedef graph_traits<Network>::vertex_iterator Vertex_iter;
typedef graph_traits<Network>::vertex_descriptor Vertex;
typedef graph_traits<Network>::edge_descriptor Edge;
typedef graph_traits<Network>::edge_iterator Edge_iter;



/*
class Interaction
{
	public:
	int present = 1;
};
*/

//typedef adjacency_list<listS, vecS, undirectedS, SIR, Interaction> Network;
Vertex_Num vert_num = 128;
//Vertex_Num vert_num = 10;
Network society(vert_num);

float cnct_prob = (float)4/(float)vert_num;
float r, p = 0.25, q = 1;
int duration = 1000;
double t;
//std::set <int> actives={};
std::vector<std::vector<int> > I;
std::vector<std::vector<int> > R;


void init_states()
{
	for( Vertex vd : make_iterator_range( vertices(society) ) )
	{
		society[vd].health = 1;
		society[vd].future = 1;
	}
	int seed = rand() % vert_num;
	//int seed = 2;
	Vertex v = vertex(seed, society);
	society[v].health = 6;
	society[v].future = 6;
	//society_origin = Network(society);
	actives = {};
	actives.insert(seed);
}

int main()
{
  clock_t begin = clock();
  //std::default_random_engine generator(std::random_device{}());
  std::default_random_engine generator;
  std::ofstream fout;
	fout.open("cdata.txt");
  double dt;
  std::exponential_distribution<double> exp(1);
	//Network society_origin;
	float cnct_prob = (float)4/(float)vert_num;

	 // Erdos Renyi constructor.
		for (size_t i = 0; i < vert_num; i++)
		{
			for (size_t j = i+1; j < vert_num; j++)
			{
				if ( dice(cnct_prob) )
				{
					add_edge(i, j, society);
					//std::cout << i << "-->" << j << '\n';
				}
			}
		}


  for (size_t i = 0; t<= duration; i++)
  {
    dt = exp(generator);
    fout<<dt<<'\n';
		t += dt;
  }

	SIR patient;
	//patient.health *= 2;
	//int jafar = a;
	//std::cout << jafar << '\n';

  std::cout << "END" << '\n';
  clock_t end = clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//std::cout << "time: " << elapsed_secs << '\n';
	std::cout << "t: " << t << '\n';
	boost::print_graph(society);

	//for ( Vertex vi : make_iterator_range( adjacent_vertices(0, society) ) )
	for ( Edge vi : make_iterator_range( in_edges(0, society) ) )
	{
		//society[vi].turn_I( society[vd].supply() );
		std::cout << vi << '\n';
	}


}
