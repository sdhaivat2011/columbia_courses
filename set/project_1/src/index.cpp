#include "api/ioformat.h"
#include "backend/stemming/porter/stem.h"

#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>
#include <utility>
#include <boost/lexical_cast.hpp>

using namespace std;

#define STOPWORDS_FILE "api/stopwords.txt"
#define DOCNO_MAX_DIGITS 10
#define LETTER(ch) (isupper(ch) || islower(ch))

map<string, string > docList;
map<string, vector<pair<int, string> > > docChar;

int stemRawText(vector<string> inputFiles, string inDir) {
	int INC = 50;
	int i_max = INC;
	static char * s;
	s = (char *) malloc(i_max + 1);
	char docID_ar[DOCNO_MAX_DIGITS];
	int docID = 0;
	FILE * fout = fopen("docInfo.dat","w");
	if(!fout) {
		cout << "Failed to load the docInfo.dat file...exiting now" << endl;
		return 1;
	}

	//doStemming(fileName.c_str(), "tmp");
	struct stemmer * z = create_stemmer();
	for(unsigned n = 0; n < inputFiles.size(); n++) {
	// Do not consider the deafult directories
		if((inputFiles.at(n).compare(".") != 0) && (inputFiles.at(n).compare("..") != 0)) {
			string absFileName = inDir + "/" + inputFiles.at(n);
			//cout << absFileName << endl;
			FILE * f = fopen(absFileName.c_str(),"r");
			if(!f) {
				cout << "Failed to load " << absFileName << "...exiting now" << endl;
				return 1;
			}
			int cPos = 0;
			while(true)
			{
				int ch = getc(f);
				cPos++;
				if (ch == EOF) {
					cPos--; 
					break;
				}
				if(LETTER(ch)) {
					ch = tolower(ch); /* forces lower case */
					ch = getc(f);
					cPos++;
					if(ch == 'N') {
						for(int pending = 3; pending > 0; pending--) {
							ch = getc(f);
							cPos++;
						}
						int j = 0;
						for(j = 0; j < DOCNO_MAX_DIGITS; j++) {
							ch = getc(f);
							cPos++;
							if(isdigit(ch)) {
								docID_ar[j] = ch;
							}
							else {
								break;
							}								
						}
						docID_ar[j] = '\0';
						docID = atoi(docID_ar);
						char * separator = "|";
						char * newline = "\n";
						// Write out to the file, then load to sqlite3 db
						fwrite(docID_ar, sizeof(char), strlen(docID_ar), fout);
						fwrite(separator, sizeof(char), strlen(separator), fout);
						fwrite(absFileName.c_str(), sizeof(char), strlen(absFileName.c_str()), fout);
						fwrite(newline, sizeof(char), strlen(newline), fout);
						break;
					}
				}
			}
			while(true)
			{  
				int ch = getc(f);
				cPos++;
				if (ch == EOF) {
					cPos--; 
					break;
				}
				if (LETTER(ch)) {
					int i = 0;
					 while(true) {
						if (i == i_max) {
							i_max += INC;
	                    			        s = (char*)realloc(s, i_max + 1);
		    				}
			    			ch = tolower(ch); /* forces lower case */
					        s[i] = ch; i++;
					        ch = getc(f);
						cPos++;
			    			if (!LETTER(ch)) { 
							ungetc(ch,f);
							cPos--;
							break;
						}
			 		}
			 		s[stem(z, s, i - 1) + 1] = 0;
					/* the previous line calls the stemmer and uses its result to
					    zero-terminate the string in s */
					if(stopwords.find(s) == stopwords.end()) { // If s not found in stopwords
						if(docList.find(s) == docList.end()) { // If s not found in docList
							string str(docID_ar);
							docList[s] = str;
						}
						else {
							string str_append(docID_ar);
							if(docList[s].length() >= str_append.length()) {
								if(/* *it != docID*/docList[s].substr(docList[s].length() - str_append.length()).compare(str_append) != 0) {
									docList[s].append("," + str_append);
								}
							}
							else {
								docList[s].append("," + str_append);
							}
						}
						if(docChar.find(s) == docChar.end()) { // If s not found in docChar
							int termPos = cPos - strlen(s);
							string termPos_s = boost::lexical_cast<string>( termPos );
							pair<int, string> p1(docID, termPos_s);
							docChar[s].push_back(p1);
						}
						else { // If s found in docChar
							int termPos = cPos - strlen(s);
							string termPos_s = boost::lexical_cast<string>( termPos );
							vector<pair<int, string> >::iterator it = docChar[s].end() - 1;
							if((*it).first == docID) {
								string p1((*it).second);
								p1.append("," + termPos_s);
								*it = make_pair(docID, p1);
							}
							else {
								pair<int, string> p1(docID, termPos_s);
								docChar[s].push_back(p1);
							}
						}
					}
				}
				else {
					//putchar(ch);
				}
			}
			fclose(f);
		}
	}

	FILE * findex = fopen("invertedIndex.dat","w");
	if(!findex) {
		cout << "Failed to load the invertedIndex.dat file...exiting now" << endl;
		return 1;
	}
	cout << "Generating the invertedIndex data..." << endl;
	for(map<string, vector<pair<int, string> > >::iterator it = docChar.begin(); it != docChar.end(); ++it) {
		string term = (*it).first;
		string outJson = term + "|{\"dI\":[" + docList[term] + "],\"dC\":{";
//		std::cout << (*it).first << "" term " << endl;
		for(std::vector<pair<int,string> >::iterator it1 = (*it).second.begin() ; it1 != (*it).second.end(); ++it1) {
			string docid_s = boost::lexical_cast<string>((*it1).first);
			const char* postingList = (*it1).second.c_str();
			int num = 0;
			for (unsigned int curr = 0; curr < strlen(postingList); curr++) {
				if(postingList[curr] == ',') {
					num++;
				}
			}
			string num_s = boost::lexical_cast<string>(num + 1);
			string str_append = "\"" + docid_s + "\":{\"l\":" + num_s + ",\"p\":[" + (*it1).second + "]}";
			outJson.append(str_append);
			if(it1 != (*it).second.end() - 1) {outJson.append(",");}
			//std::cout << (*it1).first << " " << (*it1).second << endl;
		}
		outJson.append("}}\n");
		fwrite(outJson.c_str(), sizeof(char), strlen(outJson.c_str()), findex);
//		cout << outJson << endl;
	}
	fclose(findex);
	//std::cout << '\n';
	fclose(fout);
	return 0;
}

int main(int argc, char* argv[]) {
	if(argc != 2) {
		cout << "Incorrect Usage" << endl;
		cout << "Usage: " << argv[0] << " <input_dir>" << endl;
		return 1;
	}
	system("rm -rf ../indexes/*.sqlite");
	system("rm -rf *.dat");
	cout << "Starting the indexing process..." << endl;
	cout << "Getting the files to be processed..." << endl;
	vector<string> inputFiles = load_input(argv[1]);
	cout << "Creating the database to store the index files..." << endl;
	if(createDB() != 0) return 1;
	cout << "Loading the stopwords..." << endl;
	generateStopwords(STOPWORDS_FILE);
	cout << "Starting to stem the text..." << endl;
	if(stemRawText(inputFiles, argv[1]) != 0) return 1;
	cout << "Storing into the database..." << endl;
	storeToDB();	

	return 0;
}
