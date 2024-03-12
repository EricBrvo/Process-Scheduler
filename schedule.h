//
// Created by Eric Bravo on 2/22/24.
//

#ifndef ASSN2_SCHEDULING_SCHEDULE_H
#define ASSN2_SCHEDULING_SCHEDULE_H

#include <cstring>
#include <string.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>

#include "log.h"

using namespace std;

class Process {
public:
    vector<int> currentProcess; //vector that holds burst of each Process
    unsigned int procID = 0; //index number of incoming process
    unsigned int cpuExpectedTime = 0;  //all cpu bursts added together
    unsigned int ioExpectedTime = 0;   //all io bursts added together
    unsigned int turnAroundTime = 0;   //time it took to complete Process


    ///Process constructor assigns each process with its own id and vector of bursts
    Process(unsigned int id, const std::vector<int>& processVector);

};

class Schedule {
public:

    Schedule();
    unsigned int totalTimeElapsed = 0;  //keeps track of total time it took to execute all processes

    vector<Process> readyQ;             //vector of Processes representing the ready queue
    vector<Process> blockedQ;           //vector of Processes representing the blocked queue

    unsigned int specifiedQuantum = 2;  //quantum for round robin: default value set to 2

    ///scheduling algorithms being implemented
    void roundRobin(vector<Process>& processesBeingReadIn, unsigned int quantum);
    void firstComeFirstServe(vector<Process>& processesBeingReadIn);

};


#endif //ASSN2_SCHEDULING_SCHEDULE_H
