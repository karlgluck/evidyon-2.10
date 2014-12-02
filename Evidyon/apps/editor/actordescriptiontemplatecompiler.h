#ifndef __ACTORDESCRIPTIONTEMPLATECOMPILER_H__
#define __ACTORDESCRIPTIONTEMPLATECOMPILER_H__


#include "../shared/actordescriptiontemplate.h"

namespace dc {
class dcStreamOut;
}

#include <vector>

class ActorDescriptionTemplateCompiler {
  typedef std::vector<Evidyon::ClientActorDescriptionTemplate> ClientDescriptions;
  typedef std::vector<Evidyon::ServerActorDescriptionTemplate> ServerDescriptions;
public:

  // Resets all of the descriptions, then adds the default description:  index 0 is always
  // a blank description with default entries that indicates "get this data from the server"
  // unnecessary?
  //void initialize();

  // Appends a new description template
  void addActorDescriptionTemplate(const Evidyon::ClientActorDescriptionTemplate* client_description,
                                   const Evidyon::ServerActorDescriptionTemplate* server_description);

  // Returns the array index at which the next call to addActorDescriptionTemplate will put
  // its parameter.  This is used to reference the description.
  int getNextTemplateIndex() const;

  // writes out these descriptions into the stream
  bool compileForClient(dc::dcStreamOut* stream);
  bool compileForServer(dc::dcStreamOut* stream);


private:
  ClientDescriptions client_descriptions_;
  ServerDescriptions server_descriptions_;
};



#endif
