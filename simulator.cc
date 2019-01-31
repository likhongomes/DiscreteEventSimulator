#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>

int INIT_TIME = 0;
int FIN_TIME = 1000;
int ARRIVE_MIN = 5;
int ARRIVE_MAX = 30;
int QUIT_PROB = 5;
int CPU_MIN = 3;
int CPU_MAX = 5;
int DISK1_MIN = 2;
int DISK1_MAX = 6;
int DISK2_MIN = 3;
int DISK2_MAX = 7;



bool cpuIdleState = true;

using namespace std;


class Event{
public:
  Event(int time, int jobSequenceNumber, int eventType){
    this->time = time;
    this->jobSequenceNumber = jobSequenceNumber;
    this->eventType = eventType;
  }
  int time;
  int jobSequenceNumber;
  int eventType;
};

class less_than_key{
  inline bool operator() (const Event& class1, const Event& class2)
    {
        return (class1.time < class2.time);
    }
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
  Node tail;
public:
  int size = 0;
  Queue(){
    head = NULL;
    current = NULL;
    temporary = NULL;
    tail = NULL;
  }

  //pushing the Event in Queue
  void push(Event *element){
    Node newNode = new node;
    newNode->next = NULL;
    newNode->data = element;

    if(head == NULL){
      head = newNode;
      tail = head;
      size++;
    } else {
      current = head;
      while(current->next != NULL){
        current = current->next;
      }
      current->next = newNode;
      tail = current->next;
      size++;
    }
  }


  Event pop(){
    Event *toBeReturned;
    if (head != NULL) {
      toBeReturned = head->data;
      if (size >0) {
        size--;
      }

      if(head->next != NULL || head != NULL){
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
        current = current->next;
        cout << current->data->time << " ";
      }
    } else {
      cout << "it's empty"<< endl;
    }
    cout << endl;
  }

  Event begin(){
    return *head->data;
  }

  Event end(){
    return *tail->data;
  }


};




class CPU{
private:
  int time;
  int eventType;
  int jobSequenceNumber;
  bool idleState = true;
  int cpuClock = 0;
  int processTime = 1;
  Event *processEvent = new Event(0,0,0);
public:
  CPU(){};

  void task(Queue *cpuQ, int tick, Queue *disk1Q, Queue *disk2Q, vector<Event> *priorityQ){

    if (idleState) { //if idle is true
      if (cpuQ->isempty()) { //if the cpuQ is empty
        idleState = true;
        cout << "que is empty" << endl;
        return;
      } else { //if cpuQ is not empty
        cout << "cpu is processing" << endl;
        idleState = false; //set the process to busy
        Event processEvent = cpuQ->pop();
        jobSequenceNumber = processEvent.jobSequenceNumber;
        processTime = rand()%CPU_MAX-CPU_MIN + CPU_MAX+ processEvent.time;
        
        //printLog(eventToBeLoaded);
      }
    } else { // if cpu is busy
      if (cpuClock == processTime) {

        int probability = rand()%10+1;
        Event eventToBeLoaded = Event(processTime,jobSequenceNumber,2);
        priorityQ->push_back(eventToBeLoaded);

        if (probability%QUIT_PROB) { //calculating the probablity for the job to exit
          eventType = 6;
        } else { //load the job to the diskQ
          eventType = 3;
          if (disk1Q->size < disk2Q->size) {
            disk1Q->push(&eventToBeLoaded);
          } else {
            disk2Q->push(&eventToBeLoaded);
          }
        }
        priorityQ->push_back(eventToBeLoaded);



        //Event completedJob = Event(tick+1,jobSequenceNumber,)
        cout << "Job completed " << endl;
        idleState = true;
      } else {
        cpuClock++;
      }
    }
    
    std::sort(priorityQ->begin(),priorityQ->end(),[ ]( const Event& lhs, const Event& rhs){
      return lhs.time < rhs.time;

    });
  }
};

class Disk{
private:
  int time;
  int eventType;
  int toBeEventType;
  int jobSequenceNumber;
  bool idleState = true;
  int waitTime;
  int processTime = 1;
  int DISK_MAX;
  int DISK_MIN;
  Event *processEvent = new Event(0,0,0);

public:
  Disk(int DISK_MAX, int DISK_MIN,int toBeEventType){
    this->DISK_MAX = DISK_MAX;
    this->DISK_MIN = DISK_MIN;
    this->toBeEventType = toBeEventType;
  };

