#ifndef Node_h
#define Node_h

template <class TElem>
class Node
{
    
private:
    TElem INFO;
    Node* NEXT;
    
public:
    
    //Constructor Function with parameter.
    Node(const TElem &givenInfo):INFO(givenInfo), NEXT(nullptr)
    {}
    
    
    //Returns "INFO".
    TElem& info()
    {
        return INFO;
    }
    
    
    //Assigns givenInfo to INFO.
    Node& setInfo(const TElem& givenInfo)
    {
        INFO = givenInfo;
        return *this;
    }
    
    
    //Returns "NEXT".
    Node* next() const
    {
        return NEXT;
    }
    
    //Assigns givenAddress to "NEXT".
    Node& setNext(Node* givenAddress)
    {
        NEXT = givenAddress;
        return *this;
    }
    
    
    //Checks whether the right hand side node's info is equal to INFO.
    bool operator==(const Node<TElem> &right)
    {
        return INFO == right.INFO;
    }
    
};

/*
 Make the template definition visible to compiler in the point of instantiation.
 Instantiate the types you need explicitly in a separate compile unit, so that linker can find it.
 Use keyword export.
 
 1) All member functions inlined
 2) #include "whereTheSourcedCodeIs.cpp"
 3) Mark the template definitions with the keyword "export".
 
 
 */

#endif
