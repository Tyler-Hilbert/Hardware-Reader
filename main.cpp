#include <stdio.h>
#include <string>
#include <iostream>

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
	atr.name = read.substr(0, colonPos);
	read = read.substr(colonPos);
	atr.value = read.substr(1, read.find('\n') - 1);

	if (atr.name == "MemTotal") {
	  hardware.ram = atr;
	} else if (atr.name.substr(0, 10)  == "model name") {
	  hardware.cpu = atr;
	} 
     }

      read = "";
    }
  } while (true);

  fclose(fp);
}
