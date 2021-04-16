/**
 * @author      : Sam Bell (sam@silverband.com.au)
 * @file        : Template
 * @created     : Thursday Feb 20, 2020 12:12:06 AEDT
 */

#include <boost/algorithm/string.hpp>
#include <QFile>
#include <QBuffer>
#include <QDebug>

#include "Template.h"
#include "TemplateList.h"
#include "TemplateString.h"
#include "TemplateParserException.h"
#include "TemplateExprs.h"
#include <exception>
#include <regex>
#include <ctype.h>

#ifndef DEBUG_ALL
#define DEBUG_ALL 0
#endif

#ifndef DEBUG_FOR
#if DEBUG_ALL==1
    #define DEBUG_FOR 1
#else
    #define DEBUG_FOR 0
#endif
#endif

#ifndef DEBUG_IF
#if DEBUG_ALL==1
    #define DEBUG_IF 1
#else
    #define DEBUG_IF 0
#endif
#endif

#ifndef DEBUG_INCLUDE
#if DEBUG_ALL==1
#define DEBUG_INCLUDE 1
#else
#define DEBUG_INCLUDE 0
#endif
#endif

#ifndef DEBUG_TPLTEXT
#if DEBUG_ALL==1
#define DEBUG_TPLTEXT 1
#else
#define DEBUG_TPLTEXT 0
#endif
#endif

Template::Template() {
    m_exprList->emplace("==", new TemplateExpr::Equals());
    m_exprList->emplace("!=", new TemplateExpr::NotEquals());
    m_exprList->emplace(">", new TemplateExpr::GreaterThan());
    m_exprList->emplace(">=", new TemplateExpr::GreaterThanEquals());
    m_exprList->emplace("<", new TemplateExpr::LessThan());
    m_exprList->emplace("<=", new TemplateExpr::LessThanEquals());
    
}

Template::Template(std::string pathName) {
    std::unique_ptr<std::ifstream> fileStream = std::unique_ptr<std::ifstream>(new std::ifstream(pathName));
    if (!fileStream->good()) throw new TemplateParserException("Can not open file: "+pathName);
    if (fileStream->is_open()){
        setTemplate(*fileStream);
    }
    size_t directory = pathName.find_last_of("/");
    m_tplPath = pathName.substr(0,directory+1);
    m_exprList->emplace("==", new TemplateExpr::Equals());
    m_exprList->emplace("!=", new TemplateExpr::NotEquals());
    m_exprList->emplace(">", new TemplateExpr::GreaterThan());
    m_exprList->emplace(">=", new TemplateExpr::GreaterThanEquals());
    m_exprList->emplace("<", new TemplateExpr::LessThan());
    m_exprList->emplace("<=", new TemplateExpr::LessThanEquals());
}

Template::Template(QFile &fileObject){
    if (!fileObject.open(QIODevice::ReadOnly | QIODevice::Text)) throw new TemplateParserException("Failed to read from file");
    m_templateString="";
    while (!fileObject.atEnd()){
        QByteArray line = fileObject.readLine();
        m_templateString+=std::string(line.data());
    }
    isQFile=true;

    m_exprList->emplace("==", new TemplateExpr::Equals());
    m_exprList->emplace("!=", new TemplateExpr::NotEquals());
    m_exprList->emplace(">", new TemplateExpr::GreaterThan());
    m_exprList->emplace(">=", new TemplateExpr::GreaterThanEquals());
    m_exprList->emplace("<", new TemplateExpr::LessThan());
    m_exprList->emplace("<=", new TemplateExpr::LessThanEquals());
}

void Template::setTemplate(std::ifstream &fileStream){
    m_templateString="";
    char newEventType = '\0';
    while (fileStream.good()){
        try {
            fileStream.get(newEventType);
            m_templateString+=(char)newEventType;
        } catch (std::exception &e) {
            throw new TemplateParserException("Failed to read from file");
        }
    }
}

std::string Template::trim(std::string &input){
    boost::trim(input);
    bool is_blank = true;
    for (auto i=0; i < input.size(); i++){
        if (!(isblank(input[i]) || input[i]=='\n')) is_blank=false;
    }
    if (is_blank) input="";
   
    return input;
}

