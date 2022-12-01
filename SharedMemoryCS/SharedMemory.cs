using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SharedMemoryCS
{
    public class SharedMemory : IDisposable
    {
        private System.IO.MemoryMappedFiles.MemoryMappedFile memoryMappedFile;

        private System.IO.MemoryMappedFiles.MemoryMappedViewAccessor viewAccessor;

        private uint shmSize;

        /// <summary>
        /// 新しいインスタンスを構築する。
        /// </summary>
        public SharedMemory()
        {
            memoryMappedFile = null;
            viewAccessor = null;
            shmSize = 0;
        }

        /// <summary>
        /// デストラクタ
        /// </summary>
        ~SharedMemory()
            => Dispose(false);

        /// <summary>
        /// オブジェクトのリソースを破棄する。
        /// </summary>
        /// <param name="isDisposing">Dispose()からの呼び出しの場合にはtrue, それ以外はfalse</param>
        private void Dispose(bool isDisposing)
        {
            if (isDisposing && (viewAccessor != null))
            {
                viewAccessor.Dispose();
                viewAccessor = null;
            }
            if (isDisposing && (memoryMappedFile != null))
            {
                memoryMappedFile.Dispose();
                memoryMappedFile = null;
            }
            shmSize = 0;
        }

        /// <summary>
        /// オブジェクトのリソースを破棄する。
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// 共有メモリを新規作成する。
        /// </summary>
        /// <param name="name">共有メモリ名</param>
        /// <param name="size">共有メモリサイズ</param>
        public void Create(string name, uint size)
        {
            Close();

            System.IO.MemoryMappedFiles.MemoryMappedFile mappedFile = null;
            System.IO.MemoryMappedFiles.MemoryMappedViewAccessor accessor = null;

            try
            {
                mappedFile = System.IO.MemoryMappedFiles.MemoryMappedFile.CreateNew(name, (long)(size));
                accessor = mappedFile.CreateViewAccessor();

                memoryMappedFile = mappedFile;
                viewAccessor = accessor;
                shmSize = size;
            }
            catch (Exception)
            {
                accessor?.Dispose();
                mappedFile?.Dispose();

                throw;
            }
        }
        
        /// <summary>
        /// 既存の共有メモリを開く。
        /// </summary>
        /// <param name="name">共有メモリ名</param>
        public void Open(string name, uint size)
        {
            Close();

            System.IO.MemoryMappedFiles.MemoryMappedFile mappedFile = null;
            System.IO.MemoryMappedFiles.MemoryMappedViewAccessor accessor = null;

            try
            {
                mappedFile = System.IO.MemoryMappedFiles.MemoryMappedFile.OpenExisting(name);
                accessor = mappedFile.CreateViewAccessor();

                memoryMappedFile = mappedFile;
                viewAccessor = accessor;
                shmSize = size;
            }
            catch (Exception)
            {
                accessor?.Dispose();
                mappedFile?.Dispose();
                throw;
            }
        }

        /// <summary>
        /// 共有メモリを閉じる
        /// </summary>
        public void Close()
        {
            viewAccessor?.Dispose();
            viewAccessor = null;
            memoryMappedFile?.Dispose();
            memoryMappedFile = null;
            shmSize = 0;
        }

        /// <summary>
        /// オープン/作成済みかどうかを取得する。
        /// </summary>
        public bool IsOpened {
            get => memoryMappedFile != null;
        }

        /// <summary>
        /// 領域サイズを得る。
        /// </summary>
        public uint Size {
            get => shmSize;
        }

        /// <summary>
        /// 読み出す。
        /// </summary>
        /// <param name="buffer">バッファ</param>
        /// <param name="offset">オフセット</param>
        /// <param name="length">長さ</param>
        /// <returns>読み出したバイト数</returns>
        public int Read(byte[] buffer, uint offset, uint length)
        {
            if (buffer == null)
            {
                throw new ArgumentException("buffer is null reference.");
            }
            if (offset > Size)
            {
                throw new ArgumentException("offset is overrange.");
            }

            int count;
            if (IsOpened)
            {
                uint io_length = ((offset + length) > Size)
                    ? (Size - offset) : length;
                count = viewAccessor.ReadArray(offset, buffer, 0, (int)(io_length));
            }
            else
            {
                count = -1;
            }
            return count;
        }

        /// <summary>
        /// 1バイト読み出す。
        /// </summary>
        /// <param name="offset">オフセット</param>
        /// <returns>読み出したデータ。読み出し失敗した場合には－1</returns>
        public int Read(uint offset)
        {
            if (IsOpened && (offset < Size))
            {
                return viewAccessor.ReadByte(offset);
            }
            else
            {
                return -1;
            }
        }

        /// <summary>
        /// 書き込む。
        /// </summary>
        /// <param name="data">データ</param>
        /// <param name="offset">オフセット</param>
        /// <param name="length">長さ</param>
        /// <returns>書き込んだバイト数</returns>
        public int Write(byte[] data, uint offset, uint length)
        {
            if (data == null)
            {
                throw new ArgumentException("data is specified null.");
            }
            if (offset > Size)
            {
                throw new ArgumentException("offset is overrange.");
            }

            int count;
            if (IsOpened)
            {
                uint io_length = ((offset + length) > Size)
                    ? (Size - offset) : length;
                viewAccessor.WriteArray(offset, data, 0, (int)(io_length));
                count = (int)(io_length);
            } 
            else
            {
                count = -1;
            }
            return count;
        }

        /// <summary>
        /// 1バイト書き込む。
        /// </summary>
        /// <param name="offset">オフセット</param>
        /// <param name="data">書き込むデータ</param>
        public void Write(uint offset, byte data)
        {
            if (IsOpened && (offset < Size))
            {
                viewAccessor.Write(offset, data);
            }
        }
    }
}
