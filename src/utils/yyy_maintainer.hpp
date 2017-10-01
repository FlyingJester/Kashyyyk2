#pragma once

#include <iterator>

#include <assert.h>

#include <stdlib.h>
#ifndef NDEBUG
#include <string.h>
#endif

// There is a case where we assign in a conditional (the close of the outer do
// loop in Maintainer<T>::iterator::increment). This is intentional, and put in
// parentheses how Clang wants it.
#ifdef __WATCOMC__
#pragma warning 391 10
#endif

namespace YYY {

/*===========================================================================*/
// Defines a container that does not have strong ordering, but under which
// no operations will invalidate any references or iterators other than an
// erase (which invalidates the erased iterator).
// Similar in use to a std::list, although implemented more like a std::deque
// with a linked list of blocks. It is mainly used to maintain object lifetimes
// rather than to actually do any iteration (as the name suggests).
template<class T>
class Maintainer {
public:
    // Use super secret hacks to make the block size static const.
    enum { c_block_size = 32 };
    
    // A Block holds a set of c_block_size T objects.
    // Blocks should NOT be allocated on the stack or through new. There is no
    // way to ensure that m_data is NOT constructed, and we do NOT want to do
    // a full construction on allocation. The actual elements are created using
    // placement-new statements on first creation and their destructors
    // manually invoked when they are freed.
    struct Block {
        T m_data[c_block_size];
        bool m_used[c_block_size];
        Block *m_next;
        
        unsigned count() const {
            unsigned n = 0;
            for(unsigned i = 0; i < c_block_size; i++)
                if(m_used[i])
                    n++;
            return n;
        }
        
        bool hasNext() const { return m_next != NULL; }
        const Block *next() const { return m_next; }
        Block *next() { return m_next; }
        
        struct Block *lastBlock(){
            struct Block *b = this;
            while(b->hasNext())
                b = b->next();
            return b;
        }
        
        const struct Block *lastBlock() const {
            const struct Block *b = this;
            while(b->hasNext())
                b = b->next();
            return b;
        }
        
        bool empty() const {
            for(unsigned i = 0; i < c_block_size; i++)
                if(m_used[i] != false)
                    return false;
            return true;
        }
    };

private:
    
    struct Block *m_blocks;
    
    // Adds a block to end of the current m_blocks list, returning the new end.
    inline struct Block *addBlock(){
        if(m_blocks)
            return AddBlock(m_blocks->lastBlock());
        else{
            m_blocks = (struct Block*)calloc(sizeof(struct Block), 1);
            return m_blocks;
        }
    }
    
    // Blindly appends a block to the input. Will overwrite any next block.
    inline static struct Block *AddBlock(struct Block *to){
        assert(to->m_next == NULL);
        to->m_next = (struct Block*)calloc(sizeof(struct Block), 1);
        return to->m_next;
    }
    
    // Finds the next available memory location, returning true if present and
    // populating the output arguments with the corresponding location. If no
    // locations are available, then the outBlock is set to the last block in
    // the current list and the function returns false (i is undefined in that
    // case).
    bool findNextAvailable(struct Block *&outBlock, unsigned short &i) const {
        struct Block *b = outBlock = m_blocks;
        while(b != NULL){
            outBlock = b;
            for(i = 0; i < c_block_size; i++)
                if(b->m_used[i] == false)
                    return true;
            b = b->next();
        }
        return false;
    }
    
public:
    
    Maintainer()
      : m_blocks(NULL){
        
    }
    
    ~Maintainer(){
        while(m_blocks != NULL){
            for(unsigned i = 0; i < c_block_size; i++){
                if(m_blocks->m_used[i])
                    m_blocks->m_data[i].~T();
            }
            m_blocks = m_blocks->next();
        }
    }
    
