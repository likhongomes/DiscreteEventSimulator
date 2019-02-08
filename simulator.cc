#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <sstream>
#include "ReadData.c"


//Reading data from the document
int SEED = atoi(getValue("configuration.txt", "SEED", 0, 1));
float INIT_TIME = atoi(getValue("configuration.txt", "INIT_TIME", 0, 1));
float FIN_TIME = atoi(getValue("configuration.txt", "FIN_TIME", 0, 1));
int ARRIVE_MIN = atoi(getValue("configuration.txt", "ARRIVE_MIN", 0, 1));
int ARRIVE_MAX = atoi(getValue("configuration.txt", "ARRIVE_MAX", 0, 1));
int QUIT_PROB = atoi(getValue("configuration.txt", "QUIT_PROB", 0, 1));
int CPU_MIN = atoi(getValue("configuration.txt", "CPU_MIN", 0, 1));
int CPU_MAX = atoi(getValue("configuration.txt", "CPU_MAX", 0, 1));
int DISK1_MIN = atoi(getValue("configuration.txt", "DISK1_MIN", 0, 1));
int DISK1_MAX = atoi(getValue("configuration.txt", "DISK1_MAX", 0, 1));
int DISK2_MIN = atoi(getValue("configuration.txt", "DISK2_MIN", 0, 1));
int DISK2_MAX = atoi(getValue("configuration.txt", "DISK2_MAX", 0, 1));

double totalItemAddedInDiskQ1 = 0;
double totalItemAddedInDiskQ2 = 0;
double totalItemAddedInCpuQ = 0;


