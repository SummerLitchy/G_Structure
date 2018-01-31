/*
 * CorSM.cpp
 *
 *  Created on: 2017Äê9ÔÂ4ÈÕ
 *      Author: Administrator
 */
#include "CorSM.h"

ofstream os("main_log.txt");
ofstream seetime("timestamp.txt");
CorSM::CorSM(string filename, int min_sup, int k, int hop){

	this->min_sup = min_sup;
	this->k = k;
	this->hop = hop;
	dict_head = NULL;
	//code_dictionary = NULL;

	//leaf_set.clear();
	dis_index.clear();
	cor_index.clear();
	graph.build_graph(filename);

}
void CorSM::initialize(){
	os << "initialization began" << endl;
	dict_head = new EdgeDict;
	get_frequent_edge();
	distance_process();
}



void CorSM::search(){
	os << "search began" << endl;
	while(!(leaf.empty() || (top_k_queue.size() == k &&
			top_k_queue.top().first >= leaf.top().first.first) ) ){
		elect_and_extend();

		/*if(node.size() >= 60){
			cout << "60 nodes are found" << endl;
			break;
		}*/
	}
	while(!top_k_queue.empty()){
		pair<int, pair<int, int> > rep = top_k_queue.top();
		if(top_k_queue.size() <= 5){
			node[rep.second.first].report();
			node[rep.second.second].report();
		}

		os <<"support -->" << rep.first << "   index" << rep.second.first <<"," << rep.second.second<<endl;
		top_k_queue.pop();
	}

	return ;
}




