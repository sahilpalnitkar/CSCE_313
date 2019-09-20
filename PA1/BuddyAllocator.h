#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */

class BlockHeader{
public:
	// think about what else should be included as member variables
    bool free;
	int block_size;  // size of the block
	BlockHeader* next; // pointer to the next block
    BlockHeader* buddy;
	bool isBuddy;
//    BlockHeader(){
//        next = NULL;
//        buddy = NULL;
//        free = true;
//        cout<<"BH const"<<endl;
//    }
};

class LinkedList{ //add constructor that sets the head to NULL
	// this is a special linked list that is made out of BlockHeader s. 
public:
	BlockHeader* head;		// you need a head of the list
    int length;
	void insert (BlockHeader* b){	// adds a block to the list
        BlockHeader* temp = head;
        if(head==NULL){
			head = b;
			cout<<head<<endl;
			return;
			}

        while(temp->next!=NULL){
            temp = temp->next;
        }
        temp->next = b;
	}

	void remove (BlockHeader* b){  // removes a block from the list
        if(head==NULL){
			cerr<<"Nothing"<<endl;
			return;
			}

        else if(head==b){
            head=b->next;
			b->next = NULL;
            return;
        }
		else{
			BlockHeader* temp = head;
			while(temp->next != b){
				temp = temp->next;
			}
			if (temp->next->next != NULL){
				temp->next = b->next;
				b->next = NULL;
				return;
			
			}
			else{
				temp->next = NULL;
				b->next = NULL;
			}
        }
        
	}
    
    LinkedList(){
        length = 0;
        head = NULL;
    }
};


class BuddyAllocator{
private:
	/* declare more member variables as necessary */
	vector<LinkedList> FreeList;
	int basic_block_size;
	int total_memory_size;
    char* start;
	char* memory;

private:
	/* private function you are required to implement
	 this will allow you and us to do unit test */
 
	
	bool arebuddies (BlockHeader* block1, BlockHeader* block2);
	// checks whether the two blocks are buddies are not

	BlockHeader* merge (BlockHeader* block1, BlockHeader* block2);
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around

	BlockHeader* split (BlockHeader* block);
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected

	BlockHeader* getbuddy(BlockHeader* block);
	// given a block address, this function returns the address of its buddy 


public:
	BuddyAllocator (int _basic_block_size, int _total_memory_length); 
	/* This initializes the memory allocator and makes a portion of 
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as 
	   its minimal unit of allocation. The function returns the amount of 
	   memory made available to the allocator. If an error occurred, 
	   it returns 0. 
	*/ 
    
	~BuddyAllocator(); 
	/* Destructor that returns any allocated memory back to the operating system. 
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/ 
    int _getListNo(int sizeKb);
	char* alloc(int _length);
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	int free(char* _a); 
	/* Frees the section of physical memory previously allocated 
	   using ’my_malloc’. Returns 0 if everything ok. */ 
   
	void printlist ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function should print how many free blocks of each size belong to the allocator
	at that point. The output format should be the following (assuming basic block size = 128 bytes):

	[0] (128): 5
	[1] (256): 0
	[2] (512): 3
	[3] (1024): 0
	....
	....
	 which means that at this point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
};

#endif 