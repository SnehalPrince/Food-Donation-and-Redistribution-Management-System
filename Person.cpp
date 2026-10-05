#include "Person.h"

// [ABSTRACTION]
Person::Person(const std::string& id, const std::string& name, const std::string& contact)
    : m_id(id), m_name(name), m_contact(contact) {}
