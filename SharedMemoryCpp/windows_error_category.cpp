#include <Windows.h>
#include "windows_error_category.h"


class windows_defined_error_category : public std::error_category
{
public:
	/// <summary>
	/// �J�e�S�����𓾂�B
	/// </summary>
	/// <returns>�J�e�S����</returns>
	const char* name() const noexcept override
	{
		return "Windows error";
	}

	/// <summary>
	/// ev�ɑΉ�����G���[�R�[�h��Ԃ��B
	/// </summary>
	/// <param name="ev">�G���[�R�[�h</param>
	/// <returns>�G���[���b�Z�[�W</returns>
	std::string message(int ev) const override {
		std::string message;
		LPVOID buf;

		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, (DWORD)(ev), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)(&buf), 0u, nullptr)) {

			message = std::string((const char*)(buf));

			LocalFree(buf);
		}
		else {
			message = "Unknown error code.";
		}

		return message;
	}
};


/// <summary>
/// Windows�Œ�`���ꂽ�G���[�̃J�e�S���𓾂�B
/// </summary>
/// <returns>�G���[�J�e�S���𓾂�</returns>
const std::error_category& windows_error_category() {
	static windows_defined_error_category category;
	return category;
}

