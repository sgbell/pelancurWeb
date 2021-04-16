/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : TemplateObject
 * @created     : Thursday Feb 20, 2020 12:11:46 AEDT
 */

#ifndef TEMPLATEOBJECT_H
#define TEMPLATEOBJECT_H
#include <string>
#include <map>
#include <memory>

class TemplateObject;

typedef std::map<std::string,TemplateObject*> tplobject_t;
typedef std::unique_ptr<tplobject_t> tplobject_ptr;

class TemplateObject
{
    public:
        TemplateObject ();
        virtual std::string toString() { return ""; }
        virtual std::string type() { return "TemplateObject"; }
    private:
        /* private data */
};

#endif /* end of include guard TEMPLATEOBJECT_H */

