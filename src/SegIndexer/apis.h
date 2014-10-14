#ifndef __APIS_H
#define __APIS_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <io.h>
#include <stdlib.h>
#include "NLPIR.h"
#include "PatriciaTrie.h"

using namespace std;

/////////////////////////		APIs		/////////////////////////
int getWordFreqFromDir(string word, string dir);
int getWordFreqFromFile(string word, string fn);
int getWordFreqFromText(string word, string fn);

void genPatIndexFileFromDir(string dir);
void genPatIndexFileFromFile(string src_fn);
void genPatIndexFileFromFile(string src_fn, string dst_fn);
void genPatIndexFileFromTxt(string txt, string dst_fn);

PatriciaTrie<string, int> * genPatTreeFromFile(string src);
PatriciaTrie<string, int> * genPatTreeFromIndexFile(string src);
PatriciaTrie<string, int> * genPatTreeFromTxt(string txt);

vector<string> getLeftWordsFromDir(string word, string dir);
vector<string> getLeftNWordsFromDir(string word, string dir, int n);

vector<string> getLeftWordsFromFile(string word, string fn);
vector<string> getLeftNWordsFromFile(string word, string fn, int n);

vector<string> getRightWordsFromDir(string word, string dir);
vector<string> getRightNWordsFromDir(string word, string dir, int n);

vector<string> getRightWordsFromFile(string word, string fn);
vector<string> getRightNWordsFromFile(string word, string fn, int n);

////////////////////////////////////////////////////////////////////

/////////////////////////		Utilities		////////////////////
bool isValidFile(string file){
	fstream _file;
	_file.open(file, ios::in);
	return !(!_file);
}

bool isValidDir(string dir){
	if (dir[dir.length() - 1] == '\\')
		dir = dir.substr(0, dir.length() - 1);
	bool res = false;
	WIN32_FIND_DATA wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(char2wchar(dir.c_str()), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		res = true;
	}
	FindClose(hFind);
	return res;
}

bool isAllDigit(const string& str)
{
	int i;
	for (i = 0; i != str.length(); i++)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	return true;
}

int getWordFreqFromIndexFile(string word, string fn);

string getIndexFile(string fn);

void split(wstring& s, wstring& delim, vector<int>* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		wstring wstr = s.substr(last, index - last);
		ret->push_back(atoi(wchar2char(wstr.c_str())));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		wstring wstr = s.substr(last, index - last);
		ret->push_back(atoi(wchar2char(wstr.c_str())));
	}
}


string getIndexFileName(string fn){
	string ndxF = "";
	int pos = fn.find(".");
	string indexFileName = fn.substr(0, pos);
	indexFileName.append(".ndx");

	return indexFileName;
}

// if no index file, return ""
string getIndexFile(string fn){
	string indexFileName = getIndexFileName(fn);
	return isValidFile(indexFileName) ? indexFileName : "";
}

void write2File(string dst_fn, string content){
	fstream file;
	file.open(dst_fn.c_str(), ios::out);
	file << content;
	file.close();
}
////////////////////////////////////////////////////////////////////

