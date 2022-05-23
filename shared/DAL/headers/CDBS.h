#pragma once

class CMainModule;
class CIPC;

#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include "nlohmann/json.hpp"
#include <pqxx/pqxx> 
#include <set>

#include <soci/soci.h>
#include <soci/session.h>

#define SERIAL long long

// using namespace soci;
using json = nlohmann::json;


class CDBS
{
public:
	CDBS();
	~CDBS();
	int Connect(std::string host, std::string user, std::string password, std::string db);
	int Disconnect();

	uint32_t db_login_user(const std::string & username, const std::string & password);
	uint32_t db_reg_user(const std::string & username, const std::string & password);

private:

	
	//Driver*					m_pDriver;
	// pqxx::connection*         m_pConnection;

	soci::session  * m_pConnection;

	std::shared_ptr<spdlog::logger>	m_pLog;
	
	void CheckConnection();
	
	std::string m_host; //postgres
	std::string m_user;
	std::string m_password;
	std::string m_db;

};

