/*
 * is_duplicate.cpp
 *
 *  Created on: 2017Äê9ÔÂ9ÈÕ
 *      Author: Administrator
 */
#include "CorSM.h"
void code_to_graph(const GraphCode code, Graph &g);

bool operator==(const EdgeCode &c1, const EdgeCode &c2){
	return (c1.b_script == c2.b_script && c1.e_script == c2.e_script
			&& c1.b_label == c2.b_label && c1.e_label == c2.e_label);

}
bool operator!=(const EdgeCode &c1, const EdgeCode &c2){
	return !(c1 == c2);

}
bool operator<(const EdgeCode &c1, const EdgeCode &c2){
	if(c1.b_script <= c2.b_script){
		if(c1.b_script < c2.b_script)	return false;
		else if(c1.e_script >= c2.e_script){
			if(c1.e_script > c2.e_script)	return false;
			else if(c1.b_label >= c2.b_label){
				if(c1.b_label > c2.b_label)	return false;
				else if(c1.e_label < c2.e_label)	return true;
				else return false;

			}
			else return true;

		}
		else return true;

	}
	else return true;
	return false;
}
bool operator>(const EdgeCode &c1, const EdgeCode &c2){
	return !(c1 == c2) && !(c1 < c2);
}
bool operator<(const GraphCode &c1, const GraphCode &c2){
	// c1, c2 must have same size
	for(int i=0; i<c1.size(); i++){
		if(c1[i] < c2[i])
			return true;
		else if(c2[i] < c1[i])
			return false;
	}
	return false;
}
ofstream oss("dup.txt");
static long dup_cnt;
static int cnt;
GraphCode to_min(const GraphCode code){
	Graph g;
	code_to_graph(code, g);
	//set<int> overlap_id;
	// we get g as data graph now
	GraphCode min_code;
	EdgeCode cur_min(-1,23333,23333,23333);

	vector<vector<int> > inst;
	vector<vector<int> > new_inst;
	vector<int> per_inst;
	// get forward edge first
	// condition different with get frequent edge
	// must consider edges with two same labels
	int cur_id, cur_label, new_id, new_label, cur_scr, new_scr, min_cur_scr, min_new_scr, min_new_label;
	GraphCode cur_code;
	min_code.clear();

	cur_code.clear();

	for(cur_id=0; cur_id<g.vertex.size(); cur_id++){

		cur_label = g.vertex[cur_id].label;

		for(int edge_num=0; edge_num<g.vertex[cur_id].edge.size(); edge_num++){
			dup_cnt++;
			new_id = g.vertex[cur_id].edge[edge_num].to_vid;
			new_label = g.vertex[new_id].label;
			if(cur_label <= new_label){
				// two same labels
				EdgeCode ec(0, 1, cur_label, new_label);
				per_inst.clear();

				if(ec < cur_min){
					inst.clear();
					per_inst.push_back(cur_id);
					per_inst.push_back(new_id);
					inst.push_back(per_inst);
					cur_min.set(0, 1, cur_label, new_label);
					if(!min_code.empty()){
						min_code.pop_back();
					}
					min_code.push_back(cur_min);
				}
				else if(ec == cur_min){
					per_inst.push_back(cur_id);
					per_inst.push_back(new_id);
					inst.push_back(per_inst);
				}

			}
		}
	}
	// begin extend from frequent edges

	map<int, int> id_to_script;


	// build history of this code

	set<pair<int, int> > exist_edge_script;


	while(min_code.size() < code.size()){
		bool back_exist = 0;
		int forward_exist = 23333;
		min_new_scr = 23333;
		min_new_label = 23333;
		cur_min.set(-1,23333,23333,23333);
		for(int i=0; i<min_code.size(); i++){
			if(min_code[i].b_script<min_code[i].e_script){
				exist_edge_script.insert(pair<int, int>(min_code[i].b_script, min_code[i].e_script));
			}
			else{
				exist_edge_script.insert(pair<int, int>(min_code[i].e_script, min_code[i].b_script));
			}
		}
		min_code.build_rmpath();
		/*for(int i=0;i<min_code.rmpath.size();i++) cout<<min_code.rmpath[i]<<",";
		cout<<"---->rmpath"<<endl;*/
		new_inst.clear();
		for(int inst_num=0; inst_num<inst.size(); inst_num++){
			//overlap_id.clear();
			id_to_script.clear();

			for(int v_num=0; v_num<inst[0].size(); v_num++){
				id_to_script.insert(pair<int, int>(inst[inst_num][v_num], v_num));
				//overlap_id.insert(inst[inst_num][v_num]);
			}

			//check backward edge
			cur_scr=min_code.rmpath[0]; // the last node
			cur_id = inst[inst_num][cur_scr];
			cur_label = g.vertex[cur_id].label;
			for(int edge_num=0; edge_num<g.vertex[cur_id].edge.size(); edge_num++){
				// for every vertex check its edge if exist this edge
				dup_cnt++;
				new_id = g.vertex[cur_id].edge[edge_num].to_vid;
				new_label = g.vertex[new_id].label;
				if(id_to_script.find(new_id) != id_to_script.end()){
					new_scr = id_to_script[new_id];
				}
				else{
					continue;
				}
				if(cur_scr > new_scr){
					if(exist_edge_script.find(pair<int, int>(new_scr, cur_scr)) != exist_edge_script.end()){
						// existed edge
						continue;
					}
					//backward edge

					back_exist = 1;
					// forward edge extension could not affect min_new_scr because it always > this in backward edge extension
					if(new_scr < min_new_scr || (new_scr == min_new_scr && new_label < min_new_label)){
						new_inst.clear();
						per_inst.clear();
						per_inst = inst[inst_num];
						per_inst.push_back(new_id);
						new_inst.push_back(per_inst);

						cur_min.set(cur_scr, new_scr, cur_label, new_label);


						min_new_scr = id_to_script[new_id];
						min_cur_scr = cur_scr;
						min_new_label = new_label;
					}
					else if(new_scr == min_new_scr && new_label == min_new_label){
						per_inst.clear();
						per_inst = inst[inst_num];
						per_inst.push_back(new_id);
						inst.push_back(per_inst);
					}
				}
			}
			// if no backward edge yet, try forward edge
			if(!back_exist){
				for(int v_num=0; v_num<min_code.rmpath.size(); v_num++){

					cur_scr=min_code.rmpath[v_num]; // from last node to first node, through rmpath
					cur_id = inst[inst_num][cur_scr];
					cur_label = g.vertex[cur_id].label;
					for(int edge_num=0; edge_num<g.vertex[cur_id].edge.size(); edge_num++){
						// for every vertex check its edge if exist this edge
						dup_cnt++;
						new_id = g.vertex[cur_id].edge[edge_num].to_vid;
						new_label = g.vertex[new_id].label;
						if(id_to_script.find(new_id) != id_to_script.end()){
							continue;
						}
						else{
							new_scr = inst[0].size();
						}
						EdgeCode new_edgecode(cur_scr, new_scr, cur_label, new_label);
						if(new_edgecode < cur_min){

							cur_min.set(cur_scr, new_scr, cur_label, new_label);

							new_inst.clear();
							per_inst.clear();
							per_inst = inst[inst_num];
							per_inst.push_back(new_id);
							new_inst.push_back(per_inst);

							forward_exist = v_num;
							min_new_scr = new_scr;
							min_cur_scr = cur_scr;
						}
						else if(new_edgecode == cur_min){
							per_inst.clear();
							per_inst = inst[inst_num];
							per_inst.push_back(new_id);
							new_inst.push_back(per_inst);
						}
					}
					if(v_num >= forward_exist)// we have found the deeper one in DFS
						break;

				}

			}

		}
		inst.clear();
		inst = new_inst;
		min_code.push_back(cur_min);

	}

	//min_code.report();
	cnt+=1;
	if(cnt%50000==0)
		oss<<dup_cnt<<endl;
	return min_code;
}

