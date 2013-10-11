#include "api/ioformat.h"
#include "backend/stemming/porter/stem.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <stdlib.h>

#define BUF_SIZE 256

using namespace std;

// Parses the query and breaks into tokens
vector<string> parseQuery(const char* queryInput) {
	vector<string> queryTerms;
	char word[BUF_SIZE];
	int currWP = 0;
	unsigned int i = 0;
	for(i = 0; i < strlen(queryInput) && i < BUF_SIZE - 1; i++) {
		if(queryInput[i] == ' ') {
			word[currWP] = '\0';
			string s(word);
			queryTerms.push_back(s);
			currWP = 0;
		}
		else {
			word[currWP] = queryInput[i];
			currWP++;
		}
	}
	if(i == strlen(queryInput)) {
		word[currWP] = '\0';
		string s(word);
		queryTerms.push_back(s);
		currWP = 0;
	}
	else if(i == BUF_SIZE) {
		cout << "Too large query....no results found" << endl;
	}
	return queryTerms;
}

void defaultQuery(vector<string> queryTerms, vector<string> stemQueryTerms) {
	cout << "inside defaultQuery" << endl;
}

void getDocFreq(vector<string> queryTerms, vector<string> stemQueryTerms) {
	cout << "inside getDocFreq" << endl;
}

void getFreqCount(vector<string> queryTerms, vector<string> stemQueryTerms) {
	cout << "inside getFreqCount" << endl;
}

// Get the entire document, in a pager
void getDocument(vector<string> queryTerms) {
	if(queryTerms.size() != 2) {
		cout << "Invalid Usage\nUsage: doc <docno>" << endl;
		return;
	}
	int docNo = atoi(queryTerms.at(1).c_str());
	getDoc(docNo);
}

void getTermFreq(vector<string> queryTerms, vector<string> stemQueryTerms) {
	cout << "inside getTermFreq" << endl;
}

// Gets the title of the specified document
void getTitle(vector<string> queryTerms) {
	if(queryTerms.size() != 2) {
		cout << "Invalid Usage\nUsage: title <docno>" << endl;
		return;
	}
	int docNo = atoi(queryTerms.at(1).c_str());
	getDocTitle(docNo);
}

void getSimilarity(vector<string> queryTerms, vector<string> stemQueryTerms) {
	cout << "inside getSimilarity" << endl;
}

void processQuery(const char* queryInput) {
	vector<string> queryTerms = parseQuery(queryInput);
	vector<string> stemQueryTerms;
	string firstTerm(queryTerms.at(0));
	struct stemmer * z = create_stemmer();

	for(unsigned int i = 0; i < queryTerms.size(); i++) {
		char s[BUF_SIZE];
		strcpy(s, queryTerms.at(i).c_str());
		s[stem(z, s, strlen(s) - 1) + 1] = 0;
		string str(s);
		stemQueryTerms.push_back(str);
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
