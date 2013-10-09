#include <vector>
#include <string>
#include <map>

#ifndef INPUT_H
#define INPUT_H

using namespace std;
// Map to store the document specific characteristics for a particular term
extern map<string, string> docChar;

// Map to store the documents a particular term appears in
extern map<string, vector<int> > docList;

// Map to store information specific to a document
extern map<int, string> docInfo; 

// Returns all the files present in the given directory
extern vector<string> load_input(const char* p_inputDir);

// Parses the file provided to extract structured information
int parse_input(vector<string> inputFile, string inDir);
 
#endif