std::string* Template::compile(TemplateObject *templateData) {

    std::string *compiledText = new std::string(m_templateString);
    size_t pos = 0;
   
    //debug_verbose(std::string("Template Data: "+(templateData->toString())));
    //debug_verbose(std::string("Template\n========\n"+(m_templateString)));
 
    while (true) {
        // Hit play list button in interface, and then try to work out why below is sucking on the first tag in the subtemplate of the for loop
        std::regex tag_search("(\\{\\%.*?\\%\\})");
        std::smatch tag_match;
        std::regex_search(*compiledText, tag_match, tag_search);
        if (tag_match.size()<=0){
            size_t tpl_begin = compiledText->find("{%", pos);
            if (tpl_begin == std::string::npos){
                break;
            }
            size_t tpl_end = compiledText->find("%}", tpl_begin);
            if (tpl_end == std::string::npos){
                throw new TemplateParserException("Invalid template error starts at position:"+tpl_begin);
            }
        }

        size_t tpl_pos = compiledText->find(tag_match[0], pos);

        std::string tag = tag_match[0];
        std::regex remove_start_tag ("^\\{\\%");
        std::string tag_expression = std::regex_replace (tag, remove_start_tag,"");
        std::regex remove_end_tag ("\\%\\}$");
        tag_expression = std::regex_replace (tag_expression, remove_end_tag,"");
        boost::trim(tag_expression);

        if (DEBUG_ALL==1) qDebug().noquote() << __FILE__ << "tag:" << QString::fromStdString(tag) << " - tag_expression: \"" << QString::fromStdString(tag_expression) << "\"";

        std::vector<std::string> str_array;
        boost::split(str_array, tag_expression, boost::is_any_of(" "));
        if (str_array[0].compare("for")==0){
            std::unique_ptr<std::string> forOutput = std::unique_ptr<std::string>(new std::string(""));
            if (DEBUG_FOR==1) qDebug().noquote() << __FILE__ << __LINE__ << "For loop";
            size_t end_for = compiledText->find("{% endfor %}", tpl_pos+tag.length());
            // look for internal loops
            std::unique_ptr<std::string> internal_string = std::unique_ptr<std::string>(new std::string(compiledText->substr(tpl_pos+tag.length(), end_for-tpl_pos-tag.length())));
            if (DEBUG_FOR==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*internal_string);
            std::regex for_regex ("% for ");
            std::smatch results;
            std::regex_search (*internal_string, results, for_regex);
            if (results.size()>0){
            // need to make this a function that calls itself to jump through every nested loop
                for (int endForCount=0; endForCount<results.size(); endForCount++){
                    end_for = compiledText->find("{% endfor %}", end_for+12);
                    internal_string = std::unique_ptr<std::string>(new std::string(compiledText->substr(tpl_pos+tag.length()+7, end_for-tpl_pos-tag.length()-7)));
                }
            }
            boost::trim(*internal_string);
            if (DEBUG_FOR==1) qDebug().noquote() << __FILE__ << __LINE__ << "----" << QString::fromStdString(*internal_string) << "----";
            // Need to grab the item we are iterating over and pull it from the map
            std::unique_ptr<Template> tpl = std::unique_ptr<Template>(new Template());
            tpl->setTemplate(*internal_string);
            tpl->setPath(m_tplPath);
            std::string tpl_val;
            TemplateObject *childData = NULL;
            if (DEBUG_FOR==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(templateData->toString());
            if (DEBUG_FOR==1) qDebug().noquote() << __FILE__ << __LINE__ << "trying to get: " << QString::fromStdString(tag);
            bool fail=false;
            if (str_array.size()==2){
                childData = (dynamic_cast<TemplateMap*>(templateData)->get(str_array[1]));
            } else if (str_array.size()==4 &&
                       ((TemplateMap*)templateData)->exists(str_array[3])) {
                childData = ((TemplateMap*)templateData)->get(str_array[3]);
            } else {
                fail=true;
            }
            if (!fail){
                // Duh, need to iterate over the TemplateList that is childData
                if (childData->type()=="TemplateList"){
                    for (auto data_pos=(dynamic_cast<TemplateList*>(childData))->begin(); data_pos!=(dynamic_cast<TemplateList*>(childData))->end(); data_pos++){
                        if (DEBUG_FOR==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString((*data_pos)->toString());
                        TemplateObject *currentChild = 0;
                        if (str_array.size()==4){
                            currentChild = new TemplateMap();
                            ((TemplateMap*)currentChild)->put(std::string(str_array[1]), *data_pos);
                        } else {
                           currentChild = *data_pos;
                        }
                        // debug_verbose(currentChild->toString());
                        *forOutput+=*tpl->compile(currentChild);
                    }
                } else if (childData->type()=="TemplateMap"){
                   // debug_verbose(""+childData->toString());
                    *forOutput+=*tpl->compile(childData);
                }
            }
            trim(*forOutput);
            if (DEBUG_FOR==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*forOutput);
            if (DEBUG_FOR==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*compiledText);
            int endForLength = compiledText->find("%}", end_for);
            compiledText->replace(tpl_pos, endForLength-tpl_pos+2, *forOutput);
            if (DEBUG_FOR==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*compiledText);
        } else if (str_array[0].compare("if")==0){
            if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << "If statement";

            str_array.erase (str_array.begin());

            // Find endif
            size_t end_if = compiledText->find("{% endif %}", tpl_pos+tag.length()+7);
            // look for any if statement between if and endif
            std::unique_ptr<std::string> internal_string = std::unique_ptr<std::string>(new std::string(compiledText->substr(tpl_pos+tag.length(), end_if-tpl_pos-tag.length())));
            std::regex if_regex ("% if ");
            std::smatch results;
            std::regex_search (*internal_string, results, if_regex);
            // if another if is found, look for the next endif, repeating the search for if's until actual end found
            if (results.size()>0){
                // need to make this a function that calls itself to jump through every nexted loop
                for (int endIfCount=0; endIfCount<results.size(); endIfCount++){
                    end_if = compiledText->find("{% endif %}", end_if+12);
                    internal_string = std::unique_ptr<std::string>(new std::string(compiledText->substr(tpl_pos+tag.length()+7, end_if-tpl_pos-tag.length()-7)));
                }
            }
            if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*internal_string);

            bool statement_true=false;
            // Create new class for storing the logic
            // statement.and->next_statement->or->next_statement
            if (str_array[0]=="and" || str_array[0]=="or"){
                throw new TemplateParserException("first part of statement cannot be \"and\" or \"or\"");
            }
            // Cleanse the array a little.. ie.. if there are spaces between expressions (entity.blah == "nope") we need to ditch those spaces.
            std::vector<std::string> new_strArray;
            for (auto statementPart = str_array.begin(); statementPart < str_array.end(); statementPart++){
                std::string newStatement=*statementPart;
                bool foundExpr=false;
                for (auto exprPos = m_exprList->begin(); exprPos != m_exprList->end(); exprPos++){
                    if (*statementPart==exprPos->first){
                        foundExpr=true;
                        newStatement=new_strArray.back();
                        new_strArray.pop_back();
                        newStatement+=(*statementPart);
                        *statementPart++;
                        newStatement+=(*statementPart);
                    }
                }
                new_strArray.push_back(newStatement);
            }
            str_array = new_strArray;
            for (auto statementPart = str_array.begin(); statementPart < str_array.end(); statementPart++){
                if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*statementPart);
                TemplateBaseExpr *expr = 0;
                for (auto exprPos = m_exprList->begin(); exprPos != m_exprList->end(); exprPos++){
                    std::string key = exprPos->first;
                    std::regex tag_search("(.*?)"+key+"(.*?)");
                    std::smatch tag_match;
                    std::regex_search(*statementPart, tag_match, tag_search);
                    if (tag_match.size()>0){
                        if (DEBUG_IF==1) qDebug() << __FILE__ << __LINE__ << QString::fromStdString(*statementPart) << ": " << QString::fromStdString(key);
                        expr = exprPos->second;
                        exprPos=m_exprList->end();
                    }
                }
                if (expr!=0){
                    std::string lhs="", rhs="";
                    int start = 0;
                    int end = 0;
                    while (end!=std::string::npos){
                        end = (*statementPart).find(expr->exprString(),start);
                        if (lhs==""){
                            lhs = (*statementPart).substr(start, end-start);
                        } else {
                            rhs = (*statementPart).substr(start, end-start);
                        }
                        start = end+expr->exprString().length();
                    }
                    if (lhs!="" && rhs!=""){
                        statement_true=expr->exec(templateData, lhs, rhs);
                    } else {
                        throw new TemplateParserException("Can't decypher the expression in the template");
                    }
                } else if ((*statementPart)=="and" || (*statementPart)=="or"){

                } else {
                    // split text and see if it exists in the data
                    std::vector<std::string> mapLocation;
                    boost:split(mapLocation, (*statementPart), boost::is_any_of("."));
                    TemplateObject * tplObject = templateData;
                    bool locationFound = false;
                    for (auto mapLoc_it=mapLocation.begin(); mapLoc_it< mapLocation.end(); mapLoc_it++){
                        if (tplObject->type()=="TemplateMap" && ((TemplateMap*)tplObject)->exists(*mapLoc_it)){
                            tplObject = ((TemplateMap*)tplObject)->get(*mapLoc_it);
                            locationFound=true;
                        } else {
                            locationFound=false;
                        }
                    }
                    if (locationFound){
                        statement_true=true;
                    }
                }
            }

            if (statement_true){
                // look for else between if or nested endif
                // Need to check if there is nested if's and make sure we have to correct elif/else so we can trim to it
                auto else_pos = internal_string->find("{% else %}", 0);
                auto elif_pos = internal_string->find("{% elif ", 0);
                if (elif_pos != std::string::npos){
                    if ((else_pos == std::string::npos) ||
                        (elif_pos < else_pos)) {
                        else_pos = elif_pos;
                    }
                }

                if (else_pos != std::string::npos){
                    auto internal_if = internal_string->find("{% if ", else_pos);
                    if ((internal_if != std::string::npos) &&
                        (else_pos > internal_if)) {
                        // Look for if statements between else and 0
                        std::regex if_regex ("{% if ");
                        std::smatch results;
                        std::regex_search (*internal_string, results, if_regex);
                        auto internal_endIf = 0;
                        if (results.size()>0){
                            for (int endIfCount=0; endIfCount<results.size(); endIfCount++) {
                                internal_endIf = internal_string->find("{% endif %}", internal_endIf+1);
                            }
                            else_pos = internal_string->find("{% else %}", internal_endIf);
                            elif_pos = internal_string->find("{% elif ", internal_endIf);
                            if (elif_pos != std::string::npos){
                                if ((else_pos == std::string::npos) ||
                                    (elif_pos < else_pos)){
                                    else_pos = elif_pos;
                                }
                            }
                        }
                    }
                }
                if (else_pos != std::string::npos){
                    internal_string = std::unique_ptr<std::string>(new std::string(internal_string->substr(0,else_pos)));
                }
        
                if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << "True Statement";
                if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*internal_string); 
                // parse template between 2 points
                std::unique_ptr<Template> tpl = std::unique_ptr<Template>(new Template());
                tpl->setTemplate(*internal_string);
                tpl->setPath(m_tplPath);
                std::unique_ptr<std::string> if_output=std::unique_ptr<std::string>(tpl->compile(templateData));
                trim(*if_output);
                if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*compiledText);
                int endIfLength = compiledText->find("%}", end_if);
                compiledText->replace(tpl_pos, endIfLength-tpl_pos+2, *if_output);
                if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*compiledText);
            } else {
                if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << "False";
                // Check for else in block and process template accordingly
                size_t elif_pos = internal_string->find("{% elif", 0);
                size_t else_pos = internal_string->find("{% else %}", 0);
                if (elif_pos != std::string::npos){
                    if ((else_pos == std::string::npos) ||
                        (elif_pos < else_pos)) {
                        else_pos = elif_pos;
                    }
                }
        
                if (else_pos != std::string::npos){
                    auto internal_if = internal_string->find("{% if ", else_pos);
                    if (internal_if != std::string::npos){
                        if (else_pos > internal_if) {
                            // look for any if statement between if and endif
                            std::regex if_regex ("% if ");
                            std::smatch results;
                            std::regex_search (*internal_string, results, if_regex);
                            // if another if is found, look for the next endif, repeating the search for if's until actual end found
                            if (results.size()>0){
                            // need to make this a function that calls itself to jump through every nexted loop
                                for (int endIfCount=0; endIfCount<results.size(); endIfCount++){
                                    end_if = compiledText->find("{% endif %}", end_if+12);
                                    internal_string = std::unique_ptr<std::string>(new std::string(compiledText->substr(tpl_pos+tag.length()+7, end_if-tpl_pos-tag.length()-7)));
                                }
                            }
                        }
                    } else {
                        internal_string = std::unique_ptr<std::string>(new std::string(internal_string->substr(else_pos+10)));
                    }
                    std::unique_ptr<Template> tpl = std::unique_ptr<Template>(new Template());

                    tpl->setTemplate(*internal_string);
                    tpl->setPath(m_tplPath);
                    std::unique_ptr<std::string> else_output=std::unique_ptr<std::string>(tpl->compile(templateData));
                    trim(*else_output);
                    compiledText->replace(tpl_pos, end_if-tpl_pos+12, *else_output);
                    if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*compiledText);
                } else {
                    if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*compiledText);
                    compiledText->replace(tpl_pos, end_if+11-tpl_pos, "");
                }
            }
            if (DEBUG_IF==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*compiledText);
        } else if (str_array[0].compare("include")==0){
            if (DEBUG_INCLUDE==1) qDebug().noquote() << __FILE__ << __LINE__ << "Include template";

            std::string child_tpl;
            if (str_array[1].c_str()[0]=='"'){
                auto tpl_name_end = str_array[1].find("\"",1);
                child_tpl = str_array[1].substr(1,tpl_name_end-1);
            } else {
                child_tpl = str_array[1];
            }
          
            boost::trim(child_tpl);
            
            std::unique_ptr<Template> tpl;
            if (m_tplPath==":" || 
                (m_tplPath.length()==0 && child_tpl.c_str()[0]==':')){
                std::string child_path=m_tplPath+child_tpl;
                QFile *templateFile = new QFile(child_path.c_str());
                tpl = std::unique_ptr<Template>(new Template(*templateFile));
            } else {
                tpl = std::unique_ptr<Template>(new Template(m_tplPath+child_tpl));
            }
            std::unique_ptr<std::string> includeOutput = std::unique_ptr<std::string>(new std::string(""));
            try {
                if (str_array.size()==4 && str_array[2]=="with"){
                    includeOutput = std::unique_ptr<std::string>(tpl->compile(((TemplateMap*)templateData)->get(str_array[3])));
               } else {
                    includeOutput = std::unique_ptr<std::string>(tpl->compile(templateData));
               }
            } catch (std::exception &e){
            }
            if (DEBUG_INCLUDE==1) qDebug().noquote() << __FILE__ << __LINE__ << "includeOutput: \n" << QString::fromStdString(*includeOutput);
            compiledText->replace(tpl_pos, tag.length(), *includeOutput);
            if (DEBUG_INCLUDE==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*compiledText);
        } else {
            if (DEBUG_TPLTEXT==1) qDebug().noquote() << __FILE__ << __LINE__ << "Template text";
            try {
                if (DEBUG_TPLTEXT==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*compiledText);
                TemplateObject* tempData = templateData;
                if (tempData->type()=="TemplateMap"){
                    std::vector<std::string> dataLocation;
                    boost::split(dataLocation, tag_expression, boost::is_any_of("."));
                    for (auto split_pos = dataLocation.begin(); split_pos < dataLocation.end(); split_pos++){
                        if (tempData->type()!="TemplateMap"){
                            throw std::out_of_range("Oops");
                        }
                        tempData = ((TemplateMap*)tempData)->get(*split_pos);
                    }
                    compiledText->replace(tpl_pos, tag.length(), tempData->toString());
                }
            } catch (std::out_of_range &oor){
                compiledText->replace(tpl_pos, tag.length(), "");
            }
        }
    }
    if (DEBUG_ALL==1) qDebug().noquote() << __FILE__ << __LINE__ << QString::fromStdString(*compiledText);
    if (DEBUG_ALL==1) qDebug().noquote() << __FILE__ << __LINE__ << "Done";
    return compiledText;
}
