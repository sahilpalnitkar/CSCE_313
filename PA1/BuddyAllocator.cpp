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
    memory = new char[total_memory_size];
    
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
            FreeList[j-1].insert(bb);
            // FreeList[j].remove(b);
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
    

    uint split_block_size = (uint) (block->block_size/2);
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
    BlockHeader* block = (BlockHeader*) _a;
    block->free = true;
    bool a = true;
    while(a){

        BlockHeader* buddy_block = getbuddy(block);
        // BlockHeader* buddy_block = block + block->block_size;
        cout<<block->block_size<<endl;
        cout<<buddy_block<<endl;
        // cout<<buddy_block->free<<endl;
        if (buddy_block->free == true){
            merge(block,buddy_block);
        }
        // else{
            a = false;
        // }
    }
    return 0;
}

BlockHeader* BuddyAllocator::getbuddy(BlockHeader* block){
    // start = new char[total_memory_size];
    BlockHeader* buddy_block;
    if (block->isBuddy){
        buddy_block = block - block->block_size;
        cout<<buddy_block<<endl;
    }
    else{
        buddy_block = block + block->block_size;
        cout<<buddy_block<<endl;

    }
    // BlockHeader* buddy = (BlockHeader*)((((char*)block-start) xor (block->block_size)) + start);
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
    // if (arebuddies(block1, block2) == false){
    //     cout<<block1->buddy<<endl;
    //     cout<<block2->buddy<<endl;
    //     return NULL;
    // }
    BlockHeader* num1 = NULL;
    BlockHeader* num2 = NULL;
    if (block1>block2){
        num1 = block2;
        num2 = block1;
    }
    else{
        num1 = block1;
        num2 = block2;
    }
    cout<<num1<<endl;
    cout<<num2<<endl;
    int pos = (int) (log2(num1->block_size/basic_block_size));

    num1->block_size = (num1->block_size*2);
    num2->block_size = (num2->block_size*2);

    FreeList[pos+1].insert(num1);
    FreeList[pos].remove(num1);
    cout<<"SUCCESSFULLY MERGED<"<<endl;

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
