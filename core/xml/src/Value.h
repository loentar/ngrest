#ifndef NGREST_XML_VALUE_H
#define NGREST_XML_VALUE_H

#include <string>
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

  typedef char byte;
  typedef unsigned char unsignedByte;

  //! xml value
  class NGREST_XML_EXPORT Value
  {
  public:
    //! default constructor
    Value();

    //! initializing constructor
    /*! \param  other - value
        */
    explicit Value(const Value& other);

    //! initializing constructor
    /*! \param  value - value
        */
    Value(const std::string& value);

    //! initializing constructor
    /*! \param  value - value
    */
    Value(const char* value);

    //! initializing constructor
    /*! \param  value - value
    */
    Value(float value);

    //! initializing constructor
    /*! \param  value - value
        */
    Value(double value);

    //! initializing constructor
    /*! \param  value - value
    */
    Value(byte value);

    //! initializing constructor
    /*! \param  value - value
        */
    Value(int value);

    //! initializing constructor
    /*! \param  value - value
    */
    Value(short value);

    //! initializing constructor
    /*! \param  value - value
        */
    Value(long value);

    //! initializing constructor
    /*! \param  value - value
    */
    Value(long long value);

    //! initializing constructor
    /*! \param  value - value
    */
    Value(unsignedByte value);

    //! initializing constructor
    /*! \param  value - value
    */
    Value(unsigned int value);

    //! initializing constructor
    /*! \param  value - value
    */
    Value(unsigned short value);

    //! initializing constructor
    /*! \param  value - value
    */
    Value(unsigned long value);

    //! initializing constructor
    /*! \param  value - value
    */
    Value(unsigned long long value);

    //! initializing constructor
    /*! \param  value - value
        */
    Value(bool value);


    //! copy operator
    /*! \param  other - value
        */
    Value& operator=(const Value& other);

    //! copy operator
    /*! \param  value - value
        */
    Value& operator=(const std::string& value);

    //! copy operator
    /*! \param  value - value
        */
    Value& operator=(const char* value);

    //! copy operator
    /*! \param  value - value
        */
    Value& operator=(float value);

    //! copy operator
    /*! \param  value - value
        */
    Value& operator=(double value);

    //! copy operator
    /*! \param  value - value
    */
    Value& operator=(byte value);

    //! copy operator
    /*! \param  value - value
    */
    Value& operator=(int value);

    //! copy operator
    /*! \param  value - value
    */
    Value& operator=(short value);

    //! copy operator
    /*! \param  value - value
    */
    Value& operator=(long value);

    //! copy operator
    /*! \param  value - value
    */
    Value& operator=(long long value);

    //! copy operator
    /*! \param  value - value
        */
    Value& operator=(unsignedByte value);

    //! copy operator
    /*! \param  value - value
        */
    Value& operator=(unsigned int value);

    //! copy operator
    /*! \param  value - value
        */
    Value& operator=(unsigned short value);

    //! copy operator
    /*! \param  value - value
        */
    Value& operator=(unsigned long value);

    //! copy operator
    /*! \param  value - value
    */
    Value& operator=(unsigned long long value);

    //! copy operator
    /*! \param  value - value
        */
    Value& operator=(bool value);


    //! const value cast operator
    /*! \return casted const value
        */
    operator const std::string&() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator float() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator double() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator byte() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator int() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator short() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator long() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator long long() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator unsignedByte() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator unsigned int() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator unsigned short() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator unsigned long() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator unsigned long long() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator bool() const;


    //! value cast operator
    /*! \return value
        */
    operator std::string&();


    //! explicit conversion to const string
    /*! \return const string value
        */
    const std::string& asString() const;

    //! explicit conversion to string
    /*! \return string value
        */
    std::string& asString();

    //! test target value for equality with specified value
    /*! \param  value - other value
        \return true, if values are equals
        */
    bool operator==(const Value& value) const;

  private:
    std::string value;         //!<  string value
  };

}
}

#endif // NGREST_XML_VALUE_H
