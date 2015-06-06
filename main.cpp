#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

struct attribute {
  string name;
  string value;
  //unsigned long int value;
  string unit;
};

int main(int argc, const char** argv) {
  // Get file to parse
  FILE *fp;
  fp = fopen("/proc/meminfo","r");
  string read = "";

  // Parse file
  while(!feof(fp)) {
    char c = fgetc(fp);
    read += c;
    
    // If end of attribute, parse that attribute
    if (c == '\n') {
      attribute atr;
      istringstream stream(read);
      stream >> atr.name >> atr.value >> atr.unit;
      cout << atr.name << " " << atr.value << " " << atr.unit << "\n";
      read = "";
    }
  }

  fclose(fp); 
  
  return(0);
}
