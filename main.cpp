#include <stdio.h>
#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <unordered_map>
#include <iostream>
#include <sstream>

using namespace std;


void parseProcFile(unordered_map<string,string> &attributeList, string file);
void parseDmi(unordered_map<string,string> &attributeList, string cmd);
void parseDmiRamSlots(unordered_map<string,string> &attributeList);
void parseDmiGPU(unordered_map<string,string> &attributeList);
string trim(string s);
void printHeader();

string const MANUFACTURER = "Manufacturer";
string const CPU_NAME = "Computer Name";
string const RAM = "RAM";
string const CPU = "CPU";
string const MAX_RAM = "RAM Maximum Capacity";
string const RAM_SLOT = "Ram Slot ";
int ramSlots = 0; // The number of ram slots that are on the computer
string const GPU = "Graphics Card ";
int numGPU = 0; // The number of graphics card in the computer


int main(int argc, const char** argv) {
  unordered_map<string,string> attributeList;
  parseProcFile(attributeList, "/proc/meminfo");
  parseProcFile(attributeList, "/proc/cpuinfo");
  parseDmi(attributeList, "sudo dmidecode -t system");
  parseDmi(attributeList, "sudo dmidecode -t 16");
  parseDmiRamSlots(attributeList);
  parseDmiGPU(attributeList);

  printHeader();

  cout 
    << MANUFACTURER << ": " << attributeList[MANUFACTURER] << "\n"
    << CPU_NAME << ": " << attributeList[CPU_NAME] << "\n"
    << CPU << ": " << attributeList[CPU] << "\n"
    << RAM << ": " << attributeList[RAM] << "\n"
    << MAX_RAM << ": " << attributeList[MAX_RAM] << "\n";

  for (int i = 0; i < ramSlots; i++) {
    ostringstream ram;
    ram << RAM_SLOT << i;
    cout << ram.str() <<  ": " << attributeList[ram.str()] << "\n";
  }

  for (int i = 0; i < numGPU; i++) {
    ostringstream gpu;
    gpu << GPU << i;
    cout << gpu.str() <<  ": " << attributeList[gpu.str()] << "\n";
  }

  return(0);
}

/**
   Parses proc info file.
   Adds ram and cpu to attributeList
*/
void parseProcFile(unordered_map<string,string> &attributeList, string file) {
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
	  attributeList.emplace(RAM, value); 
	 
	} else if (name == "model name") {
	  attributeList.emplace(CPU, value);
	} 
     }

      read = "";
    }
  } while (true);

  fclose(fp);
}


/** 
    Parse dmidecode cmd to get manufacturer, product name, or maximum ram capacity
*/
void parseDmi(unordered_map<string,string> &attributeList, string cmd) {
  // Get file to parse
  FILE *fp;
  fp = popen(cmd.c_str(), "r");
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

	if (name == "Manufacturer") {
	  attributeList.emplace(MANUFACTURER, value); 
	} else if (name == "Product Name") {
	  attributeList.emplace(CPU_NAME, value);
	  break;
	} else if (name == "Maximum Capacity") {
	  attributeList.emplace(MAX_RAM, value);
	  break;
	}
     }

      read = "";
    }
  } while (true);

  fclose(fp);
}

/**
   Parses dmidecode -t 17 to get the ram slots
*/
void parseDmiRamSlots(unordered_map<string,string> &attributeList) {
  // Get file to parse
  FILE *fp;
  fp = popen("sudo dmidecode -t 17", "r");
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

	if (name == "Size") {
	  ostringstream ram;
	  ram << RAM_SLOT << ramSlots;
	  attributeList.emplace(ram.str(), value);
	  ramSlots++;
	} 
     }

      read = "";
    }
  } while (true);

  fclose(fp);
}

/**
   Parses dmidecode -t baseboard to get the GPU info
*/
void parseDmiGPU(unordered_map<string,string> &attributeList) {
  // Get file to parse
  FILE *fp;
  fp = popen("sudo dmidecode -t baseboard", "r");
  string read = "";

  // Parse file
  char c;
  bool parsingGPU = false; // Whether the device being parsed in currently a video card
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
	
	// Check if the GPU has started being parsed
	if (name == "Type" && value == "Video") {
	  parsingGPU = true;
	}

	if (parsingGPU && name == "Description") {
	  ostringstream gpu;
	  gpu << GPU << numGPU;
	  attributeList.emplace(gpu.str(), value);
	  numGPU++;
	  parsingGPU = false;
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

/** 
    Prints header ascii art
*/
void printHeader() {
  cout
    << " _    _               _                          _____                _\n"
    << "| |  | |             | |                        |  __ \\              | |\n"
    << "| |__| | __ _ _ __ __| |_      ____ _ _ __ ___  | |__) |___  __ _  __| | ___ _ __\n" 
    << "|  __  |/ _` | '__/ _` \\ \\ /\\ / / _` | '__/ _ \\ |  _  // _ \\/ _` |/ _` |/ _ \\ '__|\n"
    << "| |  | | (_| | | | (_| |\\ V  V / (_| | | |  __/ | | \\ \\  __/ (_| | (_| |  __/ |\n"
    << "|_|  |_|\\__,_|_|  \\__,_| \\_/\\_/ \\__,_|_|  \\___| |_|  \\_\\___|\\__,_|\\__,_|\\___|_|\n\n";
}
