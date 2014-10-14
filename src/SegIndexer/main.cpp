#include "apis.h"
#include <ctype.h>

void printVector(vector<string> vec){
	for each (string var in vec)
	{
		cout << var << " ";
	}
	cout << endl;
}

void print(string cnt){
	cout << cnt;
}
void println(string cnt){
	cout << cnt << endl;
}

void printIndexUsage(){
	println("SegIndexer index");
	println("\t   --file filename_to_generate_index");
	println("\t   --text text_to_generate_index --out output_file_name");
	println("\t   --dir dir_to_generate_index_file");
}
void printWordFreqUsage(){
	println("SegIndexer wordfreq");
	println("\t   --word word  --file from_file");
	println("\t   --word word  --text from_text");
	println("\t   --word word  --dir directory");
}

void printWordLeftUsage(){
	println("SegIndexer wordleft");
	println("\t   --word word [--num N] --file from_file");
	println("\t   --word word [--num N] --dir from_dir");
}

void printWordRightUsage(){
	println("SegIndexer wordright");
	println("\t   --word word [--num N] --file from_file");
	println("\t   --word word [--num N] --dir from_dir");
}

void printHelpUsage(){
	println("SegIndexer help [all | index | wordfreq | wordleft | wordright]");
}

void printAllUsage(){
	println("The SegIndexer usage:");
	printIndexUsage();
	printWordFreqUsage();
	printWordLeftUsage();
	printWordRightUsage();
	printHelpUsage();
}