void code_to_graph(const GraphCode code, Graph &g){
	int b_id, e_id, b_label, e_label;
	for(int edge_num=0; edge_num<code.size(); edge_num++){
		b_id = code[edge_num].b_script;
		e_id = code[edge_num].e_script;
		b_label = code[edge_num].b_label;
		e_label = code[edge_num].e_label;


		if(b_id >= g.vertex.size()){
			Vertex v("0", b_label);
			g.vertex.push_back(v);
		}
		if(e_id >= g.vertex.size()){
			Vertex v("0", e_label);
			g.vertex.push_back(v);
		}
		Edge b_edge(0, b_id, e_id);
		Edge e_edge(0, e_id, b_id);

		g.vertex[b_id].edge.push_back(b_edge);
		g.vertex[e_id].edge.push_back(e_edge);

	}

	return ;
}
bool CorSM::is_duplicate(int max_index, GraphCode code, int index){
	// check if this code in dictionary

	EdgeDict* p = dict_head;

	//cout<<"wocao"<<endl;
	//cout<<"size is "<< code.size()<<endl;

	for(int i=0;i<code.size();i++){


		if(p->next.find(code[i]) != p->next.end()){// could remove this if trust the input
			if(i == code.size()-1){
				//cout<<p->next.find(code[i])->second->index<<" size--->"<<node.size()<<endl;
				//cout<<"find in index -->"<<p->next.find(code[i])->second->index<<endl;
				if(p->next.find(code[i])->second->index != -1){
					set_union(node[p->next.find(code[i])->second->index].sub.begin(),
							node[p->next.find(code[i])->second->index].sub.end(),
							node[max_index].sub.begin(), node[max_index].sub.end(),
							inserter(node[p->next.find(code[i])->second->index].sub,
									node[p->next.find(code[i])->second->index].sub.begin()));
					node[p->next.find(code[i])->second->index].sub.insert(max_index);
					//cout<<"sup cor avoiding-->"<<max_index<<" "<<p->next.find(code[i])->second->index<<endl;
				}

				//cout<<"found"<<endl;
				return true;
			}
			p = p->next.find(code[i])->second;
		}
		else{
			/*if(i != code.size()-1){
				cout<<"not anti-monotone!!"<<endl;
				for(map<EdgeCode, EdgeDict*>::iterator it=p->next.begin();it!=p->next.end();it++){
					EdgeCode ecc = (*it).first;
					//ecc.report();
				}
				cout<<"i   is---> "<<i<<endl;
			}*/
			//cout<<"could not find canon"<<endl;
			if(index != -1){
				// index=0xfffffff, just check, not insert
				EdgeDict* new_Dict = new EdgeDict;
				new_Dict->index = index;
				p->next.insert(pair<EdgeCode, EdgeDict*>(code[i], new_Dict));
				//cout<< "inserted index"<<endl;
			}
			return false;

		}
	}

	return false;
}
