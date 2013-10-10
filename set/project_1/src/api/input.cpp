#include "../parsers/xml/pugixml/src/pugixml.hpp"
#include "../parsers/json/cpp-json/json.h"
#include "../db/sqlite3/sqlite3.h"
#include "input.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <dirent.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

#define DB_NAME "../indexes/db_index.sqlite"

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
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
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
	pugi::xml_document doc;
	int docID = 0;
	string title, author, biblio, text; // extracted from each document provided
	char titleAr[1024], authorAr[1024], biblioAr[1024];
	map<int, string> docInfo;

	// Open a database
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open(DB_NAME, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
  	}
	rc = sqlite3_exec(db, "create table tbl1(one varchar(10), two smallint);", callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	
	// Iterate over all the files
	for(unsigned n = 0; n < inputFiles.size(); n++) {
		// Do not consider the deafult directories
		if((inputFiles.at(n).compare(".") != 0) && (inputFiles.at(n).compare("..") != 0)) {
			string absFileName = inDir + inputFiles.at(n);
			pugi::xml_parse_result result = doc.load_file(absFileName.c_str());
			if(result != 1) {
				cout << "Could not load " << absFileName << endl;
				return 1;
			}

			// Iterate over the nodes present in the XML
			for (pugi::xml_node node = doc.child("DOC").first_child(); node; node = node.next_sibling()) {
				string nodeName  = node.name();
				if(nodeName.compare("DOCNO") == 0) {
					docID = atoi(node.child_value());
				}
				else if(nodeName.compare("TITLE") == 0) {
					title = node.child_value();
					if(title.length() > 2) {
						title.assign(title.substr(1, title.length() - 2));
					}
					else {
						title.assign("*ERROR*");
					}
					strcpy(titleAr, title.c_str());
				}
				else if(nodeName.compare("AUTHOR") == 0) {
					author = node.child_value();
					if(author.length() > 2) {
						author.assign(author.substr(1, author.length() - 2));
					}
					else {
						author.assign("*ERROR*");
					}
					strcpy(authorAr, author.c_str());
				}
				else if(nodeName.compare("BIBLIO") == 0) {
					biblio = node.child_value();
					if(biblio.length() > 2) {
						biblio.assign(biblio.substr(1, biblio.length() - 2));
					}
					else {
						biblio.assign("*ERROR*");
					}
					strcpy(biblioAr, biblio.c_str());
				}
				else if(nodeName.compare("TEXT") == 0) {
					text = node.child_value();
					if(text.length() > 2) {
						text.assign(text.substr(1, text.length() - 2));
					}
					else {
						text.assign("*ERROR*");
					}
				}
			}
			
			// Populate the maps, storing in the JSON format
			string doc_value = "{\"l\" : \"" + inputFiles.at(n) + "\",\"t\" : \"";
			string newline(1, '\n');
			for(unsigned int i = 0; i < strlen(titleAr); i++) {
				string s(1, titleAr[i]);
				if(s.compare(newline) != 0) {
					doc_value.append(s);
				}
				else {
					doc_value.append("\\n");
				}
			}
			doc_value.append("\",\"a\" : \"");
			for(unsigned int i = 0; i < strlen(authorAr); i++) {
				string s(1, authorAr[i]);
				if(s.compare(newline) != 0) {
					doc_value.append(s);
				}
				else {
					doc_value.append("\\n");
				}
			}
			doc_value.append("\",\"b\" : \"");
			for(unsigned int i = 0; i < strlen(biblioAr); i++) {
				string s(1, biblioAr[i]);
				if(s.compare(newline) != 0) {
					doc_value.append(s);
				}
				else {
					doc_value.append("\\n");
				}
			}
			doc_value.append("\"}");
			
			docInfo[docID] = doc_value;
			//json::value v = json::parse(doc_value);
			//std::cout << json::pretty_print(v) << std::endl;
			//json::value z = v["t"];
			//cout << json::to_string(z) << endl;
			//std::cout << "----------" << std::endl;
			//cout << docInfo[docID] << endl;

		}
		else {
			// When it is the default directory.
			// do nothing
		}
	}
	sqlite3_close(db);
	return 0;
}