void CorSM::elect_and_extend(){
	//use leaf_node as parameter
	//following if is only for test, no ceasing condition
	int best = leaf.top().second;
	os<<"best is index-->"<<best<<", support-->"<<leaf.top().first.first<<endl;
	cout<<"best is index-->"<<best<<endl;
	leaf.pop();
	calculate_cor(best);
	cor_index.push_back(best);
	node[best].graphcode.build_rmpath();
	for(int rm=0; rm<node[best].graphcode.rmpath.size(); rm++){
		int cur_scr = node[best].graphcode.rmpath[rm];
		if(rm == 0){
			try_extend_from_last(best, cur_scr);
		}
		try_extend(best, cur_scr);
	}

	return ;
}
void CorSM::try_extend(int index, int from){
	VER_MAP::iterator ver_it;

	GROUP &g = node[index].group;
	vector<set<int> > ban;
	ban.resize(g[from].size());
	//set<int> *ban = new set<int>[g[from].size()];
	map<int,int> ex_map;
	int cur_label;
	int cnt=0;
	for(ver_it=g[from].begin(); ver_it!=g[from].end(); ver_it++){
		//set banned
		ban[cnt].clear();
		SCR_MAP::iterator scr_it;
		set_ban(g, ban[cnt], -1, from, ver_it->first);
		/*for(scr_it=ver_it->second.begin(); scr_it!=ver_it->second.end(); scr_it++){

			for(int v=0; v<scr_it->second.size(); v++){
				ban[cnt].insert(scr_it->second[v]);
			}
		}*/
		//os<<"size set --> "<< ban[cnt].size()<<endl;
		cnt++;
	}
	cnt=0;
	//os<<"set ban end"<<endl;
	for(ver_it=g[from].begin(); ver_it!=g[from].end(); ver_it++){
		int cur_v = ver_it->first;
		cur_label = graph.vertex[cur_v].label;
		int new_v, new_label;
		vector<Edge> &cur_edge = graph.vertex[cur_v].edge;
		for(int edge=0; edge<cur_edge.size(); edge++){
			new_v = cur_edge[edge].to_vid;
			if(ban[cnt].find(new_v) != ban[cnt].end())	continue;
			new_label = graph.vertex[new_v].label;
			if(ex_map.find(new_label) == ex_map.end()){
				ex_map.insert(pair<int,int>(new_label,1));
			}
			else{
				ex_map[new_label] += 1;
			}
		}

		cnt++;
	}
	// stat
	map<int,int>::iterator ex;
	set<int> valid_label;
	for(ex=ex_map.begin(); ex!=ex_map.end(); ex++){
		int new_label = ex->first;
		EdgeCode ec(from, g.size(), cur_label, new_label);
		GraphCode new_c = node[index].graphcode;
		new_c.push_back(ec);
		GraphCode min_c = to_min(new_c);
		if(is_duplicate(index, min_c, 0xffffffff)) continue;

		if(ex->second >= min_sup) valid_label.insert(ex->first);
	}
	map<int, GROUP> group;

	cnt = 0;
	for(ver_it=g[from].begin(); ver_it!=g[from].end(); ver_it++){

		int cur_v = ver_it->first;
		int new_v, new_label;
					/*os<<"cur v is-->"<<cur_v<<endl;
					for(set<int>::iterator itt=ban[cnt].begin(); itt!=ban[cnt].end(); itt++) os<<*itt<<",";
					os<<endl;*/
		vector<Edge> &ccur_edge = graph.vertex[cur_v].edge;
		for(int edge=0; edge<ccur_edge.size(); edge++){
			new_v = ccur_edge[edge].to_vid;
			//os<<"new_v -->"<<new_v<<endl;
			if(ban[cnt].find(new_v) != ban[cnt].end())	continue;
			new_label = graph.vertex[new_v].label;
			map<int,int> _;
			if(valid_label.find(new_label) != valid_label.end()){
				if(group.find(new_label) == group.end()){
					GROUP new_group;
					new_group.clear();

					new_group.resize(g.size()+1);
					new_vertex(new_group, g.size(), from, new_v, cur_v);
					// copy other into this new group from g

					copy_group(g, new_group, from, (*ver_it), g.size(), new_v);

					ONE_SCR new_scr(g.size(), vector<int>(1,new_v));
					//if(new_group[from].find(cur_v)==new_group[from].end()) cout<<"Waaaaa!"<<endl;
					new_group[from].find(cur_v)->second.insert(new_scr);
					// check MNI
					group.insert(pair<int, GROUP>(new_label, new_group));
				}
				else{
					VER_MAP::iterator find_it = group[new_label][g.size()].find(new_v);
					if(find_it == group[new_label][g.size()].end()){
						new_vertex(group[new_label], g.size(), from, new_v, cur_v);
						copy_group(g, group[new_label], from, (*ver_it), g.size(), new_v);
						if(group[new_label][from].find(cur_v) == group[new_label][from].end()){
							cout<<"!!!"<<endl;
						}
						if(group[new_label][from].find(cur_v)->second.find(g.size())
								== group[new_label][from].find(cur_v)->second.end()){
							cout << "wo cha le zenme meiyou!" <<endl;
							ONE_SCR new_scr(g.size(), vector<int>(1,new_v));
							group[new_label][from].find(cur_v)->second.insert(new_scr);
						}
						//else group[new_label][from].find(cur_v)->second.find(g.size())->second.push_back(new_v);
					}
				}
			}
		}
		cnt++;
	}
	map<int, GROUP>::iterator g_it;
	for(g_it=group.begin(); g_it!=group.end(); g_it++){

		int center = -1;
		int support = mni_support(g_it->second, center);
		if(support >= min_sup){
			TreeNode new_node;
			EdgeCode ec(from, node[index].group.size(), cur_label, g_it->first);
			//cout<<g.size()<<" middle 1  cao!"<<endl;
			new_node.graphcode = node[index].graphcode;
			new_node.graphcode.push_back(ec);

			GraphCode min_c = to_min(new_node.graphcode);
			//min_c.report();
			is_duplicate(-1, min_c, node.size());
			new_node.group = g_it->second;
			//new_node.index = node.size();
			new_node.center = center;
			new_node.sub = node[index].sub;
			new_node.sub.insert(index);
			pair<int,int> support_and_size(support, -new_node.graphcode.size());
			LEAF new_leaf(support_and_size, node.size());
			leaf.push(new_leaf);
			//cout<<g.size()<<" middle 2  cao!"<<endl;
			node.push_back(new_node);

			//new_node.report();
			//cout<<"support -->"<<support<<endl;
			//cout<<node[index].group.size()<<" middle 3  cao!"<<endl;
		}
		//cout<<g.size()<< " end cao!"<<endl;
	}
	//cout<<"extend seg-------"<<endl;

	//g.clear();
	return ;
}

