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
	/// ���L���������쐬����B
	/// </summary>
	/// <param name="name">���L��������</param>
	/// <param name="size">���L�������T�C�Y</param>
	void create(const std::string& name, uint32_t size) {
		create(name.c_str(), size);
	}
	/// <summary>
	/// ���L���������쐬����B
	/// </summary>
	/// <param name="name">���L��������</param>
	/// <param name="size">���L�������T�C�Y</param>
	void create(const char* name, uint32_t size);
	/// <summary>
	/// �����̋��L���������I�[�v������
	/// </summary>
	/// <param name="name">���L��������</param>
	/// <param name="size">���L�������T�C�Y</param>
	void open(const std::string& name, uint32_t size) {
		open(name.c_str(), size);
	}
	/// <summary>
	/// �����̋��L���������I�[�v������
	/// </summary>
	/// <param name="name">���L��������</param>
	/// <param name="size">���L�������T�C�Y</param>
	void open(const char* name, uint32_t size);
	/// <summary>
	/// ���L���������N���[�Y����B
	/// </summary>
	void close(void);

	explicit operator bool(void) const { return m_buffer != nullptr; }

	/// <summary>
	/// ���L����������ǂݏo���B
	/// </summary>
	/// <param name="ptr">�o�b�t�@�|�C���^</param>
	/// <param name="offset">���L��������̃I�t�Z�b�g</param>
	/// <param name="length">�ǂݏo������</param>
	/// <returns>�ǂݏo�����o�C�g��</returns>
	uint32_t read(void* ptr, uint32_t offset, uint32_t length) const;

	/// <summary>
	/// ���L����������1�o�C�g�ǂݏo���B
	/// </summary>
	/// <param name="offset">�I�t�Z�b�g</param>
	/// <returns>�f�[�^�B�ǂݏo���͈͊O�̏ꍇ�ɂ�-1</returns>
	int32_t read(uint32_t offset) const;

	/// <summary>
	/// ���L�������ɏ������ށB
	/// </summary>
	/// <param name="ptr">�o�b�t�@�|�C���^</param>
	/// <param name="offset">���L��������̃I�t�Z�b�g</param>
	/// <param name="length">�������ޒ���</param>
	/// <returns>�������ރo�C�g��</returns>
	uint32_t write(const void* ptr, uint32_t offset, uint32_t length);

	/// <summary>
	/// ���L��������1�o�C�g�������ށB
	/// </summary>
	/// <param name="offset">�I�t�Z�b�g</param>
	/// <param name="data">�������ރf�[�^</param>
	void write(uint32_t offset, uint8_t data);

	shmem(const shmem& obj) = delete;
	shmem& operator=(const shmem& obj) = delete;
};

