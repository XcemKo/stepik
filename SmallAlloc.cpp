#include <cstring>
class SmallAllocator {
private:
    union Header {
        using Align = long;
        
        struct {
            Header *next;
            size_t size;
        } s;
        
        Align x;
    };
    
    static constexpr unsigned int memory_size = 1048576;
    static constexpr unsigned int memory_block = 1024;
    
    Header *alloc_in_mem(unsigned int Size) {
        if (Size < memory_block) {
            Size = memory_block;
        }
        
        unsigned int al = Size * sizeof(Header);
        
        if (allocated + al > memory_size) {
            return nullptr;
        }
        
        Header *block = reinterpret_cast<Header *> (&Memory[allocated]);
        allocated += al;
        block->s.next = nullptr;
        block->s.size = Size;
        
        return block;
    }
    
    Header base;
    Header *freep{nullptr};
    unsigned int allocated{0};
    char Memory[memory_size];
public:
    void *Alloc(unsigned int Size) {
        Header *prevp = freep;
        
        if (freep == nullptr) {
            base.s.next = freep = prevp = &base;
            base.s.size = 0;
        }
        
        unsigned int new_alloc = (Size + sizeof(Header) - 1) / sizeof(Header) + 1;
        
        for (Header *p = prevp->s.next; ; prevp = p, p = p->s.next) {
            if (p->s.size >= new_alloc) {
                if (p->s.size == new_alloc) {
                    p->s.size = 0;
                    prevp = p->s.next;
                }
                else {
                    p->s.size -= new_alloc;
                    p += p->s.size;
                    p->s.size = new_alloc;
                }
                
                freep = prevp;
                
                return static_cast<void *> (p + 1);
            }

            if (p == freep) {
                p->s.next = alloc_in_mem(new_alloc);
                
                if (!p->s.next) {
                    return nullptr;
                }
            }
        }
    }
    
    void *ReAlloc(void *Pointer, unsigned int Size) {
        if (!Pointer) {
            return nullptr;
        }
        
        Header *base_ptr = static_cast<Header *> (Pointer) - 1;
        unsigned int new_alloc = (Size + sizeof(Header) - 1) / sizeof(Header) + 1;
        
        // in case we want the same size or a little bit smaller
        if (base_ptr->s.size >= new_alloc) {
            return static_cast<void *> (base_ptr + 1);
        }
        
        void *new_place = Alloc(Size);
        memcpy(new_place, Pointer, base_ptr->s.size * sizeof(Header));
        Free(Pointer);
        
        return new_place;
    }
    
    void Free(void *Pointer) {
        if (!Pointer) {
            return;
        }
        // get the header of a block
        Header *base_ptr = static_cast<Header *> (Pointer) - 1;
        Header *p = freep;
        
        while (!(base_ptr > p && base_ptr < p->s.next)) {
            if (p >= p->s.next && (base_ptr > p || base_ptr < p->s.next)) {
                break;
            }
            
            p = p->s.next;
        }
        
        if (base_ptr + base_ptr->s.size == p->s.next) {
            base_ptr->s.size += p->s.next->s.size;
            base_ptr->s.next = p->s.next->s.next;
        }
        else {
            base_ptr->s.next = p->s.next;
        }
        
        if (p + p->s.size == base_ptr) {
            p->s.size += base_ptr->s.size;
            p->s.next = base_ptr->s.next;
        }
        else {
            p->s.next = base_ptr;
        }
        
        freep = p;
    }
};