void CorSM::try_extend_from_last(int index, int from){
	// set back & banned
	GROUP &g = node[index].group;

	VER_MAP::iterator v_it;
	vector<map<int,int> > back;
	back.resize(g[from].size());
	int cnt=0;

	for(v_it=g[from].begin(); v_it!=g[from].end(); v_it++){
		//set banned
		back[cnt].clear();
		SCR_MAP::iterator scr_it;

		for(scr_it=v_it->second.begin(); scr_it!=v_it->second.end(); scr_it++){
			for(int v=0; v<scr_it->second.size(); v++){
				back[cnt].insert(pair<int,int>(scr_it->second[v], -1));
			}
		}
		set_back(g, back[cnt], -1, from, v_it->first);
		//os<<"size set --> "<< ban[cnt].size()<<endl;
		cnt++;
	}
	cnt=0;

	int cur_label;

	map<int,int> ex_map;
	vector<int> scr_to_label;
	scr_to_label.resize(g.size());
	for(v_it=g[from].begin(); v_it!=g[from].end(); v_it++){
		int cur_v = v_it->first;
		cur_label = graph.vertex[cur_v].label;

		int new_v, new_label;
		vector<Edge> &cur_edge = graph.vertex[cur_v].edge;
		for(int edge=0; edge<cur_edge.size(); edge++){
			new_v = cur_edge[edge].to_vid;

			map<int,int>::iterator mi;
			mi = back[cnt].find(new_v);
			if(mi == back[cnt].end() || mi->second == -1)	continue;
			new_label = graph.vertex[new_v].label;
			int new_scr = mi->second;
			scr_to_label[new_scr] = new_label;
			if(ex_map.find(new_scr) == ex_map.end()){
				ex_map.insert(pair<int,int>(new_scr,1));
			}
			else{
				ex_map[new_scr] += 1;
			}
		}

		cnt++;
	}
	// stat
	map<int,int>::iterator ex;
	set<int> valid_scr;
	for(ex=ex_map.begin(); ex!=ex_map.end(); ex++){
		int new_label = scr_to_label[ex->first];
		EdgeCode ec(from, g.size(), cur_label, new_label);
		GraphCode new_c = node[index].graphcode;
		new_c.push_back(ec);
		GraphCode min_c = to_min(new_c);
		if(is_duplicate(index, min_c, -1)) continue;

		if(ex->second >= min_sup) valid_scr.insert(ex->first);
	}

	map<int, GROUP> group;
	cnt=0;
	for(v_it=g[from].begin(); v_it!=g[from].end(); v_it++){
		int cur_v = v_it->first;
		vector<Edge> &cur_e = graph.vertex[cur_v].edge;
		for(int edge_n=0; edge_n<cur_e.size(); edge_n++){
			int new_v = cur_e[edge_n].to_vid;
			map<int,int>::iterator it = back[cnt].find(new_v);
			if(it != back[cnt].end()){
				int new_scr = it->second;
				if(valid_scr.find(new_scr) != valid_scr.end()){
					if(group.find(new_scr) == group.end()){
						GROUP new_group;
						new_group.clear();
							new_group.resize(g.size());
							// copy other into this new group from g
							ONE_VER new_vertex;
							new_vertex.first = new_v;
							new_group[new_scr].insert(new_vertex);
							SCR_MAP::iterator s_it;
							VER_MAP::iterator new_v_it;
							new_v_it = g[new_scr].find(new_v);

							for(s_it=new_v_it->second.begin(); s_it!=new_v_it->second.end(); s_it++){
								VER_MAP::iterator tmp_v_it;
									if(new_group[new_scr].begin()->second.find(s_it->first)
											== new_group[new_scr].begin()->second.end()){
										ONE_SCR tmp_scr(s_it->first, vector<int>());
										new_group[new_scr].begin()->second.insert(tmp_scr);
									}
								if(s_it->first < new_scr){
									for(int k=0; k<s_it->second.size(); k++){
										int tmp_vid = s_it->second[k];
										tmp_v_it = g[s_it->first].find(tmp_vid);
										copy_group(g, new_group, s_it->first, (*tmp_v_it), new_scr, tmp_vid);
										new_group[new_scr].begin()->second.find(s_it->first)->second.push_back(tmp_vid);
									}
								}
								else{
									if(back[cnt].find(new_v) == back[cnt].end())	continue;
									for(int k=0; k<s_it->second.size(); k++){
										int tmp_vid = s_it->second[k];
										tmp_v_it = g[s_it->first].find(tmp_vid);
										copy_group(g, new_group, s_it->first, (*tmp_v_it), new_scr, tmp_vid);
										new_group[new_scr].begin()->second.find(s_it->first)->second.push_back(tmp_vid);
									}
								}

							}

							group.insert(pair<int, GROUP>(new_scr, new_group));
					}
					else{
						assert(!group[new_scr][new_scr].empty());
							VER_MAP::iterator find_it = group[new_scr][new_scr].find(new_v);
							if(find_it == group[new_scr][g.size()].end()){
								ONE_VER new_vertex;
								new_vertex.first = new_v;
								group[new_scr][new_scr].insert(new_vertex);
								SCR_MAP::iterator s_it;
								VER_MAP::iterator new_v_it;
								new_v_it = g[new_scr].find(new_v);

								for(s_it=new_v_it->second.begin(); s_it!=new_v_it->second.end(); s_it++){
									VER_MAP::iterator tmp_v_it;
										if(group[new_scr][new_scr].find(new_v)->second.find(s_it->first)
												== group[new_scr][new_scr].find(new_v)->second.end()){
											ONE_SCR tmp_scr(s_it->first, vector<int>());
											group[new_scr][new_scr].find(new_v)->second.insert(tmp_scr);
										}
									if(s_it->first < new_scr){
										for(int k=0; k<s_it->second.size(); k++){
											int tmp_vid = s_it->second[k];
											tmp_v_it = g[s_it->first].find(tmp_vid);
											copy_group(g, group[new_scr], s_it->first, (*tmp_v_it), new_scr, tmp_vid);
											group[new_scr][new_scr].find(new_v)->second.find(s_it->first)->second.push_back(tmp_vid);
										}
									}
									else{
										if(back[cnt].find(new_v) == back[cnt].end())	continue;
										for(int k=0; k<s_it->second.size(); k++){
											int tmp_vid = s_it->second[k];
											tmp_v_it = g[s_it->first].find(tmp_vid);
											copy_group(g, group[new_scr], s_it->first, (*tmp_v_it), new_scr, tmp_vid);
											group[new_scr][new_scr].find(new_v)->second.find(s_it->first)->second.push_back(tmp_vid);
										}
									}

								}
							}
					}
				}
			}
		}
	}
	map<int, GROUP>::iterator g_it;
	for(g_it=group.begin(); g_it!=group.end(); g_it++){

		int center = -1;
		int support = mni_support(g_it->second, center);
		if(support >= min_sup){
			TreeNode new_node;
			EdgeCode ec(from, node[index].group.size(), cur_label, g_it->first);
			//cout<<g.size()<<" middle 1  cao!"<<endl;
			new_node.graphcode = node[index].graphcode;
			new_node.graphcode.push_back(ec);
			GraphCode min_c = to_min(new_node.graphcode);
			min_c.report();
			is_duplicate(-1, min_c, node.size());

			new_node.group = g_it->second;
			//new_node.index = node.size();
			new_node.center = center;
			new_node.sub = node[index].sub;
			new_node.sub.insert(index);
			pair<int,int> support_and_size(support, -new_node.graphcode.size());
			LEAF new_leaf(support_and_size, node.size());
			leaf.push(new_leaf);
			//cout<<g.size()<<" middle 2  cao!"<<endl;
			node.push_back(new_node);

			//new_node.report();
			//cout<<"support -->"<<support<<endl;
			//cout<<node[index].group.size()<<" middle 3  cao!"<<endl;
		}
		//cout<<g.size()<< " end cao!"<<endl;
	}
	return ;
}

