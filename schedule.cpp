//
// Created by Eric Bravo on 2/22/24.
//

#include "schedule.h"
#include "log.h"


//Process constructor that assigns each Process its own id and vector of bursts
Process::Process(unsigned int id, const std::vector<int>& processVector){
    procID = id;
    currentProcess = processVector;
}


Schedule::Schedule() {}


//stable sort
bool compareProcesses(const Process& p1, const Process& p2) {
    return p1.currentProcess.front() < p2.currentProcess.front();
}



///Round Robin Logic///

/*
 * All processes read in from the file are pushed to the ready queue in their respective orders for execution.
 * From the ready queue we extract the first process and its cpu burst and compare it with the quantum time.
 * We then take the minimum between the current process cpu burst and the quantum which will dictate how long we will
 * execute a burst. As we execute the burst we also update the current Processes attributes such as its executed CPU
 * IO burst in addition to the schedulers total elapsed time. As the cpu burst is taking place we are also executing the
 * processes in the IO queue by the same amount.
 */


void Schedule::roundRobin(vector<Process>& processesBeingReadIn, unsigned int quantum) {

    vector<Process> completedQ;     //queue to store completed processes
    unsigned int cpuBurst = 0;      //stores extracted cpu burst
    unsigned int ioBurst = 0;       //stores extracted io Burst
    readyQ = processesBeingReadIn;  //initialize ready queue


    //!START OF ROUND ROBIN

    while(!readyQ.empty() || !blockedQ.empty()){

        if(!readyQ.empty()){

            Process curr = readyQ[0];   // GETS FIRST PROCESS IN READY AND STORES IT
            readyQ.erase(readyQ.begin()); //DELETES FIRST PROCESS IN READY


            if (!curr.currentProcess.empty()) {
                cpuBurst = curr.currentProcess[0];    //extract cpu burst
                curr.currentProcess.erase(curr.currentProcess.begin()); //deletes it
            }

            unsigned int count = 0;

            for (unsigned int i = 0; i < std::min(cpuBurst,quantum); i++) {  //execute burst by either process cpu burst or quantum tim

                curr.cpuExpectedTime++;
                totalTimeElapsed++;         //execute cpu burst
                curr.turnAroundTime++;
                count++;

                for(auto it = blockedQ.begin(); it != blockedQ.end();){  //iterate through blocked executing io burst by the same amount

                    Process& blockedProcess = *it;
                    blockedProcess.ioExpectedTime++;
                    curr.turnAroundTime++;

                    if (!blockedProcess.currentProcess.empty()) {
                        blockedProcess.currentProcess[0]--;

                        //if process io Burst is finished then push back to ready queue
                        if (blockedProcess.currentProcess[0] == 0) {
                            blockedProcess.currentProcess.erase(blockedProcess.currentProcess.begin()); //ERASE ioBURST
                            readyQ.push_back(blockedProcess);
                            it = blockedQ.erase(it); // Update iterator after erase
                        } else {
                            ++it; // Increment iterator
                        }
                    } else {
                        // Handle empty currentProcess array case
                        ++it; // Increment iterator
                    }
                }

            }


            ///After process has been executed it has three options (return back to readyQ, enter io or push to completed)

            if( (cpuBurst - count) > 0){
                //if process has cpu burst left over after execution then push to the back of the ready queue
                curr.currentProcess.insert(curr.currentProcess.begin(), cpuBurst-count);
                readyQ.push_back(curr);
                log_cpuburst_execution(curr.procID, curr.cpuExpectedTime, curr.ioExpectedTime, totalTimeElapsed,
                                       QUANTUM_EXPIRED);

            }else if(curr.currentProcess.empty()){

                //if current process has no more remaining bursts then it has been completed
                //update its turn around time and push to completed queue
                curr.turnAroundTime = totalTimeElapsed;
                completedQ.push_back(curr);
                log_cpuburst_execution(curr.procID, curr.cpuExpectedTime, curr.ioExpectedTime, totalTimeElapsed,
                                       COMPLETED);

            }
            else{
                //current process has finished its cpu burst so it is pushed to blocked queue
                //after it has been push we stable sort to ensure priority is correct
                blockedQ.push_back(curr);
                std::stable_sort(blockedQ.begin(), blockedQ.end(), compareProcesses); ///stable sort
                log_cpuburst_execution(curr.procID, curr.cpuExpectedTime, curr.ioExpectedTime, totalTimeElapsed,
                                       ENTER_IO);
            }

        }else{

            ///IF EVERYTHING IS IN BLOCKED QUEUE PUSH FIRST ITEM TO READY

            Process firstBlocked = blockedQ[0];   // GETS FIRST PROCESS IN BLOCKED AND STORES IT
            ioBurst = firstBlocked.currentProcess[0];    //extract ioBurst
            auto it = blockedQ.begin(); //initialize iterator for parsing through blocked queue
            totalTimeElapsed += ioBurst;                //update total time

            while (it != blockedQ.end()) {      //iterate through io queue
                Process &blockedP = *it;
                blockedP.ioExpectedTime +=ioBurst;      //decrement all processes by the io burst of the leading process
                blockedP.currentProcess[0] -= ioBurst;

                if (blockedP.currentProcess[0] == 0) {      //if the processes io burst is finished then push to ready queue
                    blockedP.currentProcess.erase(blockedP.currentProcess.begin()); //ERASE ioBURST
                    readyQ.push_back(blockedP);
                    it = blockedQ.erase(it); // Update iterator after erase
                }else {
                    // Handle empty currentProcess array case
                    ++it; // Increment iterator
                }

            }

        }

    }

    ///calculate and print wait times and turn around times for each process
    for (const auto& completeP : completedQ) {
        int waitTime = (completeP.turnAroundTime - completeP.cpuExpectedTime) - completeP.ioExpectedTime;
        log_process_completion(completeP.procID,completeP.turnAroundTime,waitTime);
    }

}




