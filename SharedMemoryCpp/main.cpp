// ShardMemoryCpp.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include "shmem.h"

typedef std::vector<std::string> arg_t;


static bool AppRun;

struct command_entry {
	const std::string cmd;
	void (*cmd_proc)(const arg_t& args);
	const std::string description;

	command_entry(const char* cmd, void (*cmd_proc)(const arg_t& args), const char* description)
		: cmd(cmd), cmd_proc(cmd_proc), description(description)
	{
	}
};

static shmem Shmem;

static arg_t* split_token(const std::string& s, arg_t* args, const std::string& delim);
static void command_proc(const arg_t& args);
static void args_command(const arg_t& args);
static void quit_command(const arg_t& args);
static void help_command(const arg_t& args);
static void create_command(const arg_t& args);
static void open_command(const arg_t& args);
static void close_command(const arg_t& args);
static void read_command(const arg_t& args);
static void write_command(const arg_t& args);

static std::vector<command_entry> CommandEntries;

/// <summary>
/// アプリケーションのエントリポイント
/// </summary>
/// <returns>ステータス</returns>
int main()
{
	AppRun = true;

	CommandEntries.push_back(command_entry("create", create_command, "Create shared memory."));
	CommandEntries.push_back(command_entry("open", open_command, "Open shared memory."));
	CommandEntries.push_back(command_entry("close", close_command, "Close shared memory."));
	CommandEntries.push_back(command_entry("read", read_command, "Read from shared memory."));
	CommandEntries.push_back(command_entry("write", write_command, "Write onto shared memory."));
	CommandEntries.push_back(command_entry("args", args_command, "Print arguments."));
	CommandEntries.push_back(command_entry("help", help_command, "Print help message."));
	CommandEntries.push_back(command_entry("quit", quit_command, "Quit application."));
	CommandEntries.push_back(command_entry("q", quit_command, ""));

	while (AppRun) {
		std::cout << "> ";
		std::cout.flush();
		std::string line;
		if (std::getline(std::cin, line)) {
			arg_t args;
			split_token(line, &args, " \t\r\n");
			if (args.size() > 0) {
				command_proc(args);
			}
		}
	}

	return 0L;
}

/// <summary>
/// 文字列をトークン分割する。
/// </summary>
/// <param name="s">文字列</param>
/// <param name="args">トークンを格納するリスト</param>
/// <param name="delim">デリミタ</param>
/// <returns>argsが返る。</returns>
static arg_t* split_token(const std::string& s, arg_t* args, const std::string& delim) {
	if (args == nullptr) {
		throw std::invalid_argument("args is null.");
	}

	(*args).clear();

	size_t index = 0u;
	size_t length = s.length();
	while (index < length) {
		while ((index < length) // position is not tail?
			&& (delim.find(s[index]) != std::string::npos)) { // *it is delimiter?
			index++;
		}
		if (index >= length) {
			break;
		}

		size_t index_begin = index;
		while (index < length) { // position is not tail?
			char c = s[index];
			if (delim.find(c) != std::string::npos) { // *it is delimiter?
				break;
			}
			if (c == '"') { // is double quotation?
				size_t pair_pos = s.find('"', index + 1);
				if (pair_pos != std::string::npos) {
					index = pair_pos + 1;
				}
				else {
					index = length;
				}
			}
			else if (c == '\'') { // is single quotation?
				size_t pair_pos = s.find('\'', index + 1);
				if (pair_pos != std::string::npos) {
					index = pair_pos + 1;
				}
				else {
					index = length;
				}
			}
			else {
				index++;
			}
		}

		char top_char = s[index_begin];
		char tail_char = s[index - 1];
		size_t token_length = index - index_begin;
		if ((token_length >= 2u) && (top_char == '"') && (tail_char == '"')) {
			(*args).push_back(s.substr(index_begin + 1u, token_length - 2u));
		} else if ((token_length >= 2u) && (top_char == '\'') && (tail_char == '\'')) {
			(*args).push_back(s.substr(index_begin + 1u, token_length - 2u));
		}
		else {
			(*args).push_back(s.substr(index_begin, token_length));
		}
	}

	return args;
}

