/*

Detect and report Java Version
Author Keith Smelser
Tarrant County College
Utility Program
Fall 2023

Compiled with Embarcadero C++ Version 6.3

This code detects and optionally displays the installed Java version as known per the 
location of where this program is located as reported via "java -version".
This program can optionally run in a 'batch' mode and sets the error_code if there is a detected mismatch.
See Comments for WinMain().


   Command line options are:
	-q (optional) cmd line displays only (No window popup).
	-r x.x.x.x minimum required version. Can be 1-4 numbers separated by periods.
	-R x.x.x.x Same as above but installed java version must exactly match.
	The -r and -R options set the CLI errorlevel for missed version requirements.

See 
http://www.cs.uah.edu/~rcoleman/CS307/GUIs/GUI_Lab_01-MsgBox.html
https://www.oracle.com/java/technologies/downloads/
https://www.oracle.com/java/technologies/java-se-support-roadmap.html
https://docs.oracle.com/en/java/javase/11/install/version-string-format.html#GUID-DCA60310-6565-4BB6-8D24-6FF07C1C4B4E

*/

#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>

using namespace std;

const string NOT_FOUND = "Java is not installed";

// See https://docs.oracle.com/en/java/javase/11/install/version-string-format.html#GUID-DCA60310-6565-4BB6-8D24-6FF07C1C4B4E
struct versionData {
  int feature = -1;
  int interim = 0;
  int update = 0;
  int patch = 0;
};

//Globals
bool quietMode = false;
bool exactVersion = false;
bool expectedVersionPassed = false;
string cmdVersionString;
versionData cmdVersion;

/**
Convert String object to C-String
*/

char * stringToCString(string line) {
	
  char * lineX = new char [line.length()+1];
  strcpy (lineX, line.c_str());
  return lineX;
}

/*
Take the data line and obtain the Java Version
*/

string parseJavaVersionLine(string line) {

  string result = NOT_FOUND;
  char * lineX = stringToCString(line);

  // Split by space
  char * token = strtok(lineX, " ");

  while (token != NULL) {
    if (token[0] == '"') {
      // if found - remove Quotes
      result = token;
      result.erase(remove(result.begin(), result.end(), '"'), result.end());
      break;
    }
    token = strtok(NULL, " ");
  }
  return result;
}

/**
Get local temp folder
*/

string getTMPFolder() {

	char const* envTmpF1 = getenv("TMP");
	char const* envTmpF2 = getenv("TEMP");
	char const* envTmpF3 = getenv("APPDATA");
	
	string tmpFolder = ".";
	// I would expect TMP to exist - if not try TEMP,or APPDATA or finally just local dir
	if (envTmpF1 != NULL) {
		string s1(envTmpF1);
		tmpFolder = s1;
	} 
	else if (envTmpF2 != NULL) {
		string s1(envTmpF2);
		tmpFolder = s1;
	} else if (envTmpF3 != NULL) {
		string s1(envTmpF3);
		tmpFolder = s1;
	} 
	// default was (period). No need for else.
	
	return tmpFolder;
	 
}
/**
Process to retrieve installed java version as a string
*/

string checkInstalledJavaVersion() {
  string result = "OK";
  string fullFileName;
  string cmd;

  { // avoid reusing temp/tmp variables
    string tempFile = "javaVersion.txt";
    string tmpFolder = getTMPFolder();
    fullFileName = tmpFolder + "\\" + tempFile;
    //cout << fullFileName << endl;
    cmd = "java -version 2>" + fullFileName;
  }

  fstream inFile;

  //Run cmd and store it to the file	
  int returnCode = system(cmd.c_str());
  if (returnCode != 0) {
    return NOT_FOUND;
  }

  inFile.open(fullFileName);
  if (inFile.is_open()) {
    string fileLine;
    getline(inFile, fileLine);
    result = parseJavaVersionLine(fileLine);
  } else {
    result = "Unable to open file " + fullFileName;
  }

  inFile.close();
  remove(fullFileName.c_str());

  return result;

}


/**
  Compare Java versions to check for exact match
  OR 
  match or higher

*/

bool compareToCMDVersion(versionData iVer) {

  bool answer = true;
  if (exactVersion) {
    answer = (iVer.feature == cmdVersion.feature && iVer.interim == cmdVersion.interim && iVer.patch == cmdVersion.patch && iVer.update == cmdVersion.update);
  } else {
    if (iVer.feature < cmdVersion.feature) {
      answer = false;
    } else if (iVer.feature > cmdVersion.feature) {
      answer = true;
    } else if (iVer.interim < cmdVersion.interim) {
      answer = false;
    } else if (iVer.interim > cmdVersion.interim) {
      answer = true;
    } else if (iVer.patch < cmdVersion.patch) {
      answer = false;
    } else if (iVer.patch > cmdVersion.patch) {
      answer = true;
    } else if (iVer.update < iVer.update) {
      answer = false;
    }
  }
  return answer;

}

