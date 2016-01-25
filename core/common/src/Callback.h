#ifndef NGREST_CALLBACK_H
#define NGREST_CALLBACK_H

namespace ngrest {

class Exception;

template <typename R>
class Callback
{
public:
    virtual ~Callback() {}
    virtual void success(R result) = 0;
    virtual void error(const Exception& error) = 0;
};

class VoidCallback
{
public:
    virtual ~VoidCallback() {}
    virtual void success() = 0;
    virtual void error(const Exception& error) = 0;
};

} // namespace ngrest

#endif // NGREST_CALLBACK_H
