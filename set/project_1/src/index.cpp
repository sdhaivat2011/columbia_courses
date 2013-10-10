#include "api/input.h"
#include "backend/stemming/porter/stem.h"

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

#define STOPWORDS_FILE "api/stopwords.txt"

#define LETTER(ch) (isupper(ch) || islower(ch))

void stemRawText(vector<string> inputFiles, string inDir) {
	int INC = 50;
	int i_max = INC;
	static char * s;
	s = (char *) malloc(i_max + 1);

	//doStemming(fileName.c_str(), "tmp");
	struct stemmer * z = create_stemmer();
	for(unsigned n = 0; n < inputFiles.size(); n++) {
	// Do not consider the deafult directories
		if((inputFiles.at(n).compare(".") != 0) && (inputFiles.at(n).compare("..") != 0)) {
			string absFileName = inDir + inputFiles.at(n);
			//cout << absFileName << endl;
			FILE * f = fopen(absFileName.c_str(),"r");
			int cPos = 0;
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
					 //cout << s << endl;
				}
				else {
					//putchar(ch);
				}
			}
			//cout << cPos << "chars" << endl;
		}
	}	
}


int main(int argc, char* argv[]) {
	if(argc != 2) {
		cout << "Incorrect Usage" << endl;
		cout << "Usage: " << argv[0] << " <input_dir>" << endl;
		return 1;
	}

	vector<string> inputFiles = load_input(argv[1]);
	stemRawText(inputFiles, argv[1]);
	generateStopwords(STOPWORDS_FILE);
	int parse_success = parse_input(inputFiles, argv[1]);
	if(!parse_success) {

	}
	else {
		cout << "Failed to parse...exiting index" << endl;
		return 1;
	}

	return 0;
}


