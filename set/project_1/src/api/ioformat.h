#include <vector>
#include <string>
#include <map>
#include <utility>

#ifndef IOFORMAT_H
#define IOFORMAT_H

using namespace std;

// Map to store the document specific characteristics for a particular term
extern map<string, vector<pair<int, string> > > docChar;

// Map to store the documents a particular term appears in
extern map<string, string > docList;

// A map of stopwords from stopwords.txt, which was in turn obtained from http://snowball.tartarus.org/algorithms/english/stop.txt
extern map<string, int> stopwords;

// Returns all the files present in the given directory
extern vector<string> load_input(const char* p_inputDir);

// Parses the file provided to extract structured information
int parse_input(vector<string> inputFile, string inDir);

// Create an sqlite DB
int createDB();

// Stores the indes from dat files to the DB
int storeToDB();

// Parses the file stopwords.txt and generates the map of stopwords
void generateStopwords(const char* stopwordFile);
 
#endif