// from https://gist.github.com/malaggan/296c70c10165b29daf17bb2c8a49045f
// ---
// Reading a .csv.gz file using std::istream. Three streambuf implementations 
// are given, one that reads a file descriptor, one that spawns and reads a process, 
// and one that spawns gzip.
#pragma once

#include <streambuf>
#include <memory>
#include <string>
#include <vector>

// streambuf from an fd
class fd_streambuf : public std::streambuf
{
private:
  void init();
public:
  explicit fd_streambuf(int fd);
  
  fd_streambuf(fd_streambuf const &) = delete;
  fd_streambuf(fd_streambuf &&) = delete;
  fd_streambuf &operator=(fd_streambuf const &) = delete;
  fd_streambuf &operator=(fd_streambuf &&) = delete;
  
  ~fd_streambuf();
  
protected:
  explicit fd_streambuf();
  
  void set_fd(int fd);
  
protected:
  int_type underflow() override;
  
private:
  int fd_;
  static constexpr size_t put_back = 1;
  static constexpr size_t buffsize = 1024;
  std::unique_ptr<char[]> buffer;
};

// process -> fd
class process_streambuf : public fd_streambuf
{
public:
  explicit process_streambuf(std::string const &program_name, std::vector<std::string> const &args);
  
  process_streambuf(process_streambuf const &) = delete;
  process_streambuf(process_streambuf &&) = delete;
  process_streambuf &operator=(process_streambuf const &) = delete;
  process_streambuf &operator=(process_streambuf &&) = delete;
  
  ~process_streambuf();
};

// gzip -> process
class gzip_streambuf : public process_streambuf
{
public:
  explicit gzip_streambuf(std::string const & fname);
  
  gzip_streambuf(gzip_streambuf const &) = delete;
  gzip_streambuf(gzip_streambuf &&) = delete;
  gzip_streambuf &operator=(gzip_streambuf const &) = delete;
  gzip_streambuf &operator=(gzip_streambuf &&) = delete;
  
  ~gzip_streambuf();
};
