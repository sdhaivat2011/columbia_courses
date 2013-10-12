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

#ifdef _MSC_VER
#include <boost/config/compiler/visualc.hpp>
#endif
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <cassert>
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
	return 0;
}

/**
 * Callback for getDoc query
 *
 * */
static int getDocCallback(void *NotUsed, int argc, char **argv, char **azColName){
	if(argv[0]) {
		string fileName(argv[0]);
		string cmd = "cat " + fileName + "| less";
		system(cmd.c_str());
	}
	else {
		cout << "Specified doc not found" << endl;
		return 1;
	}
	return 0;
}

// Gets the doc specified by looking up the file location in docInfo table
int getDoc(int docNo) {
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
	// Run the select query
	string docNo_s = boost::lexical_cast<string>(docNo);
	string dbQuery = "select doc_value from docInfo where docID=" + docNo_s + ";";
	rc = sqlite3_exec(db, dbQuery.c_str(), getDocCallback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}	
	return 0;
}

/**
 * Callback for getDocTitle query
 *
 * */
static int getDocTitleCallback(void *NotUsed, int argc, char **argv, char **azColName){
	if(argv[0]) {
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(argv[0]);
		cout << doc.child("DOC").child_value("TITLE");
	}
	else {
		cout << "Specified doc not found" << endl;
		return 1;
	}
	return 0;
}

