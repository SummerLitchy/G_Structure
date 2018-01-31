/*
 * Graph.cpp
 *
 *  Created on: 2017Äê9ÔÂ4ÈÕ
 *      Author: Administrator
 */
#include "CorSM.h"


void Graph::build_graph(string filename){
	ifstream is(filename.c_str());
	ofstream os("build_graph_log.txt");
	if(!is){
		cout << "file not open" << endl;
		exit(0);
	}
	else{
		os << "file open successfully" << endl;

		vid_to_index.clear();
		int e_id = 0;
		int v_id = 0;
		set<int> dislabel;
		while(!is.eof()){
			char tmp[30];

			is.getline(tmp, 30);
			if(strlen(tmp) < 5){
				os << strlen(tmp) << " " << tmp << endl;
				os << "data is x,x,x ???" <<endl;
				cout << "wrong data" << endl;
				exit(0);
			}
			if(tmp[0] == 'v' && tmp[1] == ' '){
				string vid;
				int vlabel;
				string s = "";;
				int i = 2;
				for(;i<strlen(tmp); i++){
					if(tmp[i] != ' '){
						s += tmp[i];
					}
					else{
						vid = s;
						s = "";
					}
				}
				vlabel = atoi(s.c_str());
				vid_to_index.insert(pair<string, int>(vid, v_id) );
				Vertex ver(vid, vlabel);
				if(dislabel.find(vlabel) == dislabel.end())	dislabel.insert(vlabel);
				vertex.push_back(ver);
				//os << "id " << vid << " vlabel "<< vlabel<<" vindex "<< v_id<< endl;
				v_id += 1;
				// assume vid is ordered in txt file
			}
			else if(tmp[0] == 'e' && tmp[1] == ' '){
				string begin_id, end_id;
				string s = "";
				int i = 2;
				for(; i<strlen(tmp); i++){
					if(tmp[i] != ' '){
						s += tmp[i];
					}
					else{
						begin_id = s;
						s = "";
						i++;
						break;
					}
				}
				for(; i<strlen(tmp); i++){
					if(tmp[i] != ' '){
						s += tmp[i];
						if(i == strlen(tmp)-1){
							end_id = s;
							s = ""; //no label dataset
						}
					}
					else{
						end_id = s;
						s = ""; // ignore labal dataset
						break; //for edge match, delete this break
					}
				}

				//e_id = atoi(s.c_str()); //for edge match only
				int bid = vid_to_index[begin_id];
				int eid = vid_to_index[end_id];
				Edge s_to_b(e_id, bid, eid);
				Edge b_to_s(e_id, eid, bid);
				e_id ++;
				//os << "beginid " << bid << " endid"<< eid << endl;
				/*
				if(vertex[begin_id].edge[end_id].v_id.empty()){
					vertex[begin_id].edge[end_id] = new Edgelist;
				}*/
				int begin_id_label = vertex[bid].label;
				int end_id_label = vertex[eid].label;
				vertex[bid].edge.push_back(s_to_b);
				vertex[eid].edge.push_back(b_to_s);
			}
			else{
				//os << "not a vertex or edge format " << tmp << endl;
			}
		}
		// for test
		//os << "graph successfully built" << endl;
		/*for(int i=0; i<vertex.size(); i++){
			for(int j=0; j<vertex[i].edge.size(); j++){
				os << "vertex " << i << " to vertex "<< vertex[i].edge[j].to_vid << endl;
			}

		}*/
		os<<"vertex size--"<<vertex.size()<<endl;
		os<<"edge size--"<<e_id<<endl;
		os<<"dislabel--"<<dislabel.size()<<endl;
	}
	is.close();
	os.close();
	return ;

}




