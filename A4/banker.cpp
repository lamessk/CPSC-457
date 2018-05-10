/*
 * banker.cpp
 *
 * Student Name: Lamess Kharfan
 * Student Number: 10150607
 *
 * Class: CPSC 457 Winter 2018
 * Instructor: Pavol Federl
 *
 * Copyright 2017 University of Calgary. All rights reserved.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
#include <string>

using namespace std;

class Banker
{
private:
    int numProc;      // the number of processes
    int numResources; // the number of resources
    int * available;  // number of available instances of each resource
    int ** max;       // the max demand of each process, e.g., max[i][j] = k
                      // means process i needs at most k instances of resource j
    int ** allocation;// the number of resource instances already allocated
    int ** need;      // the number of resource isntances needed by each process

public:

    /* Initializing the vectors and matrixes for the Banker's Algorithm. Takes ownership of
     * all arrays.
     * @param avail  The available vector
     * @param m      The max demand matrix
     * @param alloc  The allocation matrix
     * @param p      The number of processes
     * @param r      The number of resources
     */
    Banker (int * avail, int ** m, int ** alloc, int p, int r) {
        numProc = p;
        numResources = r;

        // Setup the available vector, the max matrix, and the allocation matrix
        available = avail;
        max = m;
        allocation = alloc;

        // Initialize the need matrix
        need = new int*[numProc];
        for (int i = 0; i < numProc; i++)
            need[i] = new int[numResources];
    }

    /* Destroy the vectors and matrixes
     */
    ~Banker() {
        numProc = 0;
        numResources = 0;

        // Free all allocated memory space
        delete[] available;
        for (int i = 0; i < numProc; i++)
        {
            delete[] need[i];
            delete[] max[i];
            delete[] allocation[i];
        }
        delete[] need;
        delete[] max;
        delete[] allocation;
    }

    /* Check whether it is safe to grant the request
     * @param pid    The process that is making the request
     * @param req    The request
     * @param sequenceOrReason  The safe execution sequence returned by the algorithm
     * @return Whether granting the request will lead to a safe state.
     */
    bool isSafe (int pid, int * req, string & sequenceOrReason) {
		//Check if requesting more resources than available
		for(int i = 0; i < numResources; i++)
		{
			if(req[i] > available[i])
			{
				sequenceOrReason = "Not enough resources available.";
				return false;
			}				
		}
		
		//Calculate Need Matrix for before request
		for(int i = 0; i < numProc; i++) {
			for (int j = 0; j < numResources; j++) {
				need[i][j] = max[i][j] - allocation[i][j];
			}
		}
		
		//Check if requesting more than max resources for the process
		for (int j = 0; j < numResources; j++)
		{
			if (req[j] > need[pid][j]){
				sequenceOrReason = "Request is invalid (exceeding declared max for process)";
				return false;
			}		
		}
		
		//Update Allocation and Available for request
		//Update Need matrix for request
		for (int i = 0; i < numResources; i++){
			available[i] -= req[i];
			allocation[pid][i] += req[i];
			need[pid][i] = need[pid][i] - req[i];
		}	
		
		//Step 1: Initialization
		//Work = Available
		int * work = available;
		
		//Finish[i] = false for i = 0, 1, ..., n-1
		bool finish[numProc];
		for(int i = 0; i < numProc; i++){
			finish[i] = false;
		}
		
		//Step 2: Find an i
		//While not all processes in the sequence 
		int count  = 0;
		while (count < numProc) {
			bool executed = false;
			for(int i = 0; i < numProc; i++){
				//Find a process such that finish[i] is false
				//and need[i] <= work 
				if(finish[i] == false) {
					int j;
					for (j = 0; j < numResources; j++) {
						if(need[i][j] > work[j])
							break;
					}
					
					//If both of the conditions are true
					//We have found an i
					//Execute to step 3
					if (j == numResources)
					{
						for (int k = 0; k < numResources; k++) {
							work[k] += allocation[i][k];
						}
						
						//Add process to output string
						std::stringstream ss;
						ss << "P" << i;
						string result = ss.str();
						if(count == numProc - 1)
							sequenceOrReason = sequenceOrReason + result;
						else
							sequenceOrReason = sequenceOrReason + result + ", ";
						
						finish[i] = true;
						executed = true;
						count++;
					}	
				}			
			}
			//Check if we were able to put the process into the sequence
			if (executed == false)
			{
				sequenceOrReason = "Request results in an unsafe state";
				return false;
			}
		}	
		//Step 4: If finish[i] == true for all i, return true
		//Otherwise return false
		for (int l = 0; l < numProc; l++)
		{
			if(finish[l] == false)
			{
				sequenceOrReason = "Request results in an unsafe state";
			}		
		}
		return true;
	}
};

