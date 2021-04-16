/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : Template.h
 * @created     : Saturday Jun 01, 2019 12:15:53 UTC
 */

#ifndef TEMPLATE_EXPR_H
#define TEMPLATE_EXPR_H
#include "TemplateObject.h"
#include <string>
#include <map>
#include <memory>

class TemplateLogic {
    public:
        virtual bool exec(bool lse, bool rse) { return false; }
};

class TemplateBaseExpr {
    public:
        virtual bool exec(TemplateObject* templateData, std::string lse, std::string rse) { return false; }
        std::string convertValue(TemplateObject* templateData, std::string location);
        std::string exprString(){ return m_expression; }
    protected:
        std::string m_expression = "";
};

typedef std::map<std::string, TemplateBaseExpr*> template_expr_map_t;
typedef std::unique_ptr<template_expr_map_t> template_expr_map_ptr;

namespace TemplateExpr {
    class Equals : public TemplateBaseExpr{
        public:
            Equals() { m_expression = "=="; }
            bool exec(TemplateObject* templateData, std::string lse, std::string rse);
    };
    class NotEquals : public TemplateBaseExpr{
        public:
            NotEquals() { m_expression = "!="; }
            bool exec(TemplateObject* templateData, std::string lse, std::string rse);
    };
    class GreaterThan : public TemplateBaseExpr{
        public:
            GreaterThan() { m_expression = ">"; }
            bool exec(TemplateObject* templateData, std::string lse, std::string rse);
    };
    class GreaterThanEquals : public TemplateBaseExpr{
        public:
            GreaterThanEquals() { m_expression = ">="; }
            bool exec(TemplateObject* templateData, std::string lse, std::string rse);
    };
    class LessThan : public TemplateBaseExpr{
        public:
            LessThan() { m_expression = "<"; }
            bool exec(TemplateObject* templateData, std::string lse, std::string rse);
    };
    class LessThanEquals : public TemplateBaseExpr{
        public:
            LessThanEquals() { m_expression = "<="; }
            bool exec(TemplateObject* templateData, std::string lse, std::string rse);
    };
    class And : public TemplateLogic{
        public:
            And() { }
            bool exec(bool lse, bool rse);
    };
    class Or : public TemplateLogic{
        public:
            Or() { }
            bool exec(bool lse, bool rse);
    };

};

#endif /* end of include guard TEMPLATE_EXPR_H */