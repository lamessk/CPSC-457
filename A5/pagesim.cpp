// Author: Lamess Kharfan 
// Date: April 6, 2018
// Student ID: 10150607
// Tutorial: T02
// Version: 1
//
// Compile with g++ pagesim.cpp -o pagesim
// Run with ./pagesim <number of frames> < filename.txt 


#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
using namespace std;

void Optimal(vector<int> v, int numFrames)
{
	// Create vector for frames
	vector<int> frames;
	// Initialize number of page faults
	int pageFaults = 0;
	
	// Fill frames initially because they are empty
	for(int i = 0; i < numFrames; i++)
	{
		frames.push_back(v[i]);
		pageFaults++;
	}
	
	// Execute optimal algorithm for remaining numbers now that 
	// all of the frames are full
	for(int i = numFrames; i < v.size(); i++)
	{
		// First check if page is already inside a frame
		// If yes, no need to execute optimal algorithm 
		bool test = false;
		for(int j = 0; j < numFrames; j++)
		{
			if(v[i] == frames[j])
				test = true;
		}

		//If the element is not already inside a frame, figure out 
		//which page will not be used for the longest and replace it
		if(test == false)
		{
			int max = i + 1;
			int replace = -1; 
			// For each page in the frames
			for(int x = 0; x < frames.size(); x++)
			{
				// For each page in the input string
				int y = 0;
				for(int y = i; y < v.size(); y++)
				{
					//Check if the page at frames[i] is found again 
					//in the input string 
					if(frames[x] == v[y])
					{
						// If found, check if its index is larger than the 
						// last recorded input of max
						if(y > max)
						{
							//If yes, record the new max and index to replace
							max = y;
							replace = x;
						}
						//We have found the next occurance so leave the loop
						break;
					}
				}	
				// If we managed to iterate over the whole input string, and we didn't
				// find the page again in the string, replace that frame. 
				if (y == v.size())
				{
					replace = x;
					break;
				}
			}
			// Replace first element if there is doing left to replace,
			// so make index to replace the first one. 
			if(replace == -1)
			{
				replace = 0;
			}		
			//Change frame at replace index to new page 
			//Increment page faults
			frames[replace] = v[i];
			pageFaults++;
		}
	}
	//Output what is in the frame at the end and num page faults
	cout << "Optimal: \n";
	cout << " - frames: ";
	for(int i = 0; i < frames.size(); i++)
		cout << frames[i] << " ";
	cout << "\n - page faults: " << pageFaults << "\n";	
}


void Clock(vector<int> v, int numFrames){
	// Initialize frames vector
	vector<int> frames;
	// Initialize reference bit vector
	vector<int> referenceBit;
	// Initialize pagefaults
    int pageFaults = 0;
	//Initialize clock hand
    int clockHand = 0;
	
	// Fill frames initially because they are empty
	for(int i = 0; i < numFrames; i++)
	{
		// Push value to frame
		frames.push_back(v[i]);
		// Give each frame value 1 for reference bit
		referenceBit.push_back(1);
		pageFaults++;
	}
	
	// Execute Clock algorithm for remaining input now that 
	// all of the frames are full and we need to replace page
	for(int i = numFrames; i < v.size(); i++){
		// First check if page is already inside a frame
		// If yes, no need to execute clock algorithm 
		bool test = false;
		for(int j = 0; j < numFrames; j++){
			if(frames[j] == v[i]){
			  test = true;
			  referenceBit[j] = 1;
			  // Break because we found page already in a frame
			  break;
			}
		}
		if(test == false){
			// Index to replace
			int replace = -1;
			//Cycle until we have found a page to replace
			while(1){
				// If refbit is 0, replace page at that index
				if(referenceBit[clockHand] == 0){
					replace = clockHand;
					//Check on clock hand to make sure 
					//it stays in range of frames
					if(clockHand == numFrames -1)
						clockHand = 0;
					else
						clockHand++;
					//Break because we found a page to replace
					break;
				}
				//Otherwise, set the reference bit to 0 to indicate
				//we haven't used this page in a while 
				else{
					referenceBit[clockHand] = 0;
					//Check on clock hand to make sure 
					//it stays in range of the frames
					if(clockHand == numFrames -1)
						clockHand = 0;
					else
						clockHand++;
				}
			}
			//Replace frame at index we found with new page 
			frames[replace] = v[i];
			//Change reference bit to 1 to indicate we used it recently
			referenceBit[replace] = 1;
			pageFaults++;
		}
	}
	//Output what is in the frame at the end and num page faults
	cout << "Clock: \n";
	cout << " - frames: ";
	for(int i = 0; i < frames.size(); i++)
		cout << frames[i] << " ";
	cout << "\n - page faults: " << pageFaults << "\n";
}

void LRU(vector<int> v, int numFrames)
{
	//Initialize frames vector
	vector<int> frames;
	//Initialize last used vector 
	vector<int> lastUsed;
	//Initialize page faults to 0
	int pageFaults = 0;
	//Counter representing a clock
	int counter = 0;
	
	// Fill frames initially because they are empty
	for(int i = 0; i < numFrames; i++)
	{
		//Push value to frame
		frames.push_back(v[i]);
		//Push index of string that we pushed value at
		lastUsed.push_back(counter);
		counter++;
		pageFaults++;
	}
	
	// Execute LRU algorithm for remaining input now that 
	// all of the frames are full and we need to replace pages
	for(int i = numFrames; i < v.size(); i++)
	{
		// First check if page is already inside a frame
		// If yes, no need to execute LRU algorithm 
		bool test = false;
		for(int j = 0; j < numFrames; j++)
		{
			if(v[i] == frames[j]){
				test = true;
				lastUsed[j] = counter;
				break;
			}
		}
		//If the element is not already inside a frame, figure out 
		//which page has not been used for the longest amount of time
		if(test == false)
		{
			int replace = -1;
			int min = 5000;
			for(int x = 0; x < frames.size(); x++)
			{
				//Find smallest value of lastUsed 
				//This is the frame not used longest
				if(lastUsed[x] < min){
					min = lastUsed[x];
					replace = x; 
				}
			}
			frames[replace] = v[i];
			lastUsed[replace] = counter;
			pageFaults++;
		}
		counter++;
	}
	//Output what is in the frame at the end and num page faults
	cout << "LRU: \n";
	cout << " - frames: ";
	for(int i = 0; i < frames.size(); i++)
		cout << frames[i] << " ";
	cout << "\n - page faults: " << pageFaults << "\n";
}

int main( int argc, char ** argv)
{
	vector<int> v;
	int nframes;
    /// parse command line arguments
    if( argc != 2) {
        printf("Usage: pagesim <number of frames> < filename.txt\n");
        exit(-1);
    }
	else
		nframes = atoi(argv[1]);
	
	/// Get string from input 
    while(1) {
        int64_t num;
        if( 1 != scanf("%ld", & num)) break;
		v.push_back(num);
	}
	
	/// Call algorithms 
	Optimal(v, nframes);
	LRU(v, nframes);
	Clock(v, nframes);
    return 0;
}