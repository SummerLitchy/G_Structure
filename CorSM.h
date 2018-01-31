/*
 * CorSM.h
 *
 *  Created on: 2017Äê9ÔÂ4ÈÕ
 *      Author: Administrator
 */

#ifndef CORSM_H_
#define CORSM_H_

#include<iostream>
#include<fstream>
#include<vector>
#include<map>
#include<set>
#include<queue>
#include<cstring>
#include<cstdlib>
#include<algorithm>
#include<assert.h>

using namespace std;



struct Edge{
	int id;
	int from_vid;
	int to_vid;
	Edge(int id, int bid, int eid){
		this->id = id;
		from_vid = bid;
		to_vid = eid;
	}
};

struct Vertex{
	string id;
	int label;

	vector<Edge> edge;
	Vertex(string vid, int vlabel){
		id = vid;
		label = vlabel;

	}
};


struct EdgeCode{
	int b_script;
	int e_script;
	int b_label;
	int e_label;
	EdgeCode(){}
	EdgeCode(int bs, int es, int bl, int el){
        b_script = bs;
		e_script = es;
		b_label = bl;
		e_label = el;
	}
	void set(int bs, int es, int bl, int el){
		b_script = bs;
		e_script = es;
		b_label = bl;
		e_label = el;
	}
	void report(){
		cout<<b_script<<","<<e_script<<","<<b_label<<","<<e_label<<endl;
	}
};
struct EdgeDict: public EdgeCode{
	map<EdgeCode, EdgeDict*> next;
	int index;
	EdgeDict(){
		index = -1;
		next.clear();
	}
};

struct GraphCode: public vector<EdgeCode>{
	void build_rmpath();

	vector<int> rmpath;
	void report(){
		cout<<"{{{";
		for(int i=0; i<size(); i++){
			cout << "(" << (*this)[i].b_script << ',' << (*this)[i].e_script << ',';
			cout << (*this)[i].b_label << ',' << (*this)[i].e_label << ")";
		}
		cout<<"}}}"<<endl;
	}
};

struct Graph{
	vector<Vertex> vertex;
	map<string, int> vid_to_index;

	void build_graph(string filename);

};
typedef vector<map<int, map<int, vector<int> > > > GROUP;
typedef map<int, map<int, vector<int> > > VER_MAP;
typedef pair<int, map<int, vector<int> > > ONE_VER;
typedef map<int, vector<int> > SCR_MAP;
typedef pair<int, vector<int> > ONE_SCR;
typedef pair<pair<int,int>, int> LEAF;

typedef map<int, pair<set<int>, set<int> > > INDEX_MAP;
typedef pair<int, pair<set<int>, set<int> > > INDEX;
typedef pair<set<int>, set<int> > ONE_INDEX;

struct TreeNode{
	int support;
	int center;
	GraphCode graphcode;
	GROUP group;
	map<int, set<int> > collection;
	vector<int> rmpath;
	set<int> sub;
	//int index;
	int father;
	map<int, set<int> > vivo;

public:
	void elect_and_extend(); //use leaf_node as parameter
	void build_rmpath();



	void report();
};

GraphCode to_min(const GraphCode code);

bool operator==(const EdgeCode &c1, const EdgeCode &c2);
bool operator<(const EdgeCode &c1, const EdgeCode &c2);
bool operator<(const GraphCode &c1, const GraphCode &c2);

//

bool get_forward_edge();
bool get_backward_edge();
//static long dup_cnt;
struct THREE{
	int g1,g2,g3;
};

class CorSM{

	//EdgeDict* code_dictionary;
	EdgeDict* dict_head;
	//map<int, pair<int, int> > edge_index;
	Graph graph;
	int min_sup;
	int hop;
	int k;

	long same;
	long opposite;
	long opedge;
	long samenewedge;
	long opnewedge;
	long edge_iter;
	long ex_cnt;
    typedef pair<int, pair<int, int> > CORRELATION;

    typedef pair<int, THREE> G3_CORRELATION;

	vector<int> cor_index;
	map<int, pair<set<int>, set<int> > > dis_index;

	priority_queue<CORRELATION, vector<CORRELATION>,
		greater<CORRELATION> > top_k_queue;

	/*priority_queue<G3_CORRELATION, vector<G3_CORRELATION>,
			greater<G3_CORRELATION> > top_k_group_queue;*/

	vector<TreeNode> node;
	priority_queue<pair<pair<int, int>, int> > leaf;

public:
	CorSM(string filename, int min_sup, int k, int hop);
	void initialize();
	void search();

    void new_vertex(GROUP &group, int cur_scr, int new_scr, int cur_vid, int new_vid);
	void get_frequent_edge();

	bool is_duplicate(int max_index, GraphCode code, int index);

	void distance_process();
	set<int> bfs_from_id(int vid);
	void calculate_cor(int index);

	set<int> collect(const GROUP &g, int from_s, int s, int vid, int index);
	void t_ara(const GROUP &g, int center, int index);

	void no_9(const GROUP &g, int center, int index);


	int mni_support(const GROUP &g, int &center);

	void elect_and_extend();
	void try_extend(int index, int from);
	void try_extend_from_last(int index, int from);

	void copy_group(const GROUP &old_g, GROUP &new_g,
			int s, ONE_VER deemo, int from_s, int from_id);
	void copy_back(const GROUP &old_g, GROUP &new_g,
			int s, ONE_VER deemo, int from_s, int from_id, const map<int,int> &b_map);


	void copy_group_back(const GROUP &old_g, GROUP &new_g,
			int s, ONE_VER deemo, int from_s, int from_id, const map<int,int> b_set);

	void set_ban(const GROUP &g, set<int> &b_set, int from_s, int to_s, int vid);
	void set_back(const GROUP &g, map<int,int> &b_map, int from_s, int to_s, int vid);
};



#endif /* CORSM_H_ */
