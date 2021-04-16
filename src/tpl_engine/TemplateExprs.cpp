/**
 * @author      : Sam Bell (sam@mimpidev.com) 
 * @file        : TemplateExpr
 * @created     : Monday Feb 8, 2021 23:00:00 AEDT
 */

#include "TemplateExprs.h"
#include "TemplateMap.h"
#include "TemplateString.h"
#include <vector>
#include <boost/algorithm/string.hpp>

std::string TemplateBaseExpr::convertValue(TemplateObject* templateData, std::string location){
    std::string value="";
    
    if ((location.c_str()[0]=='"' &&
         location.c_str()[location.length()-1]=='"') ||
        (location.c_str()[0]=='\'' &&
         location.c_str()[location.length()-1]=='\'')){
        value=location.substr(1,location.length()-2);
    } else {
        std::vector<std::string> mapLocation;
        boost::split(mapLocation, location, boost::is_any_of("."));
        TemplateObject *tpl_object = templateData;
        for (auto mapLoc_it=mapLocation.begin(); mapLoc_it<mapLocation.end(); mapLoc_it++){
            if (tpl_object->type()=="TemplateMap" && ((TemplateMap*)tpl_object)->exists(*mapLoc_it)){
                tpl_object = ((TemplateMap*)tpl_object)->get(*mapLoc_it);
            } else {
                mapLoc_it=mapLocation.end();
            }
        }
        if (tpl_object->type()=="TemplateString"){
            value= tpl_object->toString();
        }
    }
    return value;
}

bool TemplateExpr::Equals::exec(TemplateObject* templateData,
                           std::string lse,
                           std::string rse){
    lse = convertValue(templateData,lse);
    rse = convertValue(templateData,rse);
    return lse==rse;
}

bool TemplateExpr::NotEquals::exec(TemplateObject* templateData,
                                   std::string lse,
                                   std::string rse){
    lse = convertValue(templateData,lse);
    rse = convertValue(templateData,rse);
    return lse!=rse;
}

bool TemplateExpr::GreaterThan::exec(TemplateObject* templateData,
                                   std::string lse,
                                   std::string rse){
    lse = convertValue(templateData,lse);
    rse = convertValue(templateData,rse);
    return strtod(lse.c_str(),NULL)>strtod(rse.c_str(),NULL);
}

bool TemplateExpr::GreaterThanEquals::exec(TemplateObject* templateData,
                                   std::string lse,
                                   std::string rse){
    lse = convertValue(templateData,lse);
    rse = convertValue(templateData,rse);
    return strtod(lse.c_str(),NULL)>=strtod(rse.c_str(),NULL);
}

bool TemplateExpr::LessThan::exec(TemplateObject* templateData,
                                   std::string lse,
                                   std::string rse){
    lse = convertValue(templateData,lse);
    rse = convertValue(templateData,rse);
    return strtod(lse.c_str(),NULL)<strtod(rse.c_str(),NULL);
}

bool TemplateExpr::LessThanEquals::exec(TemplateObject* templateData,
                                   std::string lse,
                                   std::string rse){
    lse = convertValue(templateData,lse);
    rse = convertValue(templateData,rse);
    return strtod(lse.c_str(),NULL)<=strtod(rse.c_str(),NULL);
}