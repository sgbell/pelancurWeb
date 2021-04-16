#include "RootController.h"

#include <QStringList>

#include <kwindowinfo.h>
#include <kwindowsystem.h>
#include <netwm_def.h>
#include <KIO/CommandLauncherJob>

#include <signal.h>

#include <boost/algorithm/string.hpp>

#include "tpl_engine/Template.h"
#include "tpl_engine/TemplateParserException.h"
#include "tpl_engine/TemplateList.h"
#include "tpl_engine/TemplateString.h"

bool ActivityController::validPath(const char* path,
                               const char* method){
	if (std::string(path).find("/activity/") == 0 &&
	    strcmp("GET", method) == 0){
        return true;
	}
	return false;
}

void ActivityController::createResponse(struct MHD_Connection* connection,
                                        const char* url,
									    const char* method,
									    const char* upload_data,
									    size_t* upload_data_size,
									    std::stringstream& response){
 
    /*
     * https://api.kde.org/frameworks/kwindowsystem/html/classKWindowInfo.html#ae11206ba8ebc2f02f28b81361ab1f044
	 * https://api.kde.org/frameworks/kwindowsystem/html/classKWindowSystem.html#a797ecb85728494f897c2213cc548bbf7
     */

    std::unique_ptr<TemplateMap> taskList = std::unique_ptr<TemplateMap>(new TemplateMap());
    taskList->put("taskList", this->getCurrentTasks());
    qDebug() << taskList->toString().c_str();

    // Split path
    std::vector<std::string> str_array;
    char *newUrl = ((char*)url)+1;
    boost::split(str_array, newUrl, boost::is_any_of("/"));
    while (str_array.at(str_array.size()-1).length()==0){
        str_array.pop_back();
    }

    if (str_array.size()<=1){
        // If url is /activity with no subpath, this is the action

        std::unique_ptr<Template> tpl_instance;
    	try {
            QFile *templateFile = new QFile(QStringLiteral(":taskbar.html"));
            tpl_instance = std::unique_ptr<Template>(new Template(*templateFile));
    		//qDebug() << menuList->templateData()->toString().c_str();
        } catch (TemplateParserException *ex) {
            qDebug() << ex->message().c_str();
            exit(1);
        }
    	response << tpl_instance->compile(taskList.get())->c_str();
    } else if (str_array.size()==2){
        // activity/<pid>
        qDebug() << str_array[0].c_str() << str_array[1].c_str();
        // Iterate over the task list, to find the matching pid to get the wid to use in KWindowSystem::raiseWindow()
        QList<WId> windows = KWindowSystem::windows();
        bool taskFound=false;
        QJsonObject mainObject;
        mainObject.insert("activated", "false");
    	for (auto it = windows.cbegin(), end = windows.cend(); it != end; ++it){
    		if (KWindowSystem::hasWId((*it)) &&
                !taskFound){
    			KWindowInfo info((*it),
                                 NET::Property::WMPid,
                                 NET::Property2::WM2DesktopFileName | NET::Property2::WM2UserTime | NET::Property2::WM2AllowedActions | NET::Property2::WM2Activities);
                if (info.valid()){
                    if (std::to_string(info.pid())==str_array[1]){
                        mainObject.insert("pid",info.pid());
                        KWindowSystem::raiseWindow(*it);
                        mainObject.insert("activated", "true");
                    }
                }
            }
        }
        QJsonDocument jsonDoc;
        jsonDoc.setObject(mainObject);
        response << jsonDoc.toJson().toStdString().c_str();
    } else {
        // activity/(close|restart)/<pid>
        qDebug() << str_array[0].c_str() << str_array[1].c_str() << str_array[2].c_str();
        QJsonObject mainObject;
        mainObject.insert("programClose", "false");
        if (str_array[1]=="close"){
            // Iterate over the task list, to find the matching pid to get the wid to use in KWindowSystem::raiseWindow()
            QList<WId> windows = KWindowSystem::windows();
            bool taskFound=false;
        	for (auto it = windows.cbegin(), end = windows.cend(); it != end; ++it){
        		if (KWindowSystem::hasWId((*it)) &&
                    !taskFound){
        			KWindowInfo info((*it),
                                     NET::Property::WMPid,
                                     NET::Property2::WM2DesktopFileName | NET::Property2::WM2UserTime | NET::Property2::WM2AllowedActions | NET::Property2::WM2Activities);
                    if (info.valid()){
                        if (std::to_string(info.pid())==str_array[2]){
                            taskFound=true;
                            mainObject.insert("pid",info.pid());
                            // Kill program here, with the close signal
                            kill((pid_t)info.pid(), SIGTERM);
                            mainObject.insert("programClose", "true");
                        }
                    }
                }
            }
        } else if (str_array[1]=="restart"){
            // Get the Details of the program by pid.
            // Iterate over the task list, to find the matching pid to get the wid to use in KWindowSystem::raiseWindow()
            QList<WId> windows = KWindowSystem::windows();
            bool taskFound=false;
        	for (auto it = windows.cbegin(), end = windows.cend(); it != end; ++it){
        		if (KWindowSystem::hasWId((*it)) &&
                    !taskFound){
        			KWindowInfo info((*it),
                                     NET::Property::WMPid,
                                     NET::Property2::WM2DesktopFileName | NET::Property2::WM2UserTime | NET::Property2::WM2AllowedActions | NET::Property2::WM2Activities);
                    if (info.valid()){
                        if (std::to_string(info.pid())==str_array[2]){
                            // Lookup program because the DesktopFileName is not what I expected it to be..
                            // /proc/<pid>/cmdline
                            QString procPath = QString(std::string("/proc/"+str_array[2]+"/cmdline").c_str());
                            QFile execProcPath(procPath);
                            if (execProcPath.open(QIODevice::ReadOnly)){
                                QTextStream in(execProcPath.readAll());
                                bool execFinished=false;
                                QString execPath;
                                QString args;
                                while (!in.atEnd()){
                                    QString temp = in.read(1);
                                    if (temp.toStdString().c_str()[0]=='\u0000'){
                                        temp=" ";
                                        execFinished=true;
                                    }
                                    if (!execFinished){
                                        execPath = execPath+temp;
                                    } else {
                                        args = args+temp;
                                    }
                                }
                                args = args.trimmed();
                                qDebug() << execPath;
                                qDebug() << args;
                                QStringList argsList= args.split(" ");
                                QDir workingDir(QString(std::string("proc/"+str_array[2]+"/cwd").c_str()));
                                QString cwd = workingDir.canonicalPath();
                                qDebug() << "Current working dir: " << cwd;
                                // Kill the program.
                                kill((pid_t)info.pid(), SIGTERM);
                                // Relaunch the program.
                                auto *job = new KIO::CommandLauncherJob(execPath, argsList);
                                job->start();
                                qDebug() << job->errorText();
                                qDebug() << job->errorString();
                                mainObject.remove("programClose");
                                mainObject.insert("programRestart", "true");
                            }
                        }
                    }
                }
            }
        }
        QJsonDocument jsonDoc;
        jsonDoc.setObject(mainObject);
        response << jsonDoc.toJson().toStdString().c_str();
    }

}

