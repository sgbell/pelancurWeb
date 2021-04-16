/**
 * @author      : Sam Bell (sam@silverband.com.au)
 * @file        : TemplateList
 * @created     : Tuesday Feb 25, 2020 01:49:21 UTC
 */

#ifndef TEMPLATELIST_H
#define TEMPLATELIST_H

#include <memory>
#include "TemplateObject.h"
#include <vector>


class TemplateList: public TemplateObject
{
    public:
        TemplateList ();
        void push(TemplateObject &object);
        std::string toString();
        std::vector<TemplateObject*>::iterator begin() { return m_list->begin(); }
        std::vector<TemplateObject*>::iterator end() { return m_list->end(); }
        std::string type() { return "TemplateList"; }
        unsigned int size() { return m_list->size(); }
    private:
        /* private data */
        std::unique_ptr<std::vector<TemplateObject*>> m_list;
};

#endif /* end of include guard TEMPLATELIST_H */

