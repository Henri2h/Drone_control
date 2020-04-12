#ifndef FileManagement_H
#define FileManagement_H

#define FILE_SAVE_NAME_FORMAT "/home/pi/data/save/data_%Y%m%d_%H%M%S.csv"
#define FILE_LOG_NAME_FORMAT "/home/pi/data/log/log_%Y%m%d_%H%M%S.txt"

#define FILE_LOG_NAME_SIZE 50
#define FILE_SAVE_NAME_SIZE 50

#define FILE_PATH_SETTINGS "/home/pi/data/settings.json"

#include <dirent.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <string>

#include "Ref.h"
#include "Data.h"

#include <json.hpp>

// for convenience
using json = nlohmann::json;

using namespace std;
class FileManagement
{
  private:
    // save name
    static char file_log_name[FILE_LOG_NAME_SIZE];
    static bool isFileOpen;
    static ofstream myfile;

  public:
    static void initialize();
    static void Log(string base, string text);
    static std::string listDir(const char *list);
    static string* readFile(string fileName);
    static void saveData(Data &data, float t);
    static void saveDataSettings(Data &data);
    static void readDataSettings(Data &data);
};

#endif
