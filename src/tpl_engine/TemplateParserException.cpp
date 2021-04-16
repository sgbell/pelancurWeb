/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdConnectionException
 * @created     : Saturday Jun 01, 2019 12:53:12 UTC
 */

#include "TemplateParserException.h"

TemplateParserException::TemplateParserException(){
    m_message = "Template Parsing Error";
}

TemplateParserException::TemplateParserException(std::string msg){
    m_message = msg;
}

std::string TemplateParserException::message(){
    return m_message;
}
