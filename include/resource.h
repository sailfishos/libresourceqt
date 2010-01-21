#ifndef RESOURCE_H
#define RESOURCE_H

#include <QString>

class Resource
{
public:
   Resource(const Resource &other);
   Resource();
   Resource &operator=(const Resource &other);

   ~Resource();

   QString type() const;
   void setType(const QString type);
   bool isOptional() const;
   void setOptional(bool resourceIsOptional=true);
   bool isShared() const;
   void setShared(bool shared=true);
   quint32 id() const;
   void setId(quint32 newId);

   bool operator==(const Resource &other);
private:
   QString name;
   bool optional;
   bool shared;
   quint32 identifier;
};

#endif
