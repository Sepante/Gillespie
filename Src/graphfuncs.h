#ifndef GRAPHFUNCS_H
#define GRAPHFUNCS_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edge_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/random.hpp>
#include <boost/config.hpp>

#include "SIR.h"
using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS, SIR> Network;

typedef graph_traits<Network>::edges_size_type Edge_Num;
typedef graph_traits<Network>::vertices_size_type Vertex_Num;
typedef graph_traits<Network>::vertex_iterator Vertex_iter;
typedef graph_traits<Network>::vertex_descriptor Vertex;
typedef graph_traits<Network>::edge_descriptor Edge;
typedef graph_traits<Network>::edge_iterator Edge_iter;

Vertex_Num vert_num;
Network society(vert_num);

std::vector<std::vector<Edge> > I(2);
std::vector<std::vector<Edge> > first_I(2);
std::vector<std::vector<int> > R(2);

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

void init_states()
{
	t = 0;
	//duration = durationc;
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


#endif
