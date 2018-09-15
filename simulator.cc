#include <iostream>

using namespace std;


template <class T>
class Queue{
private:
  typedef struct node{
    T data;
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

  void push(T element){
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

  T pop(){
    T toBeReturned;
    if (head != NULL) {
      toBeReturned = head->data;
      if(head->next != NULL){
        head = head->next;
      }
      return toBeReturned;
    } else {
      return 0;
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
      cout << current->data << " ";
      while(current->next != NULL){
        current = head->next;
        cout << current->data << " " << endl;
      }
    } else {
      cout << "it's empty"<< endl;
    }
  }

  void priortizeQ(){
    current = head;
    temporary = head->next;
    if (current->data > temporary->data) {
      std::cout << "it's bigger" << '\n';
    } else {
      cout << "it's not bigger" << endl;
    }
  }

};

struct Event{
public:
  Event(int time, int jobSequenceNumber, string eventType){
    time = this->time;
    jobSequenceNumber = this->jobSequenceNumber;
    eventType = this->eventType;
  }
private:
  int time;
  int jobSequenceNumber;
  string eventType;
};

int main(){
  Queue<double> q;
  Event *newEvent = new Event(10,22,"afadf");
  //q.push(newEvent);
  //Event *newEvent = new Event(10,22,"afadf");



  //q.push();
  q.printQ();
  q.priortizeQ();

  /*
  cout <<  "The Verdict " <<q.isempty() << endl;
  cout << "The size is "<<q.size << endl;
  cout << "popping " << q.pop() << endl;
  cout << "popping " << q.pop() << endl;
  std::cout << "is full " << q.isfull() << endl;
  */
}
