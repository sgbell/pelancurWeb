#include "RootController.h"

#include <QFile>

bool AssetsController::validPath(const char* path,
                               const char* method){
	if (std::string(path).find("/assets/") == 0 &&
	    strcmp("GET", method) == 0){
		std::string str_url = std::string(path);
        str_url.replace(0,1,":");
	    QFile *file = new QFile(QString(str_url.c_str()));
	    if (file->exists()){
            return true;
		}
	}
	return false;
}

void AssetsController::createResponse(struct MHD_Connection* connection,
                                      const char* url,
									  const char* method,
									  const char* upload_data,
									  size_t* upload_data_size,
									  std::stringstream& response){

	std::string str_url = std::string(url);
    str_url.replace(0,1,":");
    url = str_url.c_str();
	QFile *file = new QFile(QString(url));
	if (file->exists()){
	    file->open(QIODevice::ReadOnly);
		response << file->readAll().toStdString();
	}
}