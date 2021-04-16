#ifndef _CONTROLLER_
#define _CONTROLLER_

#include <sstream>
#include <microhttpd.h>
#include <cstring>

#include <QMap>
#include <QString>

/**
 *  Base controller for handing http requests
 */

typedef QMap<QString, QString> connparams_t;

MHD_Result readGetParameters (void *cls, 
                              enum MHD_ValueKind kind,
                              const char *key,
	                          const char *value);

class Controller {
	public:
	    /*
		 * Check if the given path and method are handled by this controller
		 */
	    virtual bool validPath(const char* path, const char* method) = 0;

		/*
		 * Handles the request
		 */
		virtual MHD_Result handleRequest(struct MHD_Connection *connection,
		                                 const char* url,
								         const char* method,
								         const char* upload_data,
								         size_t* upload_data_size) = 0;

        connparams_t m_getParams;
};

/**
 *  The dynamic controller is a controller for creating user defined pages.
 */
class DynamicController: public Controller {
	public:
	    virtual bool validPath(const char* path, const char* method) = 0;

		// User defined http Response.
		virtual void createResponse (struct MHD_Connection* connection,
		                             const char* url,
									 const char* method,
									 const char* upload_data,
									 size_t* upload_data_size,
									 std::stringstream& response) = 0;

		virtual MHD_Result handleRequest (struct MHD_Connection* connection,
		                                  const char* url,
       								      const char* method,
						       		      const char* upload_data,
								          size_t* upload_data_size) {
			std::stringstream response_string;
			int numGetVars = MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, &readGetParameters, &m_getParams);
			createResponse (connection, url, method, upload_data, upload_data_size, response_string);

			// Send response
			struct MHD_Response *response = MHD_create_response_from_buffer(strlen(response_string.str().c_str()),
			    (void*)response_string.str().c_str(), MHD_RESPMEM_MUST_COPY);
			MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
			MHD_destroy_response(response);

			return ret;
	   }
};

#endif