/// FCFS LOGIC///

/*
 * All processes read in from the file are pushed to the ready queue in their respective orders for execution.
 * From the ready queue we extract the first process and its cpu burst which dictates how long it will be executed.
 * As we execute the burst we also update the current Processes attributes such as its executed CPU
 * IO burst in addition to the schedulers total elapsed time. As the cpu burst is taking place we are also executing the
 * processes in the IO queue by the same amount.
 */

void Schedule::firstComeFirstServe(vector<Process> &processesBeingReadIn) {


    vector<Process> completedQ;     //stores completed process once their bursts are finished
    unsigned int cpuBurst = 0;      //stores current processes cpu burst
    unsigned int ioBurst = 0;       //stores current processes io burst
    readyQ = processesBeingReadIn;  //initialize ready queue

    while(!readyQ.empty() || !blockedQ.empty()){

        if(!readyQ.empty()) {

            Process curr = readyQ[0];   // extracts first process from the ready queue
            readyQ.erase(readyQ.begin()); //pop first process


            if (!curr.currentProcess.empty()) {
                cpuBurst = curr.currentProcess[0];    //extract cpu burst
                curr.currentProcess.erase(curr.currentProcess.begin()); //deletes it
            }


            for (unsigned int i = 0; i < cpuBurst; i++) {  //similuates procceses cpu burst exectuion

                curr.cpuExpectedTime++;
                totalTimeElapsed++;         //execute cpu burst
                curr.turnAroundTime++;      //update time counts


                ///as we execute the cpu burst we are also doing the same for processes in the blockedQ
                for(auto it = blockedQ.begin(); it != blockedQ.end();){  //iterate through blockedQ

                    Process& blockedProcess = *it;
                    blockedProcess.ioExpectedTime++;
                    curr.turnAroundTime++;

                    if (!blockedProcess.currentProcess.empty()) {
                        blockedProcess.currentProcess[0]--;         //decrement blocked proccesses io bursts

                        //if process io burst is finished then push to ready queue
                        if (blockedProcess.currentProcess[0] == 0) {
                            blockedProcess.currentProcess.erase(blockedProcess.currentProcess.begin()); //ERASE ioBURST
                            readyQ.push_back(blockedProcess);
                            it = blockedQ.erase(it); // Update iterator after erase
                        } else {
                            ++it; // Increment iterator
                        }
                    } else {
                        // Handle empty currentProcess array case
                        ++it; // Increment iterator
                    }
                }

            }


            ///After current processes cpu burst is finished executing it has 2 options
            if(curr.currentProcess.empty()){
                //if all burst of the current process are finished then update turn around time and push to completedQ
                curr.turnAroundTime = totalTimeElapsed;
                completedQ.push_back(curr);
                log_cpuburst_execution(curr.procID, curr.cpuExpectedTime, curr.ioExpectedTime, totalTimeElapsed,
                                       COMPLETED);

            }else{
                //process finished cpu burst but still has remaining bursts then push to blocked queue
                //after we push to blocked queue we stable sort to maintain correct priority
                blockedQ.push_back(curr);
                std::stable_sort(blockedQ.begin(), blockedQ.end(), compareProcesses); ///stable sort
                log_cpuburst_execution(curr.procID, curr.cpuExpectedTime, curr.ioExpectedTime, totalTimeElapsed,
                                       ENTER_IO);
            }


        }else{

            ///IF ALL PROCESSES ARE IN THE BLOCKED QUEUE

            Process firstBlocked = blockedQ[0];   // GETS FIRST PROCESS IN BLOCKED AND STORES IT
            ioBurst = firstBlocked.currentProcess[0];    //extract ioBurst

            auto it = blockedQ.begin(); //initialize iterator for parsing blocked queue
            totalTimeElapsed += ioBurst;                //update total time

            while (it != blockedQ.end()) {          //iterate through the length of the blocked queue
                Process &blockedP = *it;
                blockedP.ioExpectedTime +=ioBurst;      //execute io bursts
                blockedP.currentProcess[0] -= ioBurst;  //decrement all io burst for all processes by the io burst of the first process

                //if current process is finished with its io burst then push to ready queue
                if (blockedP.currentProcess[0] == 0) {
                    blockedP.currentProcess.erase(blockedP.currentProcess.begin()); //ERASE ioBURST
                    readyQ.push_back(blockedP);
                    it = blockedQ.erase(it); // Update iterator after erase
                }else {
                    // Handle empty currentProcess array case
                    ++it; // Increment iterator
                }

            }

        }

    }

    ///calculate wait time and turn around time for all processes and print them
    for (const auto& p : completedQ) {
        int waitTime = (p.turnAroundTime - p.cpuExpectedTime) - p.ioExpectedTime;
        log_process_completion(p.procID,p.turnAroundTime,waitTime);
    }


}