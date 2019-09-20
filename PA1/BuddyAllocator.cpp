#include "BuddyAllocator.h"

#include <iostream>
#include <algorithm>
using namespace std;

vector<BlockHeader*> buddy_blocks = {};

int BuddyAllocator::_getListNo(int sizeKb) {
    
    return ceil(log2(sizeKb) - log2(total_memory_size));
    
}



BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length){
    
    basic_block_size = _basic_block_size;
    total_memory_size = _total_memory_length;
    start = new char[total_memory_size];
    int j = (int) log2(total_memory_size/basic_block_size);
    memory = new char[total_memory_size];
    
    for(int i=_basic_block_size; i<=_total_memory_length; i = i*2){
        // cout<<i<<endl;
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
        b->free = false;
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
            bb->free = true;
            bool check = find(buddy_blocks.begin(), buddy_blocks.end(), bb) != buddy_blocks.end();
            if (!check){
            buddy_blocks.push_back(bb);
            }
            FreeList[j-1].insert(bb);
            // FreeList[j].remove(b);
            --j;
            // this->printlist();
        }
    }
    BlockHeader* final = FreeList[i].head;
    final->free = false;
    FreeList[i].remove(final);
    char* ret_char = (char*) final;
    // cout<<ret_char<<endl;
    return ret_char;

    // return new char [_length];
}

BlockHeader* BuddyAllocator::split(BlockHeader* block){
    

    long split_block_size = (long) (block->block_size/2);
    BlockHeader* buddy_block = new BlockHeader();
    int s2 = sizeof(BlockHeader);
    buddy_block = block + split_block_size;
    buddy_block = (BlockHeader*)malloc(s2);
    block->block_size = split_block_size;
    buddy_block->block_size = split_block_size;
    buddy_block->next = NULL;
    buddy_block->isBuddy = true;
    block->buddy = buddy_block;
    buddy_block->buddy = block;
    int pos = (int) (log2(split_block_size/basic_block_size));
    // this->FreeList[pos].insert(buddy_block);
    this->FreeList[pos].insert(block);
    this->FreeList[pos+1].remove(block);
    return buddy_block;
}



int BuddyAllocator::free(char* _a) {
    /* Same here! */
    for (int i = 0; i < buddy_blocks.size(); i++) {
		std::cout << buddy_blocks.at(i) << ' ';
	}
    BlockHeader* block = (BlockHeader*) (_a);
    block->free = true;
    bool a = true;
    // while(a){

        // if (buddy->free == true){
        reverse(buddy_blocks.begin(), buddy_blocks.end());
        for (int i = 0; i < buddy_blocks.size(); i++){
            if (buddy_blocks[i]->free){
                merge(block,buddy_blocks[i]);
                // buddy_blocks.erase(buddy_blocks.begin()+i);
            }
            else{
                // cout<<"NOT FREEEEEEEEEEE"<<endl;
            }
    /*do stuff */
        }

           
        // }
        // else{
            a = false;
        // }
    // }
        // for (int i = 0; i<FreeList.size();i++){
        //     cout<<FreeList[i].head<<endl;;
        // }
    return 0;
}

BlockHeader* BuddyAllocator::getbuddy(BlockHeader* block){
    // start = new char[total_memory_size];
    BlockHeader* buddy_block;
    // if (block->isBuddy){
    //     buddy_block = block - block->block_size;
    //     cout<<buddy_block<<endl;
    // }
    // else{
    //     buddy_block = block + block->block_size;
    //     cout<<buddy_block<<endl;

    // }
    BlockHeader* buddy = (BlockHeader*)((((char*)block-start) xor (block->block_size)) + start);
    // BlockHeader* buddy = (BlockHeader*)((((char*)block-start) xor (uintptr_t)(block->block_size)) + start);
    // char* addr = (char*) (block);
    // char* buddy = (((char*)block - memory) xor (block->block_size)) + memory;
    // cout<<buddy<<endl;
    // return (BlockHeader*) buddy;
    return (BlockHeader*) buddy_block;
}

bool BuddyAllocator::arebuddies (BlockHeader* block1, BlockHeader* block2){
    BlockHeader* buddy1 = getbuddy(block1);
    BlockHeader* buddy2 = getbuddy(block2);

    if (block1->buddy == buddy1 && block2->buddy == buddy2){
        return true;
    }
    else{
        return false;
    }
}

BlockHeader* BuddyAllocator::merge (BlockHeader* block1, BlockHeader* block2){
    if (block1->block_size != block2->block_size){
        return NULL;
    }
    cout<<"This size is equal"<<endl;
    BlockHeader* num1 = NULL;
    BlockHeader* num2 = NULL;
    if (block1<block2){
        num1 = block2;
        num2 = block1;
    }
    else{
        num1 = block1;
        num2 = block2;
    }
    int pos = (int) (log2(num1->block_size/basic_block_size));

    num1->block_size = (num1->block_size*2);
    num2->block_size = (num2->block_size*2);
    num1->free = true;
    num2->free = true;
    FreeList[pos].remove(num2);
    FreeList[pos].remove(num1);

    FreeList[pos+1].insert(num1);
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
