#ifndef NGREST_CALLBACK_H
#define NGREST_CALLBACK_H

namespace ngrest {

template <typename R, typename E>
class Callback
{
public:
    virtual ~Callback() {}
    virtual void onSuccess(R result) = 0;
    virtual void onError(E error) = 0;
};

} // namespace ngrest

#endif // NGREST_CALLBACK_H
