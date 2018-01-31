/*
 * TreeNode.cpp
 *
 *  Created on: 2017Äê9ÔÂ4ÈÕ
 *      Author: Administrator
 */
#include "CorSM.h"

void GraphCode::build_rmpath(){
	rmpath.clear();
	rmpath.push_back(size());
	//cout<<size()<<endl;
	int last_from = (*this)[size()-1].b_script;
	for(int code_num=size()-2; code_num>=0; code_num--){
		if((*this)[code_num].e_script == last_from){
			rmpath.push_back((*this)[code_num].e_script);
			last_from = (*this)[code_num].b_script;
		}
	}
	rmpath.push_back(0);
	return ;
}
void TreeNode::build_rmpath(){
	rmpath.clear();
	rmpath.push_back(graphcode.size());
	int last_from = graphcode[graphcode.size()-1].b_script;
	for(int code_num=graphcode.size()-2; code_num>=0; code_num--){
		if(graphcode[code_num].e_script == last_from){
			rmpath.push_back(graphcode[code_num].e_script);
			last_from = graphcode[code_num].b_script;
		}
	}
	rmpath.push_back(0);
	return ;
}
void TreeNode::report(){
	cout<<"{{{";
	for(int i=0; i<graphcode.size(); i++){
		cout << "(" << graphcode[i].b_script << ',' << graphcode[i].e_script << ',';
		cout << graphcode[i].b_label << ',' << graphcode[i].e_label << ")";
	}
	cout<<"}}}"<<endl;
	/*for(int i=0; i<group.size();i++){
		cout<<"{";
		for(VER_MAP::iterator it=group[i].begin();it!=group[i].end();it++){
			cout<<"<"<<it->first<<", <";
			for(SCR_MAP::iterator k=it->second.begin(); k!=it->second.end(); k++){
				cout<<k->first<<"(";
				for(int j=0;j<k->second.size();j++){
					cout<<k->second[j]<<",";
				}
				cout<<")";
			}
			cout<<">";
		}
		cout<<"}"<<endl;
	}*/
}
