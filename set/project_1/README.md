Information Retrieval System
=============================

Description
------------
We need to build a system which can query into a set of documents. For demonstration, we will be using the Cranfield corpus.  
The complete details of the requirements can be found [here](http://www1.cs.columbia.edu/~cs6998/HW1F13.pdf).  

The documentation for each of the major components is covered in individual README files in each of the folders.  
You can find detailed references on the source code [here](http://www.columbia.edu/~ds3267/IRLib/).  
The project is hosted on [github](https://github.com/sdhaivat2011/columbia_courses/tree/master/set/project_1).  

Usage
-------
If you have a tar.gz file, untar it:  
```
tar -zxvf <filename>.tar.gz
```
Change directories to the src folder.  
```
cd project_1/src
```
Run the install script (it assumes you have sh installed in /bin/sh) - Incase that is different on you computer, change the first line of the script to the appropriate location.  
```
./install
```
This will run the Makefile and generated the required executables.  
To index the files in a particular folder.  
```
./index <folder-path>
```
Here the <folder-path> must be an absolute path, not a relative one.  
The assumption here is that the files are in the following format  
```
<DOC>
<DOCNO>
...
</DOCNO>
<TITLE>
...
</TITLE>
<AUTHOR>
...
</AUTHOR>
<BIBLIO>
...
</BIBLIO>
<TEXT>
...
</TEXT>
</DOC>
```
To run queries now,  
```
./query
```
The type of queries supported:  
```
cat dog
cat !dog
df cat
freq cat
doc 1
tf 1 dog
title 2
similar cat
```


