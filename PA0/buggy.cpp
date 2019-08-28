#include <iostream>
#include <vector> //Blank A

using namespace std; //Blank B

class node {
	public: //Blank C
		int val;
		node* next;
	};

void create_LL(vector<node*>& mylist, int node_num){

	mylist.assign(node_num, NULL);

	//create a set of nodes
	for (int i = 0; i < node_num; i++) {
		mylist[i] = new node;//Blank D
		mylist[i]->val = i;
		mylist[i]->next = NULL;
	}

	//create a linked list
	for (int i = 0; i < node_num-1; i++) {
		mylist[i]->next = mylist[i+1];
	}
	mylist[node_num-1]->next = NULL;
}

int add_LL(node* ptr){
	int ret = 0;
	while(ptr) {
		ret += ptr->val;
		ptr = ptr->next;
	}
	return ret;
}

int main(int argc, char ** argv){
	const int NODE_NUM = 3;
	vector<node*> mylist;

	create_LL(mylist, NODE_NUM);
	int ret = add_LL(mylist[0]);
	cout << "The sum of nodes in LL is " << ret << endl;

	//Step4: delete nodes
	for (int i = 0; i < NODE_NUM; i++){ //Blank E
		delete mylist[i];
	}
}