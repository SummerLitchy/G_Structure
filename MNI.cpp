/*
 * MNI.cpp
 *
 *  Created on: 2017��9��7��
 *      Author: Administrator
 */
#include "CorSM.h"
int CorSM::mni_support(const GROUP &g, int &center){
	int min = 0xffffffff;
	for(int i=0; i<g.size(); i++){

		if(min > g[i].size()){
			center = i;
			min = g[i].size();
		}

	}

	return min;
}



