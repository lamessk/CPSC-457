// CPSC457 Winter 2018, University of Calgary
// C++ program for Q7 of Assignment 5.
//
// The program reads in the input, then calls the checkConsistency() to 
// check if each file has proper amount of blocks, too many blocks, or too
// few blocks allocated to it the FAT, if it contains a cycle, and if the file
// shares blocks with any other files. It also reports the number of free blocks
// at the end, and finally formats the output.
//
// Author: Lamess Kharfan 
// Date: April 6, 2018
// Student ID: 10150607
// Tutorial: T02
// Version: 1
//
// Compile with g++ -std=c++11 impl.cpp -o impl
// Run with ./impl < filename.txt (a properly formatted input file)

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <set>

using namespace std;

typedef std::string SS;
typedef std::vector<SS> VS;

struct DEntry {
    SS fname = SS( 4096, 0);
    int size = 0;
    int ind = 0;
    bool tooManyBlocks = false;
    bool tooFewBlocks = false;
    bool hasCycle = false;
    bool sharesBlocks = false;
};

static SS join( const VS & toks, const SS & sep) {
    SS res;
    bool first = true;
    for( auto & t : toks) { res += (first ? "" : sep) + t; first = false;}
    return res;
}

// Parameters:
//   blockSize - contains block size as read in from input
//   files - array containing the entries as read in from input
//   fat - array representing the FAT as read in from input
// Return value:
//   the function should return the number of free blocks
//   also, for ever entry in the files[] array, you need to set the appropriate flags:
//      i.e. tooManyBlocks, tooFewBlocks, hasCycle and sharesBlocks
int checkConsistency( int blockSize, std::vector<DEntry> & files, std::vector<int> & fat)
{
	vector<int> blocks;
	//Check how many blocks each file needs and store in blocks
	for (int i = 0; i < files.size(); i++)
	{
		int blocksNeeded = 0;
		int counter = files[i].size;
		while(counter > 0)
		{
			blocksNeeded++;
			counter = counter - blockSize;
		}		
		blocks.push_back(blocksNeeded);	
	}
	
	//For each file, find out if there are cycles, if it shares 
	//blocks with other files, or if it has too many/too few blocks 
	//allocated to it. Track indices accessed to find free blocks. 
	int freeBlocks = fat.size();
	set<int> indexes;
	vector<vector<int>> allUsed;
	for (int i = 0; i < files.size(); i++)
	{
		//Create a vector of pages already used by each file
		vector<int> used;
		int current = files[i].ind;
		//Continue to add pages until we reach -1, indicating EOF
		while(current != -1)
		{	
			//Check if a cycle exists by checking pages already used
			bool cycle = false;
			for(int x = 0; x < used.size(); x++){
				if(used[x] == current){
					cycle = true;
				}
			}
			//If cycle exists, set flag for the file
			if(cycle == true)
			{
				files[i].hasCycle = true;
				break;
			}	
			//Check if file shares any blocks with other files 
			//If it does, set sharesBlocks flag for the file and the
			//file it shares blocks with 
			for(int k = 0; k < allUsed.size(); k++)
			{
				for(int j = 0; j < allUsed[k].size(); j++)
				{
					if(current == allUsed[k][j]){
						files[i].sharesBlocks = true;
						files[k].sharesBlocks = true;
					}
				}
			}	
			//Add block index to list of indices we accessed for that file
			used.push_back(current);
			//Keep track of indexes we have accessed for freeBlocks
			indexes.insert(current);
			//Move onto next index for the file
			int temp = current;
			current = fat[temp];	
		}
		//Check if proper amount of blocks allocated to each file
		//If too many, set tooMany flag, if too few, set tooFew flag
		if(blocks[i] > used.size())
			files[i].tooFewBlocks = true;
		if(blocks[i] < used.size())
			files[i].tooManyBlocks = true;
		
		//Add the vector of blocks used by the file to the table 
		//of all blocks used by all files
		allUsed.push_back(used);	
	}
	
	//Calculate number of free blocks and return
	freeBlocks -= indexes.size();
    return freeBlocks;
}
int main()
{
    try {
        // read in blockSize, nFiles, fatSize
        int blockSize, nFiles, fatSize;
        if( 3 != scanf( "%d %d %d", & blockSize, & nFiles, & fatSize))
            throw "cannot read blockSize, nFiles and fatSize";
        if( blockSize < 1 || blockSize > 1024) throw "bad block size";
        if( nFiles < 0 || nFiles > 50) throw "bad number of files";
        if( fatSize < 1 || fatSize > 200000) throw "bad FAT size";
        // read in the entries
        std::vector<DEntry> entries;
        for( int i = 0 ; i < nFiles ; i ++ ) {
            DEntry e;
            if( 3 != scanf( "%s %d %d", (char *) e.fname.c_str(), & e.ind, & e.size))
                throw "bad file entry";
            e.fname = e.fname.c_str();
            if( e.fname.size() < 1 || e.fname.size() > 16)
                throw "bad filename in file entry";
            if( e.ind < -1 || e.ind >= fatSize) throw "bad first block in fille entry";
            if( e.size < 0 || e.size > 1073741824) throw "bad file size in file entry";
            entries.push_back( e);
        }
        // read in the FAT
        std::vector<int> fat( fatSize);
        for( int i = 0 ; i < fatSize ; i ++ ) {
            if( 1 != scanf( "%d", & fat[i])) throw "could not read FAT entry";
            if( fat[i] < -1 || fat[i] >= fatSize) throw "bad FAT entry";
        }

        int nFreeBlocks = checkConsistency( blockSize, entries, fat);
        size_t maxflen = 0;
        for( auto & e : entries ) maxflen = std::max( maxflen, e.fname.size());
        SS fmt = "  %" + std::to_string( maxflen) + "s: %s\n";

        printf( "Issues with files:\n");
        for( auto & e : entries ) {
            VS issues;
            if( e.tooFewBlocks) issues.push_back( "not enough blocks");
            if( e.tooManyBlocks) issues.push_back( "too many blocks");
            if( e.hasCycle) issues.push_back( "contains cycle");
            if( e.sharesBlocks) issues.push_back( "shares blocks");
            printf( fmt.c_str(), e.fname.c_str(), join( issues, ", ").c_str());
        }
        printf( "Number of free blocks: %d\n", nFreeBlocks);

    }
    catch( const char * err) {
        fprintf( stderr, "Error: %s\n", err);
    }
    catch( ... ) {
        fprintf( stderr, "Errro: unknown.\n");
    }
    return 0;
}
