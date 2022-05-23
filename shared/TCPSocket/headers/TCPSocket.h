//
// Created by arch on 11/13/21.
//

#ifndef TC_TEST_SERVICE_TCPSOCKET_H
#define TC_TEST_SERVICE_TCPSOCKET_H


#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <sys/stat.h>
#include <fstream>

#include "spdlog/spdlog.h"
#include "spdlog/async.h"

#include <deque>

using boost::asio::ip::tcp;

class TCPSocket;


class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(TCPSocket * parent, boost::asio::io_service& ios);

	~Session();

	tcp::socket& get_socket();

	void start();

	void handle_read(std::shared_ptr<Session>& s,
					 const boost::system::error_code& err,
					 size_t bytes_transferred);

private:
	tcp::socket socket;
	enum { max_length = 1024 };
	char data[max_length];
	std::shared_ptr<spdlog::logger>	_logger;
	TCPSocket * _parent;
};

struct socket_message{

	socket_message(const std::string & msg, std::shared_ptr<Session>& s) :
		message(msg), session(s) {}
	~socket_message() {}

	std::string message;
	std::shared_ptr<Session>& session;
};

class TCPSocket {
public:
	TCPSocket();

	void handle_accept(std::shared_ptr<Session> session,
					   const boost::system::error_code& err);

	[[nodiscard]] socket_message * get_message();
	void push_message(const std::string& msg, std::shared_ptr<Session>& session);

	void stop();

	void open_socket(uint16_t port, const std::string & url);
	bool connect_to_host(std::string ip, uint16_t port);

	bool send_data_to_host(const std::string & message);
	bool disconnect_from_host();

	bool send_reply(std::shared_ptr<Session>& session, const std::string & message);
private:
	boost::asio::io_service ios;
	tcp::acceptor * acceptor;
	std::shared_ptr<spdlog::logger>	_logger;

	tcp::socket * _socket;

	std::mutex									buffer_MX;
	std::deque<socket_message *> 				buffer;
};


#endif //TC_TEST_SERVICE_TCPSOCKET_H
