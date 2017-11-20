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


#endif