vector<string> getLeftWordsFromDir(string word, string dir){
	vector<string> vec;
	struct _finddata_t filefind;
	string curr = dir + "\\*.*";
	int  done = 0, handle;
	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1) return vec;

	int count = 0;
	while (!(done = _findnext(handle, &filefind)))
	{
		/*printf("%s\n", filefind.name);*/
		if (!strcmp(filefind.name, "..")){
			continue;
		}
		if ((_A_SUBDIR == filefind.attrib)) //是目录
		{
			curr = dir + "\\" + filefind.name;
			vector<string> new_vec = getLeftWordsFromDir(word, curr);
			for (int i = 0; i < new_vec.size(); i++)
				vec.push_back(new_vec.at(i));
		}
		else
		{
			if (filefind.name){
				string fn(filefind.name);
				char *tokenPtr = strtok(filefind.name, ".");
				tokenPtr = strtok(NULL, ",");
				if (!strcmp(tokenPtr, "txt")){
					vector<string> new_vec = getLeftWordsFromFile(word, dir + "\\" + fn);
					for (int i = 0; i < new_vec.size(); i++)
						vec.push_back(new_vec.at(i));
				}
			}
		}
	}
	_findclose(handle);
	return vec;
}
vector<string> getLeftNWordsFromDir(string word, string dir, int n){
	vector<string> vec;
	struct _finddata_t filefind;
	string curr = dir + "\\*.*";
	int  done = 0, handle;
	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1) return vec;

	int count = 0;
	while (!(done = _findnext(handle, &filefind)))
	{
		/*printf("%s\n", filefind.name);*/
		if (!strcmp(filefind.name, "..")){
			continue;
		}
		if ((_A_SUBDIR == filefind.attrib)) //是目录
		{
			curr = dir + "\\" + filefind.name;
			vector<string> new_vec = getLeftNWordsFromDir(word, curr, n);
			for (int i = 0; i < new_vec.size(); i++)
				vec.push_back(new_vec.at(i));
		}
		else
		{
			if (filefind.name){
				string fn(filefind.name);
				char *tokenPtr = strtok(filefind.name, ".");
				tokenPtr = strtok(NULL, ",");
				if (!strcmp(tokenPtr, "txt")){
					vector<string> new_vec = getLeftNWordsFromFile(word, dir + "\\" + fn, n);
					for (int i = 0; i < new_vec.size(); i++)
						vec.push_back(new_vec.at(i));
				}
			}
		}
	}
	_findclose(handle);
	return vec;
}

vector<string> getLeftWordsFromFile(string word, string fn){
	PatriciaTrie<string, int> *tree = genPatTreeFromFile(fn);

	vector<string> vec = tree->GetLeftWords(word);

	if (getIndexFile(fn) == ""){
		string ndx_file = getIndexFileName(fn);
		write2File(ndx_file, tree->GetPatTreeAsString());
		cout << "Index File : " << ndx_file << " Generated !!" << endl;
	}
	delete tree;
	tree = NULL;
	return vec;
}
vector<string> getLeftNWordsFromFile(string word, string fn, int n){
	PatriciaTrie<string, int> *tree = genPatTreeFromFile(fn);
	vector<string> vec = tree->GetLeftNWords(word, n);
	if (getIndexFile(fn) == ""){
		string ndx_file = getIndexFileName(fn);
		write2File(ndx_file, tree->GetPatTreeAsString());
		cout << "Index File : " << ndx_file << " Generated !!" << endl;
	}
	delete tree;
	tree = NULL;
	return vec;
}


vector<string> getRightWordsFromDir(string word, string dir){
	vector<string> vec;
	struct _finddata_t filefind;
	string curr = dir + "\\*.*";
	int  done = 0, handle;
	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1) return vec;

	int count = 0;
	while (!(done = _findnext(handle, &filefind)))
	{
		/*printf("%s\n", filefind.name);*/
		if (!strcmp(filefind.name, "..")){
			continue;
		}
		if ((_A_SUBDIR == filefind.attrib)) //是目录
		{
			curr = dir + "\\" + filefind.name;
			vector<string> new_vec = getRightWordsFromDir(word, curr);
			for (int i = 0; i < new_vec.size(); i++)
				vec.push_back(new_vec.at(i));
		}
		else  
		{
			if (filefind.name){
				string fn(filefind.name);
				char *tokenPtr = strtok(filefind.name, ".");
				tokenPtr = strtok(NULL, ",");
				if (!strcmp(tokenPtr, "txt")){
					vector<string> new_vec = getRightWordsFromFile(word, dir + "\\" + fn);
					for (int i = 0; i < new_vec.size(); i++)
						vec.push_back(new_vec.at(i));
				}
			}
		}
	}
	_findclose(handle);
	return vec;
}
vector<string> getRightNWordsFromDir(string word, string dir, int n){
	vector<string> vec;
	struct _finddata_t filefind;
	string curr = dir + "\\*.*";
	int  done = 0, handle;
	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1) return vec;

	int count = 0;
	while (!(done = _findnext(handle, &filefind)))
	{
		/*printf("%s\n", filefind.name);*/
		if (!strcmp(filefind.name, "..")){
			continue;
		}
		if ((_A_SUBDIR == filefind.attrib)) //是目录
		{
			curr = dir + "\\" + filefind.name;
			vector<string> new_vec = getRightNWordsFromDir(word, curr, n);
			for (int i = 0; i < new_vec.size(); i++)
				vec.push_back(new_vec.at(i));
		}
		else
		{
			if (filefind.name){
				string fn(filefind.name);
				char *tokenPtr = strtok(filefind.name, ".");
				tokenPtr = strtok(NULL, ",");
				if (!strcmp(tokenPtr, "txt")){
					vector<string> new_vec = getRightNWordsFromFile(word, dir + "\\" + fn, n);
					for (int i = 0; i < new_vec.size(); i++)
						vec.push_back(new_vec.at(i));
				}
			}
		}
	}
	_findclose(handle);
	return vec;
}

