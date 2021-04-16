#include "RootController.h"

#include "PlasmaMenuView.h"

#include <boost/algorithm/string.hpp>

#include <KIO/CommandLauncherJob>

bool ExecController::validPath(const char* path,
                               const char* method){
	if (std::string(path).find("/exec/") == 0 &&
	    strcmp("GET", method) == 0){
		std::vector<std::string> str_array;
	    char *newUrl = ((char*)path)+1;
	    boost::split(str_array, newUrl, boost::is_any_of("/"));
        while (str_array.at(str_array.size()-1).length()==0){
            str_array.pop_back();
        }
		if (str_array.size()==2){
           return true;
		}
	}
	return false;
}

void ExecController::createResponse(struct MHD_Connection* connection,
                                    const char* url,
									const char* method,
									const char* upload_data,
									size_t* upload_data_size,
									std::stringstream& response){
	PlasmaMenuView *menuList = new PlasmaMenuView();
	char *newUrl = ((char*)url)+1;
	std::vector<std::string> str_array;
	qDebug() << newUrl;
	boost::split(str_array, newUrl, boost::is_any_of("/"));
    while (str_array.at(str_array.size()-1).length()==0){
        str_array.pop_back();
    }
	// Iterate over the menu list to find the matching md5 - uid so we can execute it.
    PlasmaMenuEntry *menuItem = menuList->findEntryByUid(str_array[1].c_str(), KServiceGroup::root());
	qDebug() << menuItem->toString().c_str();

	QJsonObject mainObject;
	if (menuItem==0){
	    mainObject.insert("programExecute", "false");
	} else {
		QString execPath = menuItem->get("exec")->toString().c_str();
		QString args;
		QStringList argsList;
		auto firstIdx = execPath.toStdString().find_first_of(' ');
		if (firstIdx != std::string::npos){
		    args = execPath.toStdString().substr(firstIdx+1).c_str();
    		argsList=args.split(" ");
    		execPath = execPath.toStdString().substr(0, firstIdx).c_str();
		}
	    auto *job = new KIO::CommandLauncherJob(execPath, argsList);
        job->start();
        qDebug() << job->errorText();
        qDebug() << job->errorString();
        mainObject.insert("programExecute", "true");
	}
	QJsonDocument jsonDoc;
    jsonDoc.setObject(mainObject);
    response << jsonDoc.toJson().toStdString().c_str();
}