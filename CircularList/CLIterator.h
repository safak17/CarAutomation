#ifndef CLIterator_h
#define CLIterator_h

#include "Node.h"                       //To have Node variables.

template <class TElem>
class CLIterator
{
private:
    Node<TElem>* CURRENT;
    int POSITION;
    
public:
    
    //Constructor Function with parameter.
    CLIterator (Node<TElem>* head = nullptr)
    {
        CURRENT = head;
        POSITION = 0;
    }
    
    //Destructor Function.
    ~CLIterator()
    {
        CURRENT = nullptr;
        POSITION = 0;
    }
    
    //Returns reference of the current element.
    Node<TElem>& current() const
    {
        return *CURRENT;
    }
    
    //Returns true if the current element is not equal to nullptr.
    bool valid()
    {
        return (CURRENT != nullptr) ? true : false;
    }
    
    void setPosition(int position)
    {
        POSITION = position;
    }
    
    //Returns the "position".
    int position() const
    {
        return POSITION;
    }
    
    //If it is valid, iterates to the next element.
    void next()
    {
        if ( valid() )
        {
            CURRENT = CURRENT->next();
            POSITION++;
        }
        
    }
    
    //If it is valid, iterates to the next element. (Postfix Increment)
    CLIterator operator++(int )
    {
        next();
        return *this;
    }
    
    //Dereferencing operator will return the info about current node.
    TElem operator*()
    {
        return (*CURRENT).info();
    }
    
    //Compares the iterator with right hand side iterator to check their current node is equal.
    bool operator==(const CLIterator<TElem>& rightHandSideIterator)
    {
        return (*CURRENT) == rightHandSideIterator.current();
    }
    
    //Compares the iterator with right hand side iterator to check their current node is equal.
    bool operator!=(const CLIterator<TElem>& rightHandSideIterator)
    {
        return !(   (*this) == rightHandSideIterator   );
    }
    
    
};

#endif
