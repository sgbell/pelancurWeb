/**
 * @author      : Sam Bell (sam@silverband.com.au)
 * @file        : TemplateList
 * @created     : Tuesday Feb 25, 2020 01:53:27 UTC
 */

#include "TemplateList.h"
#include "TemplateMap.h"

#include <iostream>

TemplateList::TemplateList()
{
    m_list = std::unique_ptr<std::vector<TemplateObject*>>(new std::vector<TemplateObject*>()); 
}

void TemplateList::push(TemplateObject &object){
   m_list->push_back(&object);
}

std::string TemplateList::toString() {
    std::string outputString = "[";
    
    auto listIterator = m_list->begin();
    while (listIterator != m_list->end()){
        if ((*listIterator)->type() == "TemplateMap"){
            outputString+=(dynamic_cast<TemplateMap*>(*listIterator))->toString();
        } else if ((*listIterator)->type() == "TemplateList"){
            outputString+=(dynamic_cast<TemplateList*>(*listIterator))->toString();
        } else {
            outputString+= (*listIterator)->toString();
        }
        listIterator++;
        if (listIterator != m_list->end()){
            outputString+=", ";
        }
    }
    outputString+="]";
    
    return outputString;
}
