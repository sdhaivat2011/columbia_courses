#include "../parsers/xml/pugixml/src/pugixml.hpp"
#include "../parsers/json/cpp-json/json.h"
#include "../db/sqlite3/sqlite3.h"
#include "ioformat.h"

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <vector>
#include <dirent.h>
#include <stdlib.h>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <boost/lexical_cast.hpp>

using namespace std;

#define DB_NAME "../indexes/db_index.sqlite"

map<string, int> stopwords;

/**
 * Takes a directory as input
 * Returns all the files present in the directory
 *
 * */
vector<string> load_input(const char *p_input) {
        DIR *p_dir;
        struct dirent *p_file;
        vector<string> fileList;

        p_dir = opendir(p_input);
        while((p_file = readdir(p_dir))) {
                string str(p_file->d_name);
                fileList.push_back(str);
        }

        closedir(p_dir);
        return fileList;
}

/**
 * Callback for sqlite3 queries.
 *
 * */
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i = 0;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

// Given a list of stopwords, it generates a map for the same.
void generateStopwords(const char* stopwordFile) {
	struct stat sb;
	if (stat(stopwordFile, &sb) == -1) {
		cout << "Could not obtain stats for: " << stopwordFile << endl;
		return;
    	}
	int fileSize = (int)sb.st_size;
	char buf[fileSize];

	// Reda the stopwords file
	int fin = ::open(stopwordFile, O_RDONLY);
	if(fin) {
		int n = 0;
		while((n = read(fin, buf, fileSize)) > 0) {
		        // Do nothing
		}
	}
	else {
		cout << "Error opening " << stopwordFile << endl;
	}	
	close(fin);

	// Store in a map
	char word[15];        
	int curr = 0;
	for(int i = 0; i < fileSize;) {
		while(i < fileSize && buf[i] != ' ' && buf[i] != '\0' && buf[i] != '\n') {
			word[curr] = buf[i];
			curr++;
			i++;
		}
		word[curr] = '\0';
		if(curr > 0) {
			stopwords[word] = 1;
		}
		curr = 0;
		i++;
	}
}