    // Returns a new stable memory location, but does not perform actual
    // construction. You should use placement-new to create an object in this
    // space.
    inline T *allocate(){
        
        struct Block *block;
        unsigned short i;
        if(findNextAvailable(block, i)){
            block->m_used[i] = true;
            return block->m_data + i;
        }
        else{
            if(block)
                block = AddBlock(block);
            else
                block = addBlock();
            
            block->m_used[0] = false;
            return block->m_data;
        }
    }
    
    // Creates a default-constructed instance in stable memory and returns it.
    inline T &create(){
        T *const that = allocate();
        new (that) T();
        return *that;
    }
    
    // Equivalent to `create() = in;`
    inline void insert(T &in){
        T *const that = allocate();
        new (that) T();
        that = in;
    }
    
    class iterator {
        struct Block *m_block;
        unsigned short i;
        iterator(struct Block *block, unsigned n)
          : m_block(block)
          , i(n){}
        
        inline void increment(){
            do{
                do{
                    i++;
                }while(i < c_block_size && !m_block->m_used[i]);
            }while(i >= c_block_size && (m_block = m_block->next()));
        }
        
    public:
        
        // Used only for certain performance tricks in erase.
        inline struct Block *block() { return m_block; }
        inline const struct Block *block() const { return m_block; }
        
        iterator(struct Block *block)
          : m_block(block)
          , i(0){
            if(m_block && !(*m_block->m_used))
                increment();
        }
        
        inline bool operator == (const iterator &other) const {
            return other.m_block == m_block && (m_block == NULL || other.i == i);
        }
        
        inline bool operator != (const iterator &other) const {
            return *this != other;
        }
        
        inline iterator &operator++(){
            increment();
            return *this;
        }
        
        inline iterator operator+(int i) const {
            assert(i >= 0);
            iterator iter(m_block, i);
            while(i--)
                iter.increment();
            return iter;
        }
        
        inline iterator &operator+=(int i){
            assert(i >= 0);
            while(i--)
                increment();
            return *this;
        }
        
        inline iterator operator++(int){
            iterator z(m_block, i);
            increment();
            return z;
        }
        
        inline T &operator*(){
            return m_block->m_data[i];
        }
        
        inline T &operator->() {
            return m_block->m_data[i];
        }
        
    };
    
    inline iterator begin() { return iterator(m_blocks); }
    inline iterator end() {
        if(!m_blocks)
            return iterator(NULL);
        
        struct Block *const last = m_blocks->lastBlock();
        const unsigned short n = last->count();
        iterator iter = iterator(last);
        for(unsigned i = 0; i+1 < n; i++){
            ++iter;
        }
        
        return iter;
    }
    
    iterator erase(const iterator &iter){
        struct Block *const block = iter.block();
        
        assert(block != NULL);
        
#ifndef NDEBUG
        struct Block *b = m_blocks, *b4 = NULL;
        while(b != block && b->hasNext()){
            b4 = b;
            b = b->next();
        }
        assert(b == block);
        assert(b4 != NULL || block == m_blocks);
#endif
        
        const unsigned n = std::distance(block.m_data, &(*iter));
        block->m_used[n] = false;
        iter->~T();
#ifndef NDEBUG
        memcpy(block->m_data + n, 0, sizeof(T));
#endif
        // Clean up any empty block.
        if(block->empty()){
            if(block == m_blocks){
                struct Block *const next = block->next();
                delete m_blocks;
                m_blocks = next;
                return iterator(next);
            }
            // We previously calculated this for a consistency check only.
#ifdef NDEBUG
            // b4 will always end up with a value, since previously we checked
            // for if that == m_blocks.
            struct Block *b = m_blocks, *b4 = NULL;
            while(b != block && b->hasNext()){
                b4 = b;
                b = b->next();
            }
#endif
            b4->m_next = block->next();
            free(block);
            return iterator(b4->next());
        }
        else
            return iter+1;
    }
};

#ifdef __WATCOMC__
#pragma warning 391 9
#endif

} // namespace YYY
