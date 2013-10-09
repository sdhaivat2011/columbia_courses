#include "../parsers/xml/pugixml/src/pugixml.hpp"
#include "input.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <dirent.h>

using namespace std;

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
 * Takes files as input
 * Parses the XML documents
 * Creates a map of the features that need to be indexed
 *
 * */
int parse_input(vector<string> inputFiles, string inDir)
{
	pugi::xml_document doc;
	
	for(unsigned n = 0; n < inputFiles.size(); n++) {
		if((inputFiles.at(n).compare(".") != 0) && (inputFiles.at(n).compare("..") != 0)) {
			string absFileName = inDir + inputFiles.at(n);
			pugi::xml_parse_result result = doc.load_file(absFileName.c_str());
			if(result != 1) {
				cout << "Could not load " << absFileName << endl;
				return 0;
			}

			for (pugi::xml_node node = doc.child("DOC").first_child(); node; node = node.next_sibling()) {
				// string s  = node.name();
				string t =  node.child_value();
				if(t.length() > 2) {
					cout << t.substr(1);
				}
				else{cout << "*ERROR*";}
			}
		}
	}
	return 1;
}