using namespace std;
std::vector<std::string> vec;


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
  int responseTimes = 0;
  int totalOperation = 0;
  float totalTimeComponentIsBusy = 0;
  Component(int minTime, int maxTime, int eventType, string name){
    this->minTime = minTime;
    this->maxTime = maxTime;
    this->eventType = eventType;
    this->name = name;
  };

  void task(vector<Event> *cQueue, int tick, vector<Event> *priorityQ){
    srand(SEED);
    bool pDebug = false; //Set it to true for debugging purposes

    if (idleState) { //if idle is true
      if(pDebug) cout << name << " is in idle mode " << endl;
      if (cQueue->empty()) { //if the cQueue is empty
        idleState = true;
        if(pDebug)cout << "que is empty" << endl;
        return;
      } else { //if cQueue is not empty
        
        idleState = false; //set the process to busy
        Event processEvent = cQueue->front();
        cQueue->erase(cQueue->begin());
        jobSequenceNumber = processEvent.jobSequenceNumber;
        //cout << "Process event time " << processEvent.time << endl;
        processTime = rand()%maxTime-minTime + maxTime;
        responseTimes += processTime;
        if(pDebug)cout << name <<" just took a job: " << jobSequenceNumber << " job time: " << processTime << " cpu clock: " << cpuClock<< endl;
        //printLog(eventToBeLoaded);
      }
    } else { // if cpu is busy
      totalTimeComponentIsBusy++;
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
  ofstream file;
  file.open ("log.txt",ios_base::app);

  switch (eventType) {
    case 1: cout << "At time " << time << " Job " << jobSequenceNumber << " arrives" << endl;
    file << "At time " << time << " Job " << jobSequenceNumber << " arrives" << endl;
    break;
    case 2: cout << "At time " << time << " Job " << jobSequenceNumber << " finishes at CPU" << endl;
    file << "At time " << time << " Job " << jobSequenceNumber << " finishes at CPU" << endl;
    break;
    case 3: cout << "At time " << time << " Job " << jobSequenceNumber << " arrives at Disk" << endl;
    file << "At time " << time << " Job " << jobSequenceNumber << " arrives at Disk" << endl;
    break;
    case 4: cout << "At time " << time << " Job " << jobSequenceNumber << " finishes IO at Disk 1" << endl;
    file << "At time " << time << " Job " << jobSequenceNumber << " finishes IO at Disk 1" << endl;
    break;
    case 5: cout << "At time " << time << " Job " << jobSequenceNumber << " finishes IO at Disk 2" << endl;
    file << "At time " << time << " Job " << jobSequenceNumber << " finishes IO at Disk 2" << endl;
    break;
    case 6: cout << "At time " << time << " Job " << jobSequenceNumber << " exits" << endl;
    file << "At time " << time << " Job " << jobSequenceNumber << " exits" << endl;
    break;
  }
}

void findExitProbability(Event *event, vector<Event> *priorityQ, int tick){
  srand(SEED);
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
  //cout << "sending to disk" << endl;
  if(diskQ1->size()<diskQ2->size()){
    cout << endl;
    diskQ1->push_back(*event);
    
    totalItemAddedInDiskQ1++;
  } else {
    diskQ2->push_back(*event);
    totalItemAddedInDiskQ2++;
  } 
}





int main(){
  ofstream file;
  file.open ("stats.txt");


  //readFile();

  srand(SEED);

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

    cout << " Disk1 size " << diskQ1.size() << endl;
    cout << "Disk2 size "<<diskQ2.size() << endl;
    
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

    
    //cout << "diskQ1 size: "<<diskQ1.size() << endl;
    //cout << "diskQ2 size: "<<diskQ2.size() << endl;

    Event *event = &priorityQ.front();
    priorityQ.erase(priorityQ.begin());
    printLog(*event);
  
    
    switch (event->eventType){
      case 1: // code to be executed if n = 1;
        cpuq.push_back(*event);
        totalItemAddedInCpuQ++;
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
    cpu->task(&cpuq, tick, &priorityQ);
    disk1->task(&diskQ1, tick, &priorityQ);
    disk2->task(&diskQ2, tick, &priorityQ);
    
    tick++; //incrementing the time of the clock
    
    
  }


  cout << endl
   << "============= Stats ===============" << endl
   << "Total time ran: " << FIN_TIME - INIT_TIME << " units" << endl
   << "========== Queue Stats ============" << endl
   << "Incomplete tasks in cpuQ: " << cpuq.size() << endl
   << "Total Item added in Disk 1: " << totalItemAddedInDiskQ1 << endl
   << "Incomplete tasks in diskQ1: " << diskQ1.size() << endl
   << "Total Item added in Disk 2: " << totalItemAddedInDiskQ2 << endl
   << "Incomplete tasks in diskQ2: " << diskQ2.size() << endl
   << "Incomplete tasks in priorityQ: " << priorityQ.size() << endl
   << "======== Component Stats ==========" << endl
   << endl
  
  
   << "========== Disk1 Stats ============" << endl
   << "Total time disk1 is busy doing task: " << disk1->totalTimeComponentIsBusy << " units " << endl
   << "Average time disk1 is busy doing task: " << (disk1->totalTimeComponentIsBusy/(FIN_TIME-INIT_TIME)) << " units " << endl
   << "Total jobs completed by disk1: " << disk1->totalOperation << endl
   << "Job processing average in disk1: " << disk1->totalOperation/totalItemAddedInDiskQ1 << endl
   << "Average disk1 queue size: " << totalItemAddedInDiskQ1 << endl
   << "Average response time: " << disk1->responseTimes/disk1->totalOperation << " units"<< endl
   << "===================================" << endl
   << endl
   << "========== Disk2 Stats ============" << endl
   << "Total time disk2 is busy doing task: " << disk2->totalTimeComponentIsBusy << " units "<< endl
   << "Average time disk2 is busy doing task: " << (disk2->totalTimeComponentIsBusy/(FIN_TIME-INIT_TIME)) << " units " << endl
   << "Total jobs completed by disk2: " << disk2->totalOperation << endl
   << "Job processing average in disk2: " << disk2->totalOperation/totalItemAddedInDiskQ2 << endl
   << "Average disk2 queue size: " << totalItemAddedInDiskQ2 << endl
   << "Average response time: " << disk2->responseTimes/disk2->totalOperation << " units " << endl
   << "===================================" << endl
   << endl
   << "=========== CPU Stats =============" << endl
   << "Total time Cpu is busy doing task: " << cpu->totalTimeComponentIsBusy << " units " << endl
   << "Average time disk1 is busy doing task: " << (cpu->totalTimeComponentIsBusy/(FIN_TIME-INIT_TIME)) << " units " << endl
   << "Total jobs completed by the CPU: " << cpu->totalOperation << endl
   << "Job processing average in disk2: " << cpu->totalOperation/totalItemAddedInCpuQ << endl
   << "Average cpu queue size: " << totalItemAddedInCpuQ << endl
   << "Average response time: " << cpu->responseTimes/cpu->totalOperation << " units " << endl
   << "===================================" << endl;



}