  void task(int tick, Queue *diskQ, vector<Event> *priorityQ){
  //  if (debugModeOn) {cout << "size of cpuQ = "<<diskQ->size << endl;}
    if (idleState) {

      if (diskQ->isempty()) {
        idleState = true;
      //  if(debugModeOn){cout << "Diskque is empty" << endl;
      } else {
        //if(debugModeOn){cout << "Invoked jjjjjjjjjjjjjjjjj" << endl;}
        idleState = false;
        Event processEvent = diskQ->pop();
        jobSequenceNumber = processEvent.jobSequenceNumber;
  //      if(debugModeOn){cout << "Old Process Time " << processEvent.time <<" " << time << " "<< endl;}
        processTime = rand()%DISK_MAX-DISK_MIN + DISK_MAX+ processEvent.time;
        //processEvent.time = processTime;
    //    if(debugModeOn){cout << "New Process time " << processEvent.time << endl;}

        Event eventToBeLoaded = Event(processTime,jobSequenceNumber,eventType);
        cout <<  "CPU Event " << eventToBeLoaded.time <<" " << eventToBeLoaded.jobSequenceNumber << endl;
        priorityQ->push_back(eventToBeLoaded);
        cout << "Process Time " << processTime << endl;
      }
      //cout << "CPU IS BUSY" << endl;
    } else {
      cout << "checking " << tick << " " << processTime << endl;
      if (tick == processTime) {
        cout << "CPU UNlocked" << endl;
        idleState = true;
      }
    }
  }
};




void printLog(Event element){
  int time = element.time;
  int jobSequenceNumber = element.jobSequenceNumber;
  int eventType = element.eventType;

  switch (eventType) {
    case 1: cout << "At time " << time << " Job" << jobSequenceNumber << " arrives" << endl;
    break;
    case 2: cout << "At time " << time << " Job" << jobSequenceNumber << " finishes at CPU" << endl;
    break;
    case 3: cout << "At time " << time << " Job" << jobSequenceNumber << " arrives at Disk" << endl;
    break;
    case 4: cout << "At time " << time << " Job" << jobSequenceNumber << " finishes IO at Disk 1" << endl;
    break;
    case 5: cout << "At time " << time << " Job" << jobSequenceNumber << " finishes IO at Disk 2" << endl;
    break;
    case 6: cout << "At time " << time << " Job" << jobSequenceNumber << " exits" << endl;
    break;
  }
}



int main(){

  srand(time(NULL));
  Queue cpuQ;
  Queue disk1Q;
  Queue disk2Q;
  vector<Event> priorityQ;
  CPU *cpu = new CPU;
  Disk *disk1 = new Disk(DISK1_MAX,DISK1_MIN, 4);
  Disk *disk2 = new Disk(DISK2_MAX,DISK2_MIN, 5);
  //Creating first 
  Event *event1 = new Event(1,6,1);
  Event *event2 = new Event(2,2,1);
  Event *event3 = new Event(3,3,1);
  Event *event4 = new Event(4, 4, 1);
  Event *event5 = new Event(5, 5, 1);
  Event *event6 = new Event(6, 6, 1);
  //loading events to priorityQ
  priorityQ.push_back(*event1);
  priorityQ.push_back(*event2);
  priorityQ.push_back(*event3);
  priorityQ.push_back(*event4);
  priorityQ.push_back(*event5);
  priorityQ.push_back(*event6);

  Event *event = new Event(0, 0, 0);

  int tick = INIT_TIME; //simulation clock
  while(!priorityQ.empty() && tick < FIN_TIME){
    //measuring the job creating time randomly
    int jobCreationTime = rand()%ARRIVE_MAX-ARRIVE_MIN + ARRIVE_MIN + tick;
    
    //Creating a new event and then pushing it to the queue
    Event *newEvent = new Event(jobCreationTime,tick+1,1);
    priorityQ.push_back(*newEvent);
    
    //popping the first element from the queue
    event = &priorityQ.front();
    priorityQ.erase(priorityQ.begin());
    printLog(*event);
    
    if(event->eventType ==1){ //job created, send it to CPU
      cpuQ.push(event);
    } else if(event->eventType == 2){
      //push to diskQ
    }
    //pushing it to the cpuQ
    //cpuQ.push(newEvent);

    //starting the task for CPU & the Disks
    cpu->task(&cpuQ, tick, &disk1Q, &disk2Q, &priorityQ);
    //disk1->task(tick, &disk1Q, &priorityQ);
    //disk2->task(tick, &disk1Q, &priorityQ);
    //printing the log
    

    
    
    tick++; //incrementing the time of the clock
  }
  
  cout << "============= Stats ===============";
  cout << "list of items in cpuQ: " << cpuQ.size << endl;
  cout << "List of items in diskQ1: " << disk1Q.size << endl;
  cout << "List of items in diskQ2: " << disk2Q.size << endl;
  cout << "List of items in priorityQ: " << priorityQ.size() << endl;

}
