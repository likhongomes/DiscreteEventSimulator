#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <sstream>
#include "ReadData.c"

using namespace std;

/*

Likhon D. Gomes
CIS 3207 - Operating Systems
Lab 1 Georgio's Discrete Event Simulator.
TA: Chenglong Fu

01:41 a.m February 8th, 2019

*/

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

//Global variabled
double totalItemAddedInDiskQ1 = 0;
double totalItemAddedInDiskQ2 = 0;
double totalItemAddedInCpuQ = 0;

//The event class
class Event{
public:
  Event(int time, int jobSequenceNumber, int eventType){
    this->time = time; // This holds the job time to be executed.
    this->jobSequenceNumber = jobSequenceNumber; //The sequence number of the job
    this->eventType = eventType; // This determines the event type
  }
  int time;
  int jobSequenceNumber;
  int eventType;
};


//This is the sorting key for the vector
class less_than_key{
  inline bool operator() (const Event& class1, const Event& class2)
    {
        return (class1.time < class2.time);
    }
};


//This is the component class which can be used as the CPU or the disk.
class Component{
private:
  int eventType;
  int jobSequenceNumber = 0;
  bool idleState = true; // keeps track of the device state.
  int cpuClock = 0; // this keeps track of the device hardware
  int processTime = 1;
  int minTime; // this holds the min time for the component
  int maxTime; // this hold the max time for the componenet
  string name; // this holds the name of the componenet for debugging purposes

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

  //Actual function that executes the whole operation.
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
        processTime = rand()%maxTime-minTime + maxTime;
        responseTimes += processTime;
        if(pDebug)cout << name <<" just took a job: " << jobSequenceNumber << " job time: " << processTime << " cpu clock: " << cpuClock<< endl;
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
    //Here the queue is sorted once again.
    std::sort(priorityQ->begin(),priorityQ->end(),[ ]( const Event& lhs, const Event& rhs){
      return lhs.time < rhs.time;

    });
  }
};

//Takes an event from the queue and prints the log to the screen and a text file.
void printLog(Event element){
  int time = element.time;
  int jobSequenceNumber = element.jobSequenceNumber;
  int eventType = element.eventType;

  //opening the file to be printed.
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

//This function finds the exit probability of an event or if it will go to the disk.
void findExitProbability(Event *event, vector<Event> *priorityQ, int tick){
  srand(SEED);
  int probability = rand()%10+1;
  event->time = tick+1;
  if (probability%QUIT_PROB) { //calculating the probablity for the job to exit
      event->eventType = 6;
    } else { //load the job to the diskQ
      event->eventType = 3;
  }
  priorityQ->push_back(*event);
}

//This function sends the job to the disk with the shortest queue.
void sendToDisk(Event *event, vector<Event> *diskQ1, vector<Event> *diskQ2){
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
  //Opens the file to write the statistics in.
  ofstream file;
  file.open ("stats.txt");

  //Setting the seed for the program
  srand(SEED);

  //Setting all the parameters.
  vector<Event> priorityQ;
  vector<Event> cpuq;
  vector<Event> diskQ1;
  vector<Event> diskQ2;
  Component *cpu = new Component(CPU_MIN, CPU_MAX, 2, "CPU");
  Component *disk1 = new Component(DISK1_MIN,DISK1_MAX, 4, "disk1");
  Component *disk2 = new Component(DISK2_MIN, DISK2_MAX, 5, "disk2");

  //Creating first three events.
  Event *event1 = new Event(1,1,1);
  Event *event2 = new Event(2,2,1);
  Event *event3 = new Event(3,3,1);

  //Loading the first three events to the priority queue.
  priorityQ.push_back(*event1);
  priorityQ.push_back(*event2);
  priorityQ.push_back(*event3);

  
  int tick = INIT_TIME; //simulation clock

  //clock starts ticking
  while(!priorityQ.empty() && tick < FIN_TIME){

    
    //measuring the job creating time randomly
    int jobCreationTime = rand()%ARRIVE_MAX-ARRIVE_MIN + ARRIVE_MIN + tick;
    
    
    //Creating a new event and then pushing it to the queue
    Event *newEvent = (Event*)malloc(sizeof(Event));
    newEvent->eventType = 1;
    newEvent->time = jobCreationTime;
    newEvent->jobSequenceNumber = tick+1;

    //Pushing the job into the queue.
    priorityQ.push_back(*newEvent);

    //Sorting the queue after a job is pulled.
    std::sort(priorityQ.begin(),priorityQ.end(),[ ]( const Event& lhs, const Event& rhs){return lhs.time < rhs.time;});

    //Taking the first event in the queue
    Event *event = &priorityQ.front();
    priorityQ.erase(priorityQ.begin());
    printLog(*event);
  
    //Passing the event through the sorter to see what type of job it is.
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
        //event->eventType = 1;
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

  //Prints the statistics to the file.
  file << endl
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

   cout << endl << endl << "Please Check The Stats Document in your folder to check statistics." << endl; //Prints the last message reminding the user to check the stats file

}

