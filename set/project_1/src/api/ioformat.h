#include <vector>
#include <string>
#include <map>
#include <utility>

#ifndef IOFORMAT_H
#define IOFORMAT_H

using namespace std;

// Get total number of docs in the corpus
int getTotalDocCount();

// Get dI from the JSON object
vector<pair<int, pair<int,vector<int> > > > parseJSON(char* inputJSON);

// Query the database for the required term
char* queryDB(string term);

// Get the absolute filename locations for the provided docno.
int getDoc(int docNo);

// Get the absolute title of the docno specified
int getDocTitle(int docNo);

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
