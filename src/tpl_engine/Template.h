/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : Template.h
 * @created     : Saturday Jun 01, 2019 12:15:53 UTC
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H
#include <string>
#include <fstream>
#include <QFile>

#include "TemplateExprs.h"
#include "TemplateMap.h"

class Template
{
    public:
        Template();
        Template(std::string pathName);
	    Template(QFile &fileObject);
        void setTemplate(std::string templateString) { m_templateString = templateString; }
        void setTemplate(std::ifstream &fileStream);
        std::string* compile(TemplateObject *templateData);
        void setPath(std::string path) { m_tplPath = path; }
        std::string getTemplate() { return m_templateString; }
        std::string trim(std::string &input);
    private:
        std::string m_templateString;
        std::string m_tplPath;
        template_expr_map_ptr m_exprList = std::make_unique<template_expr_map_t>();
        bool isQFile=false;
};


#endif /* end of include guard TEMPLATE_H */
