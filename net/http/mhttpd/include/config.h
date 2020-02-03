#ifndef CONFIG_H
#define CONFIG_H

#define MHTTPD_VERSION_MAJOR 1
#define MHTTPD_VERSION_MINOR 0
#define MHTTPD_VERSION_PATCH 0
#define MHTTPD_VERSION_TWEAK 
#define MHTTPD_VERSION "1.0.0"

#include <functional>
#include <stdexcept>
#include <chrono>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <thread>
#include "helper.h"

struct configuration {
		configuration(in_addr_t in_addr, in_port_t in_port, size_t concurrency,
						std::chrono::milliseconds signal_timeout,
						std::chrono::milliseconds event_timeout) : m_concurrency(concurrency),
		m_signal_timeout(signal_timeout), 
		m_event_timeout(event_timeout) {
				addr(in_addr, in_port);
                openlog(NULL, LOG_PERROR, LOG_USER);
                //setlogmask(LOG_INFO | LOG_ERR);
		}
		configuration() : configuration(INADDR_ANY, 8000, std::thread::hardware_concurrency(),
						std::chrono::milliseconds(1000),
						std::chrono::milliseconds(500)
						) {}
        ~configuration() {
            closelog();
        }
		void addr(in_addr_t addr, in_port_t port) {
				memset(&m_addr, 0, sizeof(m_addr));
				m_addr.sin_family = AF_INET;
				m_addr.sin_addr.s_addr = htonl(addr);
				m_addr.sin_port = htons(port);
		}
		struct sockaddr_in addr() const {
				return m_addr;
		}
		void concurrency(size_t n) {
				if (n == 0) n = std::thread::hardware_concurrency();
				m_concurrency = n;
		}
		size_t concurrency() const {
				return m_concurrency;
		}
		void req_queue_size(size_t n) {
				require(n > 0, "invalid req_queue_size (> 0 required)");
				m_req_queue_size = n;
		}
		size_t req_queue_size() const {
				return m_req_queue_size;
		}
		void signal_timeout(std::chrono::milliseconds ms) {
				require(ms.count() > 0, "invalid signal_timeout (non-zero value required)");
		}
		struct timespec signal_timeout() const {
				size_t ms = m_signal_timeout.count();
				return timespec{static_cast<int>(ms/1000), static_cast<int>((ms%1000)*1000*1000)};
		}
		size_t event_timeout() const {
				return m_event_timeout.count();
		}
		private:
		struct sockaddr_in m_addr;
		size_t m_concurrency;
		size_t m_req_queue_size;
		std::chrono::milliseconds m_signal_timeout;
		std::chrono::milliseconds m_event_timeout;
};

#endif//CONFIG_H
