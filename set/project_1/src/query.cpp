#include "api/ioformat.h"
#include "backend/stemming/porter/stem.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <stdlib.h>

#define BUF_SIZE 256

using namespace std;

// Parses the query and breaks into tokens
vector<pair<string, pair<int,int> > > parseQuery(const char* queryInput) {
	vector<pair<string, pair<int,int> > > queryTerms;
	char word[BUF_SIZE];
	int currWP = 0;
	int compound = 0;
	int comp = 0;
	int negation = 0;
	unsigned int i = 0;
	pair<string, pair<int,int> > qT;	
	
	for(i = 0; i < strlen(queryInput) && i < BUF_SIZE - 1; i++) {
		if(queryInput[i] == ' ' && compound == 0) {
			word[currWP] = '\0';
			string s(word);
			qT = make_pair(s, make_pair(comp,negation));
			queryTerms.push_back(qT);
			currWP = 0;
			comp = 0;
			negation = 0;
		}
		else if(queryInput[i] == '"') {
			if(compound == 0) {
				compound = 1;
				comp = 1;
			}
			else {
				compound = 0;
			}
		}
		else if(queryInput[i] == '!') {
			negation = 1;
		}
		else {
			word[currWP] = queryInput[i];
			currWP++;
		}
	}
	if(i == strlen(queryInput)) {
		word[currWP] = '\0';
		string s(word);
		qT = make_pair(s, make_pair(comp,negation));
		queryTerms.push_back(qT);
		currWP = 0;
		comp = 0;
		negation = 0;
	}
	else if(i == BUF_SIZE) {
		cout << "Too large query....no results found" << endl;
	}
	return queryTerms;
}

void defaultQuery(vector<pair<string, pair<int,int> > > queryTerms, vector<pair<string, pair<int,int> > > stemQueryTerms) {
	for(unsigned int i = 0; i < stemQueryTerms.size(); i++) {
		//cout << queryTerms.at(i).first << " " << queryTerms.at(i).second.first << " " << queryTerms.at(i).second.second << endl;	
		if(stemQueryTerms.at(i).second.first == 0) {
			char* result = queryDB(stemQueryTerms.at(i).first);
			if(strcmp(result, "NULL") == 0) {
				cout << "Term not found" << endl;
			}
			else {
				getDIFromJSON(result);
			}
		}
	}
}

void getDocFreq(vector<pair<string, pair<int,int> > > queryTerms, vector<pair<string, pair<int,int> > > stemQueryTerms) {
	cout << "inside getDocFreq" << endl;
}

void getFreqCount(vector<pair<string, pair<int,int> > > queryTerms, vector<pair<string, pair<int,int> > > stemQueryTerms) {
	cout << "inside getFreqCount" << endl;
}

// Get the entire document, in a pager
void getDocument(vector<pair<string, pair<int,int> > > queryTerms) {
	if(queryTerms.size() != 2) {
		cout << "Invalid Usage\nUsage: doc <docno>" << endl;
		return;
	}
	int docNo = atoi(queryTerms.at(1).first.c_str());
	getDoc(docNo);
}

void getTermFreq(vector<pair<string, pair<int,int> > > queryTerms, vector<pair<string, pair<int,int> > > stemQueryTerms) {
	cout << "inside getTermFreq" << endl;
}

// Gets the title of the specified document
void getTitle(vector<pair<string, pair<int,int> > > queryTerms) {
	if(queryTerms.size() != 2) {
		cout << "Invalid Usage\nUsage: title <docno>" << endl;
		return;
	}
	int docNo = atoi(queryTerms.at(1).first.c_str());
	getDocTitle(docNo);
}

void getSimilarity(vector<pair<string, pair<int,int> > > queryTerms, vector<pair<string, pair<int,int> > > stemQueryTerms) {
	cout << "inside getSimilarity" << endl;
}

void processQuery(const char* queryInput) {
	vector<pair<string, pair<int,int> > > queryTerms = parseQuery(queryInput);
	vector<pair<string, pair<int,int> > > stemQueryTerms;
	string firstTerm(queryTerms.at(0).first);
	struct stemmer * z = create_stemmer();

	for(unsigned int i = 0; i < queryTerms.size(); i++) {
		char s[BUF_SIZE];
		strcpy(s, queryTerms.at(i).first.c_str());
		s[stem(z, s, strlen(s) - 1) + 1] = 0;
		string str(s);
		stemQueryTerms.push_back(make_pair(str, queryTerms.at(i).second));
	}
	
	if(firstTerm.compare("df") == 0) {
		getDocFreq(queryTerms, stemQueryTerms);
	}
	else if(firstTerm.compare("freq") == 0) {
		getFreqCount(queryTerms, stemQueryTerms);
	}
	else if(firstTerm.compare("doc") == 0) {
		getDocument(queryTerms);		
	}
	else if(firstTerm.compare("tf") == 0) {
		getTermFreq(queryTerms, stemQueryTerms);
	}
	else if(firstTerm.compare("title") == 0) {
		getTitle(queryTerms);
	}
	else if(firstTerm.compare("similar") == 0) {
		getSimilarity(queryTerms, stemQueryTerms);
	}
	else {
		defaultQuery(queryTerms, stemQueryTerms);
	}
}

int main() {
	string queryInput_s;
	while(true) {
		cout << "\n\nEnter a query (.exit to exit): " << endl;
		getline(cin, queryInput_s);
		if(queryInput_s.compare(".exit") == 0) {
			break;
		}
		else {
			processQuery(queryInput_s.c_str());
		}
	}
	return 0;
}