void CorSM::copy_group(const GROUP &old_g, GROUP &new_g,
		int s, ONE_VER deemo, int from_s, int from_id){

	if(new_g[s].find(deemo.first) != new_g[s].end()){
		new_g[s].find(deemo.first)->second.find(from_s)->second.push_back(from_id);
		return ;
	}
	if(deemo.second.find(from_s) == deemo.second.end()){
		ONE_SCR new_scr(from_s, vector<int>(1,from_id));
		deemo.second.insert(new_scr);
	}
	else{
		deemo.second.find(from_s)->second.clear();
		deemo.second.find(from_s)->second.push_back(from_id);
	}


	new_g[s].insert(deemo);

	SCR_MAP::iterator it;
	for(it=deemo.second.begin(); it!=deemo.second.end(); it++){
		if(it->first != from_s){
			int new_s = it->first;
			for(int v=0; v<it->second.size(); v++){
				int vid = it->second[v];
				/*VER_MAP::iterator new_v = ;
				new_v = old_g[new_s].find(vid);*/
				if(old_g[new_s].find(vid)==old_g[new_s].end()){
					cout<<"xxxx"<<endl;
					cout<<new_s<<"("<<vid<<endl;

				}

				copy_group(old_g, new_g, new_s, (*old_g[new_s].find(vid)), s, deemo.first);
			}

		}
	}
	return ;
}

