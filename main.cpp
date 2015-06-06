#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

using namespace std;

struct attribute {
  string name;
  string value;
};

struct hardware {
  attribute ram;
  attribute cpu;
} hardware;



void parseFile(string file);
string trim(string s);



int main(int argc, const char** argv) {
  parseFile("/proc/meminfo");
  parseFile("/proc/cpuinfo");

  cout 
    << hardware.ram.name
    << ": "
    << hardware.ram.value 
    << "\n"
    << hardware.cpu.name 
    << ": "
    << hardware.cpu.value
    << "\n";
  
  return(0);
}

/**
   Parses proc info file
*/
void parseFile(string file) {
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
	attribute atr;
	atr.name = trim(read.substr(0, colonPos));
	read = read.substr(colonPos);
	atr.value = trim(read.substr(1, read.find('\n') - 1));

	if (atr.name == "MemTotal") {
	  hardware.ram = atr;
	} else if (atr.name == "model name") {
	  hardware.cpu = atr;
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
