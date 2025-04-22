#pragma once

#include <istream>
#include <string>
#include <memory>
#include <vector>

struct zip_t;

namespace cjbp {

/**
 * Abstract class representing a class path.
 *
 * A ClassPath takes a fully-qualified class name (e.g. "java.lang.String"), and returns an input stream if the class's bytecode
 * can be found.
 */
class ClassPath {
public:
    virtual ~ClassPath() noexcept = default;

    ClassPath(const ClassPath&) = delete;
    ClassPath(ClassPath&&) = delete;
    ClassPath& operator=(const ClassPath&) = delete;
    ClassPath& operator=(ClassPath&&) = delete;

    /**
     * Finds the bytecode stream for a class by its fully-qualified name. May return nullptr if the class cannot be found.
     *
     * @param name The fully-qualified class name (e.g. "java.lang.String").
     */
    virtual std::shared_ptr<std::istream> findClass(const std::string &name) = 0;

protected:
    ClassPath() = default;
};

/**
 * A CompositeClassPath combines multiple ClassPaths. It will search each ClassPath in order until it finds the requested class.
 */
class CompositeClassPath : public ClassPath {
public:
    explicit CompositeClassPath(std::vector<std::shared_ptr<ClassPath>> classPaths) : classPaths_(std::move(classPaths)) { }
    ~CompositeClassPath() noexcept override = default;

    std::shared_ptr<std::istream> findClass(const std::string &name) override;

private:
    std::vector<std::shared_ptr<ClassPath>> classPaths_;
};

/**
 * A FileClassPath represents a single file containing a class's bytecode. It matches the file's bytecode to the given class name.
 */
class FileClassPath : public ClassPath {
public:
    FileClassPath(std::string name, std::string path);
    ~FileClassPath() noexcept override = default;

    std::shared_ptr<std::istream> findClass(const std::string &name) override;

private:
    bool isValid_;
    std::string name_, path_;
};

/**
 * A DirectoryClassPath represents a directory containing .class files.
 */
class DirectoryClassPath : public ClassPath {
public:
    explicit DirectoryClassPath(std::string path);
    ~DirectoryClassPath() noexcept override = default;

    std::shared_ptr<std::istream> findClass(const std::string &name) override;

private:
    std::string path_;
};

/**
 * A JarClassPath represents a JAR file containing .class files.
 *
 * It uses the kuba-zip library to read the JAR file.
 */
class JarClassPath : public ClassPath {
public:
    explicit JarClassPath(const std::string &path);
    ~JarClassPath() noexcept override;

    std::shared_ptr<std::istream> findClass(const std::string &name) override;

private:
    zip_t *zip_;
};

} // namespace cjbp