void CorSM::copy_back(const GROUP &old_g, GROUP &new_g,
		int s, ONE_VER deemo, int from_s, int from_id, const map<int,int> &b_map){

	if(deemo.second.find(from_s) == deemo.second.end()){
		ONE_SCR new_scr(from_s, vector<int>(1,from_id));
		deemo.second.insert(new_scr);
	}
	else{
		deemo.second.find(from_s)->second.clear();
		deemo.second.find(from_s)->second.push_back(from_id);
	}


	new_g[s].insert(deemo);

	SCR_MAP::iterator it;
	for(it=deemo.second.begin(); it!=deemo.second.end(); it++){
		if(it->first != from_s){
			int new_s = it->first;
			for(int v=0; v<it->second.size(); v++){
				int vid = it->second[v];
				/*VER_MAP::iterator new_v = ;
				new_v = old_g[new_s].find(vid);*/
				if(old_g[new_s].find(vid)==old_g[new_s].end()){
					cout<<"xxxx"<<endl;
					cout<<new_s<<"("<<vid<<endl;

				}

				copy_group(old_g, new_g, new_s, (*old_g[new_s].find(vid)), s, deemo.first);
			}

		}
	}
	return ;
}


void CorSM::set_ban(const GROUP &g, set<int> &b_set, int from_s, int to_s, int vid){
	b_set.insert(vid);
	VER_MAP::const_iterator v_it;
	v_it = g[to_s].find(vid);
	SCR_MAP::const_iterator s_it;
	for(s_it=v_it->second.begin(); s_it!=v_it->second.end(); s_it++){
		if(s_it->first != from_s){
			for(int i=0; i<s_it->second.size(); i++){
				set_ban(g, b_set, to_s, s_it->first, s_it->second[i]);
			}
		}
	}

	return ;
}
void CorSM::set_back(const GROUP &g, map<int,int> &b_map, int from_s, int to_s, int vid){
	if(b_map.find(vid) == b_map.end())
		b_map.insert(pair<int,int>(vid, to_s));
	VER_MAP::const_iterator v_it;
	v_it = g[to_s].find(vid);
	SCR_MAP::const_iterator s_it;
	for(s_it=v_it->second.begin(); s_it!=v_it->second.end(); s_it++){
		if(s_it->first != from_s){
			for(int i=0; i<s_it->second.size(); i++){
				set_back(g, b_map, to_s, s_it->first, s_it->second[i]);
			}
		}
	}

	return ;
}
