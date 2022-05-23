//
// Created by arch on 9/7/21.
//

#ifndef NEW_SC_SERVER_H
#define NEW_SC_SERVER_H

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <uuid/uuid.h>


#include <spdlog/spdlog.h>
#include <json.hpp>
#include <DAL/CDBS.h>

#include <tcpsocket/TCPSocket.h>

#define TOPIC_OUT "sc/out"

using namespace spdlog;
using json = nlohmann::json;

class Controller {
public:
	Controller();
	~Controller();

	bool set_args(int argc, char **av);
	void start();
	void stop();
	void regUser(std::shared_ptr<Session>&, const json & answer);
	void loginUser(std::shared_ptr<Session>&, const json & answer);
	void create_answer(std::shared_ptr<Session>&, uint32_t cmd_code, int ret_code, const std::string & msg = "");

private:
	[[noreturn]] void worker();

//CMD FUNCTIONS
private:


private:

	uint16_t						_port;
	//DB
	std::string						_db_hostname;
	std::string						_db_username;
	std::string						_db_userpass;
	std::string						_db_database;


	uint8_t							_log_level;
	uint8_t							_thread_workers;
	uint8_t 						_socket_threads;

	std::shared_ptr<TCPSocket>		_socket;

	std::vector<std::thread>		socket_threads;
	std::vector<std::thread>		worker_threads;

	std::shared_ptr<CDBS>			_db;
	std::string						_service_uuid;

	std::shared_ptr<spdlog::logger>	_logger;
};


#endif //NEW_SC_SERVER_H
