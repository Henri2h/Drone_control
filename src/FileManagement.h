
#define FILE_SAVE_NAME_FORMAT "/home/pi/data/save/data_%Y%m%d_%H%M%S.csv"
#define FILE_LOG_NAME_FORMAT "/home/pi/data/log/log_%Y%m%d_%H%M%S.txt"

#define FILE_LOG_NAME_SIZE 50
#define FILE_SAVE_NAME_SIZE 50

#include <dirent.h>
#include <sstream>
#include <ctime>

#include "Ref.h"

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
    static void saveData(float t, int stabilisation_mode, float *commands_gen, float *acceleration, float *rates, float *ang, float *pid_debug, double *status);
};