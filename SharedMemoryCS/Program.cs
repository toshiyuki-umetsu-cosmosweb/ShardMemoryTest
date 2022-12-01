using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SharedMemoryCS
{
    class Program
    {
        /// <summary>
        /// 実行状態
        /// </summary>
        private static bool AppRun { get; set; } = true;

        /// <summary>
        /// シェアドメモリ
        /// </summary>
        private static SharedMemory Shmem { get; set; } = new SharedMemory();

        /// <summary>
        /// コマンドエントリテーブル
        /// </summary>
        private static List<CommandEntry> CommandEntries { get; set; } = new List<CommandEntry>();

        static void Main(string[] args)
        {
            try
            {
                CommandEntries.Add(new CommandEntry("args", ArgsCommand, "Print arguments."));
                CommandEntries.Add(new CommandEntry("create", CreateCommand, "Create shared memory."));
                CommandEntries.Add(new CommandEntry("open", OpenCommand, "Open shared memory."));
                CommandEntries.Add(new CommandEntry("close", CloseCommand, "Create shared memory."));
                CommandEntries.Add(new CommandEntry("read", ReadCommand, "Read from shared memory."));
                CommandEntries.Add(new CommandEntry("write", WriteCommand, "Write onto shared memory."));
                CommandEntries.Add(new CommandEntry("help", HelpCommand, "Print help message."));
                CommandEntries.Add(new CommandEntry("quit", QuitCommand, "Quit application."));
                CommandEntries.Add(new CommandEntry("q", QuitCommand, string.Empty));
                while (AppRun)
                {
                    Console.Write("> ");
                    Console.Out.Flush();
                    var line = Console.ReadLine();
                    if (line == null)
                    {
                        break;
                    }

                    var cmdArgs = SplitTokens(line, " \t\r\n");
                    if (cmdArgs.Length > 0)
                    {
                        CommandProc(cmdArgs);
                    }
                }
            }
            finally
            {
                Shmem.Dispose();
            }
        }

        /// <summary>
        /// コマンドを実行する。
        /// </summary>
        /// <param name="args">コマンド引数</param>
        private static void CommandProc(string[] args)
        {
            try
            {
                var cmdEntry = CommandEntries.FirstOrDefault((entry) => entry.Command.Equals(args[0]));
                if (cmdEntry != null)
                {
                    cmdEntry.Invoke(args);
                }
                else
                {
                    Console.Error.WriteLine($"Command not found. :{args[0]}");
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine(e.Message);
            }
        }

        /// <summary>
        /// 行をトークン分割する。
        /// </summary>
        /// <param name="line">行</param>
        /// <param name="delim">デリミタ</param>
        /// <returns>分割されたトークン</returns>
        static string[] SplitTokens(string line, string delim)
        {
            var args = new List<string>();
            int index = 0;
            int length = line.Length;
            while (index < length)
            {
                while ((index < length) // Position is not tail?
                     && (delim.IndexOf(line[index]) != -1)) // Is delimiter
                {
                    index++;
                }
                if (index >= length)
                {
                    break;
                }

                int index_begin = index;
                while (index < length)
                {
                    char c = line[index];
                    if (delim.IndexOf(c) != -1) // Is delimiter
                    {
                        break;
                    }
                    if (c == '"') // is double quotation?
                    {
                        int pair_pos = line.IndexOf('"', index + 1);
                        if (pair_pos >= 0)
                        {
                            index = pair_pos + 1;
                        }
                        else
                        {
                            index = length;
                        }
                    }
                    else if (c == '\'') // Is single quotation?
                    {
                        int pair_pos = line.IndexOf('\'', index + 1);
                        if (pair_pos >= 0)
                        {
                            index = pair_pos + 1;
                        }
                        else
                        {
                            index = length;
                        }
                    }
                    else
                    {
                        index++;
                    }
                }
                char top_char = line[index_begin];
                char tail_char = line[index - 1];
                int token_length = index - index_begin;
                if ((token_length >= 2) && (top_char == '"') && (tail_char == '"'))
                {
                    args.Add(line.Substring(index_begin + 1, token_length - 2));
                }
                else if ((token_length >= 2) && (top_char == '\'') && (tail_char == '\''))
                {
                    args.Add(line.Substring(index_begin + 1, token_length - 2));
                }
                else
                {
                    args.Add(line.Substring(index_begin, token_length));
                }
            }

            return args.ToArray();
        }

        /// <summary>
        /// quitコマンドを処理する。
        /// </summary>
        /// <param name="args">コマンド引数</param>
        private static void QuitCommand(string[] args)
        {
            AppRun = false;
        }

        /// <summary>
        /// argsコマンドを処理する。
        /// </summary>
        /// <param name="args">コマンド引数</param>
        private static void ArgsCommand(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                Console.WriteLine($"[{i}]{args[i]}");
            }
        }

        /// <summary>
        /// helpコマンドを処理する。
        /// </summary>
        /// <param name="args">コマンド引数</param>
        private static void HelpCommand(string[] args)
        {
            foreach (var entry in CommandEntries)
            {
                if (!string.IsNullOrEmpty(entry.Description))
                {
                    Console.WriteLine($"{entry.Command} - {entry.Description}");
                }
            }
        }

        /// <summary>
        /// createコマンドを処理する。
        /// </summary>
        /// <param name="args">コマンド引数</param>
        private static void CreateCommand(string[] args)
        {
            if (args.Length == 3)
            {
                uint size = Convert.ToUInt32(args[2]);
                Shmem.Create(args[1], size);
            }
            else
            {
                Console.WriteLine($"usage:");
                Console.WriteLine($"  open name$ size#");
            }
        }

        /// <summary>
        /// openコマンドを処理する。
        /// </summary>
        /// <param name="args">コマンド引数</param>
        private static void OpenCommand(string[] args)
        {
            if (args.Length == 3)
            {
                uint size = Convert.ToUInt32(args[2]);
                Shmem.Open(args[1], size);
            }
            else
            {
                Console.WriteLine($"usage:");
                Console.WriteLine($"  open name$ size#");
            }
        }

        /// <summary>
        /// closeコマンドを処理する。
        /// </summary>
        /// <param name="args">コマンド引数</param>
        private static void CloseCommand(string[] args)
        {
            Shmem.Close();
        }

        /// <summary>
        /// readコマンドを処理する。
        /// </summary>
        /// <param name="args">コマンド引数</param>
        private static void ReadCommand(string[] args)
        {
            if (args.Length != 3)
            {
                Console.WriteLine("usage:");
                Console.WriteLine("  read offset# length#");
            }
            else
            {
                uint offset = Convert.ToUInt32(args[1]);
                uint length = Convert.ToUInt32(args[2]);

                for (uint i = 0; i < length; i++)
                {
                    int data = Shmem.Read(offset + i);
                    if (data >= 0)
                    {
                        Console.Write(data.ToString("X2"));
                        Console.Write(' ');
                    }
                    else
                    {
                        break;
                    }
                }
                Console.Out.WriteLine();
            }
        }

        /// <summary>
        /// writeコマンドを処理する。
        /// </summary>
        /// <param name="args">コマンド引数</param>
        private static void WriteCommand(string[] args)
        {
            if (args.Length < 3)
            {
                Console.WriteLine("usage:");
                Console.WriteLine("  write offset# data1 [ data2 [ ... ] ]");
            }
            else
            {
                uint offset = Convert.ToUInt32(args[1]);
                for (int i = 2; i < args.Length; i++)
                {
                    byte data = Convert.ToByte(args[i]);
                    Shmem.Write((uint)(offset + i - 2), data);
                }
            }
        }

        class CommandEntry
        {
            /// <summary>
            /// 新しいインスタンスを構築する。
            /// </summary>
            /// <param name="command">コマンド</param>
            /// <param name="procedure">処理</param>
            /// <param name="description">詳細説明</param>
            public CommandEntry(string command, Action<string[]> procedure, string description)
            {
                Command = command;
                Procedure = procedure;
                Description = description;
            }

            /// <summary>
            /// コマンド
            /// </summary>
            public string Command { get; private set; }
            /// <summary>
            /// ディスクリプション
            /// </summary>
            public string Description { get; private set; }
            /// <summary>
            /// プロシージャ
            /// </summary>
            private Action<string[]> Procedure { get; set; }

            /// <summary>
            /// 処理を実行する。
            /// </summary>
            /// <param name="args">コマンド引数</param>
            public void Invoke(string[] args)
                => Procedure?.Invoke(args);

        }
    }
}
