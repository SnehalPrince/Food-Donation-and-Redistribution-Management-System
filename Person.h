#ifndef PERSON_H
#define PERSON_H

#include <string>

// [ABSTRACTION]
// Abstract base class representing any entity (donor, recipient) in the food bank system.
class Person {
private:
    // [ENCAPSULATION]
    std::string m_id;
    std::string m_name;
    std::string m_contact;

public:
    Person(const std::string& id, const std::string& name, const std::string& contact);
    virtual ~Person() {}

    const std::string& getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    const std::string& getContact() const { return m_contact; }

    void setName(const std::string& name) { m_name = name; }
    void setContact(const std::string& contact) { m_contact = contact; }

    // [POLYMORPHISM]
    virtual void display() const = 0;
};

#endif // PERSON_H
