#include <Windows.h>
#include "windows_error_category.h"


class windows_defined_error_category : public std::error_category
{
public:
	/// <summary>
	/// カテゴリ名を得る。
	/// </summary>
	/// <returns>カテゴリ名</returns>
	const char* name() const noexcept override
	{
		return "Windows error";
	}

	/// <summary>
	/// evに対応するエラーコードを返す。
	/// </summary>
	/// <param name="ev">エラーコード</param>
	/// <returns>エラーメッセージ</returns>
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
/// Windowsで定義されたエラーのカテゴリを得る。
/// </summary>
/// <returns>エラーカテゴリを得る</returns>
const std::error_category& windows_error_category() {
	static windows_defined_error_category category;
	return category;
}