TemplateList* ActivityController::getCurrentTasks(){
    TemplateList* taskList = new TemplateList();
        
    QList<WId> windows = KWindowSystem::windows();
	for (auto it = windows.cbegin(), end = windows.cend(); it != end; ++it){
		if (KWindowSystem::hasWId((*it))){
			KWindowInfo info((*it),
                             NET::Property::WMPid | NET::Property::WMName | NET::Property::WMVisibleName | NET::Property::WMState | NET::Property::WMDesktop,
                             NET::Property2::WM2DesktopFileName | NET::Property2::WM2UserTime | NET::Property2::WM2AllowedActions | NET::Property2::WM2Activities); // Need to specify the properties as a bitmask
            if (info.valid()){
                ApplicationDetails *taskInfo = new ApplicationDetails();
                taskInfo->setPid(info.pid());
                taskInfo->setName(info.visibleName().toStdString());
                // Better off looking up /proc/<pid>/cmdline to then find the related desktop entry
                taskInfo->setUri(info.desktopFileName().toStdString());
                if (((TemplateString*)taskInfo->get("uri"))->toString()!="org.kde.plasmashell" &&
                    ((TemplateString*)taskInfo->get("uri"))->toString()!="org.kde.pelancurWeb")
                   taskList->push(*taskInfo);
            }
        }
    }
    return taskList;
}