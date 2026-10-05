#ifndef RECIPIENT_H
#define RECIPIENT_H

#include "Person.h"

// [INHERITANCE]
// Represents a recipient organization (shelter, NGO, community kitchen) inheriting from Person.
class RecipientOrganization : public Person {
private:
    // [ENCAPSULATION]
    std::string m_location;

public:
    RecipientOrganization(const std::string& id, const std::string& name, const std::string& contact, const std::string& location);
    virtual ~RecipientOrganization() {}

    const std::string& getLocation() const { return m_location; }
    void setLocation(const std::string& location) { m_location = location; }

    // [POLYMORPHISM]
    void display() const override;
};

#endif // RECIPIENT_H
