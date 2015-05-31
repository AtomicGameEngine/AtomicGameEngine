// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

// see https://developer.mozilla.org/en-US/docs/Mozilla/Projects/SpiderMonkey/Parser_API

#include <Atomic/Container/Str.h>
#include <Atomic/Container/Vector.h>

using namespace Atomic;

namespace rapidjson
{
template<typename CharType> struct UTF8;
class CrtAllocator;
template <typename BaseAllocator> class MemoryPoolAllocator;
template <typename Encoding, typename Allocator> class GenericValue;
typedef GenericValue<UTF8<char>, MemoryPoolAllocator<CrtAllocator> > Value;
template <typename Encoding, typename Allocator> class GenericDocument;
typedef GenericDocument<UTF8<char>, MemoryPoolAllocator<CrtAllocator> > Document;
}

namespace AtomicEditor
{

enum JSASTType
{
    JSAST_UNDEFINED,
    JSAST_COMMENT,
    JSAST_PROGRAM,
    JSAST_PROPERTY,
    // Expression
    JSAST_ASSIGNMENTEXPRESSION,
    JSAST_LOGICALEXPRESSION,
    JSAST_IDENTIFIER,
    JSAST_CALLEXPRESSION,
    JSAST_VARIABLEDECLARATOR,
    JSAST_BINARYEXPRESSION,
    JSAST_MEMBEREXPRESSION,
    JSAST_LITERAL,
    JSAST_ARRAYEXPRESSION,
    JSAST_FUNCTIONEXPRESSION,
    JSAST_NEWEXPRESSION,
    JSAST_UNARYEXPRESSION,
    JSAST_UPDATEEXPRESSION,
    JSAST_CONDITIONALEXPRESSION,
    JSAST_OBJECTEXPRESSION,
    JSAST_THISEXPRESSION,
    // Statements
    JSAST_RETURNSTATEMENT,
    JSAST_EMPTYSTATEMENT,
    JSAST_EXPRESSIONSTATEMENT,
    JSAST_LABELEDSTATEMENT,
    JSAST_VARIABLEDECLARATION,
    JSAST_FUNCTIONDECLARATION,
    JSAST_IFSTATEMENT,
    JSAST_BLOCKSTATEMENT
};


class JSASTProgram;
class JSASTStatement;
class JSASTEmptyStatement;
class JSASTExpressionStatement;
class JSASTExpression;
class JSASTVisitor;
class JSASTBlockStatement;

class JSASTNode
{    
public:

    struct Loc
    {
        int startLine_;
        int startColumn_;
        int endLine_;
        int endColumn_;

        Loc()
        {
            startLine_ = startColumn_ = -1;
            endLine_ = endColumn_ = -1;
        }

        bool Valid() const { return startLine_ != -1 && startColumn_ != -1 &&
                    endLine_ != -1 && endColumn_ != -1; }

    };

    JSASTNode(JSASTType type);
    virtual bool Parse(const rapidjson::Value& value);

    JSASTType GetType() { return type_; }
    const Loc& GetLoc() { return loc_; }

    virtual void Accept(JSASTVisitor* visitor) = 0;

protected:

    bool ParseLoc(const rapidjson::Value &value);
    bool ParseRange(const rapidjson::Value &value);

    JSASTStatement* ParseStatement(const rapidjson::Value &value);
    JSASTExpression* ParseExpression(const rapidjson::Value &value, JSASTType astType = JSAST_UNDEFINED);

    bool ParseExpressionArray(const rapidjson::Value &value, Vector<JSASTExpression *> &expressions);
    bool ParseStatementArray(const rapidjson::Value &value, Vector<JSASTStatement *> &statements);

private:

    Loc loc_;
    JSASTType type_;
    // character range
    int rangeStart_;
    int rangeEnd_;


};

class JSASTComment: public JSASTNode
{
public:

    JSASTComment();
    virtual ~JSASTComment();

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    const String& GetValue() { return value_; }
    bool IsBlock() { return isBlock_; }

private:

    bool isBlock_;
    String value_;

};

class JSASTProgram : public JSASTNode
{
public:

