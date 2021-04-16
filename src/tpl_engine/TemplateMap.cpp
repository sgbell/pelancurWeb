/**
 * @author      : Sam Bell (sam@silverband.com.au)
 * @file        : TemplateData
 * @created     : Thursday Feb 20, 2020 12:12:08 AEDT
 */

#include "TemplateMap.h"
#include "TemplateString.h"
#include "TemplateList.h"

#include <iostream>
#include <QString>

TemplateMap::TemplateMap()
{
    m_sourceData = tplobject_ptr(new tplobject_t());
}

TemplateMap::~TemplateMap(){
    
}

void TemplateMap::put(std::string key, TemplateObject* value){
    m_sourceData->insert(std::pair<std::string, TemplateObject*>(key,value));
}

void TemplateMap::put(std::string key, std::string value){
    put(key, new TemplateString(value));
}

void TemplateMap::put(std::string key, QString value){
    put(key, new TemplateString(value));
}

TemplateObject* TemplateMap::get(std::string key){
    return m_sourceData->at(key);
}

QString TemplateMap::getQString(std::string key){
    return QString(get(key)->toString().c_str());
}

std::string TemplateMap::toString(){
    std::string outputString = "{";

    auto dataIterator = m_sourceData->begin();
    while (dataIterator != m_sourceData->end()){
        outputString+=dataIterator->first+":";
        if ((*dataIterator->second).type() == "TemplateMap"){
            outputString+=(dynamic_cast<TemplateMap*>(dataIterator->second))->toString();
        } else if ((*dataIterator->second).type() == "TemplateList"){
            outputString+=(dynamic_cast<TemplateList*>(dataIterator->second))->toString();
        } else {
            outputString+=dataIterator->second->toString();
        }
        dataIterator++;
        if (dataIterator != m_sourceData->end()){
            outputString+=", ";   
        }
    }
    outputString+="}";
    return outputString;
}

bool TemplateMap::exists(std::string key){
    auto dataIterator = m_sourceData->find(key);
    return dataIterator != m_sourceData->end();
}
