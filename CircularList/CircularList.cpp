#include "CircularList.h"

//Constructor Function.
template <class TElem>
CircularList<TElem>::CircularList()
:HEAD(nullptr),TAIL(nullptr),SIZE(0),MAX_SIZE(65535)
{
    //MAX_SIZE is the maximum number where can be stored in 16 bit system.
    CLIterator<TElem>(HEAD);
}

//Destructor Function.
template <class TElem>
CircularList<TElem>::~CircularList()
{
    CLIterator<TElem> it (HEAD);
    
    while( it.position() < size() )
    {
        delete &it.current();
        it++;
    }
    
    HEAD = TAIL = nullptr;
    SIZE = 0;
    
}//end ~CircularList





/*          GETTERS         */
//Returns the "size".
template <class TElem>
int CircularList<TElem>::size() const
{
    return SIZE;
}

//Returns the "max_size".
template <class TElem>
int CircularList<TElem>::max_size() const
{
    return MAX_SIZE;
}

//Returns "head".
template <class TElem>
Node<TElem>* CircularList<TElem>::head() const
{
    return HEAD;
}

//Returns the reference of first element in the list.
template <class TElem>
TElem& CircularList<TElem>::pop_front()
{
    return HEAD->info();
}

//Returns the reference of last element in the list.
template <class TElem>
TElem& CircularList<TElem>::pop_back() const
{
    return TAIL->info();
}





/*          EMPTY-FULL         */
//Checks whether CircularList is empty or not.
template <class TElem>
bool CircularList<TElem>::isEmpty() const
{
    return (SIZE == 0)? true : false;
}

//Checks whether CircularList is empty or not.
template <class TElem>
bool CircularList<TElem>::isFull() const
{
    return ( !isEmpty() && SIZE==MAX_SIZE);
}





/*          ADD         */
//Adds the givenElement to the beginning of CircularList.
template <class TElem>
bool CircularList<TElem>::push_front(const TElem &givenElement)
{
    if ( SIZE != MAX_SIZE )
    {
        Node<TElem>* node = new Node<TElem>(givenElement);
        
        if ( HEAD == nullptr )
            HEAD = TAIL = node;
        else
        {
            node->setNext(HEAD);
            HEAD = node;
        }
        
        //  Circular list's tail next = head.
        TAIL->setNext(HEAD);
        
        SIZE++;
        return true;
    }//If there is still space.
    
    return false;
}

//Adds the givenElement to the end of CircularList.
template <class TElem>
bool CircularList<TElem>::push_back(const TElem &givenElement)
{
    if ( SIZE != MAX_SIZE )
    {
        Node<TElem>* node = new Node<TElem>(givenElement);
        
        if ( HEAD == nullptr )
            HEAD = TAIL = node;
        else
        {
            TAIL->setNext(node);
            TAIL = node;
        }
        
        TAIL->setNext(HEAD);
        SIZE++;
        return true;
    }//If there is still space.
    
    return false;
}

//Adds the givenElement to the specified position.
template <class TElem>
bool CircularList<TElem>::push_at(const TElem &givenElement, int position)
{
    if ( position < 0 || position > SIZE )
        return false;
    else if ( position == 0 )
        return push_front(givenElement);
    else if ( position == SIZE )
        return push_back(givenElement);
    
    else
    {
        
        //To add givenElement to the specified position, we have to know the position-1 th element.
        CLIterator<TElem> it(HEAD);
        while( it.valid() && it.position() != position-1 )
            it++;
        
        if ( ! it.valid() )
            return false;
        
        Node<TElem>* node = new Node<TElem>(givenElement);
        
        node->setNext( it.current().next() );
        it.current().setNext( node );
        
        SIZE++;
        return true;
    }
    
}


