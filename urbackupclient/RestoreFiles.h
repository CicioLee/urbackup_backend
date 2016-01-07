#pragma once

#include "../Interface/Thread.h"
#include <string>
#include "../urbackupcommon/fileclient/FileClient.h"
#include "../Interface/File.h"
#include "../urbackupcommon/fileclient/tcpstack.h"
#include "../urbackupcommon/fileclient/FileClientChunked.h"
#include "../Interface/Database.h"
#include <memory>
#include <stack>

namespace
{
	class ScopedRestoreUpdater;
}

class RestoreFiles : public IThread, public FileClient::ReconnectionCallback, public FileClientChunked::ReconnectionCallback
{
public:
	RestoreFiles(int64 restore_id, int64 status_id, int64 log_id, std::string client_token, std::string server_token, std::string restore_path, bool single_file)
		: restore_id(restore_id), status_id(status_id), client_token(client_token), server_token(server_token), tcpstack(true), filelist_del(NULL), filelist(NULL),
		log_id(log_id), restore_path(restore_path), single_file(single_file)
	{

	}

	void operator()();

	virtual IPipe * new_fileclient_connection( );

	void log(const std::string& msg, int loglevel);

	std::string get_restore_path()
	{
		return restore_path;
	}

	bool is_single_file()
	{
		return single_file;
	}

private:
	
	bool connectFileClient(FileClient& fc);
	bool downloadFilelist(FileClient& fc);

	void restore_failed(FileClient& fc, THREADPOOL_TICKET metadata_dl);

	int64 calculateDownloadSize();

	bool downloadFiles(FileClient& fc, int64 total_size, ScopedRestoreUpdater& restore_updater);

	bool removeFiles( std::string restore_path, std::stack<std::vector<std::string> > &folder_files, std::vector<std::string> &deletion_queue );

	bool deleteFilesOnRestart(std::vector<std::string> &deletion_queue);

	bool deleteFileOnRestart(const std::string& fpath);

	bool deleteFolderOnRestart(const std::string& fpath);

	bool renameFilesOnRestart(std::vector<std::pair<std::string, std::string> >& rename_queue);

	std::auto_ptr<FileClientChunked> createFcChunked();

	int64 restore_id;

	int64 status_id;
	int64 log_id;

	IFile* filelist;
	ScopedDeleteFile filelist_del;

	std::string client_token;
	std::string server_token;

	CTCPStack tcpstack;

	IDatabase* db;

	std::string restore_path;
	bool single_file;
};