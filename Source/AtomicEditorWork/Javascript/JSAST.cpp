// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <Atomic/IO/Log.h>
#include "JSAST.h"
#include "JSASTVisitor.h"

using namespace rapidjson;

namespace AtomicEditor
{

JSASTNode::JSASTNode(JSASTType type) : type_(type), rangeStart_(0), rangeEnd_(0)
{

}

bool JSASTNode::Parse(const rapidjson::Value& value)
{
    assert(value.IsObject());

    for (Value::ConstMemberIterator itr = value.MemberBegin();
         itr != value.MemberEnd(); ++itr)
    {
        String name = itr->name.GetString();

        if (name == "loc")
        {
            ParseLoc(itr->value);
        }
        else if (name == "type")
        {
            // TODO: Verify type
        }
        else if (name == "range")
        {
            ParseRange(itr->value);
        }

    }

    return true;

}

bool JSASTNode::ParseLoc(const rapidjson::Value& value)
{
    // SpiderMonkey can have this for loc :/
    if (value.IsNull())
        return true;

    assert(value.IsObject());

    const Value::Member* mstart = value.FindMember("start");
    assert(mstart);

    const Value::Member* mend = value.FindMember("end");
    assert(mend);

    loc_.startLine_ = mstart->value["line"].GetInt();
    loc_.startColumn_ = mstart->value["column"].GetInt();
    loc_.endLine_ = mend->value["line"].GetInt();
    loc_.endColumn_ = mend->value["column"].GetInt();

    return true;
}

bool JSASTNode::ParseRange(const rapidjson::Value& value)
{
    assert(value.IsArray() && value.Size() == 2);

    rangeStart_ = value[SizeType(0)].GetInt();
    rangeEnd_ = value[SizeType(1)].GetInt();

    return true;

}

JSASTStatement* JSASTNode::ParseStatement(const rapidjson::Value &value)
{
    assert(value.IsObject());

    JSASTStatement* statement = NULL;

    const Value::Member* mtype = value.FindMember("type");
    assert(mtype);

    String type = mtype->value.GetString();

    if (type == "ExpressionStatement")
    {
        statement = new JSASTExpressionStatement();
    }
    else if (type == "VariableDeclaration")
    {
        statement = new JSASTVariableDeclaration();
    }
    else if (type == "ReturnStatement")
    {
        statement = new JSASTReturnStatement();
    }
    else if (type == "FunctionDeclaration")
    {
        statement = new JSASTFunctionDeclaration();
    }
    else if (type == "IfStatement")
    {
        statement = new JSASTIfStatement();
    }
    else if (type == "BlockStatement")
    {
        statement = new JSASTBlockStatement();
    }
    else if (type == "EmptyStatement")
    {
        statement = new JSASTBlockStatement();
    }
    else if (type == "ForStatement")
    {
        statement = new JSASTForStatement();
    }
    else if (type == "LabeledStatement")
    {
        statement = new JSASTLabeledStatement();
    }

    if (!statement)
    {
        LOGWARNINGF("Unknown JSASTStatement: %s", type.CString());
    }
    else
    {
        statement->Parse(value);
    }

    return statement;

}

JSASTExpression* JSASTNode::ParseExpression(const rapidjson::Value &value, JSASTType astType)
{
    if (!value.IsObject())
        return NULL;

    JSASTExpression* expr = NULL;

    const Value::Member* mtype = value.FindMember("type");
    assert(mtype);

    String type = mtype->value.GetString();
    if (type == "Identifier" && (astType == JSAST_UNDEFINED || astType == JSAST_IDENTIFIER))
    {
        expr = new JSASTIdentifier();
    }
    else if (type == "Literal" && (astType == JSAST_UNDEFINED || astType == JSAST_LITERAL))
    {
        expr = new JSASTLiteral();
    }
    else if (type == "UnaryExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_UNARYEXPRESSION))
    {
        expr = new JSASTUnaryExpression();
    }
    else if (type == "UpdateExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_UPDATEEXPRESSION))
    {
        expr = new JSASTUpdateExpression();
    }
    else if (type == "NewExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_NEWEXPRESSION))
    {
        expr = new JSASTNewExpression();
    }
    else if (type == "FunctionExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_FUNCTIONEXPRESSION))
    {
        expr = new JSASTFunctionExpression();
    }
    else if (type == "BinaryExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_BINARYEXPRESSION))
    {
        expr = new JSASTBinaryExpression();
    }
    else if (type == "AssignmentExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_ASSIGNMENTEXPRESSION))
    {
        expr = new JSASTAssignmentExpression();
    }
    else if (type == "LogicalExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_LOGICALEXPRESSION))
    {
        expr = new JSASTAssignmentExpression();
    }
    else if (type == "CallExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_CALLEXPRESSION))
    {
        expr = new JSASTCallExpression();
    }
    else if (type == "VariableDeclarator" && (astType == JSAST_UNDEFINED || astType == JSAST_VARIABLEDECLARATOR))
    {
        expr = new JSASTVariableDeclarator();
    }
    else if (type == "MemberExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_MEMBEREXPRESSION))
    {
        expr = new JSASTMemberExpression();
    }
    else if (type == "ArrayExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_ARRAYEXPRESSION))
    {
        expr = new JSASTArrayExpression();
    }
    else if (type == "ObjectExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_OBJECTEXPRESSION))
    {
        expr = new JSASTObjectExpression();
    }
    else if (type == "ConditionalExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_CONDITIONALEXPRESSION))
    {
        expr = new JSASTConditionalExpression();
    }
    else if (type == "ThisExpression" && (astType == JSAST_UNDEFINED || astType == JSAST_THISEXPRESSION))
    {
        expr = new JSASTThisExpression();
    }

    if (!expr)
    {
        LOGWARNINGF("Unknown JSASTExpression: %s", type.CString());
    }
    else
    {
        expr->Parse(value);
    }

    return expr;

}

bool JSASTNode::ParseExpressionArray(const rapidjson::Value &value, Vector<JSASTExpression *> &expressions)
{

    assert(value.IsArray());

    for (Value::ConstValueIterator itr = value.Begin(); itr != value.End(); itr++)
    {
        JSASTExpression* expr = ParseExpression(*itr);
        //assert(expr);
        expressions.Push(expr);

    }

    return true;
}


bool JSASTNode::ParseStatementArray(const rapidjson::Value &value, Vector<JSASTStatement *> &statements)
{

    assert(value.IsArray());

    for (Value::ConstValueIterator itr = value.Begin(); itr != value.End(); itr++)
    {
        JSASTStatement* stmt = ParseStatement(*itr);
        //assert(stmt);
        statements.Push(stmt);

    }

    return true;
}

bool JSASTThisExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    return true;
}

void JSASTThisExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


bool JSASTIdentifier::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    if (value.FindMember("name"))
        name_ = value["name"].GetString();
    return true;
}

void JSASTIdentifier::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


bool JSASTAssignmentExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    operator_ = value["operator"].GetString();

    leftExpression_ = ParseExpression(value["left"]);
    rightExpression_ = ParseExpression(value["right"]);

    return true;
}

void JSASTAssignmentExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}

bool JSASTLogicalExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    operator_ = value["operator"].GetString();

    leftExpression_ = ParseExpression(value["left"]);
    rightExpression_ = ParseExpression(value["right"]);

    return true;
}

void JSASTLogicalExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}



bool JSASTBinaryExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    operator_ = value["operator"].GetString();

    leftExpression_ = ParseExpression(value["left"]);
    rightExpression_ = ParseExpression(value["right"]);

    return true;
}

void JSASTBinaryExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


bool JSASTLiteral::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    if (!value.FindMember("value") || !value.FindMember("raw"))
    {
        literalType_ = LITERAL_UNKNOWN;
        return false;
    }

    const rapidjson::Value& v = value["value"];

    raw_ = value["raw"].GetString();;

    if (v.IsNull())
        literalType_ = LITERAL_NULL;
    else if (v.IsString())
        literalType_ = LITERAL_STRING;
    else if (v.IsNumber())
        literalType_ = LITERAL_NUMBER;
    else if (v.IsBool())
        literalType_ = LITERAL_BOOLEAN;



    return true;
}

void JSASTLiteral::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


bool JSASTArrayExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    ParseExpressionArray(value["elements"], elements_);

    return true;
}

void JSASTArrayExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}

bool JSASTProperty::Parse(const rapidjson::Value& value)
{
    key_ = ParseExpression(value["key"]);
    assert(key_);

    value_ = ParseExpression(value["value"]);
    assert(value_);

    return true;
}

void JSASTProperty::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}

bool JSASTObjectExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    const rapidjson::Value& jprops = value["properties"];

    assert(jprops.IsArray());

    for (Value::ConstValueIterator itr = jprops.Begin(); itr != jprops.End(); itr++)
    {

        JSASTProperty* property = new JSASTProperty();
        property->Parse(*itr);
        properties_.Push(property);

    }


    return true;
}

void JSASTObjectExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


bool JSASTMemberExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    computed_ = value["computed"].GetBool();
    object_ = ParseExpression(value["object"]);
    property_ = ParseExpression(value["property"]);

