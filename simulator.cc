#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <sstream>


int INIT_TIME = 4;
int FIN_TIME = 1000;
int ARRIVE_MIN = 5;
int ARRIVE_MAX = 30;
int QUIT_PROB = 5;
int CPU_MIN = 1;
int CPU_MAX = 3;
int DISK1_MIN = 2;
int DISK1_MAX = 6;
int DISK2_MIN = 3;
int DISK2_MAX = 7;

bool cpuIdleState = true;

using namespace std;
std::vector<std::string> vec;

void readFile(){

  string output;
  std::ifstream myfile;
  myfile.open("configuration.txt");
  if (myfile.is_open()){
    while (getline(myfile, output)){
      vec.push_back(output);
    }
  } else {
    std::cout << "File not found" << std::endl;
  }
  cout << vec[0] << " " << vec[1] << endl;

  myfile.close();
  return;
}




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




class Component{
private:
  int eventType;
  int jobSequenceNumber = 0;
  bool idleState = true;
  int cpuClock = 0;
  int processTime = 1;
  int minTime;
  int maxTime;
  string name;
  Event *processEvent = new Event(0,0,0);
public:
  int totalOperation = 0;
  Component(int minTime, int maxTime, int eventType, string name){
    this->minTime = minTime;
    this->maxTime = maxTime;
    this->eventType = eventType;
    this->name = name;
  };

  void task(vector<Event> *cpuQ, int tick, Queue *disk1Q, Queue *disk2Q, vector<Event> *priorityQ){

    bool pDebug = false;
    if (idleState) { //if idle is true
    if(pDebug) cout << name << " is in idle mode " << endl;
      if (cpuQ->empty()) { //if the cpuQ is empty
        idleState = true;
        if(pDebug)cout << "que is empty" << endl;
        return;
      } else { //if cpuQ is not empty
        
        idleState = false; //set the process to busy
        Event processEvent = cpuQ->front();
        cpuQ->erase(cpuQ->begin());
        jobSequenceNumber = processEvent.jobSequenceNumber;
        //cout << "Process event time " << processEvent.time << endl;
        processTime = rand()%maxTime-minTime + maxTime;
        if(pDebug)cout << name <<" just took a job: " << jobSequenceNumber << " job time: " << processTime << " cpu clock: " << cpuClock<< endl;
        //printLog(eventToBeLoaded);
      }
    } else { // if cpu is busy
      if (cpuClock == processTime) {
        totalOperation++;
        int probability = rand()%10+1;
        Event *eventToBeLoaded = (Event*)malloc(sizeof(Event));
        eventToBeLoaded->eventType = eventType;
        eventToBeLoaded->time = tick+1;
        eventToBeLoaded->jobSequenceNumber = jobSequenceNumber;
        priorityQ->push_back(*eventToBeLoaded);
        cpuClock = 0;
        idleState = true;
      } else {
        cpuClock++;
        if(pDebug) cout << name << " is still processing job " << jobSequenceNumber << " waiting for time " << processTime << " cpu time " << cpuClock << endl; 
      }
    }
    
    std::sort(priorityQ->begin(),priorityQ->end(),[ ]( const Event& lhs, const Event& rhs){
      return lhs.time < rhs.time;

    });
  }
};






void printLog(Event element){
  int time = element.time;
  int jobSequenceNumber = element.jobSequenceNumber;
  int eventType = element.eventType;

  switch (eventType) {
    case 1: cout << "At time " << time << " Job " << jobSequenceNumber << " arrives" << endl;
    break;
    case 2: cout << "At time " << time << " Job " << jobSequenceNumber << " finishes at CPU" << endl;
    break;
    case 3: cout << "At time " << time << " Job " << jobSequenceNumber << " arrives at Disk" << endl;
    break;
    case 4: cout << "At time " << time << " Job " << jobSequenceNumber << " finishes IO at Disk 1" << endl;
    break;
    case 5: cout << "At time " << time << " Job " << jobSequenceNumber << " finishes IO at Disk 2" << endl;
    break;
    case 6: cout << "At time " << time << " Job " << jobSequenceNumber << " exits" << endl;
    break;
  }
}