vector<string> getRightWordsFromFile(string word, string fn){
	PatriciaTrie<string, int> *tree = genPatTreeFromFile(fn);
	vector<string> vec = tree->GetRightWords(word);

	if (getIndexFile(fn) == ""){
		string ndx_file = getIndexFileName(fn);
		write2File(ndx_file, tree->GetPatTreeAsString());
		cout << "Index File : " << ndx_file << " Generated !!" << endl;
	}
	delete tree;
	tree = NULL;
	return vec;
}
vector<string> getRightNWordsFromFile(string word, string fn, int n){
	PatriciaTrie<string, int> *tree = genPatTreeFromFile(fn);
	vector<string> vec = tree->GetRightNWords(word, n);

	if (getIndexFile(fn) == ""){
		string ndx_file = getIndexFileName(fn);
		write2File(ndx_file, tree->GetPatTreeAsString());
		cout << "Index File : " << ndx_file << " Generated !!" << endl;
	}
	delete tree;
	tree = NULL;
	return vec;
}

// get the word frequency from a directory(read all index file under the dir)
// word		: the word to count
// dir		: the directory to read
int getWordFreqFromDir(string word, string dir){
	struct _finddata_t filefind;
	string curr = dir + "\\*.*";
	int   done = 0, handle;
	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1) return -1;

	int count = 0;
	while (!(done = _findnext(handle, &filefind)))
	{
		/*printf("%s\n", filefind.name);*/
		if (!strcmp(filefind.name, "..")){
			continue;
		}
		if ((_A_SUBDIR == filefind.attrib)) //是目录
		{
			curr = dir + "\\" + filefind.name;
			count += getWordFreqFromDir(word, curr);
		}
		else//不是目录，是文件     
		{
			if (filefind.name){
				string fn(filefind.name);
				char *tokenPtr = strtok(filefind.name, ".");
				tokenPtr = strtok(NULL, ",");
				if (!strcmp(tokenPtr, "txt")){
					count += getWordFreqFromFile(word, dir + "\\" + fn);
				}
			}
		}
	}
	_findclose(handle);
	return count;
}

// get the word frequency from a file
// word		: the word to count
// fn		: the file name to read
int getWordFreqFromFile(string word, string fn){
	string ndx_fn = getIndexFile(fn); // find the index file first
	if (ndx_fn == ""){
		PatriciaTrie<string, int> * tree = genPatTreeFromFile(fn);
		if (tree == NULL) return -1;
		PatriciaTrieNode<string, int>* node = tree->LookupNode(word);
		int count = (node == NULL) ? 0 : node->GetFrequency();

		string ndx_file = getIndexFileName(fn);
		write2File(ndx_file, tree->GetPatTreeAsString());
		delete tree;
		tree = NULL;
		cout << "Index File : " << ndx_file << " Generated !!" << endl;
		return count;
	}
	else{
		// if the index file is already there, just scan the index file
		return getWordFreqFromIndexFile(word, ndx_fn);
	}
}

