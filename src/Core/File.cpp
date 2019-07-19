#include <Core/File.h>
#include <cassert>
#include <sstream>
#include <stdexcept>

namespace Donut
{

File::File():
    _file(nullptr) {}

File::File(const std::filesystem::path& path, FileMode mode):
    _file(nullptr)
{
	Open(path, mode);
}

File::~File()
{
	Close();
}

void File::Open(const std::filesystem::path& path, FileMode mode)
{
	Close();

#ifdef _WIN32
	_wfopen_s(&_file, path.wstring().c_str(), L"rb");
#else
	_file = std::fopen(path.c_str(), "rb");
#endif

	if (_file == nullptr) throw std::runtime_error("fopen failed");
}

void File::Close()
{
	if (_file != nullptr) std::fclose(_file);

	_file = nullptr;
}

void File::Seek(std::size_t position, FileSeekMode mode) const
{
	assert(_file != nullptr);
	std::fseek(_file, static_cast<long>(position), static_cast<int>(mode));
}

std::size_t File::Position() const
{
	assert(_file != nullptr);
	return static_cast<std::size_t>(std::ftell(_file));
}

std::size_t File::Size() const
{
	assert(_file != nullptr);

	std::size_t origPos = Position();
	Seek(0, FileSeekMode::End);

	std::size_t size = Position();
	Seek(origPos, FileSeekMode::Begin);

	return size;
}

void File::Flush()
{
	assert(_file != nullptr);
	std::fflush(_file);
}

std::string File::ReadAll(const std::filesystem::path& filename)
{
	const File file(filename, FileMode::Read);
	const std::size_t size = file.Size();

	char* buffer = new char[size];
	file.ReadBytes(buffer, size);
	return std::string(buffer, size);
}

std::string File::ReadString(std::size_t length)
{
	std::vector<char> str(length);
	ReadBytes(reinterpret_cast<uint8_t*>(str.data()), length);

	uint32_t l = 0;
	for (; l < length; ++l)
	{
		if (str[l] == 0) break;
	}

	return std::string(str.data(), l);
}

} // namespace Donut
