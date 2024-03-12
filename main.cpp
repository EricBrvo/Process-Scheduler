#include "log.h"
#include <iostream>
#include <cstring>
#include <string.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <sstream>
#include "schedule.h"
#include <unistd.h>

#define NORMAL_EXIT 0


int main(int argc, char **argv) {

    Schedule schedule;

    std::string burstFile = argv[1];     ///reads in mandatory argument (bursts.txt)
    std::ifstream firstProcess(burstFile);
    vector<Process> processesBeingReadIn;  //VECTOR HOLDS ALL PROCESS (P0,P1,P2)
    std::string line;

    if (!firstProcess.is_open()) {
        std::cerr << "Unable to open file " << burstFile << std::endl;   //error handling: check if file cant be opened
        return 1;
    }


    ///reads in option arguments (strategy,quantum)
    int opt;
    std:: string strategy = "fcfs";     //default strategy initialized to FCFS
    int quant = 0;                      //variable to store specified quantum
    while ((opt = getopt(argc, argv, "s:q:")) != -1) {

        switch(opt){

            case 's':               //if strategy is provided (rr) then assign
                strategy = optarg;
                break;

            case 'q':               //if quantum is specified then store
                quant = atoi(optarg);
                if(quant <= 0){                 //error handling: if quantum is 0 or negative number then terminate
                    cout << "Time quantum must be a number and bigger than 0" << std::endl;
                    exit(NORMAL_EXIT);
                }
                break;

        }
    }


    ///reading in lines from file
    int idCount = 0;            //counts number of Processes being read in from file
    while (std::getline(firstProcess, line)) {

        std::vector<int> processVector;  //temp vector that holds all bursts of current Process that is being read in
        std::istringstream iss(line);
        int processBurst;    //temp variable that holds current int (burst) on the current line/Process
        while (iss >> processBurst) {

            if(processBurst <= 0){
                cout << "A burst number must be bigger than 0" << std::endl;  //error handling: if burst is negative then terminate
                exit(NORMAL_EXIT);
            }else{
                processVector.push_back(processBurst);   //if it is a valid burst then push to vector for current Process
            }

        }
        if(processVector.size() %2 ==0){
            cout << "There must be an odd number of bursts for each process" << std::endl;  //error handling: if the number of bursts in the process is even then terminate
            exit(NORMAL_EXIT);
        }
        Process process(idCount, processVector);   //create a new process with its own id and vector of bursts
        processesBeingReadIn.push_back(process);      //add newly created Process to a vector (will symbolize ready queue)
        idCount++;                                   //increment id for the next Process to be read in
    }

    ///once all processes are stored, iterate and print their values (bursts)
    for (const auto& process : processesBeingReadIn) {

        for (int data : process.currentProcess) {
            std::cout << data << " ";
        }
        std::cout << std::endl;
    }


    ///deciding which algorithm to run

    if(strategy == "rr"){       //strategy rr (round robin) is specified

        if(quant > 0){
            schedule.specifiedQuantum = quant;      //assign quantum value to the quantum specified by the user
        }
        schedule.roundRobin(processesBeingReadIn,schedule.specifiedQuantum);    //call round robin with vector of all Processes and quantum

    }else{
        schedule.firstComeFirstServe(processesBeingReadIn); //strategy is not specified, default call first come first serve
    }

    return 0;
}
