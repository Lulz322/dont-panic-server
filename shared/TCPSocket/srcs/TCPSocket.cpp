//
// Created by arch on 11/13/21.
//

#include "../headers/TCPSocket.h"
#include <functional>

void Session::handle_read(std::shared_ptr<Session> &s, const boost::system::error_code &err, size_t bytes_transferred) {
	static uint32_t transfered;
	if (!err) {
		transfered += bytes_transferred;
		_parent->push_message(std::string(data, bytes_transferred), s);
		memset(data, 0, max_length);
		socket.async_read_some(
				boost::asio::buffer(data, max_length),
				boost::bind(&Session::handle_read, this,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
	} else {
		_logger->error("err (recv): {0}", err.message());
		transfered = 0;
	}
}

void Session::start() {
	memset(data, 0, max_length);
	socket.async_read_some(
			boost::asio::buffer(data, max_length),
			boost::bind(&Session::handle_read, this,
						shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
}

tcp::socket &Session::get_socket() {
	return socket;
}

Session::~Session() {

}

Session::Session(TCPSocket * parent, boost::asio::io_service &ios)
		: _parent(parent), socket(ios) {
	_logger = spdlog::get("main")->clone("TCPSession");
	_logger->info("Session created");


}

void TCPSocket::push_message(const std::string& msg, std::shared_ptr<Session>& session) {
	std::lock_guard<decltype(buffer_MX)> lock(buffer_MX);
	socket_message * tmp = new socket_message(msg, session);

	buffer.emplace_back(tmp);
	buffer_MX.unlock();
//	update_am_i_free();
}

socket_message * TCPSocket::get_message() {
	std::lock_guard<decltype(buffer_MX)> lock(buffer_MX);
	if (!buffer.empty()){
		socket_message * front = buffer.front();
		buffer.pop_front();
		buffer_MX.unlock();
//		update_am_i_free();
		return front;
	}
	return nullptr;
}

void TCPSocket::handle_accept(std::shared_ptr<Session> session, const boost::system::error_code &err) {
	if (!err) {
		session->start();
		session = std::make_shared<Session>(this, ios);
		acceptor->async_accept(session->get_socket(),
							  boost::bind(&TCPSocket::handle_accept, this, session,
										  boost::asio::placeholders::error));
	}
	else {
		_logger->error("err: {0}", err.message());
		session.reset();
	}
}

TCPSocket::TCPSocket()
{
	_logger = spdlog::get("main")->clone("TCPSocket");
	_logger->info("TCPSocket object created");
	acceptor = nullptr;

	open_socket(1488, "test");
}



void TCPSocket::stop()
{
	acceptor->close();
}

void TCPSocket::open_socket(uint16_t port, const std::string & url) {
	acceptor =  new tcp::acceptor(ios, tcp::endpoint(tcp::v4(), port));
	_logger->info("TCPSocket opened on port {0}", port);
	std::shared_ptr<Session> session = std::make_shared<Session>(this, ios);
	acceptor->async_accept(session->get_socket(),
						  boost::bind(&TCPSocket::handle_accept, this,
									  session,
									  boost::asio::placeholders::error));
	std::thread([this](){
		ios.run();
	}).detach();

}

bool TCPSocket::disconnect_from_host() {
	_socket->close();
	delete _socket;
	return true;
}

bool TCPSocket::connect_to_host(std::string ip, uint16_t port) {

	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
	_socket = new tcp::socket(ios);
	_socket->connect(endpoint);

	_socket->send(boost::asio::buffer("Hello world"));
}

bool TCPSocket::send_data_to_host(const std::string & message) {
	return _socket->send(boost::asio::buffer(message.c_str(), message.size()));
}

bool TCPSocket::send_reply(std::shared_ptr<Session>& session, const std::string & message) {
	return session->get_socket().send(boost::asio::buffer(message.c_str(), message.size()));
}
