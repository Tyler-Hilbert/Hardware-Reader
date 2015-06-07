#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <unordered_map>

using namespace std;


void parseFile(unordered_map<string,string> &attributeList, string file);
string trim(string s);


int main(int argc, const char** argv) {
  unordered_map<string,string> attributeList;
  parseFile(attributeList, "/proc/meminfo");
  parseFile(attributeList, "/proc/cpuinfo");

  cout
    << "RAM: "
    << attributeList["RAM"]
    << "\n"
    << "CPU: "
    << attributeList["CPU"]
    << "\n";
  return(0);
}

/**
   Parses proc info file
*/
void parseFile(unordered_map<string,string> &attributeList, string file) {
  // Get file to parse
  FILE *fp;
  fp = fopen(file.c_str(),"r");
  string read = "";

  // Parse file
  char c;
  do {
    c = fgetc(fp);

    if (feof(fp)) {
      break;
    }

    read += c;
    
    // If end of attribute, parse that attribute
    if (c == '\n') {
      int colonPos;
      if((colonPos = read.find(':') ) != -1) {
	string name = trim(read.substr(0, colonPos));
	read = read.substr(colonPos);
	string value = trim(read.substr(1, read.find('\n') - 1));

	if (name == "MemTotal") {
	  attributeList.emplace("RAM", value); 
	 
	} else if (name == "model name") {
	  attributeList.emplace("CPU", value);
	} 
     }

      read = "";
    }
  } while (true);

  fclose(fp);
}


/**
   Returns string without leading and trailing white space
*/
string trim(string str) {
  string s = str;
  s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
  s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
  return s;
}
