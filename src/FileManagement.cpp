#include <dirent.h>

class FileManagement{
    public:
 static void listDir(const char * list){
    

    DIR *d;
    
    struct dirent *dir;
    d = opendir(list);

if(d){
    while((dir=readdir(d)) != NULL){
 cout << "[ DIR ] : " << dir->d_name << "\n";   
    }
}
closedir(d);


}};