#pragma once
#include <Windows.h>
#include <string>

class shmem
{
private:
	HANDLE m_mapfile;
	LPVOID m_buffer;
	uint32_t m_size;

public:
	shmem(void) : m_mapfile(nullptr), m_buffer(nullptr), m_size(0u) {}
	~shmem(void) {
		close();
	}

	uint32_t size(void) const { return m_size; }

	/// <summary>
	/// 共有メモリを作成する。
	/// </summary>
	/// <param name="name">共有メモリ名</param>
	/// <param name="size">共有メモリサイズ</param>
	void create(const std::string& name, uint32_t size) {
		create(name.c_str(), size);
	}
	/// <summary>
	/// 共有メモリを作成する。
	/// </summary>
	/// <param name="name">共有メモリ名</param>
	/// <param name="size">共有メモリサイズ</param>
	void create(const char* name, uint32_t size);
	/// <summary>
	/// 既存の共有メモリをオープンする
	/// </summary>
	/// <param name="name">共有メモリ名</param>
	/// <param name="size">共有メモリサイズ</param>
	void open(const std::string& name, uint32_t size) {
		open(name.c_str(), size);
	}
	/// <summary>
	/// 既存の共有メモリをオープンする
	/// </summary>
	/// <param name="name">共有メモリ名</param>
	/// <param name="size">共有メモリサイズ</param>
	void open(const char* name, uint32_t size);
	/// <summary>
	/// 共有メモリをクローズする。
	/// </summary>
	void close(void);

	explicit operator bool(void) const { return m_buffer != nullptr; }

	/// <summary>
	/// 共有メモリから読み出す。
	/// </summary>
	/// <param name="ptr">バッファポインタ</param>
	/// <param name="offset">共有メモリ上のオフセット</param>
	/// <param name="length">読み出す長さ</param>
	/// <returns>読み出したバイト数</returns>
	uint32_t read(void* ptr, uint32_t offset, uint32_t length) const;

	/// <summary>
	/// 共有メモリから1バイト読み出す。
	/// </summary>
	/// <param name="offset">オフセット</param>
	/// <returns>データ。読み出し範囲外の場合には-1</returns>
	int32_t read(uint32_t offset) const;

	/// <summary>
	/// 共有メモリに書き込む。
	/// </summary>
	/// <param name="ptr">バッファポインタ</param>
	/// <param name="offset">共有メモリ上のオフセット</param>
	/// <param name="length">書き込む長さ</param>
	/// <returns>書き込むバイト数</returns>
	uint32_t write(const void* ptr, uint32_t offset, uint32_t length);

	/// <summary>
	/// 共有メモリに1バイト書き込む。
	/// </summary>
	/// <param name="offset">オフセット</param>
	/// <param name="data">書き込むデータ</param>
	void write(uint32_t offset, uint8_t data);

	shmem(const shmem& obj) = delete;
	shmem& operator=(const shmem& obj) = delete;
};