void findExitProbability(Event *event, vector<Event> *priorityQ, int tick){
  int probability = rand()%10+1;
  event->time = tick+1;
  if (probability%QUIT_PROB) { //calculating the probablity for the job to exit
      //Event eventToBeLoaded = Event(processTime,jobSequenceNumber,6);
      event->eventType = 6;
    } else { //load the job to the diskQ
      event->eventType = 3;
  }
  priorityQ->push_back(*event);
  //printLog(*event);
}

void sendToDisk(Event *event, vector<Event> *diskQ1, vector<Event> *diskQ2){
  if(diskQ1->size()<diskQ2->size())
    diskQ1->push_back(*event);
  else
    diskQ2->push_back(*event);
}





int main(){

  //readFile();

  srand(time(NULL));
  Queue cpuQ;
  Queue disk1Q;
  Queue disk2Q;

  vector<Event> priorityQ;
  vector<Event> cpuq;
  vector<Event> diskQ1;
  vector<Event> diskQ2;
  Component *cpu = new Component(CPU_MIN, CPU_MAX, 2, "CPU");
  Component *disk1 = new Component(DISK1_MIN,DISK1_MAX, 4, "disk1");
  Component *disk2 = new Component(DISK2_MIN, DISK2_MAX, 5, "disk2");

  //Creating first 
  Event *event1 = new Event(1,1,1);
  Event *event2 = new Event(2,2,1);
  Event *event3 = new Event(3,3,1);
  //loading events to priorityQ
  priorityQ.push_back(*event1);
  priorityQ.push_back(*event2);
  priorityQ.push_back(*event3);

  //Event *event = new Event(0, 0, 0);
  
  int tick = INIT_TIME; //simulation clock
  while(!priorityQ.empty() && tick < FIN_TIME){

    

    //measuring the job creating time randomly
    int jobCreationTime = rand()%ARRIVE_MAX-ARRIVE_MIN + ARRIVE_MIN + tick;
    
    //Creating a new event and then pushing it to the queue
    Event *newEvent = (Event*)malloc(sizeof(Event));
    newEvent->eventType = 1;
    newEvent->time = jobCreationTime;
    newEvent->jobSequenceNumber = tick+1;

    priorityQ.push_back(*newEvent);

    std::sort(priorityQ.begin(),priorityQ.end(),[ ]( const Event& lhs, const Event& rhs){
      return lhs.time < rhs.time;

    });

    
    Event *event = &priorityQ.front();
    priorityQ.erase(priorityQ.begin());
    printLog(*event);
  
    
    switch (event->eventType){
      case 1: // code to be executed if n = 1;
        cpuq.push_back(*event);
          break;
      case 2: // code to be executed if n = 2;
          findExitProbability(event, &priorityQ, tick);
            break;
      case 3: // code to be executed if n = 3;
          sendToDisk(event, &diskQ1, &diskQ2);
          break;
      case 4: // code to be executed if n = 4;
        event->eventType = 1;
        //priorityQ.push_back(*event); //sending the job back to the queue
          break;
      case 5: // code to be executed if n = 5;
      //priorityQ.push_back(*event); // sending the job back to the queue
          break;
      case 6: // code to be executed if n = 6;
          break;
    }

    //starting the task for CPU & the Disks
    cpu->task(&cpuq, tick, &disk1Q, &disk2Q, &priorityQ);
    disk1->task(&cpuq, tick, &disk1Q, &disk2Q, &priorityQ);
    disk2->task(&cpuq, tick, &disk1Q, &disk2Q, &priorityQ);
    
    tick++; //incrementing the time of the clock
    
    
  }
/*
  while(!priorityQ.empty()){
    Event event = priorityQ.front();
    priorityQ.erase(priorityQ.begin());
    cout << event.eventType << " " << event.jobSequenceNumber <<endl;
  }
  */
  cout << "============= Stats ===============" << endl;
  cout << "List of items in cpuQ: " << cpuq.size() << endl;
  cout << "List of items in diskQ1: " << diskQ1.size() << endl;
  cout << "List of items in diskQ2: " << diskQ2.size() << endl;
  cout << "List of items in priorityQ: " << priorityQ.size() << endl;
  cout << endl;
  cout << "Total operations by the CPU: " << cpu->totalOperation << endl;
  cout << "Total operations by the Disk1: " << disk1->totalOperation << endl;
  cout << "Total operations by the Disk2: " << disk2->totalOperation << endl;
  cout << "===================================" << endl;
}
