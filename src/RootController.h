#include "Controller.h"

#include <QtCore>
#include "tpl_engine/TemplateMap.h"
#include "tpl_engine/TemplateList.h"

class RootController: public DynamicController {
	public:
	    virtual bool validPath (const char* path,
	                            const char* method);
		virtual void createResponse (struct MHD_Connection* connection,
		                             const char* url,
									 const char* method,
									 const char* upload_data,
									 size_t* upload_data_size,
									 std::stringstream &response);
};

class ExecController: public DynamicController {
	public:
	    virtual bool validPath (const char* path,
	                            const char* method);
		virtual void createResponse (struct MHD_Connection* connection,
		                             const char* url,
									 const char* method,
									 const char* upload_data,
									 size_t* upload_data_size,
									 std::stringstream &response);
};

class AssetsController: public DynamicController {
	public:
	    virtual bool validPath (const char* path,
	                            const char* method);
		virtual void createResponse (struct MHD_Connection* connection,
		                             const char* url,
									 const char* method,
									 const char* upload_data,
									 size_t* upload_data_size,
									 std::stringstream &response);
};

class ActivityController: public DynamicController {
	public:
	    virtual bool validPath (const char* path,
	                            const char* method);
		virtual void createResponse (struct MHD_Connection* connection,
		                             const char* url,
									 const char* method,
									 const char* upload_data,
									 size_t* upload_data_size,
									 std::stringstream &response);

        TemplateList* getCurrentTasks ();
};

class ApplicationDetails : public TemplateMap {
	public:
	    ApplicationDetails() {

		}
	    void setPid(int _pid){ this->put("pid", std::to_string(_pid)); }
	    void setName(std::string _name){ this->put ("name", _name); }
	    void setUri(std::string _uri){ this->put("uri", _uri); }
};