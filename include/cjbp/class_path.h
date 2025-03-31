#pragma once

#include <istream>
#include <string>
#include <memory>
#include <vector>

struct zip_t;

namespace cjbp {

class ClassPath {
public:
    virtual ~ClassPath() noexcept = default;

    ClassPath(const ClassPath&) = delete;
    ClassPath(ClassPath&&) = delete;
    ClassPath& operator=(const ClassPath&) = delete;
    ClassPath& operator=(ClassPath&&) = delete;

    // May return nullptr if the class is not found.
    virtual std::shared_ptr<std::istream> findClass(const std::string &name) = 0;

protected:
    ClassPath() = default;
};

class CompositeClassPath : public ClassPath {
public:
    explicit CompositeClassPath(std::vector<std::shared_ptr<ClassPath>> classPaths) : classPaths_(std::move(classPaths)) { }
    ~CompositeClassPath() noexcept override = default;

    std::shared_ptr<std::istream> findClass(const std::string &name) override;

private:
    std::vector<std::shared_ptr<ClassPath>> classPaths_;
};

class FileClassPath : public ClassPath {
public:
    FileClassPath(std::string name, std::string path);
    ~FileClassPath() noexcept override = default;

    std::shared_ptr<std::istream> findClass(const std::string &name) override;

private:
    bool isValid_;
    std::string name_, path_;
};

class DirectoryClassPath : public ClassPath {
public:
    explicit DirectoryClassPath(std::string path);
    ~DirectoryClassPath() noexcept override = default;

    std::shared_ptr<std::istream> findClass(const std::string &name) override;

private:
    std::string path_;
};

class JarClassPath : public ClassPath {
public:
    explicit JarClassPath(const std::string &path);
    ~JarClassPath() noexcept override;

    std::shared_ptr<std::istream> findClass(const std::string &name) override;

private:
    zip_t *zip_;
};

} // namespace cjbp
