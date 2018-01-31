/*
 * GetEdge.cpp
 *
 *  Created on: 2017Äê9ÔÂ5ÈÕ
 *      Author: Administrator
 */
#include "CorSM.h"

//ofstream os1("get_frequent_edge_log.txt");
void CorSM::new_vertex(GROUP &group, int cur_scr, int new_scr, int cur_vid, int new_vid){    
	ONE_VER new_vertex;
	ONE_SCR new_edge;

	new_vertex.first = cur_vid;
    new_vertex.second.clear();
    new_edge.first = new_scr;
    new_edge.second.clear();
    new_edge.second.push_back(new_vid);
    new_vertex.second.insert(new_edge);
    group[cur_scr].insert(new_vertex);

	return ;
}

void CorSM::get_frequent_edge(){
	//os1 << "began getting frequent edge" << endl;
	int cur_id, new_id, cur_label, new_label;
	map<pair<int, int>, GROUP> edge_to_group;
	pair<int, int> edge_code;
	GROUP new_group;

	edge_to_group.clear();
	for(cur_id=0; cur_id<=graph.vertex.size(); cur_id++){
		cur_label = graph.vertex[cur_id].label;

		for(int edge_num=0; edge_num<graph.vertex[cur_id].edge.size(); edge_num++){
			new_id = graph.vertex[cur_id].edge[edge_num].to_vid;
			new_label = graph.vertex[new_id].label;
			if(cur_label <= new_label){

				// directly store this edge
				edge_code.first = cur_label;
				edge_code.second = new_label;

				if(edge_to_group.find(edge_code) == edge_to_group.end()){
					//cout<<cur_id<<" "<<new_id<<" "<<cur_label<<" "<<new_label<<"-newone---";
					new_group.clear();
					new_group.resize(2);
                    // create replica for s0
					new_vertex(new_group, 0, 1, cur_id, new_id);
                    new_vertex(new_group, 1, 0, new_id, cur_id);

					edge_to_group.insert(pair<pair<int,int>, GROUP>(edge_code,new_group) );
				}
				else{
					VER_MAP::iterator find_cur = edge_to_group[edge_code][0].find(cur_id);
					if(find_cur == edge_to_group[edge_code][0].end()){
                        new_vertex(edge_to_group[edge_code], 0, 1, cur_id, new_id);

					}
					else{
                        (find_cur)->second[1].push_back(new_id);
					}
					VER_MAP::iterator find_new = edge_to_group[edge_code][1].find(new_id);
                    if(find_new == edge_to_group[edge_code][1].end()){
                        new_vertex(edge_to_group[edge_code], 1, 0, new_id, cur_id);

					}
					else{
                        find_new->second[0].push_back(cur_id);
					}
				}

			}
		}
	}
    map<pair<int, int>, GROUP>::iterator check_it;
    for(check_it=edge_to_group.begin();check_it!=edge_to_group.end();check_it++){
        new_group = check_it->second;
        int center = -1;
        int support = mni_support(new_group, center);
        if(support >= min_sup){
            TreeNode new_node;
            EdgeCode ec(0, 1, check_it->first.first, check_it->first.second);
            new_node.graphcode.push_back(ec);
            is_duplicate(-1, new_node.graphcode, node.size());
            new_node.group = new_group;
            //new_node.index = node.size();
            new_node.center = center;

            pair<int,int> support_and_size(support, -1);
            LEAF new_leaf(support_and_size, node.size());
            leaf.push(new_leaf);
            node.push_back(new_node);
            //cout<<"support->>"<<support<<"-->";
            //new_node.report();
        }
    }

	return ;
}

bool get_forward_edge(){



	return true;
}
bool get_backward_edge(){




	return true;
}