int main (int argc, char * const argv[])
{
    ifstream config;       // Configuration file
    string conffile;       // The configuration file name
    int numProc;           // The number of processes
    int numResources;      // The number of resources
    std::string sequenceOrReason;       // The execution sequence returned by the Banker's Algorithm
    int i, j, index;       // Indices for the vectors and matrixes
    int pid;               // The ID of the process that is making the request
    string reqStr;         // The request vector in string format

    // Read in the config file name from the commanda-line arguments
    if (argc < 2)
    {
        cout << "Usage: banker <config file>\n";
        return 0;
    }
    else
    {
        conffile = argv[1];
    }

    // Open the file
    config.open(conffile.c_str());

    // Get the number of process and the number of resources
    string line, var, equal;    // strings for parsing a line in the config file
    getline(config, line);
    istringstream iss(line);
    iss >> var >> equal >> numProc;     // Get the number of processes
    iss.clear();

    getline(config, line);
    iss.str(line);
    iss >> var >> equal >> numResources;    // Get the number of resources
    iss.clear();

    // Create the available vector, the max matrix, and the allocation matrix
    // according to the number of processes and the number of resources
    int * available = new int[numResources];
    int ** max = new int*[numProc];
    int ** allocation = new int*[numProc];
    for (int i = 0; i < numProc; i++)
    {
        max[i] = new int[numResources];
        allocation[i] = new int[numResources];
    }

    // Get the available vector
    getline(config, line);
    replace(line.begin(), line.end(), '<', ' ');  // Remove "<" and ">"
    replace(line.begin(), line.end(), '>', ' ');
    iss.str(line);
    iss >> var >> equal;
    for (j = 0; j < numResources; j++)        // Read in the "available" vector
        iss >> available[j];
    iss.clear();

    // Get the max matrix and the allocation matrix
    for (i = 0; i < numProc; i++)
    {
        getline(config, line);
        replace(line.begin(), line.end(), '<', ' ');
        replace(line.begin(), line.end(), '>', ' ');
        iss.str(line);
        iss >> var;
        index = atoi(&var.at(1));            // Get the process ID
        if (index < 0 || index >= numProc)
        {
            cerr << "Invalid process ID: " << var << endl;
            return 0;
        }

        // Get the number of resources allocated to process "index".
        for (j = 0; j < numResources; j++)
            iss >> allocation[index][j];

        // Get the max allocation to process "index".
        for (j = 0; j < numResources; j++)
            iss >> max[index][j];

        iss.clear();
    }

    // Get the request vector
    int * request = new int[numResources];
    getline(config, line);
    reqStr = line.substr(line.find('<'), line.find('>') - line.find('<') + 1);
    replace(line.begin(), line.end(), '<', ' ');
    replace(line.begin(), line.end(), '>', ' ');
    iss.str(line);
    iss >> var >> pid >> equal;
    for (j = 0; j < numResources; j++)          // Read in the "request" vector
        iss >> request[j];
    iss.clear();

    // Check the request using the Banker's algorithm.
    Banker * banker = new Banker(available, max, allocation, numProc, numResources);
    if (banker -> isSafe(pid, request, sequenceOrReason))
        cout << "Grant request " << reqStr << " from P" << pid << ".\n"
             << "Sequence: " << sequenceOrReason << ".\n";
    else
        cout << "Reject request " << reqStr << " from P" << pid << ".\n"
             << "Reason: " << sequenceOrReason << "\n";
}
