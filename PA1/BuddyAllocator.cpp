#include "BuddyAllocator.h"
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length){
  
  int bbs = _basic_block_size;
  int total_length = _total_memory_length;
  // basic_block_size = _basic_block_size;
  total_memory_size = _total_memory_length;
  int j = (int)(log2(total_length/bbs));

  
  // _total_memory_length = ceil(log2(_total_memory_length));
  // _basic_block_size = ceil(log2(_basic_block_size));
  start = new char[total_length];
	// BlockHeader* b = start;
 
  for(int i = bbs; i <= total_length; i = i*2){
    LinkedList* temp = new LinkedList();
    temp->head = NULL;
    FreeList.push_back(*temp);
  }
  FreeList[j].head = (BlockHeader*) start;
  FreeList[j].head -> block_size = total_length;
  FreeList[j].head -> next = NULL;
  FreeList[j].head -> free = true;  
  // for(int i = 0; i<j-1; i++){
  //   FreeList[i].head = NULL;
  //   }
  // this-> printlist();

}
BuddyAllocator::~BuddyAllocator (){
  delete(start);
}

char* BuddyAllocator::alloc(int _length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  // */
  int rx = _length + basic_block_size;
  // int rx = _length + basic_block_size;
  
  // cout<<rx<<endl;
  int k = (int) log2(total_memory_size/basic_block_size);
  cout<<basic_block_size<<endl;
  int i = log2(rx/basic_block_size);
  // cout<<i<<endl;
  if (FreeList[i].head){
    BlockHeader* b = FreeList[i].head;
    FreeList[i].remove(b);
    cout<< ((char*) (b+1))<<endl;
    // b->free = false;
    return (char*) (b+1);
    // return new char*[];
    } 
  else{
    int j = i;
    while((FreeList[j].head == NULL) && (j<k)){
      j++;
    }
    if (j>k){
      return 0;
      }
    cout<<j<<endl;
    while (j>i){
      BlockHeader* b = FreeList[j].head;
      BlockHeader* bb = split(b);
      FreeList[j-1].insert(bb);
      FreeList[j].remove(b);
      cout<<"removed"<<endl;
      // printlist();
      
      cout<<"Inserted"<<endl;
      printlist();
      --j;;
    }
    BlockHeader* final = FreeList[i].head;
    final->free = false;
    FreeList[i].remove(final);
    }

    
  return new char [_length];
  }


BlockHeader* BuddyAllocator::split(BlockHeader* block){
  int split_block_size = (int)(block->block_size/2);
  BlockHeader* buddy_block_header = (BlockHeader*) block + split_block_size;
  block->buddy = buddy_block_header;
  block->block_size = split_block_size;
  cout<<buddy_block_header->block_size;
  buddy_block_header->block_size = split_block_size;
  cout<<buddy_block_header->block_size;

  int list_position = (int) (log2(split_block_size/basic_block_size));
  
  this->FreeList[list_position].insert(buddy_block_header);
  // buddy_block_header->block_size = (buddy_block_header->block_size)/2;
  
  return buddy_block_header;

}

BlockHeader* BuddyAllocator::merge (BlockHeader* block1, BlockHeader* block2){
  
}


int BuddyAllocator::free(char* _a) {
  /* Same here! */
  BlockHeader* b = (BlockHeader*) (_a - sizeof(BlockHeader));
  while(b){
    int i = log2(sizeof(b)/basic_block_size);
    FreeList[i].insert(b);
    BlockHeader* bb = b->buddy;
    if (bb){
      FreeList[i].insert(b);
      return 0;
    }
    else{
      FreeList[i].remove(bb);
      BlockHeader* m = merge(b,bb);
      FreeList[i+1].insert(m);
      return 0;
    }  
  }
  delete _a;
  return 0;
}



void BuddyAllocator::printlist (){
  cout << "Printing the Freelist in the format \"[index] (block size) : # of blocks\"" << endl;
  for (int i=0; i<FreeList.size(); i++){
    cout << "[" << i <<"] (" << ((1<<i) * basic_block_size) << ") : ";  // block size at index should always be 2^i * bbs
    int count = 0;
    BlockHeader* b = FreeList [i].head;
    // go through the list from head to tail and count
    while (b){
      count ++;
      // block size at index should always be 2^i * bbs
      // checking to make sure that the block is not out of place
      if (b->block_size != (1<<i) * basic_block_size){
        cerr << "ERROR:: Block is in a wrong list" << endl;
        exit (-1);
      }
      b = b->next;
    }
    cout << count << endl;  
  }
}