    JSASTProgram(const String& path, const String& json);
    virtual ~JSASTProgram();

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    static JSASTProgram* ParseFromJSON(const String& path, const String& json);

    unsigned GetStatementCount() { return body_.Size(); }
    JSASTStatement* GetStatement(unsigned idx) { return body_[idx]; }

    unsigned GetCommentCount() { return comments_.Size(); }
    JSASTComment* GetComment(unsigned idx) { return comments_[idx]; }


protected:

private:

    Vector<JSASTStatement*> body_;
    rapidjson::Document* document_;

    Vector<JSASTComment*> comments_;

};

class JSASTExpression : public JSASTNode
{
public:

    virtual ~JSASTExpression() {}

    virtual bool Parse(const rapidjson::Value& value)
    {
        return JSASTNode::Parse(value);
    }

protected:
    JSASTExpression(JSASTType type) :  JSASTNode(type) {}

};


class JSASTIdentifier : public JSASTExpression
{

public:
    JSASTIdentifier() : JSASTExpression(JSAST_IDENTIFIER) {}

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

private:

    String name_;
};

class JSASTThisExpression : public JSASTExpression
{

public:
    JSASTThisExpression() : JSASTExpression(JSAST_THISEXPRESSION) {}

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

private:

};


class JSASTLiteral : public JSASTExpression
{

public:

    enum LiteralType
    {
        LITERAL_UNKNOWN,
        LITERAL_NULL,
        LITERAL_STRING,
        LITERAL_NUMBER,
        LITERAL_BOOLEAN,
        LITERAL_REGEX
    };

    JSASTLiteral() : JSASTExpression(JSAST_LITERAL), literalType_(LITERAL_UNKNOWN) {}

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    LiteralType GetLiteralType() { return literalType_; }

private:

    //value: string | boolean | null | number | RegExp;
    LiteralType literalType_;
    String raw_;
};

class JSASTArrayExpression : public JSASTExpression
{

public:
    JSASTArrayExpression() : JSASTExpression(JSAST_ARRAYEXPRESSION) {}

    virtual ~JSASTArrayExpression()
    {
        for (unsigned i = 0; i < elements_.Size(); i++)
            delete elements_[i];

        elements_.Clear();
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    unsigned GetElementCount() { return elements_.Size(); }
    JSASTExpression* GetElement(unsigned idx) { return elements_[idx]; }

private:

    Vector<JSASTExpression*> elements_;

};

class JSASTProperty: public JSASTNode
{

public:

    JSASTProperty() : JSASTNode(JSAST_PROPERTY), key_(0), value_(0) {}
    virtual ~JSASTProperty()
    {
        if (key_)
            delete key_;

        if (value_)
            delete value_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetKey() { return key_; }
    JSASTExpression* GetValue() { return value_; }


private:
    JSASTExpression* key_;
    JSASTExpression* value_;

};


class JSASTObjectExpression : public JSASTExpression
{

public:
    JSASTObjectExpression() : JSASTExpression(JSAST_OBJECTEXPRESSION) {}

    virtual ~JSASTObjectExpression()
    {
        for (unsigned i = 0; i < properties_.Size(); i++)
            delete properties_[i];

        properties_.Clear();
    }

    unsigned GetPropertyCount() { return properties_.Size(); }
    JSASTProperty* GetProperty(unsigned idx) { return properties_[idx]; }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

private:

    Vector<JSASTProperty*> properties_;
};



class JSASTAssignmentExpression : public JSASTExpression
{

public:
    JSASTAssignmentExpression() : JSASTExpression(JSAST_ASSIGNMENTEXPRESSION),
        leftExpression_(0), rightExpression_(0) {}

    virtual ~JSASTAssignmentExpression()
    {
        if (leftExpression_)
            delete leftExpression_;
        if (rightExpression_)
            delete rightExpression_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    const String& GetOperator() { return operator_; }
    JSASTExpression* GetLeft() { return leftExpression_; }
    JSASTExpression* GetRight() { return rightExpression_; }

private:

    String operator_;
    JSASTExpression* leftExpression_;
    JSASTExpression* rightExpression_;

};

class JSASTLogicalExpression : public JSASTExpression
{

public:
    JSASTLogicalExpression() : JSASTExpression(JSAST_LOGICALEXPRESSION),
        leftExpression_(0), rightExpression_(0) {}