/// <summary>
/// コマンドを処理する。
/// </summary>
/// <param name="args">コマンド引数</param>
static void command_proc(const arg_t& args) {
	try {
		std::vector<command_entry>::const_iterator it = CommandEntries.begin();
		while (it != CommandEntries.end()) {
			if ((*it).cmd == args[0]) {
				(*it).cmd_proc(args);
				return;
			}
			it++;
		}
		std::cerr << "Unknown command : " << args[0] << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;


	}
}
/// <summary>
/// argsコマンドを処理する
/// </summary>
/// <param name="args">コマンド引数</param>
static void args_command(const arg_t& args) {
	size_t no = 0u;
	for (arg_t::const_iterator it = args.begin(); it != args.end(); it++) {
		std::cout << '[' << no << ']' << (*it) << std::endl;
		no++;
	}
}

/// <summary>
/// quitコマンドを処理する。
/// </summary>
/// <param name="args">コマンド引数</param>
static void quit_command(const arg_t& args) {
	AppRun = false;
	return;
}
/// <summary>
/// helpコマンドを処理する。
/// </summary>
/// <param name="args">コマンド引数</param>
static void help_command(const arg_t& args) {
	std::vector<command_entry>::const_iterator it = CommandEntries.begin();
	while (it != CommandEntries.end()) {
		if ((*it).description.length() > 0) {
			std::cout << std::setw(16) << (*it).cmd << " - " << (*it).description << std::endl;
		}
		it++;
	}
}
/// <summary>
/// createコマンドを処理する。
/// </summary>
/// <param name="args">コマンド引数</param>
static void create_command(const arg_t& args) {
	if (args.size() >= 3u) {
		uint32_t size = strtoul(args[2].c_str(), nullptr, 0u);
		Shmem.create(args[1], size);
	}
	else {
		std::cout << "usage:" << std::endl;
		std::cout << "  create name$ size#" << std::endl;
	}
}

/// <summary>
/// openコマンドを処理する。
/// </summary>
/// <param name="args">コマンド引数</param>
static void open_command(const arg_t& args) {
	if (args.size() >= 3u) {
		uint32_t size = strtoul(args[2].c_str(), nullptr, 0u);
		Shmem.open(args[1], size);
	}
	else {
		std::cout << "usage:" << std::endl;
		std::cout << "  open name$ size#" << std::endl;
	}
}

/// <summary>
/// closeコマンドを処理する。
/// </summary>
/// <param name="args">コマンド引数</param>
static void close_command(const arg_t& args) {
	Shmem.close();
}

/// <summary>
/// readコマンドを処理する。
/// </summary>
/// <param name="args">コマンド引数</param>
static void read_command(const arg_t& args) {
	if (!Shmem) {
		std::cerr << "Not opened." << std::endl;
	}
	else if (args.size() < 3u) {
		std::cout << "usage:" << std::endl;
		std::cout << "  read offset# length#" << std::endl;
	}
	else {
		uint32_t offset = strtoul(args[1].c_str(), nullptr, 0u);
		uint32_t length = strtoul(args[2].c_str(), nullptr, 0u);

		for (uint32_t i = 0u; i < length; i++) {
			int32_t data = Shmem.read(offset + i);
			if (data < 0L) {
				break;
			}
			std::cout << std::showbase << std::setfill('0') << std::hex << data << ' ';
		}
		std::cout << std::endl;
	}
}
/// <summary>
/// writeコマンドを処理する。
/// </summary>
/// <param name="args">コマンド引数</param>
static void write_command(const arg_t& args) {
	if (!Shmem) {
		std::cerr << "Not opened." << std::endl;
		return;
	}
	else if (args.size() < 3u) {
		std::cout << "usage:" << std::endl;
		std::cout << "  write offset# data1# [ data2# [ ... ] ]";
	}
	else {
		uint32_t offset = strtoul(args[1].c_str(), nullptr, 0u);
		
		for (uint32_t i = 2u; i < args.size(); i++) {
			uint8_t data = (uint8_t)(strtoul(args[i].c_str(), nullptr, 0u));
			Shmem.write(offset + i - 2u, data);
		}
	}
}

