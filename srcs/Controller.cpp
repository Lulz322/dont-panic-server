//
// Created by arch on 9/7/21.
//

#include "Controller.h"
#include <memory>
#include <fstream>


Controller::Controller() {
	_thread_workers = 1;
	_port = 8000;
	_log_level = 1;
	_socket_threads = 1;


	uuid_t binuuid;
	uuid_generate_random(binuuid);

	char uuid[41];
	uuid_unparse(binuuid, uuid);
	_service_uuid = uuid;


	_logger = spdlog::get("main")->clone("Controller");
	_logger->info("Service id {0} created", _service_uuid);
}

Controller::~Controller() {
	_logger->info("Controller downed");
}

bool Controller::set_args(int argc, char **av) {
	std::vector<std::string> words;

	for (size_t i = 1; i < argc; ++i){
		words.emplace_back(av[i]);
	}

	for (size_t i = 0; i < words.size(); ++i)
	{
		if (words[i].find("-dbhost") != std::string::npos){
			if (words.size() >= i + 1)
				_db_hostname = words[++i];
		}else if (words[i].find("-dbuser") != std::string::npos){
			if (words.size() >= i + 1)
				_db_username = words[++i];
		}else if (words[i].find("-dbpass") != std::string::npos){
			if (words.size() >= i + 1)
				_db_userpass = words[++i];
		}else if (words[i].find("-dbbase") != std::string::npos){
			if (words.size() >= i + 1)
				_db_database = words[++i];
		}
	}

	return true;
}

void Controller::start() {
	boost::asio::io_context io_context;
	_socket = std::make_shared<TCPSocket>();

	_db = std::make_shared<CDBS>();
	_db->Connect(_db_hostname, _db_username, _db_userpass, _db_database);
	for (uint8_t i = 0; i < _thread_workers; ++i)
	{
		worker_threads.emplace_back(std::thread(&Controller::worker, this));
		worker_threads[worker_threads.size() - 1].detach();
	}

	io_context.run();
}

[[noreturn]] void Controller::worker() {
	socket_message * msg = nullptr;
	for (;;){
		while ((msg = _socket->get_message()) == nullptr)
			usleep(100);

		json cmd;
		info("[GET] {0}", msg->message);
		try {
			cmd = json::parse(msg->message);
			if (!cmd["cmd"].is_number_integer())
			{
				_logger->warn("Missing or invalid v, seq or service_id.[{0}]", msg->message);
				create_answer(msg->session, cmd["cmd"], -1, "Invalid json");
				continue;
			}

			uint32_t cmd_id = cmd["cmd"];
			switch (cmd_id) {
				case 10001:
					regUser(msg->session, cmd); break;
				case 10000:
					loginUser(msg->session, cmd); break;
				default:
					break;
			}
			
		}catch (...){

		}
		delete msg;
	}
}

void Controller::stop() {
	worker_threads.clear();
	socket_threads.clear();
}

void Controller::regUser(std::shared_ptr<Session>& s, const json & answer) {

	if (_db->db_reg_user(answer["data"]["login"], answer["data"]["password"]) == 0)
		create_answer(s, answer["cmd"], -1001, "User already exist");
	else
		create_answer(s, answer["cmd"], 0, "User successful registered");

}

void Controller::loginUser(std::shared_ptr<Session>& s, const json &answer) {
	if (_db->db_login_user(answer["data"]["login"], answer["data"]["password"]) == 0)
		create_answer(s, answer["cmd"], -1000, "Cannot find user w/ that user and pass");
	else
		create_answer(s, answer["cmd"], 0, "OK");
}

void Controller::create_answer(std::shared_ptr<Session>& s, uint32_t cmd_code, int ret_code, const std::string & msg) {
	json out;

	out["cmd"] = cmd_code;
	out["data"]["ret_code"] = ret_code;
	out["data"]["message"] = msg;

	_socket->send_reply(s, out.dump());
}

