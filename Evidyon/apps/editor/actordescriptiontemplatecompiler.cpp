#include "actordescriptiontemplatecompiler.h"
#include <dc/filestream>
#include "../shared/actorid.h"

#include <dc/debug>


void ActorDescriptionTemplateCompiler::addActorDescriptionTemplate(
    const Evidyon::ClientActorDescriptionTemplate* client_description,
    const Evidyon::ServerActorDescriptionTemplate* server_description) {
  client_descriptions_.push_back(*client_description);
  server_descriptions_.push_back(*server_description);
}

int ActorDescriptionTemplateCompiler::getNextTemplateIndex() const {
  ASSERT(server_descriptions_.size() == client_descriptions_.size());
  return static_cast<int>(client_descriptions_.size());
}

bool ActorDescriptionTemplateCompiler::compileForClient(dc::dcStreamOut* stream) {
  ASSERT(server_descriptions_.size() == client_descriptions_.size()) else return false;
  {
    size_t elements = client_descriptions_.size();
    if (APP_FATAL(elements >= Evidyon::MAX_ACTOR_DESCRIPTION_TEMPLATES)
                 ("There are too many unique actor description templates to be correctly "\
                  "encoded into actors' ID numbers.  Increase MAX_ACTOR_DESCRIPTION_TEMPLATES_BITS "\
                  "then rebuild Evidyon with more available templates.")) {
      return false;
    }
    CONFIRM(stream->write(&elements, sizeof(elements))) else return false;
  }
  {
    for (ClientDescriptions::iterator i = client_descriptions_.begin();
         i != client_descriptions_.end(); ++i) {
      const Evidyon::ClientActorDescriptionTemplate* description = &(*i);
      CONFIRM(stream->write(description,
                            sizeof(Evidyon::ClientActorDescriptionTemplate))) else return false;
    }
  }

  return true;
}

bool ActorDescriptionTemplateCompiler::compileForServer(dc::dcStreamOut* stream) {
  ASSERT(server_descriptions_.size() == client_descriptions_.size()) else return false;
  {
    size_t elements = server_descriptions_.size();
    if (APP_FATAL(elements >= Evidyon::MAX_ACTOR_DESCRIPTION_TEMPLATES)
                 ("There are too many unique actor description templates to be correctly "\
                  "encoded into actors' ID numbers.  Increase MAX_ACTOR_DESCRIPTION_TEMPLATES_BITS "\
                  "then rebuild Evidyon with more available templates.")) {
      return false;
    }
    CONFIRM(stream->write(&elements, sizeof(elements))) else return false;
  }
  {
    for (ServerDescriptions::iterator i = server_descriptions_.begin();
         i != server_descriptions_.end(); ++i) {
      const Evidyon::ServerActorDescriptionTemplate* description = &(*i);
      CONFIRM(stream->write(description,
                            sizeof(Evidyon::ServerActorDescriptionTemplate))) else return false;
    }
  }

  return true;
}
