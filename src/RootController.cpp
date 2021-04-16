#include "RootController.h"

#include <time.h>
#include <cstring>
#include <string>

#include "PlasmaMenuView.h"
#include "tpl_engine/Template.h"
#include "tpl_engine/TemplateParserException.h"

bool RootController::validPath(const char* path,
                               const char* method){
	if (strcmp(path, "/") ==0 &&
	    strcmp("GET", method) ==0){
        return true;
	}
	return false;
}

void RootController::createResponse(struct MHD_Connection* connection,
                                    const char* url,
									const char* method,
									const char* upload_data,
									size_t* upload_data_size,
									std::stringstream& response){

	// Build the view we are going to send back
	PlasmaMenuView *menuList = new PlasmaMenuView();
	menuList->readMenuFolderInfo();
	std::unique_ptr<Template> tpl_instance;
	try {
        QFile *templateFile = new QFile(QStringLiteral(":main.html"));
        tpl_instance = std::unique_ptr<Template>(new Template(*templateFile));
		//qDebug() << menuList->templateData()->toString().c_str();
    } catch (TemplateParserException *ex) {
        qDebug() << ex->message().c_str();
        exit(1);
    }
	response << tpl_instance->compile(menuList->templateData())->c_str();
}