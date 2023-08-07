// us
#include "streams.hh"

// std
#include <streambuf>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <unistd.h>

void fd_streambuf::init()
{
	setg(buffer.get() + buffsize, buffer.get() + buffsize, buffer.get() + buffsize);
}

fd_streambuf::fd_streambuf(int fd)
	: fd_{fd}, buffer{std::make_unique<char[]>(buffsize)}
{
	init();
}

fd_streambuf::fd_streambuf()
	: fd_{-1}, buffer{std::make_unique<char[]>(buffsize)}
{
	init();
}

void fd_streambuf::set_fd(int fd)
{
	fd_ = fd;
}

auto fd_streambuf::underflow() -> int_type
{
	assert(fd_ != -1);

	if (std::less<char*>()(gptr(), egptr())) // buffer not exhausted
		return traits_type::to_int_type(*gptr());

	char *base = buffer.get();
	char *start = base;

	if (eback() == base) // true when this isn't the first fill
		{
			std::memmove(base, egptr() - put_back, put_back);
			start += put_back;
		}

	int nbytes = read(fd_, start, buffsize - (start - base));
	if(nbytes == 0)
		return traits_type::eof();

	setg(base, start, start + nbytes);
	return traits_type::to_int_type(*gptr());
}

fd_streambuf::~fd_streambuf()
{
	close(fd_);
}

process_streambuf::process_streambuf(std::string const &program_name, std::vector<std::string> const &args)
{
	int readpipe[2];
	readpipe[0] = -1; /* child -> parent */
	readpipe[1] = -1;
	int pipestatus = pipe(readpipe);
	assert(0 == pipestatus);
	int const parent_read_fd = readpipe[0];
	int const child_write_fd = readpipe[1];
	int childpid = fork();
	if (childpid == 0)  /* in the child */
		{
			close(parent_read_fd);
			int grandchildpid = fork(); // create a grandchild
			if(grandchildpid == 0) /* in the grandchild */
				{
					// close stdout and stderr, and make them reference child_write_fd instead
					dup2(child_write_fd, 1);
					dup2(child_write_fd, 2);
					close(child_write_fd);
					char **args0 = new char*[args.size()+1]();
					for (size_t i = 0; i < args.size(); i++) {
						args0[i] = strdup(args[i].c_str());
					}
					args0[args.size()] = nullptr;
					execvp(program_name.c_str(), args0);
					// execlp should not return
					assert(false);
				}
			else if (grandchildpid > 0)
				{
					std::_Exit(EXIT_SUCCESS); // orphan the grandchild so init inherits it and we don't have to wait for it in order to avoid making it a zombie
				}
			else
				{
					std::_Exit(EXIT_FAILURE); // grapndchild creation failed
				}
		}
	else if (childpid > 0) /* in the parent */
		{
			close(child_write_fd);
			int status;
			//			wait(childpid, &status, 0);
			wait();
			if(status)
				fprintf(stderr, "Child process failed to create a %s process\n", program_name.c_str());
			// fd is ready now
			set_fd(parent_read_fd);
		}
	else
		{
			fprintf(stderr, "Cannot create a new child\n");
			assert(false);
		}
}

process_streambuf::~process_streambuf() = default;

gzip_streambuf::gzip_streambuf(std::string const & fname)
	: process_streambuf{"gzip",{"-q","-d","-c",fname}}
{}

gzip_streambuf::~gzip_streambuf() = default;
