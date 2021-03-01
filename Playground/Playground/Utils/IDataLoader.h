#ifndef INTERFACE_DATA_LOADER
#define INTERFACE_DATA_LOADER

#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <string>

#include "../FileUtils/IFile.h"
#include "../FileUtils/RawFile.h"

namespace MyUtils
{
	class IDataLoader
	{
	public:

		struct LoadedData
		{
			std::vector<uint8_t> rawData;
			int w;
			int h;
			int channelsCount;
		};

		struct FileHandle 
		{
			IFile * f;
			bool closeAtFinish;
		};

		std::vector<LoadedData> loaded;

		std::vector<FileHandle> files;
		std::thread thread;
		std::atomic<bool> finished;

		std::string dataName;

		std::function<void(IDataLoader * loader)> onFinishCallback;

		IDataLoader(const char * dataName) :
			finished(false),
			dataName(dataName),
			onFinishCallback(nullptr)
		{
		}

		virtual ~IDataLoader() = default;

		void AddFile(IFile * f, bool closeAtFinish)
		{
			FileHandle fh;
			fh.f = f;
			fh.closeAtFinish = closeAtFinish;
			this->files.push_back(fh);
		};

		bool AddFile(const char * fileName)
		{
			RawFile* rf = new RawFile(fileName);

			if (rf->IsOpened() == false)
			{
				delete rf;
				return false;
			}

			FileHandle fh;
			fh.f = rf;
			fh.closeAtFinish = true;
			this->files.push_back(fh);

			return true;

		};

		virtual void Start() = 0;
	};
}

#endif
