/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * COPYRIGHT NOTICE. 
 * Copyright (c), 2015 Lierda.
 * All rights reserved.
 * 
 * @file dbuscxx.
 * @brief 
 * 
 * @version 1.0
 * @author au
 * @date 2015/05/22-12:29:01
 * @email dongliang@lierda.com
 * 
 * @revision:
 *  - 1.0 2015/05/22 by au.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __DBUSCXX_H_
#define __DBUSCXX_H_

#include <iostream>

#include "debug.h"
#include "dbus/dbus.h"

//#ifdef __cplusplus
//extern "C" {
//#endif
namespace util {
	class DBusConn {
		public:
			DBusConn(std::string _addr="unix:path=/var/run/dbus/system_bus_socket", 
					std::string _well="dbus.default.app");
			virtual ~DBusConn();

			bool init();
			void free();
			bool addMatch(std::string _match);
			void delMatch(std::string _match);

			DBusMessage *recvMsg(int _timeoutMs) ;
			bool sendMsg(DBusMessage *_msg) ;

			DBusMessage *sendWithReplyAndBlock(DBusMessage *_msg, int _timeoutMs) ;

		private:
			std::string addr;
			std::string well;
			DBusConnection *conn;
	};

	DBusConn::DBusConn(std::string _addr, std::string _well) : addr(_addr), well(_well) {conn = NULL;}
	DBusConn::~DBusConn() {}
	bool DBusConn::init() {
		DBusError err;

		if (conn != NULL) return true;

		dbus_error_init(&err);
		conn = dbus_connection_open_private(addr.c_str(), &err);
		if (conn == NULL) {
			debug_print("[%s]: %s\n", __func__, err.message);
			dbus_error_free(&err);
			return false;
		}

		if (!dbus_bus_register(conn, &err)) {
			debug_print("[%s]: %s\n", __func__, err.message);
			dbus_error_free(&err);
			free();
			return false;
		}

		if (dbus_bus_request_name(conn, well.c_str(), 0, &err) == -1) {
			debug_print("[%s]: %s\n", __func__, err.message);
			dbus_error_free(&err);
			free();
			return false;	
		}
		return true;
	}
	void DBusConn::free() {
		if (conn != NULL) {
			dbus_connection_close(conn);
			conn = NULL;
		}
	}
	bool DBusConn::addMatch(std::string _match) {
		DBusError err;
		dbus_error_init(&err);
		if (conn == NULL) {
			return false;
		}
		dbus_bus_add_match(conn, _match.c_str(), &err);
		if (dbus_error_is_set(&err)) {
			debug_print("[%s]: %s\n", __func__, err.message);
			dbus_error_free(&err);
			return false;
		}
		return true;
	}
	void DBusConn::delMatch(std::string _match) {
		DBusError err;
		dbus_error_init(&err);
		if (conn == NULL) {
			return;
		}
		dbus_bus_remove_match(conn, _match.c_str(), &err);
		if (dbus_error_is_set(&err)) {
			debug_print("[%s]: %s\n", __func__, err.message);
			dbus_error_free(&err);
			return;
		}
		return;     
	}
	DBusMessage * DBusConn::recvMsg(int _timeoutMs) {
		if (conn == NULL) throw int(0);
		if (dbus_connection_read_write(conn, _timeoutMs)) {
			DBusMessage *msg = dbus_connection_pop_message(conn);
			return msg;
		} else {
			throw int(0);
		}
		return NULL;
	}
	bool DBusConn::sendMsg(DBusMessage *_msg) {
		if (conn == NULL) throw int(0);
		if (dbus_connection_send(conn, _msg, NULL)) return true;
		return false;
	}

	DBusMessage *DBusConn::sendWithReplyAndBlock(DBusMessage *_msg, int _timeoutMs)  {
		if (conn == NULL) throw(0);
		DBusError err;
		dbus_error_init(&err);
		DBusMessage *reply = dbus_connection_send_with_reply_and_block(conn, _msg, _timeoutMs, &err);
		if (reply == NULL) {
			debug_print("[%s] : %s\n", __func__, err.message);
			dbus_error_free(&err);
			return NULL;
		}
		return reply;
	}
};

//#ifdef __cplusplus
//}
//#endif

#endif
