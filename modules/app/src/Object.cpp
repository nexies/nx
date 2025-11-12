//
// Created by nexie on 09.11.2025.
//

#include "app/Object.hpp"


using namespace nx;

class Object::Impl {
    friend class Object;

    Impl(const std::string & objectName) :
        m_objectName(objectName)
    { }
    std::string m_objectName;
};

Object::Object() :
    _impl(new Impl(""))
{
}

Object::Object(const std::string &name) :
    _impl(new Impl(name))
{
}

std::string Object::objectName() const {
    return _impl->m_objectName;
}

void Object::setObjectName(const std::string & name) {
    if (name != objectName())
        // TODO:
        // emit(objectNameChanged, name);
    _impl->m_objectName = name;
}

EventLoop * Object::_getThisEventLoop() {
    return nullptr;
}