    return true;
}

void JSASTMemberExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}



bool JSASTCallExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    callee_ = ParseExpression(value["callee"]);

    ParseExpressionArray(value["arguments"], arguments_);

    return true;

}

void JSASTCallExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


bool JSASTNewExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    callee_ = ParseExpression(value["callee"]);

    ParseExpressionArray(value["arguments"], arguments_);

    return true;

}

void JSASTNewExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


bool JSASTVariableDeclarator::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    id_ = ParseExpression(value["id"]);

    const Value::Member* init = value.FindMember("init");
    if (init && init->value.IsObject())
    {
        init_ = ParseExpression(init->value);
    }

    return true;

}

void JSASTVariableDeclarator::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


bool JSASTUnaryExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    argument_ = ParseExpression(value["argument"]);

    prefix_ = value["prefix"].IsTrue();

    operator_ = value["operator"].GetString();

    return true;

}

void JSASTUnaryExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}

bool JSASTUpdateExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    argument_ = ParseExpression(value["argument"]);

    prefix_ = value["prefix"].IsTrue();

    operator_ = value["operator"].GetString();

    return true;

}

void JSASTUpdateExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}

bool JSASTConditionalExpression::Parse(const rapidjson::Value& value)
{
    JSASTExpression::Parse(value);

    test_ = ParseExpression(value["test"]);
    consequent_ = ParseExpression(value["consequent"]);
    alternate_ = ParseExpression(value["alternate"]);

    return true;
}

void JSASTConditionalExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}



bool JSASTFunctionExpression::Parse(const rapidjson::Value& value)
{

    JSASTExpression::Parse(value);

    if (!value["id"].IsNull())
    {
        id_ = (JSASTIdentifier*) ParseExpression(value["id"],  JSAST_IDENTIFIER);
    }

    ParseExpressionArray(value["params"], params_);

    const rapidjson::Value& body = value["body"];

    String type = body["type"].GetString();

    if (type == "BlockStatement")
    {
        body_ = (JSASTBlockStatement*) ParseStatement(body);
    }
    else
    {
        return true;
    }


    return true;
}

void JSASTFunctionExpression::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


JSASTFunctionExpression::~JSASTFunctionExpression()
{
    if (id_)
        delete id_;
    if (rest_)
        delete rest_;
    if (body_)
        delete body_;



    for (unsigned i = 0; i < params_.Size(); i++)
        delete params_[i];
    for (unsigned i = 0; i < defaults_.Size(); i++)
        delete defaults_[i];

    params_.Clear();
    defaults_.Clear();
}

// STATEMENTS

bool JSASTExpressionStatement::Parse(const rapidjson::Value& value)
{
    JSASTStatement::Parse(value);

    const Value::Member* expr = value.FindMember("expression");
    assert(expr);

    expression_ = ParseExpression(expr->value);

    return expression_ != NULL;

}

void JSASTLabeledStatement::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}

bool JSASTLabeledStatement::Parse(const rapidjson::Value& value)
{
    JSASTStatement::Parse(value);

    const Value::Member* body = value.FindMember("body");
    assert(body);

    const Value::Member* label = value.FindMember("label");
    assert(label);


    body_ = ParseStatement(body->value);
    label_ = (JSASTIdentifier*) ParseExpression(label->value,  JSAST_IDENTIFIER);

    return body_ && label_;

}

void JSASTExpressionStatement::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


bool JSASTReturnStatement::Parse(const rapidjson::Value& value)
{
    JSASTStatement::Parse(value);

    if (!value["argument"].IsNull())
    {
        argument_ = ParseExpression(value["argument"]);
    }


    return true;
}

void JSASTReturnStatement::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}



bool JSASTVariableDeclaration::Parse(const rapidjson::Value& value)
{
    JSASTDeclaration::Parse(value);

    kind_ = value["kind"].GetString();

    ParseExpressionArray(value["declarations"], declarations_);

    return true;

}

void JSASTVariableDeclaration::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


JSASTComment::JSASTComment() : JSASTNode(JSAST_COMMENT) , isBlock_(false)
{

}

JSASTComment::~JSASTComment()
{

}

bool JSASTComment::Parse(const rapidjson::Value& value)
{
    JSASTNode::Parse(value);

    // Block or Line
    if (value.FindMember("type"))
    {
        String type = value["type"].GetString();
        isBlock_ = type == "Block";
    }

    if (value.FindMember("value"))
    {
        value_ = value["value"].GetString();
    }

    return true;
}

void JSASTComment::Accept(JSASTVisitor *visitor)
{
    visitor->visit(this);
}