int getWordFreqFromText(string word, string txt){
	PatriciaTrie<string, int> * tree = genPatTreeFromTxt(txt);
	if (tree == NULL) return -1;
	PatriciaTrieNode<string, int> *node = tree->LookupNode(word);
	int count = (node == NULL) ? 0 : node->GetFrequency();
	delete tree;
	tree = NULL;

	return count;
}

// get the word frequency from an index file
// word		: the word to count
// fn		: the file name to read
int getWordFreqFromIndexFile(string word, string fn){
	int count = 0;

	std::wifstream infile(fn.c_str());
	if (!infile) {
		return NULL;
	}

	infile.imbue(std::locale("CHS"));

	wstring buffer;
	while (std::getline(infile, buffer)) {
		size_t last = 1;
		size_t index = buffer.find_first_of(L",", last);
		last = index + 1;
		index = buffer.find_first_of(L",", last + 1); // to avoid the possible ,
		wstring wkey = buffer.substr(last, index - last);
		string key = wchar2char(wkey.c_str());

		if (key == word){
			last = index + 1;
			index = buffer.find_first_of(L",", last);
			last = index + 1;

			index = buffer.find_first_of(L",", last);
			wstring wcount = buffer.substr(last, index - last);
			count = atoi(wchar2char(wcount.c_str()));
			break;
		}
	}

	//if (!infile.eof()) {
	//	throw std::runtime_error("fatal error");
	//}
	return count;
}

void genPatIndexFileFromFile(string src_fn){
	string ndx_f = getIndexFileName(src_fn);
	genPatIndexFileFromFile(src_fn, ndx_f);
}

// generate the index file from a file content
// src_fn		: input source file name
// dst_fn		: the output index file name
// return		: void
void genPatIndexFileFromFile(string src_fn, string dst_fn){
	PatriciaTrie<string, int> * tree = genPatTreeFromFile(src_fn);
	if (tree == NULL) return;
	string content = tree->GetPatTreeAsString();
	delete tree;
	tree = NULL;

	// write to file
	write2File(dst_fn, content);
	cout << "Index File : " << dst_fn << " Generated !!" << endl;
}

// generate the index file from a given string
// txt		: input source text
// fn		: the output index file name
// return	: void
void genPatIndexFileFromTxt(string txt, string fn){
	PatriciaTrie<string, int> * tree = genPatTreeFromTxt(txt);
	if (tree == NULL) return;
	string content = tree->GetPatTreeAsString();
	delete tree;
	tree = NULL;
	// write to file
	write2File(fn, content);
	cout << "Index File : " << fn << " Generated !!" << endl;
}

void genPatIndexFileFromDir(string dir){
	struct _finddata_t filefind;
	string curr = dir + "\\*.*";
	int  done = 0, handle;
	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1) return;

	int count = 0;
	while (!(done = _findnext(handle, &filefind)))
	{
		/*printf("%s\n", filefind.name);*/
		if (!strcmp(filefind.name, "..")){
			continue;
		}
		if ((_A_SUBDIR == filefind.attrib)) //是目录
		{
			curr = dir + "\\" + filefind.name;
			genPatIndexFileFromDir(curr);
		}
		else
		{
			if (filefind.name){
				string fn(filefind.name);
				char *tokenPtr = strtok(filefind.name, ".");
				tokenPtr = strtok(NULL, ",");
				if (!strcmp(tokenPtr, "txt")){
					genPatIndexFileFromFile(dir + "\\" + fn, dir + "\\" + filefind.name + ".ndx");
				}
			}
		}
	}
	_findclose(handle);
}

