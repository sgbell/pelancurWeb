/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : TemplateParserException
 * @created     : Saturday Jun 01, 2019 12:41:46 UTC
 */

#ifndef TEMPLATEPARSEREXCEPTION_H

#define TEMPLATEPARSEREXCEPTION_H
#include <exception>
#include <string>


class TemplateParserException : public std::exception {
    public:
        TemplateParserException();
        TemplateParserException(std::string msg);
        std::string message();
    private:
        std::string m_message;
};

#endif /* end of include guard TEMPLATEPARSEREXCEPTION_H */

