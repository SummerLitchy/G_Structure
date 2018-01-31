/*
 * calculate_correlation.cpp
 *
 *  Created on: 2017Äê9ÔÂ28ÈÕ
 *      Author: Administrator
 */
#include "CorSM.h"
set<int> CorSM::bfs_from_id(int vid){	
	set<int> cor_ver;

	queue<int> go;
	queue<int> next_go;
	int cur_id, new_id, cur_label, new_label;
	while(!go.empty())	go.pop();
	while(!next_go.empty())	next_go.pop();
	cor_ver.clear();
	go.push(vid);
	cor_ver.insert(vid);
	for(int i=0; i<hop; i++){

		while(!next_go.empty())	next_go.pop();
		while(!go.empty()){
			cur_id = go.front();
			cur_label = graph.vertex[cur_id].label;
			go.pop();
			for(int edge_num=0; edge_num<graph.vertex[cur_id].edge.size(); edge_num++){
				new_id = graph.vertex[cur_id].edge[edge_num].to_vid;
				new_label = graph.vertex[new_id].label;
				if(cor_ver.find(new_id) == cor_ver.end()){
					next_go.push(new_id);
					cor_ver.insert(new_id);
				}
			}
		}
		go = next_go;

	}
	return cor_ver;

}

void CorSM::distance_process(){
	//system("time");
	int cur_id;
	//set<int> final_edge;
	set<int> cur_ver;	
	int correlation;
	for(int index=0; index<node.size(); index++){		

		for(int scr=0; scr<node[index].group.size(); scr++){

			if(node[index].group.empty()){
				cout<<"empty group???"<<endl; exit(0);
			}
			else if(node[index].group.size()!=2){
				cout<<"not 2 vertices during initialization???"<<endl; exit(0);
			}

			VER_MAP::iterator it;
			for(it=node[index].group[scr].begin(); it!=node[index].group[scr].end();it++){
				cur_id = it->first;
				if(dis_index.find(cur_id) == dis_index.end()){
					ONE_INDEX new_pair;
					new_pair.first = bfs_from_id(cur_id);
					dis_index.insert(INDEX(cur_id, new_pair) );
				}
			}
		}
	}
}

void CorSM::calculate_cor(int index){
	// update the cor_pattern first

	for(int scr=0; scr<node[index].group.size(); scr++){
		VER_MAP::iterator it;
		set<int>::iterator cor_v;
		for(it=node[index].group[scr].begin(); it!=node[index].group[scr].end();it++){
			int cur_id = it->first;
			node[index].vivo.insert(pair<int, set<int> >(cur_id, set<int>() ) );
			for(cor_v=dis_index[cur_id].first.begin(); cor_v!=dis_index[cur_id].first.end(); cor_v++){
				dis_index[*cor_v].second.insert(index);
			}
		}
	}

	VER_MAP::iterator v_it;
	int cen = node[index].center;
	for(v_it=node[index].group[cen].begin(); v_it!=node[index].group[cen].end(); v_it++){
		collect(node[index].group, -1, cen, v_it->first, index);
	}
	t_ara(node[index].group, cen, index);
	return ;

}
set<int> CorSM::collect(const GROUP &g, int from_s, int s, int vid, int index)
{

	if(!node[index].vivo[vid].empty())	return node[index].vivo[vid];

	VER_MAP::const_iterator v_it;
	v_it = g[s].find(vid);
	set<int> this_set = dis_index[v_it->first].second;

	if(v_it->second.size() == 1 && v_it->second.begin()->first == from_s){
		return this_set;
	}
	node[index].vivo[vid] = dis_index[v_it->first].second;
	SCR_MAP::const_iterator s_it;
	for(s_it=v_it->second.begin(); s_it!=v_it->second.end(); s_it++){


		if(s_it->first != from_s){
			for(int i=0; i<s_it->second.size(); i++){
				int new_vid = s_it->second[i];
				set<int> tmp = collect(g, s, s_it->first, new_vid, index);
				set_union(node[index].vivo[vid].begin(), node[index].vivo[vid].end(),
						tmp.begin(), tmp.end(), inserter(node[index].vivo[vid], node[index].vivo[vid].begin() ) );

			}

		}
	}
	return node[index].vivo[vid];
}
void CorSM::t_ara(const GROUP &g, int center, int index){
	VER_MAP::const_iterator v_it;
	vector<int> stat(node.size(), 0);

	set<int>::const_iterator it;
	for(v_it=g[center].begin(); v_it!=g[center].end(); v_it++){

		set<int> tmps = node[index].vivo[v_it->first];
		for(it=node[index].vivo[v_it->first].begin(); it!=node[index].vivo[v_it->first].end(); it++){
			stat[*it] += 1;

		}
	}
	//cout<<"node size-->"<<node.size()<<endl;
	//cout<<"queue size-->"<<top_k_queue.size()<<endl;
	for(int j=0; j<cor_index.size(); j++){
		//cout<<"see-->"<<stat[i]<<endl;
		int i = cor_index[j];
		//cout<<"i-->"<<i<<endl;
		if(node[index].sub.find(i) != node[index].sub.end() || i == index || stat[i] < min_sup)	continue;
		if(top_k_queue.size() == k && stat[i] >= top_k_queue.top().first){
			top_k_queue.pop();
			CORRELATION cor(stat[i], pair<int,int>(i, index));
			top_k_queue.push(cor);
		}
		else if(top_k_queue.size() < k){
			CORRELATION cor(stat[i], pair<int,int>(i, index));
			top_k_queue.push(cor);
		}
	}

	return ;
}
/*
void CorSM::no_9(const GROUP &g, int center, int index){
	// cal
	VER_MAP::const_iterator v_it;
	set<int>::iterator it1,it2;
	map<pair<int,int>, int> stat;
	for(v_it=g[center].begin(); v_it!=g[center].end(); v_it++){

		set<int> &tmps = node[index].vivo[v_it->first];
		for(it1=tmps.begin(); it1!=tmps.end(); it1++){
			for(it2=it1; it2!=tmps.end(); it2++){
				if(it2==it1) continue;
				int g2 = *it1, g3 = *it2;
				if(stat.find(pair<int,int>(g2,g3)) == stat.end()){
					stat.insert(pair<pair<int,int>, int>(pair<int,int>(g2,g3), 1));
				}
				else{
					stat[pair<int,int>(g2,g3)] += 1;
				}
			}
		}
	}

	//stat
	map<pair<int,int>, int>::iterator m_it;
	for(m_it=stat.begin(); m_it!=stat.end(); m_it++){
		int g1 = m_it->first.first, g2 = m_it->first.second;
		if(node[index].sub.find(g1) != node[index].sub.end() ||node[index].sub.find(g2) != node[index].sub.end() || m_it->second < min_sup)	continue;
		if(top_k_group_queue.size() == k && m_it->second >= top_k_group_queue.top().first){
			top_k_group_queue.pop();
			G3_CORRELATION tmp;
			tmp.first = m_it->second;
			tmp.second.g1 = index;
			tmp.second.g2 = g1; tmp.second.g3 = g2;
			top_k_group_queue.push(tmp);
		}
		else if(top_k_queue.size() < k){
			G3_CORRELATION tmp;
			tmp.first = m_it->second;
			tmp.second.g1 = index;
			tmp.second.g2 = g1; tmp.second.g3 = g2;
			top_k_group_queue.push(tmp);
		}

	}


	return ;
}
*/
