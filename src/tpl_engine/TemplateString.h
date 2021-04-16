/**
 * @author      : Sam Bell (sam@silverband.com.au)
 * @file        : TemplateString
 * @created     : Monday Feb 24, 2020 12:55:01 AEDT
 */

#ifndef TEMPLATESTRING_H
#define TEMPLATESTRING_H
#include <string>
#include <QString>

#include "TemplateObject.h"

class TemplateString : public TemplateObject
{
    public:
        TemplateString (std::string newValue) {
            m_value = newValue;
        }
        TemplateString (QString _value){
            m_value = _value.toStdString();
        }
        std::string toString() { return m_value; }
        QString toQString() { return QString(m_value.c_str()); }
        std::string type() { return "TemplateString"; }
    private:
        /* private data */
        std::string m_value;
};

#endif /* end of include guard TEMPLATESTRING_H */

