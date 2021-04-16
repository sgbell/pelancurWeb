/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : Template.h
 * @created     : Saturday Jun 01, 2019 12:15:53 UTC
 */

#ifndef TEMPLATE_MAP_H
#define TEMPLATE_MAP_H

#include "TemplateObject.h"
#include <QString>

class TemplateMap : public TemplateObject
{
    public:
        TemplateMap();
        virtual ~TemplateMap();
        std::string toString();
        void put(std::string key, TemplateObject* value);
        void put(std::string key, std::string value);
        void put(std::string key, QString value);
        TemplateObject* get(std::string key);
        QString getQString(std::string key);
        std::string type() { return "TemplateMap"; }
        bool exists(std::string key);
    private:
        tplobject_ptr m_sourceData;
};


#endif /* end of include guard TEMPLATE_H */
