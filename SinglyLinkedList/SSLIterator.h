#ifndef SSLIterator_h
#define SSLIterator_h

#include "Node.h"                       //To have Node variables.

template <class TElem>
class SSLIterator
{
private:
    Node<TElem>* CURRENT;
    int POSITION;
    
public:
    
    //Constructor Function with parameter.
    SSLIterator (Node<TElem>* head = NULL)
    {
        CURRENT = head;
        POSITION = 0;
    }
    
    //Destructor Function.
    ~SSLIterator()
    {
        CURRENT = NULL;
        POSITION = 0;
    }
    
    //Returns reference of the current element.
    Node<TElem>& current() const
    {
        return *CURRENT;
    }
    
    //Returns true if the current element is not equal to NULL.
    bool valid()
    {
        return (CURRENT != NULL) ? true : false;
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
    SSLIterator operator++(int )
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
    bool operator==(const SSLIterator<TElem>& rightHandSideIterator)
    {
        return (*CURRENT) == rightHandSideIterator.current();
    }
    
    //Compares the iterator with right hand side iterator to check their current node is equal.
    bool operator!=(const SSLIterator<TElem>& rightHandSideIterator)
    {
        return !(   (*this) == rightHandSideIterator   );
    }
    
    
};

#endif
