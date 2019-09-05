// File Name: composite_tree.hpp
// Date:
// Reviewer:

// Header file implementing a composite tree.

#ifndef ILRD_COMPOSITE_TREE_HPP
#define ILRD_COMPOSITE_TREE_HPP

#include <memory> //std::shared_ptr
#include <string> //std::string
#include <vector> //std::vector

//------------------------------------------------------------------------------

namespace ilrd {
class Entry {
public:
    explicit Entry(const std::string& path);
    virtual ~Entry();
    virtual void display(int depth = 0) = 0;
    virtual std::shared_ptr<Entry> find(const std::string& name) = 0;

    const std::string& get_path() const;
    std::string get_name() const;

    Entry(const Entry& other) = delete;
    Entry& operator=(const Entry& other) = delete;

private:
    std::string m_path;
};

class File : public Entry {
public:
    explicit File(const std::string& path);
    ~File() override;
    void display(int depth = 0) override;
    std::shared_ptr<Entry> find(const std::string& name) override;

private:
    const std::string color = "\033[1;32m";
    const std::string reset = "\033[0m";
};

class Directory : public Entry {
public:
    ~Directory() override;
    void display(int depth = 0) override;
    std::shared_ptr<Entry> find(const std::string& name) override;

    size_t get_dir_num() const;
    size_t get_file_num() const;
    // throw class bad_path: public std::exception
    static std::shared_ptr<Directory> create(const std::string& path,
                                             bool only_dir = false);

private:
    explicit Directory(const std::string& path, bool only_dir = false);
    void add(std::shared_ptr<Entry> entry);

    std::vector<std::shared_ptr<Entry> > m_vec;
    bool m_only_dir;
    size_t m_dir_num;
    size_t m_file_num;

    const std::string color = "\033[1;34m";
    const std::string reset = "\033[0m";
};

} // namespace ilrd
#endif // ILRD_COMPOSITE_TREE_HPP
