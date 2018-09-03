#include "SinglyLinkedList.h"

//Constructor Function.
template <class TElem>
SinglyLinkedList<TElem>::SinglyLinkedList()
:HEAD(nullptr),TAIL(nullptr),SIZE(0),MAX_SIZE(65535)
{
    //MAX_SIZE is the maximum number where can be stored in 16 bit system.
    SSLIterator<TElem>(HEAD);
}

//Destructor Function.
template <class TElem>
SinglyLinkedList<TElem>::~SinglyLinkedList()
{
    SSLIterator<TElem> it (HEAD);
    
    while( it.valid () )
    {
        delete &it.current();
        it++;
    }
    
    HEAD = TAIL = nullptr;
    SIZE = 0;
    
}//end ~SinglyLinkedList





/*          GETTERS         */
//Returns the "size".
template <class TElem>
int SinglyLinkedList<TElem>::size() const
{
    return SIZE;
}

//Returns the "max_size".
template <class TElem>
int SinglyLinkedList<TElem>::max_size() const
{
    return MAX_SIZE;
}

//Returns "head".
template <class TElem>
Node<TElem>* SinglyLinkedList<TElem>::head() const
{
    return HEAD;
}

//Returns the reference of first element in the list.
template <class TElem>
TElem& SinglyLinkedList<TElem>::pop_front()
{
    return HEAD->info();
}

//Returns the reference of last element in the list.
template <class TElem>
TElem& SinglyLinkedList<TElem>::pop_back() const
{
    return TAIL->info();
}





/*          EMPTY-FULL         */
//Checks whether SinglyLinkedList is empty or not.
template <class TElem>
bool SinglyLinkedList<TElem>::isEmpty() const
{
    return (SIZE == 0)? true : false;
}

//Checks whether SinglyLinkedList is empty or not.
template <class TElem>
bool SinglyLinkedList<TElem>::isFull() const
{
    return ( !isEmpty() && SIZE==MAX_SIZE);
}





/*          ADD         */
//Adds the givenElement to the beginning of SinglyLinkedList.
template <class TElem>
bool SinglyLinkedList<TElem>::push_front(const TElem &givenElement)
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
        
        SIZE++;
        return true;
    }//If there is still space.
    
    return false;
}

//Adds the givenElement to the end of SinglyLinkedList.
template <class TElem>
bool SinglyLinkedList<TElem>::push_back(const TElem &givenElement)
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
        
        SIZE++;
        return true;
    }//If there is still space.
    
    return false;
}

//Adds the givenElement to the specified position.
template <class TElem>
bool SinglyLinkedList<TElem>::push_at(const TElem &givenElement, int position)
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
        SSLIterator<TElem> it(HEAD);
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
bool SinglyLinkedList<TElem>::push_after (const TElem& givenElement, const TElem& specifiedElement )
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
bool SinglyLinkedList<TElem>::push_sorted (const TElem& givenElement )
{
    if( isEmpty() )
        return push_front(givenElement);
    
    SinglyLinkedList<TElem>::iterator it;
    for(it = begin(); it.valid(); it++)
        if( givenElement < it.current().info() )
            return push_at(givenElement, it.position());
    
    return push_back(givenElement);
}





/*          FIND         */
//Finds givenElement and returns its address.
template <class TElem>
Node<TElem>* SinglyLinkedList<TElem>::find (const TElem& givenElement)
{
    SSLIterator<TElem> it(HEAD);
    
    while(it.valid() &&
          it.current().info() != givenElement )
        it++;
    
    return  ( it.valid() &&
             it.current().info() == givenElement )? &it.current() : nullptr;
}

//Finds the previous element from the givenElement and returns its address.
template <class TElem>
Node<TElem>* SinglyLinkedList<TElem>::findPreviousFrom(const TElem &givenElement)
{
    if ( find(givenElement) )
    {
        SSLIterator<TElem> it(HEAD);
        
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
bool SinglyLinkedList<TElem>::remove_front()
{
    if( HEAD != nullptr )
    {
        Node<TElem>* willBeDeletedElement = HEAD;
        
        (SIZE == 1)? HEAD=TAIL=nullptr : HEAD = HEAD->next();
        
        delete willBeDeletedElement;
        SIZE--;
        return true;
        
    }//end IF
    
    return false;
}

//Removes the first element in the list.
template <typename TElem>
bool SinglyLinkedList<TElem>::remove_back()
{
    if( HEAD != nullptr )
    {
        if ( SIZE == 1 )
            return remove_front();
        
        
        Node<TElem>* willBeDeletedElement = TAIL;
        Node<TElem>* previousOne = findPreviousFrom(TAIL->info());
        
        TAIL = previousOne;
        TAIL->setNext(nullptr);
        
        delete willBeDeletedElement;
        SIZE--;
        return true;
        
    }//end IF
    
    return false;
}

//Removes the givenElement from the list.
template <typename TElem>
bool SinglyLinkedList<TElem>::remove(const TElem& givenElement)
{
    
    Node<TElem>* willBeDeletedElement = find(givenElement);
    
    if( willBeDeletedElement != nullptr)
    {
        
        if ( willBeDeletedElement == HEAD )
            return remove_front();
        
        else if (willBeDeletedElement == TAIL)
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
SSLIterator<TElem> SinglyLinkedList<TElem>::begin()
{
    iterator i(HEAD);
    
    return i;
}

//Returns the iterator to the element past the end of the sequence.
template <typename TElem>
SSLIterator<TElem> SinglyLinkedList<TElem>::end()
{
    iterator i(TAIL);
    
    return i;
}
