#include <Windows.h>
#include <stdexcept>
#include <cstring>
#include <system_error>
#include "windows_error_category.h"
#include "shmem.h"

void shmem::create(const char* name, uint32_t size) {
	if ((name == nullptr) || (size == 0)) {
		throw new std::invalid_argument("name or size is invalid.");
	}
	else {
		close();

		HANDLE mapfile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0u, size, name);
		if (mapfile == nullptr) {
			DWORD error = GetLastError();
			std::error_code ec(GetLastError(), windows_error_category());
			throw std::system_error(ec);
		}

		LPVOID buffer = MapViewOfFile(mapfile, FILE_MAP_ALL_ACCESS, 0, 0, size);
		if (buffer == nullptr) {
			DWORD error = GetLastError();
			CloseHandle(mapfile);
			std::error_code ec(GetLastError(), windows_error_category());
			throw std::system_error(ec);
		}
		memset(buffer, 0u, size);

		m_mapfile = mapfile;
		m_buffer = buffer;
		m_size = size;
	}
}

void shmem::open(const char* name, uint32_t size) {
	if ((name == nullptr) || (size == 0)) {
		throw new std::invalid_argument("name or size is invalid.");
	}
	else {
		HANDLE mapfile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);
		if (mapfile == nullptr) {
			DWORD error = GetLastError();
			std::error_code ec(GetLastError(), windows_error_category());
			throw std::system_error(ec);
		}

		LPVOID buffer = MapViewOfFile(mapfile, FILE_MAP_ALL_ACCESS, 0u, 0u, size);
		if (buffer == nullptr) {
			DWORD error = GetLastError();
			CloseHandle(mapfile);
			std::error_code ec(GetLastError(), windows_error_category());
			throw std::system_error(ec);
		}

		m_mapfile = mapfile;
		m_buffer = buffer;
		m_size = size;
	}
}

void shmem::close(void) {
	if (m_buffer != nullptr) {
		UnmapViewOfFile(m_buffer);
		m_buffer = NULL;
	}

	if (m_mapfile != nullptr) {
		CloseHandle(m_mapfile);
		m_mapfile = nullptr;
	}

	m_size = 0u;

	return;
}


uint32_t shmem::read(void* ptr, uint32_t offset, uint32_t length) const {
	if (ptr == nullptr) {
		throw new std::invalid_argument("name is nullptr.");
	}
	if (offset > this->size()) {
		throw new std::invalid_argument("offset is over range.");
	}

	uint32_t io_len;
	if (*this) {
		const uint8_t* rp = reinterpret_cast<const uint8_t*>(m_buffer);
		io_len = ((offset + length) > this->size())
			? this->size() - offset : length;
		memcpy(ptr, rp + offset, io_len);
	}
	else {
		io_len = 0u;
	}
	return io_len;
}

/// <summary>
/// 共有メモリから1バイト読み出す。
/// </summary>
/// <param name="offset">オフセット</param>
/// <returns>データ。読み出し範囲外の場合には-1</returns>
int32_t shmem::read(uint32_t offset) const {
	int32_t retval;
	if (offset < m_size) {
		retval = *(reinterpret_cast<const uint8_t*>(m_buffer) + offset);
	}
	else {
		retval = -1;
	}

	return retval;
}
uint32_t shmem::write(const void* ptr, uint32_t offset, uint32_t length) {
	if (ptr == nullptr) {
		throw new std::invalid_argument("name is nullptr.");
	}
	if (offset > this->size()) {
		throw new std::invalid_argument("offset is over range.");
	}

	uint32_t io_len;
	if (*this) {
		uint8_t* wp = reinterpret_cast<uint8_t*>(m_buffer);
		io_len = ((offset + length) > this->size())
			? this->size() - offset : length;
		memcpy(wp + offset, ptr, length);
	}
	else {
		io_len = 0u;
	}

	return io_len;
}
/// <summary>
/// 共有メモリに1バイト書き込む。
/// </summary>
/// <param name="offset">オフセット</param>
/// <param name="data">書き込むデータ</param>
void shmem::write(uint32_t offset, uint8_t data) {
	if (offset < m_size) {
		*(reinterpret_cast<uint8_t*>(m_buffer) + offset) = data;
	}
}