// Create a sqlite DB to store the index tables.
int createDB() {
	// Open a database
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc = 0;

	rc = sqlite3_open(DB_NAME, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
  	}
	// Create the tables
	rc = sqlite3_exec(db, "create table docInfo(docID int, doc_value varchar(100));", callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	rc = sqlite3_exec(db, "create table invertedIndex(term text, term_json blob);", callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
	return 0;
}

#define BUFFER_SIZE 256

// Store the indexes to the DB
int storeToDB() {
	system("./db/sqlite3/shell -separator '|' ../indexes/db_index.sqlite '.import docInfo.dat docInfo'");
	system("./db/sqlite3/shell -separator '|' ../indexes/db_index.sqlite '.import invertedIndex.dat invertedIndex'");
//	sqlite3 *db;
//	FILE * pFile;
//	char sSQL [BUFFER_SIZE] = "\0";
//	char sInputBuf [BUFFER_SIZE] = "\0";
//	sqlite3_stmt * stmt;
//	char * sErrMsg = 0;
//	const char * tail = 0;
//	int n;
//	system("wc -l docInfo.dat");
//	
//	sqlite3_open(DB_NAME, &db);
//	sprintf(sSQL, "INSERT INTO docInfo VALUES(@dI, @dV)");
//	sqlite3_prepare_v2(db,  sSQL, BUFFER_SIZE, &stmt, &tail);
//	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
//	pFile = fopen ("docInfo.dat","r");
//	while (!feof(pFile)) {
//	        fgets (sInputBuf, BUFFER_SIZE, pFile);

//		sqlite3_bind_text(stmt, 1, strtok(sInputBuf, "|"), -1, SQLITE_TRANSIENT); /* Get docID */
//		sqlite3_bind_text(stmt, 2, strtok(NULL, "\n"), -1, SQLITE_TRANSIENT);  /* Get doc_value */
//		
//		sqlite3_step(stmt);     /* Execute the SQL Statement */
//		sqlite3_clear_bindings(stmt);   /* Clear bindings */
//		sqlite3_reset(stmt);        /* Reset VDBE */
//		n++;
//	}
//	fclose (pFile);
//	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
//	cout << n << " inserts" << endl;
	return 0;
}

/**
 * Takes files as input
 * Parses the XML documents
 * Creates a map of the features that need to be indexed
 *
 * */
int parse_input(vector<string> inputFiles, string inDir)
{
	//stemRawText(inputFiles, inDir);
//	pugi::xml_document doc;
//	int docID = 0;
//	string title, author, biblio, text; // extracted from each document provided
//	char titleAr[1024], authorAr[1024], biblioAr[1024];
//	map<int, string> docInfo;

//	// Open files for writing to
//	int file_docInfo = open("docInfo.dat", O_WRONLY|O_CREAT,0640);
//	if(file_docInfo < 0) {
//		cout << "Could not open docInfo.dat" << endl;
//		return 1;
//	}

//	// Open a database
//	sqlite3 *db;
//	char *zErrMsg = 0;
//	int rc;

//	rc = sqlite3_open(DB_NAME, &db);
//	if( rc ){
//		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//		sqlite3_close(db);
//		return(1);
//  	}
//	rc = sqlite3_exec(db, "create table docInfo(docID int, doc_value blob);", callback, 0, &zErrMsg);
//	if( rc!=SQLITE_OK ){
//		fprintf(stderr, "SQL error: %s\n", zErrMsg);
//		sqlite3_free(zErrMsg);
//	}
//	
//	// Iterate over all the files
//	for(unsigned n = 0; n < inputFiles.size(); n++) {
//		// Do not consider the deafult directories
//		if((inputFiles.at(n).compare(".") != 0) && (inputFiles.at(n).compare("..") != 0)) {
//			string absFileName = inDir + inputFiles.at(n);
//			pugi::xml_parse_result result = doc.load_file(absFileName.c_str());
//			if(result != 1) {
//				cout << "Could not load " << absFileName << endl;
//				return 1;
//			}

//			// Iterate over the nodes present in the XML
//			for (pugi::xml_node node = doc.child("DOC").first_child(); node; node = node.next_sibling()) {
//				string nodeName  = node.name();
//				if(nodeName.compare("DOCNO") == 0) {
//					docID = atoi(node.child_value());
//				}
//				else if(nodeName.compare("TITLE") == 0) {
//					title = node.child_value();
//					if(title.length() > 2) {
//						title.assign(title.substr(1, title.length() - 2));
//					}
//					else {
//						// TODO: Fix this
//						title.assign("*ERROR*");
//					}
//					strcpy(titleAr, title.c_str());
//				}
//				else if(nodeName.compare("AUTHOR") == 0) {
//					author = node.child_value();
//					if(author.length() > 2) {
//						author.assign(author.substr(1, author.length() - 2));
//					}
//					else {
//						// TODO: Fix this
//						author.assign("*ERROR*");
//					}
//					strcpy(authorAr, author.c_str());
//				}
//				else if(nodeName.compare("BIBLIO") == 0) {
//					biblio = node.child_value();
//					if(biblio.length() > 2) {
//						biblio.assign(biblio.substr(1, biblio.length() - 2));
//					}
//					else {
//						// TODO: Fix this
//						biblio.assign("*ERROR*");
//					}
//					strcpy(biblioAr, biblio.c_str());
//				}
//				else if(nodeName.compare("TEXT") == 0) {
//					text = node.child_value();
//					if(text.length() > 2) {
//						text.assign(text.substr(1, text.length() - 2));
//					}
//					else {
//						// TODO: Fix this
//						text.assign("*ERROR*");
//					}
//					stemRawText(absFileName);
//				}
//			}
//			
//			// Populate the maps, storing in the JSON format
//			string doc_value = "{\"l\" : \"" + absFileName + "\",\"t\" : \"";
//			string newline(1, '\n');
//			string quote("'");
//			for(unsigned int i = 0; i < strlen(titleAr); i++) {
//				string s(1, titleAr[i]);
//				if(s.compare(newline) == 0) {
//					doc_value.append("\\n");
//				}
//				else {
//					doc_value.append(s);
//				}
//			}
//			doc_value.append("\",\"a\" : \"");
//			for(unsigned int i = 0; i < strlen(authorAr); i++) {
//				string s(1, authorAr[i]);
//				if(s.compare(newline) == 0) {
//					doc_value.append("\\n");
//				}
//				else {
//					doc_value.append(s);
//				}
//			}
//			doc_value.append("\",\"b\" : \"");
//			for(unsigned int i = 0; i < strlen(biblioAr); i++) {
//				string s(1, biblioAr[i]);
//				if(s.compare(newline) == 0) {
//					doc_value.append("\\n");
//				}
//				else {
//					doc_value.append(s);
//				}
//			}
//			doc_value.append("\"}");
//						
//			string docID_s = boost::lexical_cast<string>( docID );
//			// Write doc_value into a file
//			string output = docID_s + "|" + doc_value + "\n";

//			write(file_docInfo, output.c_str(), output.length());
//			
//			//json::value v = json::parse(doc_value);
//			//std::cout << json::pretty_print(v) << std::endl;
//			//json::value z = v["t"];
//			//cout << json::to_string(z) << endl;
//			//std::cout << "----------" << std::endl;
//			//cout << docInfo[docID] << endl;

//		}
//		else {
//			// When it is the default directory.
//			// do nothing
//		}
//	}
//	system("./db/sqlite3/shell.exe -separator '|' ../indexes/db_index.sqlite '.import docInfo.dat docInfo'");
//	close(file_docInfo);
//	sqlite3_close(db);
	return 0;
}
