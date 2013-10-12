#include "api/ioformat.h"
#include "backend/stemming/porter/stem.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sys/time.h>
#include <unistd.h>
#include <boost/bind.hpp>

#define BUF_SIZE 256

using namespace std;

vector<int> allDocs;

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
	vector<pair<int,int> > docUnion; // The doc no. and the score
	for(unsigned int i = 0; i < stemQueryTerms.size(); i++) {
		if(stemQueryTerms.at(i).second.first == 0) { // Meaning it is not a compound word
			char* result = queryDB(stemQueryTerms.at(i).first);
			if(strcmp(result, "NULL") == 0) {
				cout << "Term not found" << endl;
			}
			else {
				vector<pair<int, pair<int,vector<int> > > > vStruct = parseJSON(result);

				if(stemQueryTerms.at(i).second.second == 0) { // Meaning it is not a negation
					if(docUnion.size() == 0) { // Meaning this is the first set being added
						for(unsigned int j = 0; j < vStruct.size(); j++) {
							pair<int,int> tmp = make_pair(vStruct.at(j).first,vStruct.at(j).second.first);
							docUnion.push_back(tmp);
						}
					}
					else { // Adding to an already existing set
						vector<pair<int,int> > tmp;
						
						//it = set_union(tmp.begin(), tmp.end(), v.begin(), v.end(), docUnion.begin());
						unsigned int j = 0, k = 0, p = 0;
						while(j < docUnion.size() && k < vStruct.size()) {
							if(docUnion.at(j).first < vStruct.at(k).first) {
								tmp.push_back(docUnion.at(j));
								j++;
							}
							else if(docUnion.at(j).first > vStruct.at(k).first) {
								tmp.push_back(make_pair(vStruct.at(k).first, vStruct.at(k).second.first));
								k++;
							}
							else {
								tmp.push_back(make_pair(docUnion.at(j).first, docUnion.at(j).second + vStruct.at(k).second.first));
								j++;
								k++;
							}
							p++;
						}
						while(j < docUnion.size()) {
							tmp.push_back(docUnion.at(j));
							j++;
							p++;
						}
						while(k < vStruct.size()) {
							tmp.push_back(make_pair(vStruct.at(k).first, vStruct.at(k).second.first));
							k++;
							p++;
						}
						tmp.resize(p);
						docUnion.resize(p);
						docUnion = tmp;
						//docUnion.resize(it - docUnion.begin());
					}
				}
				else { // Meaning that there is a negation
					if(docUnion.size() == 0) { // Meaning this is the first set being added
						unsigned int j = 0, p = 1;
						while(j < vStruct.size()) {
							if(vStruct.at(j).first != p) {
								docUnion.push_back(make_pair(p, 1));
							}
							else {
								j++;
							}
							p++;
						}
						while(p <= allDocs.size()) {
							docUnion.push_back(make_pair(p, 1));
							p++;
						}
					}
					else {
						unsigned int j = 0, p = 1, q = 0, count = 0;
						vector<pair<int,int> > tmp;
						while(j < vStruct.size() && q < docUnion.size() && p <= allDocs.size()) {
							if(p != vStruct.at(j).first && p != docUnion.at(q).first) {
								tmp.push_back(make_pair(p, 1));
								p++; count++;
							}
							else if(p == vStruct.at(j).first && p != docUnion.at(q).first) {
								j++; p++;
							}
							else if(p != vStruct.at(j).first && p == docUnion.at(q).first) {
								tmp.push_back(make_pair(p, 1+docUnion.at(q).second));								
								q++;p++;count++;
							}
							else {
								tmp.push_back(make_pair(p, docUnion.at(q).second));
								j++;p++;q++;count++;
							}
						}
						tmp.resize(count);
						docUnion.resize(count);
						docUnion = tmp;
					}
				}
			}
		}
		else { // Compound words present
			cout << "Compound words not handled" << endl;
//			char* result = queryDB(stemQueryTerms.at(i).first);
		}
	}
	std::sort(docUnion.begin(), docUnion.end(), boost::bind(&std::pair<int, int>::second, _1) > boost::bind(&std::pair<int, int>::second, _2));
	cout << "\nOrdered set of docs in which the query was found; format<docno, score>" << endl;
	for(unsigned int i = 0; i < docUnion.size(); i++) {
		cout << docUnion.at(i).first << ":" << docUnion.at(i).second << " ";
	}
	cout << "\n\n" << docUnion.size() << " matching results found" << endl;
	cout << endl << "\nDisplaying snippets of top 10 results" << endl;
	for(unsigned int i = 0; i < docUnion.size() && i < 10; i++) {
		getDocSnippet(queryTerms, docUnion.at(i).first);
	}
}
void getDocFreq(vector<pair<string, pair<int,int> > > queryTerms, vector<pair<string, pair<int,int> > > stemQueryTerms) {
	if(queryTerms.size() != 2) {
		cout << "Invalid Usage\nUsage: df <term>" << endl;
		return;
	}
	if(stemQueryTerms.at(1).second.first == 0) { // Not a compound word
		getDocFreqDB(stemQueryTerms.at(1).first);
	}
	else {
		cout << "Compound words not handled" << endl;
	}
}

void getFreqCount(vector<pair<string, pair<int,int> > > queryTerms, vector<pair<string, pair<int,int> > > stemQueryTerms) {
	if(queryTerms.size() != 2) {
		cout << "Invalid Usage\nUsage: freq <term>" << endl;
		return;
	}
	if(stemQueryTerms.at(1).second.first == 0) { // Not a compound word
		getFreqDB(stemQueryTerms.at(1).first);
	}
	else {
		cout << "Compound words not handled" << endl;
	}
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
	if(queryTerms.size() != 3) {
		cout << "Invalid Usage\nUsage: tf <docno> <term>" << endl;
		return;
	}
	int docNo = atoi(queryTerms.at(1).first.c_str());
	if(stemQueryTerms.at(2).second.first == 0) { // Not a compound word
		getTermFreqDB(stemQueryTerms.at(2).first, docNo);
	}
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
	if(queryTerms.size() != 2) {
		cout << "Invalid Usage\nUsage: similar <term>" << endl;
		return;
	}
	char* term;
	strcpy(term, stemQueryTerms.at(1).first.c_str());
	cout << "Not functioning correctly" << endl;
	//getSimilarityDB(term);
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

// Array holding the indexes of all the docIDs in the corpus, helpful for set operations
void generateCorpusVector(int totalDocCount) {
	for(unsigned int i = 0; i < totalDocCount; i++)
		allDocs.push_back(i+1);
}

int main() {
	string queryInput_s;
	int totalDocCount = getTotalDocCount();
	generateCorpusVector(totalDocCount);
	while(true) {
		cout << "\n\nEnter a query (.exit to exit): " << endl;
		getline(cin, queryInput_s);
		
		struct timeval start, end;
		long mtime, seconds, useconds;    
		gettimeofday(&start, NULL);

		if(queryInput_s.compare(".exit") == 0) {
			break;
		}
		else {
			processQuery(queryInput_s.c_str());
		}
		gettimeofday(&end, NULL);
		seconds  = end.tv_sec  - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;

		mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

		printf("\nElapsed time: %ld milliseconds\n", mtime);
	}
	return 0;
}
