#ifndef RESOURCELIBRARY_H
#define RESOURCELIBRARY_H

class ResourceLibrary
{
public:
   virtual ~ResourceLibrary() {}
   virtual bool initialize() = 0;
   virtual bool connectToServer() = 0;
   virtual bool reserve() = 0;
};

#endif