    virtual ~JSASTLogicalExpression()
    {
        if (leftExpression_)
            delete leftExpression_;
        if (rightExpression_)
            delete rightExpression_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    const String& GetOperator() { return operator_; }
    JSASTExpression* GetLeft() { return leftExpression_; }
    JSASTExpression* GetRight() { return rightExpression_; }

private:

    String operator_;
    JSASTExpression* leftExpression_;
    JSASTExpression* rightExpression_;

};


class JSASTUnaryExpression : public JSASTExpression
{

public:
    JSASTUnaryExpression() : JSASTExpression(JSAST_UNARYEXPRESSION),
        argument_(0), prefix_(false) {}

    virtual ~JSASTUnaryExpression()
    {
        if (argument_)
            delete argument_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    const String& GetOperator() { return operator_;}

    JSASTExpression* GetArgument() { return argument_; }

private:

    String operator_;
    bool prefix_;
    JSASTExpression* argument_;

};

class JSASTUpdateExpression : public JSASTExpression
{

public:
    JSASTUpdateExpression() : JSASTExpression(JSAST_UPDATEEXPRESSION),
        argument_(0), prefix_(false) {}

    virtual ~JSASTUpdateExpression()
    {
        if (argument_)
            delete argument_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    const String& GetOperator() { return operator_;}

    JSASTExpression* GetArgument() { return argument_; }

private:

    String operator_;
    bool prefix_;
    JSASTExpression* argument_;

};

class JSASTFunctionExpression : public JSASTExpression
{

public:
    JSASTFunctionExpression() : JSASTExpression(JSAST_FUNCTIONEXPRESSION),
        id_(0), rest_(0), body_(0), generator_(false), expression_(false) {}

    virtual ~JSASTFunctionExpression();

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTIdentifier* GetID() { return id_; }
    JSASTBlockStatement* GetBodyStatement() { return body_; }

    unsigned GetParamsCount() { return params_.Size(); }
    JSASTExpression* GetParam(unsigned idx) { return params_[idx]; }

private:

    JSASTIdentifier* id_;
    Vector<JSASTExpression*> params_;
    Vector<JSASTExpression*> defaults_;
    JSASTIdentifier* rest_;

    JSASTBlockStatement* body_;

    bool generator_;
    bool expression_;

};


class JSASTBinaryExpression : public JSASTExpression
{

public:
    JSASTBinaryExpression() : JSASTExpression(JSAST_BINARYEXPRESSION),
        leftExpression_(0), rightExpression_(0) {}

    virtual ~JSASTBinaryExpression()
    {
        if (leftExpression_)
            delete leftExpression_;
        if (rightExpression_)
            delete rightExpression_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetLeft() { return leftExpression_; }
    JSASTExpression* GetRight() { return rightExpression_; }

    const String& GetOperator() { return operator_;}

private:

    String operator_;
    JSASTExpression* leftExpression_;
    JSASTExpression* rightExpression_;

};

class JSASTMemberExpression : public JSASTExpression
{

public:
    JSASTMemberExpression() : JSASTExpression(JSAST_MEMBEREXPRESSION),
        object_(0), property_(0), computed_(false) {}

    virtual ~JSASTMemberExpression()
    {
        if (object_)
            delete object_;
        if (property_)
            delete property_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetObject() { return object_;}
    JSASTExpression* GetProperty() { return property_; }


private:

    JSASTExpression* object_;
    JSASTExpression* property_;
    bool computed_;
};

class JSASTConditionalExpression : public JSASTExpression
{
public:

    JSASTConditionalExpression() : JSASTExpression(JSAST_CONDITIONALEXPRESSION), test_(0), consequent_(0), alternate_(0) {}

