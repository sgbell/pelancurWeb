#include "WebServer.h"
#include "Controller.h"
#include <QDebug>

MHD_Result WebServer::request_handler (void* cls,
                                struct MHD_Connection* connection,
								const char* url,
								const char* method,
								const char* version,
								const char* upload_data,
								size_t* upload_data_size,
								void** ptr){
	//std::cout << "Request: " << "url" << ", Method: " << method << std::endl;

	WebServer* server = (WebServer*)cls;

	Controller* controller = 0;
	for (int i=0; i< server->controllers.size(); i++){
		Controller *c = server->controllers.at(i);
		if (c->validPath(url, method)){
			controller = c;
			break;
		}
	}

	if (!controller){
		std::cout << "Path not found: " << url << method << std::endl;
		struct MHD_Response *response = MHD_create_response_from_buffer(0, 0, MHD_RESPMEM_PERSISTENT);
		return MHD_queue_response (connection, MHD_HTTP_NOT_FOUND, response);
	}

	return controller->handleRequest(connection, url, method, upload_data, upload_data_size);
}

WebServer::WebServer(int p){
	port = p;
	daemon = 0;
}

void WebServer::addController (Controller* controller){
	controllers.push_back(controller);
}

int WebServer::start() {
    std::cout << "Server Started" << std::endl;
	daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
	         port,
			 NULL,
			 NULL,
			 &request_handler,
			 this,
			 MHD_OPTION_END);

	if (!daemon)
	    return 1;

    running=true;

	return 0;
}

void WebServer::stop(){
	MHD_stop_daemon(daemon);
	running=false;
}

void WebServer::stopServer(){
	qDebug() << "Stop Server called";
	this->stop();
}

MHD_Result readGetParameters (void *cls, 
                              enum MHD_ValueKind kind,
                              const char *key,
	                          const char *value){
	
	((connparams_t*)cls)->insert(key,value);
    return MHD_YES;
}