////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv){
	if (argc == 1){
		printAllUsage();
		return 0;
	}
	if (!strcmp(argv[1], "index")){
		if (argc <= 3) { println("Error: insufficient parameters provided!"); printIndexUsage(); return 0; }
		else if (argc > 6) { println("Error: more parameters provided!"); printIndexUsage(); return 0; }
		else if(argc == 4){
			void(*pfun)(string);
			if (!strcmp(argv[2], "--file")){
				if (isValidFile(argv[3])){
					pfun = genPatIndexFileFromFile;
				}
				else{
					println("Error: error file_name provided!"); printIndexUsage(); return 0;
				}
			}
			else if (!strcmp(argv[2], "--dir")){
				if (isValidDir(argv[3])){
					pfun = genPatIndexFileFromDir;
				}
				else{
					println("Error: error directory path provided!"); printIndexUsage(); return 0;
				}
			}
			else{
				println("Error: wrong parameters provided!"); printIndexUsage(); return 0;
			}
			pfun(argv[3]);
		}
		else if (argc == 6){
			int txt_pos = 0, file_pos = 0;
			if (!strcmp(argv[2], "--text"))
				txt_pos = 2, file_pos = 4;
			else if (!strcmp(argv[4], "--text"))
				txt_pos = 4, file_pos = 2;
			else{
				println("Error: wrong parameters provided!"); printIndexUsage(); return 0;
			}
			genPatIndexFileFromTxt(argv[txt_pos + 1], argv[file_pos + 1]);
		}
	}
	else if (!strcmp(argv[1], "wordfreq")){
		if (argc < 6) { println("Error: insufficient parameters provided!"); printWordFreqUsage(); return 0; }
		else if (argc > 6) { println("Error: more parameters provided!"); printWordFreqUsage(); return 0; }
		else{
			// 6
			int word_pos = 0, other_pos = 0;
			if (!strcmp(argv[2], "--word"))
			{	word_pos = 2; other_pos = 4;}
			else if (!strcmp(argv[4], "--word"))
			{	word_pos = 4; other_pos = 2;}
			else
			{	println("Error: error parameters provided!"); printWordFreqUsage(); return 0;}

			int(*pfun)(string word, string cnt);
			if (!strcmp(argv[other_pos], "--file")){
				if (isValidFile(argv[other_pos+1]))	
					pfun = getWordFreqFromFile;
				else{
					println("Error: error file_name provided!"); printWordFreqUsage(); return 0;
				}
			}
			else if (!strcmp(argv[other_pos], "--text")){
				pfun = getWordFreqFromText;
			}
			else if (!strcmp(argv[other_pos], "--dir")){
				if (isValidDir(argv[other_pos + 1])) 
					pfun = getWordFreqFromDir;
				else{
					println("Error: error dir_name provided!"); printWordFreqUsage(); return 0;
				}
			}
			else
			{
				println("Error: error parameters provided!"); printWordFreqUsage(); return 0;
			}
			cout << "Key word: "<<argv[word_pos + 1]  << " 共出现" << pfun(argv[word_pos + 1], argv[other_pos + 1]) << "次！" ;
		}
	}
	else if (!strcmp(argv[1], "wordleft")){
		if (argc < 6) { println("Error: insufficient parameters provided!"); printWordLeftUsage(); return 0; }
		else if (argc > 8) { println("Error: more parameters provided!"); printWordLeftUsage(); return 0; }
		else if (argc % 2 != 0) { println("Error: error parameters provided!"); printWordLeftUsage(); return 0; }
		else{
			// 6, 8
			vector<string>(*pfn)(string, string);
			pfn = NULL;
			vector<string>(*pNfn)(string, string, int);
			pNfn = NULL;
			int word_pos = 0, num_pos = 0, other_pos = 0;
			for (int j = 2; j < argc; j += 2){
				if (!strcmp(argv[j], "--word")) word_pos = j;
				else if (!strcmp(argv[j], "--num"))  {
					if (isAllDigit(argv[j + 1])) num_pos = j;
					else
					{ println("Error: error parameters provided!"); printWordLeftUsage(); return 0;	}
				}
				else if (!strcmp(argv[j], "--file") || !strcmp(argv[j], "--dir")) other_pos = j;
				else{
					println("Error: error parameters provided!"); printWordLeftUsage(); return 0;
				}
			}
			if (!strcmp(argv[other_pos], "--file")){
				if (isValidFile(argv[other_pos + 1])){
					if (num_pos != 0) pNfn = getLeftNWordsFromFile;
					else pfn = getLeftWordsFromFile;
				}
				else{
					println("Error: error file name provided!"); printWordLeftUsage(); return 0;
				}
			}
			else if (!strcmp(argv[other_pos], "--dir")){
				if (isValidDir(argv[other_pos + 1])){
					if (num_pos != 0) pNfn = getLeftNWordsFromDir;
					else pfn = getLeftWordsFromDir;
				}
				else{
					println("Error: error directory name provided!"); printWordLeftUsage(); return 0;
				}
			}

			vector<string> res;
			if (pNfn != NULL)
				res = pNfn(argv[word_pos + 1], argv[other_pos + 1], atoi(argv[num_pos + 1]));
			else if (pfn != NULL)
				res = pfn(argv[word_pos + 1], argv[other_pos + 1]);

			cout << argv[word_pos + 1] << " Left words are:";
			printVector(res);

		}
	}
	else if (!strcmp(argv[1], "wordright")){
		if (argc < 6) { println("Error: insufficient parameters provided!"); printWordRightUsage(); return 0; }
		else if (argc > 8) { println("Error: more parameters provided!"); printWordRightUsage(); return 0; }
		else if (argc % 2 != 0) { println("Error: error parameters provided!"); printWordRightUsage(); return 0; }
		else{
			// 6, 8
			vector<string>(*pfn)(string, string);
			pfn = NULL;
			vector<string>(*pNfn)(string, string, int);
			pNfn = NULL;
			int word_pos = 0, num_pos = 0, other_pos = 0;
			for (int j = 2; j < argc; j += 2){
				if (!strcmp(argv[j], "--word")) word_pos = j;
				else if (!strcmp(argv[j], "--num"))  {
					if (isAllDigit(argv[j + 1])) num_pos = j;
					else
					{
						println("Error: error parameters provided!"); printWordRightUsage(); return 0;
					}
				}
				else if (!strcmp(argv[j], "--file") || !strcmp(argv[j], "--dir")) other_pos = j;
				else{
					println("Error: error parameters provided!"); printWordRightUsage(); return 0;
				}
			}
			if (!strcmp(argv[other_pos], "--file")){
				if (num_pos != 0) pNfn = getRightNWordsFromFile;
				else pfn = getRightWordsFromFile;
			}
			else if (!strcmp(argv[other_pos], "--dir")){
				if (num_pos != 0) pNfn = getRightNWordsFromDir;
				else pfn = getRightWordsFromDir;
			}

			vector<string> res;
			if (pNfn != NULL)
				res = pNfn(argv[word_pos + 1], argv[other_pos + 1], atoi(argv[num_pos + 1]));
			else if (pfn != NULL)
				res = pfn(argv[word_pos + 1], argv[other_pos + 1]);

			cout << argv[word_pos + 1] << " Right words are:";
			printVector(res);
		}
	}
	else if (!strcmp(argv[1], "help")){
		if (argc == 2) { printHelpUsage(); return 0; }
		else if (argc > 3) { println("Error help command!"); printHelpUsage(); return 0; }
		else {
			if (!strcmp(argv[2], "index")){ printIndexUsage(); return 0; }
			else if (!strcmp(argv[2], "wordfreq")){ printWordFreqUsage(); return 0; }
			else if (!strcmp(argv[2], "wordleft")){ printWordLeftUsage(); return 0; }
			else if (!strcmp(argv[2], "wordright")){ printWordRightUsage(); return 0; }
			else if (!strcmp(argv[2], "all")){ printAllUsage(); return 0; }
			else{ println("Error: Unknow command!"); printHelpUsage(); return 0; }
		}
	}
	else{
		println("Unsupported command typed");
	}

	//int count = getWordFreqFromFile("金属", "D:\\data\\新建文本文档.txt");
	//cout << "出现 " << count << " 次。"<< endl;
	//genPatIndexFileFromFile("D:\\data\\新建文本文档.txt", "D:\\data\\新建文本文档_result.txt");

	//PatriciaTrie<string, int> * tree = genPatTreeFromFile("D:\\data\\新建文本文档.txt");
	//vector<string> wecStr = tree->GetRightNWords("金属", 2);
	//printVector(wecStr);
	//genPatTreeFromIndexFile("D:\\data\\新建文本文档_result.txt");
	//genPatIndexFileFromDir("D:\\data\\主要零部件及外部设施和机构\\动力系统2");
	system("pause");
}
