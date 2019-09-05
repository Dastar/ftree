#include <iostream>
#include <exception>  // exception

#include "composite_tree.hpp"
using namespace ilrd;

static void test();
static void help();
static void run(int argc, char const *argv[]);
static void run_test();
template <typename T>
static void check_equal(T result, T expected, int line, bool fatal=false);
template <typename T>
static void check_equal(T result, T expected, std::string error, bool fatal=false);

int main(int argc, char const *argv[])
{
    try {
        run(argc, argv);
    }
    catch (std::exception& e) {
        std::cout<<e.what()<<std::endl;
    }

    
    return 0;
}

template <typename T>
static void check_equal(T result, T expected, int line, bool fatal)
{
    if (result != expected)
    {
        std::cout<<"ERROR on line "<<line<<std::endl;
        if (fatal) {
            exit(1);
        }
    }
}

template <typename T>
static void check_equal(T result, T expected, std::string error, bool fatal)
{
    if (result != expected)
    {
        std::cout<<"ERROR: "<<error<<std::endl;
        if (fatal) {
            exit(1);
        }
    }
}

static void run(int argc, char const *argv[])
{
    std::string disp(".");
    std::string srch(".");

    bool dir_only = false;
    bool search   = false;

    // check arguments
    int i(1);
    for (i = 1; i < argc && *argv[i] == '-'; ++i) {
        std::string param(argv[i]);

        if (param == "-h") {
            help();
            return;
        }
        else if (param == "-t") {
            run_test();
            return;
        }
        else if (param == "-d") {
            dir_only = true;
        }
        else if (param == "-f") {
            search = true;
        }
    } // for

    // check display / search folders
    if (i == argc - 1 && search) {
        srch = argv[i];
    } 
    else if (i == argc - 1) {
        disp = argv[i];
    }
    else if (search && i == argc - 2) {
        disp = argv[i];
        srch = argv[i + 1];
    }

    std::shared_ptr<Directory> dir = Directory::create(disp, dir_only);
    if (search) {
        dir->find(srch)->display();
    }
    else {
        dir->display();
    }

}

static void help()
{
    std::cout<<"FindTree                    User Manual                    FindTree\n\n";
    std::cout<<"\033[1mNAME\033[0m\n";
    std::cout<<"       FindTree - list contents of directories in a tree-like format.\n\n";
    std::cout<<"\033[1mLISTING OPTIONS\033[0m\n";
    std::cout<<"       .      Display (search in) current directory\n\n";
    std::cout<<"       ..     Display (search in) parent directory\n\n";
    std::cout<<"       -h     Print help\n\n";
    std::cout<<"       -d     List directories only\n\n";
    std::cout<<"       -f     If given only name of a file, searching this file in\n";
    std::cout<<"              current directory. If given path in first param and\n";
    std::cout<<"              file name in second, searching for file in given\n";
    std::cout<<"              directory.\n\n";
    std::cout<<"\033[1mAUTHOR\033[0m\n";
    std::cout<<"       Evgeny (dastar.ron@gmail.com)\n";
}

static void run_test()
{
    // test
    // for best performens run test.sh
    try {
        test();
    } catch (std::exception& e) {
        std::cout<<"\n"<<e.what()<<std::endl;
    }
}

static void test()
{
    // Create tree
    std::shared_ptr<Directory> dir = Directory::create("./Website");
    check_equal(dir.get() == nullptr, false, __LINE__);
    check_equal(dir->get_dir_num(), 9UL, "Not enought folders");
    check_equal(dir->get_file_num(), 17UL, "Not enought files");
    check_equal(dir->get_name(), std::string("Website"), __LINE__);
    check_equal(dir->get_path(), std::string("./Website"), __LINE__);

    // Check two equal trees
    std::shared_ptr<Directory> dir2 = Directory::create("./Website");
    check_equal(dir.get() != dir2.get(), true, __LINE__);
    check_equal(dir->get_dir_num(), dir2->get_dir_num(), __LINE__);
    check_equal(dir->get_file_num(), dir2->get_file_num(), __LINE__);

    // Check find folder
    check_equal(dir->find("imgs")->get_name(), std::string("imgs"), __LINE__);
    check_equal(dir->find("imgs")->get_path(), std::string("./Website/imgs"), __LINE__);

    // Check find file
    check_equal(dir->find("c")->get_name(), std::string("c"), __LINE__);
    check_equal(dir->find("c")->get_path(), std::string("./Website/imgs/png/c"), __LINE__);

    // Check find error
    check_equal(dir->find("ERROR")->get_name(), std::string("."), __LINE__);
    check_equal(dir->find("ERROR")->get_path(), std::string("."), __LINE__);

    std::cout<<"Display on error search: ";
    dir->find("ERROR")->display();
    
    std::cout<<"\nDisplay a tree: \n";
    dir->display();

    // Try to open doesn't existing folder / file
    std::shared_ptr<Directory> error_dir = Directory::create("error_folder");
}