// Gets the doc specified by looking up the file location in docInfo table
int getDocTitle(int docNo) {
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
	// Run the select query
	string docNo_s = boost::lexical_cast<string>(docNo);
	string dbQuery = "select doc_value from docInfo where docID=" + docNo_s + ";";
	rc = sqlite3_exec(db, dbQuery.c_str(), getDocTitleCallback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	return 0;
}

/**
 * Callback for queryDB query
 *
 * */
static int queryDBCallback(void* data, int argc, char **argv, char **azColName){
	if(argv[0]) {
		char **result_str = (char **)data;
		*result_str = (char *)calloc(strlen(argv[0]),sizeof(char));
		strcpy(*result_str,argv[0]);
	}
	else {
		cout << "Specified term not found" << endl;
		return 1;
	}
	return 0;
}

// Gets the doc specified by looking up the file location in docInfo table
char* queryDB(string term) {
	// Open a database
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc = 0;
	char* data = 0;

	rc = sqlite3_open(DB_NAME, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		//return(1);
  	}

	// Run the query
	string dbQuery = "select term_json from invertedIndex where term=\"" + term + "\";";
	rc = sqlite3_exec(db, dbQuery.c_str(), queryDBCallback, &data, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	if(data == 0) {
		return "NULL";
	}
	return data;
}

// get dI from the inputJSON
vector<pair<int, pair<int,vector<int> > > > parseJSON(char* inputJSON) {
	string str(inputJSON);
	stringstream ss(str);
	vector<pair<int, pair<int,vector<int> > > > docs;
	
	boost::property_tree::ptree pt;
	boost::property_tree::read_json(ss, pt);

        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("dI"))
        {
		pair<int, pair<int,vector<int> > > docsTmp;
		vector<int> positions;

		assert(v.first.empty()); // array elements have no names
		int i = v.second.get<int>("");

		string i_s = boost::lexical_cast<string>(i);
		string nodeName_len = "dC." + i_s + ".l";
		string nodeName_pos = "dC." + i_s + ".p";
		int len = pt.get<int>(nodeName_len);
		
		BOOST_FOREACH(boost::property_tree::ptree::value_type &v1, pt.get_child(nodeName_pos)) {
			assert(v1.first.empty()); // array elements have no names
			int j = v1.second.get<int>("");
			positions.push_back(j);
		}

		docsTmp = make_pair(i, make_pair(len, positions));
		docs.push_back(docsTmp);
        }
	return docs;
}

/**
 * Callback for getTotalDocCount query
 *
 * */
static int getTotalDocCountCallback(void* data, int argc, char **argv, char **azColName){
	if(argv[0]) {
		char **result_str = (char **)data;
		*result_str = (char *)calloc(strlen(argv[0]),sizeof(char));
		strcpy(*result_str,argv[0]);
	}
	else {
		cout << "Specified term not found" << endl;
		return 1;
	}
	return 0;
}

// Gets the doc specified by looking up the file location in docInfo table
int getTotalDocCount() {
	// Open a database
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc = 0;
	char* data = 0;

	rc = sqlite3_open(DB_NAME, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		//return(1);
  	}

	// Run the query
	string dbQuery = "select count(*) from docInfo;";
	rc = sqlite3_exec(db, dbQuery.c_str(), getTotalDocCountCallback, &data, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	if(data == 0) {
		return 0;
	}
	return atoi(data);
}

/**
 * Callback for getFreqDB query
 *
 * */
static int getDocFreqDBCallback(void* data, int argc, char **argv, char **azColName){
	if(argv[0]) {
		char **result_str = (char **)data;
		*result_str = (char *)calloc(strlen(argv[0]),sizeof(char));
		strcpy(*result_str,argv[0]);
	}
	else {
		cout << "Specified term not found" << endl;
		return 1;
	}
	return 0;
}

void getDocFreqDB(string term) {
	// Open a database
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc = 0;
	char* data = 0;

	rc = sqlite3_open(DB_NAME, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		//return(1);
  	}

	// Run the query
	string dbQuery = "select term_json from invertedIndex where term=\"" + term + "\";";
	rc = sqlite3_exec(db, dbQuery.c_str(), getDocFreqDBCallback, &data, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	string str(data);
	stringstream ss(str);
	
	boost::property_tree::ptree pt;
	boost::property_tree::read_json(ss, pt);
	int len = 0;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("dI"))
        {
		len++;
        }

	cout << len << endl;
}

/**
 * Callback for getFreqDB query
 *
 * */
static int getFreqDBCallback(void* data, int argc, char **argv, char **azColName){
	if(argv[0]) {
		char **result_str = (char **)data;
		*result_str = (char *)calloc(strlen(argv[0]),sizeof(char));
		strcpy(*result_str,argv[0]);
	}
	else {
		cout << "Specified term not found" << endl;
		return 1;
	}
	return 0;
}


void getFreqDB(string term) {
	// Open a database
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc = 0;
	char* data = 0;

	rc = sqlite3_open(DB_NAME, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		//return(1);
  	}

	// Run the query
	string dbQuery = "select term_json from invertedIndex where term=\"" + term + "\";";
	rc = sqlite3_exec(db, dbQuery.c_str(), getFreqDBCallback, &data, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	string str(data);
	stringstream ss(str);
	
	boost::property_tree::ptree pt;
	boost::property_tree::read_json(ss, pt);
	int len = 0;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("dI"))
        {
		assert(v.first.empty()); // array elements have no names
		int i = v.second.get<int>("");

		string i_s = boost::lexical_cast<string>(i);
		string nodeName_len = "dC." + i_s + ".l";
		len += pt.get<int>(nodeName_len);
        }

	cout << len << endl;
}

/**
 * Callback for getTermFreqDB query
 *
 * */
static int getTermFreqDBCallback(void* data, int argc, char **argv, char **azColName){
	if(argv[0]) {
		char **result_str = (char **)data;
		*result_str = (char *)calloc(strlen(argv[0]),sizeof(char));
		strcpy(*result_str,argv[0]);
	}
	else {
		cout << "Specified term not found" << endl;
		return 1;
	}
	return 0;
}

void getTermFreqDB(string term, int docNo) {
	// Open a database
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc = 0;
	char* data = 0;

	rc = sqlite3_open(DB_NAME, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		//return(1);
  	}

	// Run the query
	string docNo_s = boost::lexical_cast<string>(docNo);
	string dbQuery = "select term_json from invertedIndex where term=\"" + term + "\";";
	rc = sqlite3_exec(db, dbQuery.c_str(), getTermFreqDBCallback, &data, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	string str(data);
	stringstream ss(str);
	
	boost::property_tree::ptree pt;
	boost::property_tree::read_json(ss, pt);

	string nodeName_len = "dC." + docNo_s + ".l";
	int len = pt.get<int>(nodeName_len);

	cout << len << endl;
}


/**
 * Callback for getDocSnippet query
 *
 * */
static int getDocSnippetCallback(void* data, int argc, char **argv, char **azColName){
	if(argv[0]) {
		char **result_str = (char **)data;
		*result_str = (char *)calloc(strlen(argv[0]),sizeof(char));
		strcpy(*result_str,argv[0]);
	}
	else {
		cout << "Specified term not found" << endl;
		return 1;
	}
	return 0;
}


// Grep for the terms in the file
void grepString(vector<string> grepStr, char* data) {
	int offset; 
	string line;
	ifstream Myfile;
	Myfile.open (data);
	int found = 0;	
		
	if(Myfile.is_open())
	{
		while(!Myfile.eof())
		{
			getline(Myfile,line);
			for(int i = 0; i < grepStr.size(); i++) {
				if ((offset = line.find(grepStr.at(i).c_str(), 0)) != string::npos) 
				{
					cout << data << ":"<< line  <<endl;
					found = 1;
					break;
				}
			}
			if(found)
				break;
		}
		Myfile.close();
	}
	else
		cout<<"Unable to open this file."<<endl;

}

// Generate a doc snippet given doc no and queryterms
void getDocSnippet(vector<pair<string, pair<int,int> > > queryTerms, int docno) {
	// Open a database
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc = 0;
	char* data = 0;

	rc = sqlite3_open(DB_NAME, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		//return(1);
  	}

	// Run the query
	string docno_s = boost::lexical_cast<string>(docno);
	string dbQuery = "select doc_value from docInfo where docID=" + docno_s + ";";
	rc = sqlite3_exec(db, dbQuery.c_str(), getDocSnippetCallback, &data, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	vector<string> grepStr;
	for(int j = 0; j < queryTerms.size(); j++) {
		if(queryTerms.at(j).second.second != 1) {
			grepStr.push_back(queryTerms.at(j).first);
		}
	}
	grepString(grepStr, data);
}


