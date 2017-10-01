#pragma once

namespace YYY {

template<class T, unsigned Size>
class RingBuffer {
    T m_data[Size];
    unsigned m_start, m_end;
public:
    class const_iterator;
    class iterator;
    
    static bool iteratorsEqual(const const_iterator &a, const const_iterator &b);
    static bool iteratorsEqual(const iterator &a, const const_iterator &b);
    static bool iteratorsEqual(const const_iterator &a, const iterator &b);
    static bool iteratorsEqual(const iterator &a, const iterator &b);
    
    RingBuffer()
      : m_start(0)
      , m_end(0){
        
    }
    
    T &operator[] (unsigned i){
        return m_data[(i + m_start) % Size];
    }
    
    const T &operator[] (unsigned i) const {
        return m_data[(i + m_start) % Size];
    }
    
    unsigned size() const {
        if(m_start == m_end)
            return 0;
        if(m_start < m_end)
            return m_end - m_start;
        else
            return (m_end + Size) - m_start;
    }
    
    T *add_back(){
        if(m_end == Size - 1){
            if(m_start == 0)
                return NULL;
            m_end = 0;
            return m_data + Size - 1;
        }
        else{
            if(m_end + 1 == m_start)
                return NULL;
            m_end++;
            return m_data + m_end - 1;
        }
    }
    
    T *add_front(){
        if(m_start == 0){
            if(m_end == Size - 1)
                return NULL;
            m_start = Size - 1;
            return m_data + Size - 1;
        }
        else{
            if(m_end + 1 == m_start)
                return NULL;
            return m_data + --m_start;
        }
    }
    
    bool push_back(T &that){
        T *const b = add_back();
        if(b == NULL)
            return false;
        b[0] = that;
        return true;
    }
    
    bool push_front(T &that){
        T *const b = add_front();
        if(b == NULL)
            return false;
        b[0] = that;
        return true;
    }
    
    bool empty() const { return m_end == m_start; }
    
    bool pop_back(){
        if(m_end == m_start)
            return false;
        if(m_end == 0)
            m_end = Size - 1;
        else
            m_end--;
        return true;
    }
    
    bool pop_front(){
        if(m_end == m_start)
            return false;
        if(m_start == Size - 1)
            m_start = 0;
        else
            m_start++;
        return true;
    }
    
    void clear(){
        m_start = m_end = 0;
    }
    
    class iterator{
        T *m_that;
        T *const m_data;
        
        void increment(){
            m_that++;
            if(m_that >= m_data + Size)
                m_that = m_data;
        }
        
        void decrement(){
            m_that--;
            if(m_data > m_that)
                m_that = m_data + Size - 1;
        }
        
    public:
        
        friend class RingBuffer<T, Size>;
        
        bool operator == (const iterator &other){
            return RingBuffer<T, Size>::iteratorsEqual(other, *this);
        }
        
        bool operator == (const const_iterator &other){
            return RingBuffer<T, Size>::iteratorsEqual(other, *this);
        }
        
        bool operator != (const iterator &other){
            return !RingBuffer<T, Size>::iteratorsEqual(other, *this);
        }
        
        bool operator != (const const_iterator &other){
            return !RingBuffer<T, Size>::iteratorsEqual(other, *this);
        }
        
        T &operator *() { return *m_that; }
        
        iterator &operator++() {
            increment();
            return *this;
        }
        
        iterator &operator--() {
            decrement();
            return *this;
        }
        
        iterator operator++(int){
            iterator iter = *this;
            increment();
            return iter;
        }
        
        iterator operator--(int){
            iterator iter = *this;
            decrement();
            return iter;
        }
    };
    
    class const_iterator{
        const T *m_that;
        const T *const m_data;
        
        void increment(){
            m_that++;
            if(m_that >= m_data + Size)
                m_that = m_data;
        }
        
        void decrement(){
            m_that--;
            if(m_data > m_that)
                m_that = m_data + Size - 1;
        }
        
    public:
        
        friend class RingBuffer<T, Size>;
        
        bool operator == (const iterator &other){
            return RingBuffer<T, Size>::iteratorsEqual(other, *this);
        }
        
        bool operator == (const const_iterator &other){
            return RingBuffer<T, Size>::iteratorsEqual(other, *this);
        }
        
        bool operator != (const iterator &other){
            return !RingBuffer<T, Size>::iteratorsEqual(other, *this);
        }
        
        bool operator != (const const_iterator &other){
            return !RingBuffer<T, Size>::iteratorsEqual(other, *this);
        }
        
        const T &operator *() const { return *m_that; }
        
        const_iterator &operator++() {
            increment();
            return *this;
        }
        
        const_iterator &operator--() {
            decrement();
            return *this;
        }
        
        const_iterator operator++(int){
            const_iterator iter = *this;
            increment();
            return iter;
        }
        
        const_iterator operator--(int){
            const_iterator iter = *this;
            decrement();
            return iter;
        }
    };
    
    iterator begin() {
        iterator i;
        i.m_data = m_data;
        i.m_that = m_data + m_start;
        return i;
    }
    
    const_iterator begin() const {
        const_iterator i;
        i.m_data = m_data;
        i.m_that = m_data + m_start;
        return i;
    }
    
    const_iterator cbegin() const {
        return begin();
    }

    iterator end() {
        iterator i;
        i.m_data = m_data;
        i.m_that = m_data + m_end;
        return i;
    }
    
    const_iterator end() const {
        const_iterator i;
        i.m_data = m_data;
        i.m_that = m_data + m_end;
        return i;
    }
    
    const_iterator cend() const {
        return end();
    }

};

template<typename T, unsigned Size>
bool RingBuffer<T, Size>::iteratorsEqual(const RingBuffer<T, Size>::const_iterator &a, const RingBuffer<T, Size>::const_iterator &b){
    return a.m_that == b.m_that;
}

template<typename T, unsigned Size>
bool RingBuffer<T, Size>::iteratorsEqual(const RingBuffer<T, Size>::iterator &a, const RingBuffer<T, Size>::const_iterator &b){
    return a.m_that == b.m_that;
}

template<typename T, unsigned Size>
bool RingBuffer<T, Size>::iteratorsEqual(const RingBuffer<T, Size>::const_iterator &a, const RingBuffer<T, Size>::iterator &b){
    return a.m_that == b.m_that;
}

template<typename T, unsigned Size>
bool RingBuffer<T, Size>::iteratorsEqual(const RingBuffer<T, Size>::iterator &a, const RingBuffer<T, Size>::iterator &b){
    return a.m_that == b.m_that;
}

} // namespace YYY
