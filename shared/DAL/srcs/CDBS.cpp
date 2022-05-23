#include "../headers/CDBS.h"
#include <iostream>


CDBS::CDBS()
	: m_pConnection(NULL)
{
	m_pLog = spdlog::get("main")->clone("DBS");
}

CDBS::~CDBS()
{
	if (m_pConnection != nullptr)
	{
		m_pLog->info("DB server close connection.");
		delete m_pConnection;
	}
}

int CDBS::Connect(std::string host, std::string user, std::string password, std::string db)
{
	if (m_pConnection != nullptr)
		Disconnect();
	
	m_host		= host;
	m_user		= user;
	m_password	= password;
	m_db		= db;
	
	try {
		m_pConnection = new soci::session(host, "dbname=" + db + " user=" + user + " password=" + password);
		
		if (m_pConnection->is_connected())
			m_pLog->info("DB server connected. (host[{0}], user[{1}])", host, user);

		return 0;
	}
	catch (std::exception &e) {
		m_pLog->error("Exception: {0}", e.what());
		return -1;
	}
}

int CDBS::Disconnect()
{
	try {
		delete m_pConnection;
		m_pConnection = nullptr;
	}catch (std::exception &e) {
		m_pLog->error("Exception: {0}");
		return -1;
	}
	return 0;
}


void CDBS::CheckConnection()
{
		if (m_pConnection != nullptr)
		{
			if (!m_pConnection->is_connected())
				m_pConnection->reconnect();
		}


}

uint32_t CDBS::db_login_user(const std::string &username, const std::string &password) {
	CheckConnection();
	SERIAL id = 0;
	try {
		*m_pConnection << "select id from users where username = '" << username <<
			"' AND password = '" << password << "'", soci::into(id);


	}catch (const std::exception &e){
		m_pLog->error("Exception: {0}", e.what());
	}

	return id;
}

uint32_t CDBS::db_reg_user(const std::string &username, const std::string &password) {
	CheckConnection();
	uint32_t id = 0;
	try {
		id = db_login_user(username, password);
		if (id == 0)
		{
			*m_pConnection << "insert into users(username, password) values(:username, :password) RETURNING id",
					soci::use(username),  soci::use(password), soci::into(id);
		} else{
			return 0;
		}


	}catch (const std::exception &e){
		m_pLog->error("Exception: {0}", e.what());
	}

	return id;
}