//Adds the givenElement after the specifiedElement.
template <class TElem>
bool CircularList<TElem>::push_after (const TElem& givenElement, const TElem& specifiedElement )
{
    
    Node<TElem>* s = find (specifiedElement);
    
    if ( s != nullptr )
    {
        if ( s == HEAD )
            return push_at(givenElement, 1);
        else if ( s == TAIL )
            return push_back(givenElement);
        
        Node<TElem>* node = new Node<TElem>(givenElement);
        node->setNext( s->next() );
        s->setNext( node );
        
        SIZE++;
        return true;
    }
    
    return false;
    
}

template <class TElem>
bool CircularList<TElem>::push_sorted (const TElem& givenElement )
{
    if( isEmpty() )
        return push_front(givenElement);
    
    CircularList<TElem>::iterator it;
    for(it = begin(); it.position() < size(); it++)
        if( givenElement < it.current().info() )
            return push_at(givenElement, it.position());
    
    return push_back(givenElement);
}





/*          FIND         */
//Finds givenElement and returns its address.
template <class TElem>
Node<TElem>* CircularList<TElem>::find (const TElem& givenElement)
{
    CLIterator<TElem> it(HEAD);
    
    while(it.valid() &&
          it.current().info() != givenElement )
        it++;
    
    return  ( it.valid() &&
             it.current().info() == givenElement )? &it.current() : nullptr;
}

//Finds the previous element from the givenElement and returns its address.
template <class TElem>
Node<TElem>* CircularList<TElem>::findPreviousFrom(const TElem &givenElement)
{
    if ( find(givenElement) )
    {
        CLIterator<TElem> it(HEAD);
        
        while(it.valid() &&
              it.current().next() != nullptr &&
              it.current().next()->info() != givenElement )
            it++;
        
        return ( it.valid() &&
                it.current().next() != nullptr &&
                it.current().next()->info() == givenElement ) ? &it.current() : nullptr;
    }
    
    return nullptr;
}





/*          REMOVE         */
//Removes the first element in the list.
template <typename TElem>
bool CircularList<TElem>::remove_front()
{
    if( HEAD != nullptr )
    {
        Node<TElem>* willBeDeletedElement = HEAD;
        
        if( SIZE == 1 )
        {
            HEAD = TAIL = nullptr;
        }
        else
        {
            HEAD = HEAD->next();
            TAIL->setNext(HEAD);
        }
        
        delete willBeDeletedElement;
        SIZE--;
        return true;
        
    }//end IF
    
    return false;
}

//Removes the first element in the list.
template <typename TElem>
bool CircularList<TElem>::remove_back()
{
    if( HEAD != nullptr )
    {
        if ( SIZE == 1 )
            return remove_front();
        
        
        Node<TElem>* willBeDeletedElement = TAIL;
        Node<TElem>* previousOne = findPreviousFrom(TAIL->info());
        
        TAIL = previousOne;
        TAIL->setNext(HEAD);    //  Circular List.
        
        delete willBeDeletedElement;
        SIZE--;
        return true;
        
    }//end IF
    
    return false;
}

//Removes the givenElement from the list.
template <typename TElem>
bool CircularList<TElem>::remove(const TElem& givenElement)
{
    
    Node<TElem>* willBeDeletedElement = find(givenElement);
    
    if( willBeDeletedElement != nullptr)
    {
        
        if ( willBeDeletedElement == HEAD )
            return remove_front();
        
        if (willBeDeletedElement == TAIL)
            return remove_back();
        
        
        Node<TElem>* previosOne = findPreviousFrom(givenElement);
        previosOne->setNext( willBeDeletedElement->next() );
        delete willBeDeletedElement;
        SIZE--;
        return true;
        
        
    }//if FOUND
    
    return false;
}





//Returns the iterator whose current node is HEAD node.
template <typename TElem>
CLIterator<TElem> CircularList<TElem>::begin()
{
    iterator i(HEAD);
    
    return i;
}

//Returns the iterator whose current node is TAIL node.
template <typename TElem>
CLIterator<TElem> CircularList<TElem>::end()
{
    iterator i(TAIL);
    i.setPosition( SIZE -1 );
    
    return i;
}
