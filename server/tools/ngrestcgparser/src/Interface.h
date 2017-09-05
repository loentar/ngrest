/*
 *  Copyright 2016 Utkin Dmitry <loentar@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of ngrest: http://github.com/loentar/ngrest
 */

#ifndef NGREST_CODEGEN_INTERFACE_H
#define NGREST_CODEGEN_INTERFACE_H

#include <string>
#include <list>
#include <map>
#include "ngrestcodegenparserexport.h"

namespace ngrest {
namespace codegen {

typedef std::list<std::string> StringList; //!< string list
typedef std::map<std::string, std::string> StringMap; //!< string map

//! data type
struct NGREST_CODEGENPARSER_EXPORT DataType
{
    enum class Type  //! data type enum
    {
        Unknown,        //!<  unknown
        Generic,        //!<  generic type
        String,         //!<  string type
        DataObject,     //!<  DataObject
        Enum,           //!<  enum
        Struct,         //!<  struct
        Typedef,        //!<  typedef
        Template        //!<  template container (list, map, etc)
    };

    bool isConst = false;            //!<  const type
    bool isRef = false;              //!<  reference type
    Type type = Type::Generic;       //!<  data type enum
    std::string usedName;            //!<  name as it used
    std::string prefix;              //!<  prefix
    std::string name;                //!<  type name (int, string, etc.)
    std::string ns;                  //!<  namespace
    std::string ownerName;           //!<  owner struct name in format "StructName[::SubType]"
    std::list<DataType> params;      //!<  template parameters
};

//!  parameter
struct NGREST_CODEGENPARSER_EXPORT Param
{
    DataType dataType;         //!<  param data type
    std::string name;          //!<  param name
    std::string description;   //!<  param description
    std::string details;       //!<  detailed description
    StringMap options;         //!<  param metacomments options
};

//!  service operation
struct NGREST_CODEGENPARSER_EXPORT Operation
{
    Param returnType;          //!<  return type
    std::string name;          //!<  operation name
    std::list<Param> params;   //!<  parameters
    bool isConst = false;      //!<  operation is const
    std::string description;   //!<  operation description
    std::string details;       //!<  detailed description
    bool isAsynch = false;     //!<  operation is asynchronous(client)
    StringMap options;         //!<  operation metacomments options
};


//! base type for data types
struct NGREST_CODEGENPARSER_EXPORT BaseType
{
    //!< base type enum
    enum Type
    {
        Unknown = 0, //!< unknown/unintialized
        Enum    = 1, //!< type is enum
        Struct  = 2, //!< type is struct
        Typedef = 4, //!< type is typedef
        Service   = 8, //!< service
        Any = Enum | Struct | Typedef //!< any type
    };

    const Type type;          //!<  type
    std::string name;         //!<  name
    std::string ns;           //!<  namespace
    std::string ownerName;    //!<  owner struct name in format "StructName[::SubType]"
    std::string description;  //!<  service description
    std::string details;      //!<  detailed description
    bool isExtern = false;    //!<  imported from other interface
    bool isForward = true;    //!<  is forward declaration
    StringMap options;        //!<  metacomments options

    BaseType(Type type);
    BaseType& operator=(const BaseType& other);
};

//! enum
struct NGREST_CODEGENPARSER_EXPORT Enum: public BaseType
{
    //! enum operation
    struct Member
    {
        std::string name;         //!<  enum operation name
        std::string value;        //!<  enum operation value (optional)
        std::string description;  //!<  enum operation description
    };

    std::list<Member> members;    //!<  members

    Enum();
    Enum& operator=(const Enum& other);
};

//!  structure field
struct NGREST_CODEGENPARSER_EXPORT Field: public Param
{
};

//! struct
struct NGREST_CODEGENPARSER_EXPORT Struct: public BaseType
{
    std::string parentName;          //!<  parent struct name (with namespace as used)
    std::string parentNs;            //!<  parent struct namespace (actual)
    std::list<Field> fields;         //!<  struct fields
    std::list<Struct> structs;       //!<  sub struct list
    std::list<struct Enum> enums;    //!<  sub enum list

    Struct();
    Struct& operator=(const Struct& other);
};

//! typedef
struct NGREST_CODEGENPARSER_EXPORT Typedef: public BaseType
{
    DataType dataType;     //!<  defined data type

    Typedef();
    Typedef& operator=(const Typedef& other);
};

//! service class
struct NGREST_CODEGENPARSER_EXPORT Service: public BaseType
{
    StringList modules;               //!<  list of modules to engage
    std::list<Operation> operations;  //!<  service operations

    Service();
    Service& operator=(const Service& other);
};

//! include info
struct NGREST_CODEGENPARSER_EXPORT Include
{
    std::string interfaceName;    //!<  interface name
    std::string ns;               //!<  interface namespace
    std::string fileName;         //!<  file name
    std::string filePath;         //!<  file path
};

//! service interface
struct NGREST_CODEGENPARSER_EXPORT Interface
{
    std::string name;             //!<  interface name, based on input filename
    std::string ns;               //!<  interface main namespace
    std::string fileName;         //!<  input file name
    std::string filePath;         //!<  input file path
    StringMap options;            //!<  metacomments options
    std::list<Include> includes;  //!<  included files
    std::list<Enum> enums;        //!<  enum list
    std::list<Typedef> typedefs;  //!<  typedef list
    std::list<Struct> structs;    //!<  struct list
    std::list<Service> services;  //!<  services list
};

//! project
struct NGREST_CODEGENPARSER_EXPORT Project
{
    std::string name;                   //!<  project name
    std::string ns;                     //!<  project namespace
    std::list<Interface> interfaces;    //!<  interface list
};

}
}

#endif // NGREST_CODEGEN_INTERFACE_H
