#include "cjbp/class_path.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "zip/zip.h"

namespace cjbp {

std::shared_ptr<std::istream> CompositeClassPath::findClass(const std::string &name) {
    for (auto &classPath : this->classPaths_) {
        std::shared_ptr<std::istream> stream = classPath->findClass(name);
        if (stream != nullptr) return stream;
    }
    return nullptr;
}



FileClassPath::FileClassPath(std::string name, std::string path) : name_(std::move(name)), path_(std::move(path)) {
    this->isValid_ = std::filesystem::exists(path_) && std::filesystem::is_regular_file(path_);
}

std::shared_ptr<std::istream> FileClassPath::findClass(const std::string &name) {
    if (!this->isValid_ || this->name_ != name) return nullptr;

    return std::make_shared<std::ifstream>(this->path_, std::ios::binary);
}



DirectoryClassPath::DirectoryClassPath(std::string path) : path_(std::move(path)) {
    if (!this->path_.empty() && this->path_.back() != '/') this->path_ += '/';
}

std::shared_ptr<std::istream> DirectoryClassPath::findClass(const std::string &name) {
    std::string path = this->path_ + name + ".class";
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) return nullptr;

    return std::make_shared<std::ifstream>(path, std::ios::binary);
}



class ZipEntryStreamBuf : public std::streambuf {
public:
    ZipEntryStreamBuf(void *buf, size_t size);
    ~ZipEntryStreamBuf() noexcept override;

private:
    void *buf_;
    size_t size_;
};

class ZipEntryIStream : public std::istream {
public:
    explicit ZipEntryIStream(std::unique_ptr<ZipEntryStreamBuf> buf);
    ~ZipEntryIStream() noexcept override = default;

private:
    std::unique_ptr<ZipEntryStreamBuf> buf_;
};

ZipEntryStreamBuf::ZipEntryStreamBuf(void *buf, size_t size) : buf_(buf), size_(size) {
    this->setg(static_cast<char *>(this->buf_), static_cast<char *>(this->buf_), static_cast<char *>(this->buf_) + this->size_);
}

ZipEntryStreamBuf::~ZipEntryStreamBuf() noexcept {
    if (this->buf_ != nullptr) {
        free(this->buf_);
        this->buf_ = nullptr;
    }
}

ZipEntryIStream::ZipEntryIStream(std::unique_ptr<ZipEntryStreamBuf> buf) : std::istream(buf.get()), buf_(std::move(buf)) { }

JarClassPath::JarClassPath(const std::string &path) {
    this->zip_ = zip_open(path.c_str(), 0, 'r');
    if (this->zip_ == nullptr) throw std::runtime_error("Failed to open jar file " + path);
}

JarClassPath::~JarClassPath() {
    if (this->zip_ != nullptr) zip_close(this->zip_);
}

std::shared_ptr<std::istream> JarClassPath::findClass(const std::string &name) {
    zip_entry_open(this->zip_, name.c_str());

    void *buf = nullptr;
    size_t size = 0;
    if (zip_entry_read(this->zip_, &buf, &size) <= 0) {
        zip_entry_close(this->zip_);
        return nullptr;
    }

    std::shared_ptr<std::istream> stream = std::make_shared<ZipEntryIStream>(std::make_unique<ZipEntryStreamBuf>(buf, size));
    zip_entry_close(this->zip_);
    return stream;
}

} // namespace cjbp
