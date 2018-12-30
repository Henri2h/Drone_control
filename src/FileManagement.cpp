#include <dirent.h>
#include <sstream>

class FileManagement
{
  public:
    static std::string listDir(const char *list)
    {

        std::stringstream ss;
        DIR *d;

        struct dirent *dir;
        d = opendir(list);

        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                ss << dir->d_name << ", ";
            }
        }
        closedir(d);

        return ss.str();
    }
};