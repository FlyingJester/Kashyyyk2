/* 
 *  Copyright (c) 2016-2017 Martin McDonough.  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 * - Products derived from this software may not be called "Kashyyyk", nor may
 *     "YYY" appear in their name, without prior written permission of
 *     the copyright holders.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/*---------------------------------------------------------------------------*/

#ifndef YYY_MAINTAINER_HPP
#define YYY_MAINTAINER_HPP
#pragma once

/*---------------------------------------------------------------------------*/

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

#if (( defined _MSC_VER ) && ( _MSC_VER >= 1500 )) || ( defined __GNUC__ )
#define YYY_ITERATORS_NEED_TYPE_TRAITS
#include <type_traits>
#include <iterator>

#ifdef _MSC_VER
#define YYY_MSC_ONLY_TYPENAME typename
#else
#define YYY_MSC_ONLY_TYPENAME
#endif

#endif

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/
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
        bool m_used[c_block_size]; // TODO: Use a bitfield?
        Block *m_next;
        
        inline unsigned count() const {
            unsigned n = 0;
            for(unsigned i = 0; i < c_block_size; i++)
                if(m_used[i])
                    n++;
            return n;
        }
        
        inline bool hasNext() const { return m_next != NULL; }
        inline const Block *next() const { return m_next; }
        inline Block *next() { return m_next; }
        
        inline struct Block *lastBlock(){
            struct Block *b = this;
            while(b->hasNext())
                b = b->next();
            return b;
        }
        
        inline const struct Block *lastBlock() const {
            const struct Block *b = this;
            while(b->hasNext())
                b = b->next();
            return b;
        }
        
        inline bool empty() const {
            for(unsigned i = 0; i < c_block_size; i++)
                if(m_used[i] != false)
                    return false;
            return true;
        }
    };

private:

    struct Block *m_blocks;
    
    // Blindly appends a block to the input. Will overwrite any next block.
    inline static struct Block *AddBlock(struct Block *to){
        assert(to->m_next == NULL);
        to->m_next = (struct Block*)calloc(sizeof(struct Block), 1);
        return to->m_next;
    }

protected:
    
    // Adds a block to end of the current m_blocks list, returning the new end.
    inline struct Block *addBlock(){
        if(m_blocks)
            return AddBlock(m_blocks->lastBlock());
        else{
            m_blocks = (struct Block*)calloc(sizeof(struct Block), 1);
            return m_blocks;
        }
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
        struct Block *block = m_blocks;
        while(block != NULL){
            for(unsigned i = 0; i < c_block_size; i++){
                if(block->m_used[i] == true){
                    block->m_data[i].~T();
#ifndef NDEBUG
                    block->m_used[i] = false;
#endif
                }
            }
            free(block);
            block = block->next();
        }
    }
    
    // Returns a new stable memory location, but does not perform actual
    // construction. You should use placement-new to create an object in this
    // space.
    inline T *allocate(){
        
        struct Block *block = NULL;
        
        if(m_blocks == NULL){
            m_blocks = block = addBlock();
            block->m_used[0] = true;
            return block->m_data;
        }
        
        unsigned short i;
        if(findNextAvailable(block, i)){
            block->m_used[i] = true;
            return block->m_data + i;
        }
        else{
            assert(block != NULL);
            block = AddBlock(block);
            
            // Return the first value.
            block->m_used[0] = true;
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
    
    template<typename Type, typename BlockType>
    class iterator_base
    {
#ifdef YYY_ITERATORS_NEED_TYPE_TRAITS
        static constexpr bool type_is_const = std::is_const<Type>::value;
#endif
        typedef iterator_base<Type, BlockType> this_type;
        
        BlockType *m_block;
        unsigned short m_i;
        
        // Constructor used for known good data.
        iterator_base(BlockType *block, unsigned n)
          : m_block(block)
          , m_i(n){
            assert(m_block == NULL || m_block->m_used[m_i]);
        }
        
        // Increments to the next found used location.
        inline void increment(){
            assert(m_block != NULL);
            
            do{
                m_i++;
                
                if(m_i >= c_block_size){
                    m_i = 0;
                    m_block = m_block->next();
                }
                
                if(m_block == NULL || m_block->m_used[m_i])
                    return;
                
            }while(1);
        }
        
        
    public:
#ifndef NDEBUG
    inline const BlockType *DEBUG_getBlock() const { return m_block; }
    inline const unsigned short DEBUG_getIndex() const { return m_i; }
#endif
    
#ifdef YYY_ITERATORS_NEED_TYPE_TRAITS
        struct const_forward_iterator_t : public std::forward_iterator_tag, public std::output_iterator_tag {};
        typedef YYY_MSC_ONLY_TYPENAME std::conditional<
            type_is_const,
            std::forward_iterator_tag,
            const_forward_iterator_t>::type iterator_category;
#endif
        
        typedef Type value_type;
        typedef unsigned short distance_type;
        typedef Type &reference;
        typedef Type *pointer;
        
        inline BlockType *block() { return m_block; }
        inline const BlockType *block() const { return m_block; }
        inline unsigned short getI() const { return m_i; }

        iterator_base(BlockType *block)
          : m_block(block)
          , m_i(0){
            if(m_block != NULL && !(*m_block->m_used))
                increment();
        }
        
        template<typename OtherT, typename OtherBlock>
        inline bool operator == (const iterator_base<OtherT, OtherBlock> &other) const {
            return other.block() == m_block && (m_block == NULL || other.getI() == m_i);
        }
        
        template<typename OtherType>
        inline bool operator != (const OtherType &other) const {
            return !(*this == other);
        }
        
        inline this_type &operator++(){
            increment();
            return *this;
        }
        
        inline this_type operator+(int i) const {
            assert(i >= 0);
            this_type iter(m_block, m_i);
            while(i--)
                iter.increment();
            return iter;
        }
        
        inline this_type &operator+=(int i){
            assert(i >= 0);
            while(i--)
                increment();
            return *this;
        }
        
        inline this_type operator++(int){
            this_type z(m_block, m_i);
            increment();
            return z;
        }
        
        inline Type &operator*(){
            return m_block->m_data[m_i];
        }
        
        inline Type *operator->() {
            return m_block->m_data + m_i;
        }
    };
    
    typedef iterator_base<T, Block> iterator;
    typedef iterator_base<const T, const Block> const_iterator;
    
    inline iterator begin() { return iterator(m_blocks); }
    inline const_iterator begin() const { return const_iterator(m_blocks); }
    inline const_iterator cbegin() const { return const_iterator(m_blocks); }
    
    inline iterator end() { return iterator(NULL); }
    inline const_iterator end() const { return const_iterator(NULL); }
    inline const_iterator cend() const { return const_iterator(NULL); }
    
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

/*---------------------------------------------------------------------------*/

#ifdef __WATCOMC__
#pragma warning 391 9
#endif

} // namespace YYY

/*---------------------------------------------------------------------------*/

#undef YYY_MSC_ONLY_TYPENAME
#undef YYY_ITERATORS_NEED_TYPE_TRAITS

#endif YYY_MAINTAINER_HPP