// generate pat tree structure from a file
// src		: input source file
// return	: the tree data structure
PatriciaTrie<string, int> * genPatTreeFromFile(string src){
	string ndx_fn = getIndexFile(src); // find the index file first
	if (ndx_fn == ""){
		wchar_t linex[100];
		FILE* f1;
		wchar_t* wsrc = char2wchar(src.c_str());
		f1 = _wfopen(wsrc, L"rt+,ccs=UTF-8");
		locale loc("");
		wcout.imbue(loc);

		wstring content;
		while (!feof(f1))
		{
			fgetws(linex, 100, f1);
			content.append(linex);
		}

		fclose(f1);
		char* cnt = wchar2char(content.c_str());
		return genPatTreeFromTxt(cnt);
	}
	else{
		return genPatTreeFromIndexFile(ndx_fn);
	}
}

// generate pat tree structure from a index file
// src		: input source index file
// return	: the tree data structure
PatriciaTrie<string, int> * genPatTreeFromIndexFile(string src){
	std::wifstream infile(src.c_str());
	if (!infile) {
		return NULL;
	}

	infile.imbue(std::locale("CHS"));

	PatriciaTrie<string, int> *tree = new PatriciaTrie<string, int>();

	std::wstring buffer;
	while (std::getline(infile, buffer)) {
		size_t last = 1;
		size_t index = buffer.find_first_of(L",", last);
		wstring wbit = buffer.substr(last, index - last);
		int bit = atoi(wchar2char(wbit.c_str()));

		last = index + 1;
		index = buffer.find_first_of(L",", last + 1); // to avoid the possible ,
		wstring wkey = buffer.substr(last, index - last);
		string key = wchar2char(wkey.c_str());

		last = index + 1;
		index = buffer.find_first_of(L",", last);
		wstring wtype = buffer.substr(last, index - last);
		string type = wchar2char(wtype.c_str());

		last = index + 1;
		index = buffer.find_first_of(L",", last);
		wstring wcount = buffer.substr(last, index - last);
		int count = atoi(wchar2char(wcount.c_str()));

		wstring arrayStr = buffer.substr(index + 2, buffer.length() - index - 4);
		vector<int> poslist;
		wstring delim(L",");
		split(arrayStr, delim, &poslist);

		for each (int var in poslist)
		{
			tree->Insert(key, var, type);
		}
	}

	if (!infile.eof()) {
		throw std::runtime_error("fatal error");
	}
	return tree;
}

// generate pat tree structure from a given txt
// src		: input source input text
// return	: the tree data structure
PatriciaTrie<string, int> * genPatTreeFromTxt(string txt){
	//初始化分词组件
	if (!ICTCLAS_Init())//数据在当前路径下，默认为GBK编码的分词 (or ICTCLAS_Init("", UTF8_CODE) 来设置编码格式)
	{
		wcout << "NLPIR 初始化失败" << endl;
		return NULL;
	}

	ICTCLAS_SetPOSmap(ICT_POS_MAP_SECOND); // 选取标注集

	char *pcstr = (char*)malloc(strlen(txt.c_str()) + 1);
	memcpy(pcstr, txt.c_str(), strlen(txt.c_str()));
	pcstr[strlen(txt.c_str())] = '\0';

	//const wchar_t * sResult;
	int nCount;
	const result_t *pResult = ICTCLAS_ParagraphProcessA(pcstr, &nCount);

	PatriciaTrie<string, int> *tree = new PatriciaTrie<string, int>();

	int i = 1;
	for (i = 0; i<nCount; i++)
	{
		//pResult[i].word_type: 0 -- 核心词典, 1 -- 用户词典, 2 -- 专业词典
		//printf("start:%d, length:%d,POS_ID:%d,Word_ID:%d, Word:%s\n",
		//	pResult[i].start, pResult[i].length, pResult[i].iPOS, pResult[i].word_ID, pcstr + pResult[i].start);
		char *tstr = (char *)new char[pResult[i].length + 1];
		memcpy(tstr, pcstr + pResult[i].start, pResult[i].length);
		tstr[pResult[i].length] = '\0';
		//printf("%s inserted!\n", tstr);
		tree->Insert(tstr, pResult[i].start, pResult[i].sPOS);
		delete[] tstr;
	}
	delete[] pcstr;
	return tree;
}

#endif