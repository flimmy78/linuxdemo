/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * COPYRIGHT NOTICE. 
 * Copyright (c), 2015 Lierda.
 * All rights reserved.
 * 
 * @file xmlrpc.
 * @brief 
 * 
 * @version 1.0
 * @author au
 * @date 2015/07/27-15:40:31
 * @email dongliang@lierda.com
 * 
 * @revision:
 *  - 1.0 2015/07/27 by au.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __XMLRPC_H_
#define __XMLRPC_H_
#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "xmlrpc-c/base.h"
#include "xmlrpc-c/server.h"
#include "xmlrpc-c/client.h"
#include "xmlrpc-c/server_abyss.h"
#include "xmlrpc-c/transport.h"
#include "curl/curl.h"

#include "debug.h"
#ifdef __cplusplus
extern "C" {
#endif

    namespace util {
#define DEFINE_XMLRPC_FUNC(name)                                        \
        static xmlrpc_value * name(                                     \
                                   xmlrpc_env *   const envP,           \
                                   xmlrpc_value * const paramArrayP,    \
                                   void *         const serverInfo,     \
                                   void *         const channelInfo)
#define METHOD_INFO(x) {#x, &(x)}
#define NAME "Xmlrpc-c Test Client"
#define VERSION "1.0"
        class XmlRpcServer {
        public:
            XmlRpcServer(int _port = 9090, 
                         struct xmlrpc_method_info3 const *methodInfo = NULL,
                         unsigned int methodCnt = 0);
            virtual ~XmlRpcServer();
            void start();
            void stop();
        private:
            int port;
            struct xmlrpc_method_info3 const *methodInfo;
            unsigned int methodCnt;
            volatile bool fStart;
        };
        class XmlRpcClient {
        private:
            typedef enum {
                my_false = 0,
                my_true = 1
            } my_bool;
            struct ___xmlrpc_client {
                my_bool myTransport;
                struct xmlrpc_client_transport *   transportP;
                struct xmlrpc_client_transport_ops transportOps;
                xmlrpc_dialect                     dialect;
                xmlrpc_progress_fn *               progressFn;
            };
            struct ___xmlrpc_client_transport {
                CURL * syncCurlSessionP;
            };
            static bool fault_occurred(xmlrpc_env * const envP) {
                if (envP->fault_occurred) {
                    return true;
                }
                return false;
            }
        public:
            XmlRpcClient(int _timeoutMs = 20000, int _conntimeoutMs = 5000);
            virtual ~XmlRpcClient();
            bool init();
            void free();
            bool call(string url, string func, string fmt, ...);
        private:
            int timeoutMs;
            int conntimeoutMs;
            xmlrpc_client *client;
        };

        XmlRpcServer::XmlRpcServer(int _port, 
            struct xmlrpc_method_info3 const *_methodInfo,
                                   unsigned int _methodCnt) :
        port(_port), methodInfo(_methodInfo), methodCnt(_methodCnt){
            fStart = false;
        }
        XmlRpcServer::~XmlRpcServer() {
            stop();
        }
        void XmlRpcServer::start() {
            if (methodCnt <= 0) {
                debug_print("No XmlRpc Server Functions!\n");
                return;
            }

            fStart = true;
            while (fStart) {
                xmlrpc_server_abyss_parms 		serverparm;
                xmlrpc_registry 				*registryP;
                xmlrpc_env 						env;
                unsigned int					i = 0;

                xmlrpc_env_init(&env);
                registryP = xmlrpc_registry_new(&env);

                for (i = 0 ; i < methodCnt; i++) {
                    xmlrpc_registry_add_method3(&env, registryP, &methodInfo[i]);
                }

                serverparm.config_file_name = NULL;
                serverparm.registryP        = registryP;
                serverparm.port_number      = port;
                serverparm.log_file_name    = "/dev/null";

                printf("Running XML-RPC server at %d ...\n", port);
                xmlrpc_server_abyss(&env, &serverparm, XMLRPC_APSIZE(log_file_name));
            }
        }
        void XmlRpcServer::stop() {
            fStart = false;
        }

        XmlRpcClient::XmlRpcClient(int _timeoutMs, int _conntimeoutMs):
        timeoutMs(_timeoutMs), conntimeoutMs(_conntimeoutMs){
            client = NULL;
        }
        XmlRpcClient::~XmlRpcClient() {
            if (client != NULL) {
                free();
            }
        }
        bool XmlRpcClient::init() {
            xmlrpc_env 							env;
            struct xmlrpc_client_transport 		*xct = NULL;
            struct ___xmlrpc_client_transport 	*xct1 = NULL;

            xmlrpc_env_init(&env);
            xmlrpc_client_setup_global_const(&env);
            if (fault_occurred(&env)) {
                debug_print("[%s error] : %s\n", 
                            __func__, 
                            env.fault_string);
                return false;
            }
            xmlrpc_client_create(&env, XMLRPC_CLIENT_NO_FLAGS, NAME,
                                 VERSION, NULL, 0, &client);
            if (fault_occurred(&env)) {
                debug_print("[%s error] : %s\n",
                            __func__, env.fault_string);
                return false;
            }
            xct = ((struct ___xmlrpc_client *)client)->transportP;
            xct1 = (struct ___xmlrpc_client_transport *)xct;
            curl_easy_setopt(xct1->syncCurlSessionP, 
                             CURLOPT_TIMEOUT_MS, timeoutMs);
            curl_easy_setopt(xct1->syncCurlSessionP, 
                             CURLOPT_CONNECTTIMEOUT_MS, conntimeoutMs);
            curl_easy_setopt(xct1->syncCurlSessionP, 
                             CURLOPT_NOSIGNAL, 1);
            xmlrpc_env_clean(&env);
            return true;
        }
        void XmlRpcClient::free() {
            xmlrpc_client_destroy(client);
            xmlrpc_client_teardown_global_const();            
        }
        bool XmlRpcClient::call(string url, string func, string fmt, ...) {
            xmlrpc_env 			env;
            xmlrpc_value 		*resultP 	= NULL;
            xmlrpc_int			ret;
            va_list 			args;

            if (client == NULL) {
                throw int(1);
                return false;
            }

            xmlrpc_env_init(&env);
            va_start(args, fmt);
            xmlrpc_client_call2f_va(&env, client, 
                                    url.c_str(),
                                    func.c_str(), 
                                    fmt.c_str(), 
                                    &resultP, args);
            va_end(args);
            if (fault_occurred(&env)) {
                debug_print("[%s error] : %s\n", __func__, env.fault_string);
                xmlrpc_env_clean(&env);
                return false;
            } 

            xmlrpc_read_int(&env, resultP, &ret);
            xmlrpc_DECREF(resultP);
            xmlrpc_env_clean(&env);

            if (ret != 0) {
                return false;
            }            
            return true;
        }
    };
#ifdef __cplusplus
}
#endif
#endif
