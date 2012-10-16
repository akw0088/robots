#include "include.h"
#include "heap.h"

class Graph
{
public:
	Graph();
	void load(node_t *node, int num_nodes);
	int insert(node_t *node);
	int remove(int i);
	float *dijkstra(int start);
	int *dijkstra_path(int start, int end, int *path_length);
	int *astar_path(ref_t *ref, int start, int end, int *path_length);
	int modify_weight(int seq1, int seq2, float weight);
	~Graph();
private:
	int	num_nodes;
	node_t	*node;
};
