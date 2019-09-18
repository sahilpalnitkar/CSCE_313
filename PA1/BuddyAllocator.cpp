#include "BuddyAllocator.h"

#include <iostream>

using namespace std;



int BuddyAllocator::_getListNo(int sizeKb) {
    
    return ceil(log2(sizeKb) - log2(total_memory_size));
    
}



BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length){
    
    basic_block_size = _basic_block_size;
    total_memory_size = _total_memory_length;
    start = new char[total_memory_size];
    int j = (int) log2(total_memory_size/basic_block_size);
    
    for(int i=_basic_block_size; i<=_total_memory_length; i = i*2){
        cout<<i<<endl;
        LinkedList* temp = new LinkedList();
        FreeList.push_back(*temp);
        //FreeList[i].length=0;
    }
    
    FreeList[j].head = (BlockHeader*) start;
    FreeList[j].head -> block_size = total_memory_size;
    FreeList[j].head -> next = NULL;
    FreeList[j].head -> free = true;  
    
    ++FreeList[j].length;
    
}



BuddyAllocator::~BuddyAllocator (){
    
    delete(start);
    
}



char* BuddyAllocator::alloc(int _length) {
    
    /* This preliminary implementation simply hands the call over the
     
     the C standard library!
     
     Of course this needs to be replaced by your implementation.
     
     */
    
    int rx = _length + basic_block_size;
    int k = (int) log2(total_memory_size/basic_block_size);
    int i = (int) log2(rx/basic_block_size);
    
    //use to traverse through the list of free blocks to delete the last block on the linked list
    
    if(FreeList[i].head){
        BlockHeader* b = FreeList[i].head;
        FreeList[i].remove(b);
        return (char*) (b+1);
    }
    
    else{
        int j = i;
        while(((FreeList[j]).head == NULL) && (j<k)){
            j++;
        }
        if(j>k){
            return 0;
        }
        while(j>i){
            BlockHeader* b = FreeList[j].head;
            BlockHeader* bb = split(b);
            FreeList[j-1].insert(b);
            FreeList[j].remove(b);
            --j;
            this->printlist();
        }
    }
    BlockHeader* final = FreeList[i].head;
    final->free = false;
    FreeList[i].remove(final);
    
    return new char [_length];
}

BlockHeader* BuddyAllocator::split(BlockHeader* block){
    
    int split_block_size = (int) (block->block_size/2);
    BlockHeader* buddy_block_size = (BlockHeader*) block + split_block_size;
    buddy_block_size->block_size = split_block_size;
    block->block_size = split_block_size;
    buddy_block_size -> next = NULL;
    block->buddy = buddy_block_size;
    buddy_block_size->buddy = block;
    int pos = (int) (log2(split_block_size/basic_block_size));
    this->FreeList[pos].insert(buddy_block_size);
    return buddy_block_size;
}



int BuddyAllocator::free(char* _a) {
    /* Same here! */
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
                printf("a<<1 = %d\n", 1<<i);
                cerr << "ERROR:: Block is in a wrong list" << endl;
                exit (-1);
            }
            b = b->next;
        }
        cout << count << endl;
    }
}
