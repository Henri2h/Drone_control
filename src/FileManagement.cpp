#include "FileManagement.h"

// class variables
bool FileManagement::isFileOpen = false;
char FileManagement::file_log_name[FILE_LOG_NAME_SIZE];
ofstream FileManagement::myfile;

void FileManagement::initialize()
{
    isFileOpen = false;
    time_t now = time(0);
    strftime(file_log_name, sizeof(file_log_name), FILE_LOG_NAME_FORMAT, localtime(&now));
}
void FileManagement::Log(string base, string text)
{
    ofstream save_file;
    save_file.open(FileManagement::file_log_name, std::ofstream::app);
    save_file << "[ " << base << " ] : " << text << "\n";
    std::cout << "[ " << base << " ] : " << text << "\n";
    save_file.close();
}

std::string FileManagement::listDir(const char *list)
{

    std::stringstream ss;
    DIR *d;

    struct dirent *dir;
    d = opendir(list);

    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            ss << dir->d_name << ",\n";
        }
    }
    closedir(d);

    return ss.str();
}

string *FileManagement::readFile(string fileName)
{

    string colour[4] = {fileName, "Red",
                        "Orange", "Yellow"};
    return colour;
}

void FileManagement::saveData(Data &data, float t)
{
    string sep = ",";
    if (data.status[status_Saving] == 1 && FileManagement::isFileOpen == false)
    {
        FileManagement::Log("FileManagement", "going to save");
        // open file
        static char name[FILE_SAVE_NAME_SIZE];
        time_t now = time(0);
        strftime(name, sizeof(name), FILE_SAVE_NAME_FORMAT, localtime(&now));
        FileManagement::myfile.open(name);

        FileManagement::isFileOpen = true;
        FileManagement::myfile << "t" << sep
                               << "stabilisation mode" << sep
                               << "commands_gen[0]" << sep << "commands_gen[1]" << sep << "commands_gen[2]" << sep << "commands_gen[3]" << sep
                               << "rates[0]" << sep << "rates[1]" << sep << "rates[2]" << sep
                               << "acceleration[0]" << sep << "acceleration[1]" << sep << "acceleration[2]" << sep
                               << "ang[0]" << sep << "ang[1]" << sep << "ang[2]" << sep
                               << "pid_out[0]" << sep << "pid_out[1]" << sep << "pid_out[2]"
                               << "\n";
    }

    if (data.status[status_Saving] == 1)
    {
        if (data.status[status_experience_mode] == 2)
        {
            t = data.time_exp; // we are doing an experiment so use this time
        }
        // save data in a csv format
        FileManagement::myfile << t << sep
                               << data.stabilisation_mode << sep
                               << data.commands_gen[0] << sep << data.commands_gen[1] << sep << data.commands_gen[2] << sep << data.commands_gen[3] << sep
                               << data.rates[0] << sep << data.rates[1] << sep << data.rates[2] << sep
                               << data.acceleration[0] << sep << data.acceleration[1] << sep << data.acceleration[2] << sep
                               << data.ang[0] << sep << data.ang[1] << sep << data.ang[2] << sep
                               << data.pid_debug[0] << sep << data.pid_debug[1] << sep << data.pid_debug[2]
                               << "\n";
    }

    if (data.status[status_Saving] == 0 && FileManagement::isFileOpen)
    {
        std::cout << "[ FileSaving ] : file saving ended\n";
        FileManagement::isFileOpen = false;
        FileManagement::myfile.close();
    }
}

void FileManagement::saveDataSettings(Data &data)
{
    json j;
    for (int i = 0; i < gains_length; i++)
    {
        j["gains_" + std::to_string(i)] = data.controller_gains[i];
    }
    
    // parameters
    j["IMU_filter_mode"] = data.parameters[params_IMU_Filter_mode];
    j["IMU_filter_value"] = data.parameters[params_IMU_Filter_value];

    ofstream file;
    file.open(FILE_PATH_SETTINGS, std::ofstream::trunc);
    file << j.dump();
    file.close();
    FileManagement::Log("File", "Settings saved to settings.json");
}
void FileManagement::readDataSettings(Data &data)
{
    ifstream file;
    
    file.open(FILE_PATH_SETTINGS);
    
    if(file.is_open()){
        json j;
        file >> j;
        file.close();
        for (int i = 0; i < gains_length; i++)
        {
            data.controller_gains[i] = j["gains_" + std::to_string(i)].get<float>();
            FileManagement::Log("Gains : " +  std::to_string(i), std::to_string(data.controller_gains[i]));
        }

        // parameters
        data.parameters[params_IMU_Filter_mode] = j["IMU_filter_mode"].get<float>();
        data.parameters[params_IMU_Filter_value] = j["IMU_filter_value"].get<float>();

        FileManagement::Log("File", "Data loaded from settings.json");
    }
}