JSASTProgram::JSASTProgram(const String &path, const String &json) : JSASTNode(JSAST_PROGRAM)
  , document_(new Document())
{

    if (document_->Parse<0>(json.CString()).HasParseError())
    {
        LOGERRORF("Could not parse JSON data from %s", path.CString());
        return;
    }
    else
    {
        //StringBuffer buffer;
        //PrettyWriter<StringBuffer> writer(buffer, &(document_->GetAllocator()));
        //writer.SetIndent(' ', 3);
        //document_->Accept(writer);
        //LOGINFOF("%s", buffer.GetString());
    }

    Parse(*document_);

}



bool JSASTProgram::Parse(const rapidjson::Value& value)
{
    JSASTNode::Parse(value);

    for (Value::ConstMemberIterator itr = value.MemberBegin();
         itr != value.MemberEnd(); ++itr)
    {
        String name = itr->name.GetString();

        if (name == "body")
        {
            ParseStatementArray(itr->value, body_);
        }

        if (name == "comments")
        {
            if (itr->value.IsArray())
            {
                for (Value::ConstValueIterator citr = itr->value.Begin();
                     citr != itr->value.End();
                     citr++)
                {
                    JSASTComment* comment = new JSASTComment();
                    assert(citr->IsObject());
                    comment->Parse(*citr);
                    comments_.Push(comment);
                }
            }
        }

    }

    return true;

}

JSASTProgram::~JSASTProgram()
{
    for (unsigned i = 0; i < comments_.Size(); i++)
        delete comments_[i];

    comments_.Clear();

    delete document_;
    document_ = 0;
}

void JSASTProgram::Accept(JSASTVisitor *visitor)
{
    visitor->visit(this);
}


JSASTProgram* JSASTProgram::ParseFromJSON(const String& path, const String& json)
{
    JSASTProgram* cunit = new JSASTProgram(path, json);

    return cunit;
}

bool JSASTIfStatement::Parse(const rapidjson::Value& value)
{
    JSASTStatement::Parse(value);

    test_ = ParseExpression(value["test"]);
    consequent_ = ParseStatement(value["consequent"]);

    if (!value["alternate"].IsNull())
        alternate_ = ParseStatement(value["alternate"]);

    return true;
}

void JSASTIfStatement::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


bool JSASTFunctionDeclaration::Parse(const rapidjson::Value& value)
{
    JSASTDeclaration::Parse(value);

    if (!value["id"].IsNull())
    {
        id_ = (JSASTIdentifier*) ParseExpression(value["id"],  JSAST_IDENTIFIER);
    }

    ParseExpressionArray(value["params"], params_);

    const rapidjson::Value& body = value["body"];

    String type = body["type"].GetString();

    if (type == "BlockStatement")
    {
        body_ = (JSASTBlockStatement*) ParseStatement(body);
    }
    else
    {

    }


    return true;
}

void JSASTFunctionDeclaration::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}

JSASTFunctionDeclaration::~JSASTFunctionDeclaration()
{
    if (id_)
        delete id_;
    if (rest_)
        delete rest_;
    if (body_)
        delete body_;

    for (unsigned i = 0; i < params_.Size(); i++)
        delete params_[i];
    for (unsigned i = 0; i < defaults_.Size(); i++)
        delete defaults_[i];

    params_.Clear();
    defaults_.Clear();
}

bool JSASTBlockStatement::Parse(const rapidjson::Value& value)
{
    if (value.FindMember("body"))
    {
        ParseStatementArray(value["body"], body_);

    }

    return true;

}

void JSASTBlockStatement::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}

bool JSASTForStatement::Parse(const rapidjson::Value& value)
{
    JSASTStatement::Parse(value);

    if (value.FindMember("init") && !value.FindMember("init")->value.IsNull())
    {
        const Value::Member* member = value.FindMember("init");

        String type = member->value.FindMember("type")->value.GetString();
        if (type == "VariableDeclaration")
        {
            initVariable_ = (JSASTVariableDeclaration*) ParseStatement(member->value);
        }
        else
        {
            initExpression_ = ParseExpression(member->value);
        }
    }

    if (value.FindMember("test") && !value.FindMember("test")->value.IsNull())
    {
        test_ = ParseExpression(value["test"]);
    }

    if (value.FindMember("update") && !value.FindMember("update")->value.IsNull())
    {
        update_ = ParseExpression(value["update"]);
    }

    if (value.FindMember("body"))
    {
        body_ = ParseStatement(value["body"]);
    }

    return true;
}

void JSASTForStatement::Accept(JSASTVisitor* visitor)
{
    visitor->visit(this);
}


}