/*
From String to Version Data
*/

versionData loadVersionData(string myData) {

  versionData result;
  char * lineX = stringToCString(myData);
  char * token = strtok(lineX, ".");
  int count = 1;
  while (token != NULL) {
    switch (count++) {
    case 1:
      result.feature = stoi(token);
      break;
    case 2:
      result.interim = stoi(token);
      break;
    case 3:
      result.patch = stoi(token);
      break;
    case 4:
      result.update = stoi(token);
      break;
    }
    token = strtok(NULL, ".");
  }

  return result;
}

/**
  Parse the cmd line and set globals

*/

void parseCmdLine(string cmdLine) {
  if (cmdLine.size() == 0) {
    return;
  }

  char * lineX = stringToCString(cmdLine);

  // Split by space
  char * token = strtok(lineX, " ");

  while (token != NULL) {
    if (strcmp(token, "-q") == 0) {
      quietMode = true;
    } else if (strcmp(token, "-r") == 0) {
      token = strtok(NULL, " ");
      if (token == NULL) {
        break;
      }
      cmdVersionString = token;
      cmdVersion = loadVersionData(cmdVersionString);
      expectedVersionPassed = true;

    } else if (strcmp(token, "-R") == 0) {
      token = strtok(NULL, " ");
      if (token == NULL) {
        cout << "Break on " << token << endl;
        break;
      }
      exactVersion = true;
      cmdVersionString = token;
      cmdVersion = loadVersionData(cmdVersionString);
      expectedVersionPassed = true;
    } else {
    
	     string msg = "Command line options are:\n";
	     msg+="-q (optional) cmd line displays only (No window popups).\n";
	     msg+="-r x.x.x.x minimum required version. Can be 1-4 numbers separated by periods.\n";
	     msg+="-R x.x.x.x Same as above but installed java version must exactly match.\n";
	     msg+="The -r and -R options set the CLI errorlevel for missed version requirements.\n";
	     
    	 MessageBox(NULL, msg.c_str(), "Java Version", MB_OK | MB_ICONERROR);
    	 exit(1);
      
    }
    token = strtok(NULL, " ");
  }
}

/**
  Main starts here
  No cmd line - displays installed version via Windows
  cmd line 
  -q  = quiet - No Windows - only console output (Optional)
  -r 1.1.1.1  = minimum required version
  -R 1.1.1.1  = must be this version only
  version numbers can be 1, 1.1, 1.1.1, or 1.1.1.1
*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  ShowWindow(GetConsoleWindow(), SW_HIDE); // Hides terminal window for Windows modes
  string cmdLine = lpCmdLine;
  parseCmdLine(cmdLine);
  if (quietMode) {
  	ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL); 
  }
  string answer = checkInstalledJavaVersion();
  int exitResult = 0;

  if (cmdLine.size() == 0)
    if (quietMode) {
      cout << answer << endl;
    } else {
      MessageBox(NULL, answer.c_str(), "Java Version", MB_OK |  MB_ICONINFORMATION);
    }
  else {
    if (answer == NOT_FOUND) {
      if (quietMode) {
        cout << answer << endl;
      } else {
        MessageBox(NULL, answer.c_str(), "Java Version", MB_OK | MB_ICONWARNING);
      }
      exitResult = 1;

    } else {

      versionData installed = loadVersionData(answer);
      string msg;
      string subMsg = "Installed  :   " + answer;
      if (expectedVersionPassed) {
        subMsg += "\nRequired   :   " + cmdVersionString;
      }
      if (compareToCMDVersion(installed)) {
        msg = "Java version OK\n" + subMsg;
        if (quietMode) {
          cout << msg << endl;
        } else {
          MessageBox(NULL, msg.c_str(), "Java Version", MB_OK | MB_ICONINFORMATION);
        }
      } else {
        exitResult = installed.feature;
        msg = "Java version requirements mismatch\n" + subMsg;
        if (quietMode) {
          cout << msg << endl;
        } else {
          MessageBox(NULL, msg.c_str(), "Java Version", MB_OK | MB_ICONSTOP);
        }
      }
    }
  }

  return exitResult;
}