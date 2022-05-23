#include <iostream>
#include "../headers/Controller.h"
#include <csignal>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
volatile sig_atomic_t flag = 0;
void my_function(int sig){ // can be called asynchronously
	flag = 1; // set flag
}

inline static void init_logger()
{
	spdlog::init_thread_pool(16384, 2);
	auto tp = spdlog::thread_pool();
	auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("vs_sc.log", 1024 * 1024 * 10, 3);
	std::vector<spdlog::sink_ptr> sinks{ stdout_sink, rotating_sink };
	auto logger = std::make_shared<spdlog::async_logger>("main", sinks.begin(), sinks.end(), tp, async_overflow_policy::block);
	register_logger(logger);
	set_default_logger(logger);
}

int main(int argc, char **av) {
	init_logger();
	signal(SIGINT, my_function);
	signal(SIGILL, my_function);
	signal(SIGTERM, my_function);
	Controller server;

	if (!server.set_args(argc, av))
		return EXIT_FAILURE;


	server.start();

	while (true) {
		if (flag){
			std::cout << "\nSigint exiting\n";
			server.stop();
			break;
		}
		usleep(1000);
	}

	return EXIT_SUCCESS;
}
