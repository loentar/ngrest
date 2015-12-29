#ifndef NGREST_SERVICEGROUP_H
#define NGREST_SERVICEGROUP_H

#include <string>
#include <vector>

namespace ngrest {

class ServiceWrapper;

/**
 * @brief Group of services to deploy from plugin
 */
class ServiceGroup
{
public:
    ServiceGroup();
    virtual ~ServiceGroup();

    /**
     * @brief gets name of service group
     * @return name of service group
     */
    virtual std::string getName() = 0;

    /**
     * @brief get list of service wrappers provided by group
     * @return list of service wrappers
     */
    virtual const std::vector<ServiceWrapper*>& getServices() = 0;
};

} // namespace ngrest

#endif // NGREST_SERVICEGROUP_H
