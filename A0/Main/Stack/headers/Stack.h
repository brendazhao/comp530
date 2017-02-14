
#ifndef STACK_H
#define STACK_H

template <class Data>
// this is the node class used to build up the LIFO stack
class Node {
      
private:
	Data holdMe;
	Node *next;
	
public:
	Node(Data &_holdMe)
		:holdMe(_holdMe),next(NULL){};
	~Node(){}
	Node* getNext(){return next;}
	void setNext(Node* _next){next=_next;}
	Data getData(){return holdMe;}
};

// a simple LIFO stack
template <class Data> 
class Stack {
	Node <Data> *head;
	Node <Data> *tail;
public:

	// destroys the stack
	~Stack () {
 		while(head!= NULL){  
	        tail = head;  
	        head = head->getNext();  
	        delete tail;  
    	}  
    	tail = NULL;  
	}

	// creates an empty stack
	Stack () { 
		head = NULL;
		tail = NULL;
	}

	// adds pushMe to the top of the stack
	void push (Data data) { 
	    Node<Data> *cur = new Node<Data>(data);  
	    if(head == NULL){  
	        head = cur;  
	        tail = head;  
	    }  
	    else{
	    	//tail->next = cur;
	    	tail->setNext(cur);
	    	tail = tail->getNext();
    	}  
	}

	// return true if there are not any items in the stack
	bool isEmpty () { 
		return head==NULL;
	}
	
	// pops the item on the top of the stack off, returning it...
	// if the stack is empty, the behavior is undefined
	Data pop () { 
		Node<Data> *cur = head;
		Data value;
		if(head == tail){
            value = tail->getData();
			delete head;	
			head = NULL;
			tail = NULL;
		}
		else{	
			while(cur->getNext()!= tail)
				cur = cur->getNext();
			value=tail->getData();
			delete tail;	
			tail = cur;
			cur->setNext(NULL);
			//cur->next= NULL;
		}
		return value;		
	}
};

#endif
