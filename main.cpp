/*
 * main.cpp
 *
 *  Created on: 2017Äê9ÔÂ7ÈÕ
 *      Author: Administrator
 */
#include "CorSM.h"
#include <ctime>
int main(){








	string filepath1 = "./../Datasets/chemical compound/chemical.txt";
	string filepath2= "./../Datasets/tiny.txt";
	string filepath3 = "./../Datasets/DBLP/DBLP_Graph.txt";
	string filepath4 = "./../Datasets/Protein_Function/Yeast_Function_Graph.txt";
	string filepath5= "./../Datasets/cycle.txt";
	/*string path;
	cout << "Please enter the file path in Datasets: " << flush;

	cin >> path;
	filepath += path;*/
	/*int file_ord, minsup, topk, hop;
	string filepath;
	cin >> file_ord >> minsup >> topk >> hop;
	switch(file_ord){
	case 1: filepath = filepath1;break;
	case 2: filepath = filepath2;break;
	case 3: filepath = filepath3;break;
	case 4: filepath = filepath4;break;
	}
	cout<<filepath<<endl;*/
	cout<<"???"<<endl;
	//CorSM coco(filepath3, 5000, 300, 0);
	//CorSM coco(filepath4, 200, 50, 1);
	//CorSM coco(filepath5, 1, 20, 0);
	//CorSM coco(filepath2, 1, 50, 0);
	CorSM coco(filepath1, 12, 50, 0);
	ofstream timeof("timing.txt");
	time_t now_time;
	now_time = time(NULL);
	timeof<<"initialization begin-->"<<now_time<<endl;
	//CorSM coco(filepath3, 30000, 100, 0);
	coco.initialize();

	now_time = time(NULL);
	timeof<<"initialization end-->"<<now_time<<endl;
	coco.search();

	now_time = time(NULL);

	timeof<<"search end-->"<<now_time<<endl;
	timeof.close();
	cout<<"search end"<<endl;
	return 0;
}


