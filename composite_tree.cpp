#include <exception>  // exception
#include <fstream>    // std::ifstream
#include <iostream>   // cout endl
#include <sys/stat.h> // struct stat
#include <dirent.h>   // DIR, struct dirent, opendir

#include "composite_tree.hpp"

#define unused_var(name) (void)(name);

namespace ilrd {
//******************* Free functions
static void cout_setw(int n);
static void cout_setw(int n)
{
    while (n >= 0) {
        --n;
        std::cout << " ";
    }
}

//******************* Exception
class bad_path : public std::exception {
public:
    explicit bad_path(const std::string& error = "Bad path") : m_error(error)
    {}

    virtual const char* what() const throw()
    {
        return m_error.c_str();
    }

private:
    std::string m_error;
};

//******************* Not Found
// wrapper to nullptr
// if file doesn't found, return an instance of this class
class NotFound : public Entry {
public:
    explicit NotFound();
    ~NotFound();
    void display(int depth = 0) override;
    std::shared_ptr<Entry> find(const std::string& name) override;

    static const std::string error_folder;
};

const std::string NotFound::error_folder(".");

NotFound::NotFound() : Entry(NotFound::error_folder)
{}

NotFound::~NotFound()
{}

void NotFound::display(int depth)
{
    unused_var(depth);
    // may be *throw* instead
    std::cout << "file doesn't found" << std::endl;
}

std::shared_ptr<Entry> NotFound::find(const std::string& name)
{
    unused_var(name);
    return std::shared_ptr<Entry>(new NotFound());
}

//******************* DirWrapper
class DirWrapper {
public:
    explicit DirWrapper(const std::string& path);
    ~DirWrapper();

    // wrapper to POSIX's readdir(3)
    dirent* read() const;
    bool is_open() const;

    // return file path "path/name"
    std::string get_full_path(const std::string& name = "") const;

    // checks if path is a valid file path and if it leads to a folder
    static bool is_dir(const std::string& path);

    DirWrapper(const DirWrapper&) = delete;
    DirWrapper& operator=(const DirWrapper&) = delete;

    static const char s_slash = '/';

private:
    std::string m_path;
    DIR* m_dir;
};

DirWrapper::DirWrapper(const std::string& path) : m_path(path), m_dir(nullptr)
{
    if (is_dir(path)) {
        m_dir = opendir(path.c_str());
    }
}

DirWrapper::~DirWrapper()
{
    closedir(m_dir);
}

bool DirWrapper::is_dir(const std::string& path)
{
    // check if path is a valid file path
    std::ifstream test(path.c_str());
    if (!test) {
        return false;
    }

    // check if path leads to a folder
    struct stat path_stat;
    stat(path.c_str(), &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

dirent* DirWrapper::read() const
{
    return (readdir(m_dir));
}

std::string DirWrapper::get_full_path(const std::string& name) const
{
    std::string path(m_path);

    // add back slash if it doesn't there
    if (path.back() != s_slash) {
        path += s_slash;
    }

    return (path + name);
}

bool DirWrapper::is_open() const
{
    return (m_dir != nullptr);
}

//******************* Entry
Entry::Entry(const std::string& path) : m_path(path)
{
    // if last char is a slash - delete it
    if (m_path.back() == DirWrapper::s_slash) {
        m_path.erase(m_path.size() - 1);
    }
}

Entry::~Entry()
{}

std::string Entry::get_name() const
{
    size_t index = m_path.find_last_of(DirWrapper::s_slash);
    return std::string(m_path.substr(index + 1));
}

const std::string& Entry::get_path() const
{
    return m_path;
}

//******************* Directory
Directory::Directory(const std::string& path, bool only_dir)
    : Entry(path), m_vec(), m_only_dir(only_dir), m_dir_num(0), m_file_num(0)
{}

Directory::~Directory()
{}

void Directory::display(int depth)
{
    if (0 < depth) {
        cout_setw(depth * 3);
        std::cout << "─ ";
    }

    std::cout << color << get_name() << reset << std::endl;

    for (auto entry : m_vec) {
        entry->display(depth + 1);
    }

    if (0 == depth) {
        std::cout<<m_dir_num<<" directories";

        if (false == m_only_dir) {
            std::cout<<", "<<m_file_num<<" files";
        }

        std::cout<<std::endl;
    } 
}

std::shared_ptr<Entry> Directory::find(const std::string& name)
{
    std::shared_ptr<Entry> rsl(new NotFound());

    for (auto entry : m_vec) {
        if (entry->get_name() == name) {
            return entry;
        }

        rsl = entry->find(name);
        if (rsl->get_path() != NotFound::error_folder) {
            // we found the file
            break;
        }
    }

    return rsl;
}

size_t Directory::get_dir_num() const
{
    return m_dir_num;
}

size_t Directory::get_file_num() const
{
    return m_file_num;
}

std::shared_ptr<Directory> Directory::create(const std::string& path, bool only_dir)
{
    if (DirWrapper::is_dir(path) == false) {
        throw bad_path("error opening a dir");
    }

    std::shared_ptr<Directory> directory(new Directory(path, only_dir));

    DirWrapper dir(path);
    if (dir.is_open()) {
        dirent* dp;
        while ((dp = dir.read()) != nullptr) {
            std::string name(dp->d_name);

            // ignore this directory and parent-directory
            if (name.front() == '.') {
                continue;
            }

            if (DirWrapper::is_dir(dir.get_full_path(name))) {
                // name is a directory and we can access it
                std::shared_ptr<Directory> new_dir = create(dir.get_full_path(name), only_dir);
                directory->add(new_dir);

                // count number of directories and files
                directory->m_dir_num  += 1 + new_dir->m_dir_num;
                directory->m_file_num += new_dir->m_file_num;
            } else if(false == only_dir) {
                // name is a file
                directory->add(std::shared_ptr<File>(new File(dir.get_full_path(name))));
                directory->m_file_num += 1;
            }
        } // while
    }     // if (dir.is_open())

    return directory;
}

void Directory::add(std::shared_ptr<Entry> entry)
{
    m_vec.push_back(entry);
}

//******************* File
File::File(const std::string& path) : Entry(path)
{}

File::~File()
{}

void File::display(int depth)
{
    std::string name(get_name());
    cout_setw(depth * 3);

    if (name.find(".out") == std::string::npos) {
        std::cout << "─ " << get_name() << std::endl;
    } else {
        std::cout << "─ " << color << get_name() << reset << std::endl;
    }
}

std::shared_ptr<Entry> File::find(const std::string& name)
{
    // file doesn't have files to search
    unused_var(name);
    return std::shared_ptr<Entry>(new NotFound());
}

} // namespace ilrd
