#include <iostream>

using namespace std;


class Event{
public:
  Event(int time, int jobSequenceNumber, string eventType){
    this->time = time;
    this->jobSequenceNumber = jobSequenceNumber;
    this->eventType = eventType;
  }
  int time;
  int jobSequenceNumber;
  string eventType;
};

class Queue{
private:
  typedef struct node{
    Event *data;
    node *next;
  } *Node;

  Node head;
  Node current;
  Node temporary;
public:
  int size = 0;
  Queue(){
    head = NULL;
    current = NULL;
    temporary = NULL;
  }

  void push(Event *element){
    Node newNode = new node;
    newNode->next = NULL;
    newNode->data = element;

    if(head == NULL){
      head = newNode;
      size++;
    } else {
      current = head;
      while(current->next != NULL){
        current = current->next;
      }
      current->next = newNode;
      size++;
    }
  }

  Event pop(){
    Event *toBeReturned;
    if (head != NULL) {
      toBeReturned = head->data;
      if(head->next != NULL){
        head = head->next;
      }
      return *toBeReturned;
    } else {
      toBeReturned = NULL;
      return *toBeReturned;
    }
  }

  bool isempty(){
    if(head == NULL){
      return true;
    } else return false;
  }
  bool isfull(){
    if (head != NULL) {
      return true;
    } else return false;
  }

  void printQ(){
    cout << "The printed Que is ";
    if (head != NULL) {
      Node current = head;
      cout << current->data->time << " ";
      while(current->next != NULL){
        current = head->next;
        cout << current->data->time << " " << endl;
      }
    } else {
      cout << "it's empty"<< endl;
    }
  }
};

int main(){
  Queue q;
  Event *newEvent = new Event(0,22,"afadf");
  q.push(newEvent);
  Event *newEvent1 = new Event(20,22,"afadf");
  q.push(newEvent1);
  Event *newEvent2 = new Event(300,22,"afadf");
  //q.push(newEvent2);

  //q.push();
  q.printQ();
  //q.priortizeQ();


  //cout <<  "The Verdict " <<q.isempty() << endl;
  cout << "The size is "<<q.size << endl;
  //cout << "popping " << q.pop() << endl;
  //cout << "popping " << q.pop() << endl;
  //std::cout << "is full " << q.isfull() << endl;

}