    virtual ~JSASTConditionalExpression()
    {
        if (test_)
            delete test_;
        if (consequent_)
            delete consequent_;
        if (alternate_)
            delete alternate_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetTest() { return test_; }
    JSASTExpression* GetConsequent() { return consequent_; }
    JSASTExpression* GetAlternate() { return alternate_; }

private:

    JSASTExpression* test_;
    JSASTExpression* consequent_;
    JSASTExpression* alternate_;

};

class JSASTCallExpression : public JSASTExpression
{

public:
    JSASTCallExpression() : JSASTExpression(JSAST_CALLEXPRESSION),
        callee_(0){}

    virtual ~JSASTCallExpression()
    {
        if (callee_)
            delete callee_;

        for (unsigned i = 0; i < arguments_.Size(); i++)
        {
            delete arguments_[i];
        }

        arguments_.Clear();
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetCallee() { return callee_; }
    unsigned GetArgumentCount() { return arguments_.Size(); }
    JSASTExpression* GetArgument(unsigned idx) { return arguments_[idx]; }

private:

    JSASTExpression* callee_;
    Vector<JSASTExpression*> arguments_;

};

class JSASTNewExpression : public JSASTExpression
{

public:
    JSASTNewExpression() : JSASTExpression(JSAST_NEWEXPRESSION),
        callee_(0){}

    virtual ~JSASTNewExpression()
    {
        if (callee_)
            delete callee_;

        for (unsigned i = 0; i < arguments_.Size(); i++)
        {
            delete arguments_[i];
        }

        arguments_.Clear();
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetCallee() { return callee_; }
    unsigned GetArgumentCount() { return arguments_.Size(); }
    JSASTExpression* GetArgument(unsigned idx) { return arguments_[idx]; }


private:

    JSASTExpression* callee_;
    Vector<JSASTExpression*> arguments_;

};

class JSASTVariableDeclarator : public JSASTExpression
{
public:

    JSASTVariableDeclarator() : JSASTExpression(JSAST_VARIABLEDECLARATOR), id_(0), init_(0) {}

    virtual ~JSASTVariableDeclarator()
    {
        if (id_)
            delete id_;
        if (init_)
            delete init_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetID() { return id_; }
    JSASTExpression* GetInit() { return init_; }


private:

    JSASTExpression* id_;
    JSASTExpression* init_;

};

class JSASTStatement : public JSASTNode
{

public:

    virtual ~JSASTStatement() {}

    virtual bool Parse(const rapidjson::Value& value)
    {
        return JSASTNode::Parse(value);
    }

protected:
    JSASTStatement(JSASTType type) :  JSASTNode(type) {}

};

class JSASTEmptyStatement : public JSASTStatement
{

public:
    JSASTEmptyStatement() : JSASTStatement(JSAST_EMPTYSTATEMENT) {}

    virtual bool Parse(const rapidjson::Value& value)
    {
        return JSASTStatement::Parse(value);
    }
    virtual void Accept(JSASTVisitor* visitor);

};

class JSASTExpressionStatement : public JSASTStatement
{

public:
    JSASTExpressionStatement(JSASTType type = JSAST_EXPRESSIONSTATEMENT) : JSASTStatement(type),
        expression_(0) {}

    virtual ~JSASTExpressionStatement()
    {
        if (expression_)
            delete expression_;
    }


    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetExpression() { return expression_; }

protected:

    JSASTExpression* expression_;

};

class JSASTLabeledStatement : public JSASTStatement
{

public:
    JSASTLabeledStatement(JSASTType type = JSAST_LABELEDSTATEMENT) : JSASTStatement(type),
        label_(0), body_(0) {}

    virtual ~JSASTLabeledStatement()
    {
        if (label_)
            delete label_;

        if (body_)
            delete body_;

    }

    JSASTIdentifier* GetLabel() { return label_; }
    JSASTStatement* GetBody() { return body_; }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

protected:

    JSASTIdentifier* label_;
    JSASTStatement* body_;

};


class JSASTReturnStatement : public JSASTStatement
{

public:
    JSASTReturnStatement(JSASTType type = JSAST_RETURNSTATEMENT) : JSASTStatement(type),
        argument_(0) {}

    virtual ~JSASTReturnStatement()
    {
        if (argument_)
            delete argument_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetArgument() { return argument_; }

protected:

    JSASTExpression* argument_;

};


class JSASTDeclaration : public JSASTStatement
{

public:

    virtual bool Parse(const rapidjson::Value& value) { return JSASTStatement::Parse(value); }

protected:

    JSASTDeclaration(JSASTType type) : JSASTStatement(type) {}

};

class JSASTBlockStatement : public JSASTStatement
{
public:
    JSASTBlockStatement() : JSASTStatement(JSAST_BLOCKSTATEMENT) {}

    virtual ~JSASTBlockStatement()
    {
        for (unsigned i = 0; i < body_.Size(); i++)
            delete body_[i];

        body_.Clear();
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    unsigned GetStatementCount() { return body_.Size(); }
    JSASTStatement* GetStatement(unsigned idx) { return body_[idx]; }


private:

    Vector<JSASTStatement*> body_;

};

class JSASTIfStatement : public JSASTStatement
{
public:

    JSASTIfStatement() : JSASTStatement(JSAST_IFSTATEMENT), test_(0), consequent_(0), alternate_(0) {}

    virtual ~JSASTIfStatement()
    {
        if (test_)
            delete test_;
        if (consequent_)
            delete consequent_;
        if (alternate_)
            delete alternate_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetTest() { return test_; }
    JSASTStatement* GetConsequent() { return consequent_; }
    JSASTStatement* GetAlternate() { return alternate_; }

private:

    JSASTExpression* test_;
    JSASTStatement* consequent_;
    JSASTStatement* alternate_;

};

class JSASTVariableDeclaration : public JSASTDeclaration
{

public:

    JSASTVariableDeclaration() : JSASTDeclaration(JSAST_VARIABLEDECLARATION) {}

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    const String& GetKind()  { return kind_; }

    unsigned GetDeclarationsCount() { return declarations_.Size(); }
    JSASTExpression* GetDeclaration(unsigned idx) { return declarations_[idx]; }

protected:

    // "var" | "let" | "const";
    String kind_;

    // Array of JSASTVariableDeclarator
    Vector<JSASTExpression*> declarations_;

};


class JSASTFunctionDeclaration : public JSASTDeclaration
{

public:
    JSASTFunctionDeclaration() : JSASTDeclaration(JSAST_FUNCTIONDECLARATION),
        id_(0), rest_(0), body_(0), generator_(false), expression_(false) {}

    virtual ~JSASTFunctionDeclaration();

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTIdentifier* GetID() { return id_; }
    JSASTBlockStatement* GetBodyStatement() { return body_; }

    unsigned GetParamsCount() { return params_.Size(); }
    JSASTExpression* GetParam(unsigned idx) { return params_[idx]; }

private:

    JSASTIdentifier* id_;
    Vector<JSASTExpression*> params_;
    Vector<JSASTExpression*> defaults_;
    JSASTIdentifier* rest_;

    JSASTBlockStatement* body_;

    bool generator_;
    bool expression_;

};

/*
interface ForStatement <: Statement {
    type: "ForStatement";
    init: VariableDeclaration | Expression | null;
    test: Expression | null;
    update: Expression | null;
    body: Statement;
}
*/


class JSASTForStatement : public JSASTStatement
{
public:

    JSASTForStatement() : JSASTStatement(JSAST_IFSTATEMENT),
        initVariable_(0), initExpression_(0), test_(0), update_(0), body_(0)
    {

    }

    virtual ~JSASTForStatement()
    {
        if (initVariable_)
            delete initVariable_;

        if (initExpression_)
            delete initExpression_;

        if (test_)
            delete(test_);

        if(update_)
            delete update_;

        if (body_)
            delete body_;
    }

    virtual bool Parse(const rapidjson::Value& value);
    virtual void Accept(JSASTVisitor* visitor);

    JSASTExpression* GetTest() { return test_; }
    JSASTVariableDeclaration* GetInitVariable() { return initVariable_; }
    JSASTExpression* GetInitExpression() { return initExpression_; }
    JSASTExpression* GetUpdate() { return update_; }
    JSASTStatement* GetBody() { return body_; }

private:
    JSASTVariableDeclaration* initVariable_;
    JSASTExpression* initExpression_;
    JSASTExpression* test_;
    JSASTExpression* update_;
    JSASTStatement* body_;

};


}


