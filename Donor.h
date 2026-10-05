#ifndef DONOR_H
#define DONOR_H

#include "Person.h"

// [INHERITANCE]
// Represents a food donor inheriting from Person.
class Donor : public Person {
private:
    // [ENCAPSULATION]
    std::string m_source;

public:
    Donor(const std::string& id, const std::string& name, const std::string& contact, const std::string& source);
    virtual ~Donor() {}

    const std::string& getSource() const { return m_source; }
    void setSource(const std::string& source) { m_source = source; }

    // [POLYMORPHISM]
    void display() const override;
};

#endif // DONOR_H
