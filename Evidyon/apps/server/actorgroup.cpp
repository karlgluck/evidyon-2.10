#include "actorgroup.h"
#include "actor.h"
#include "../common/null.h"
#include "globalactormanager.h"
#include "worldregion.h"
#include <dc/debug>

// for sending group messages
#include "encodepackets.h"
#include "globalnetworkmanager.h"

namespace Evidyon {


ActorGroup::ActorGroup() {
  number_of_members_ = 0;
  for (int i = 0; i < MAX_MEMBERS; ++i) {
    members_[i] = NULL;
  }
  is_open_ = true;
}

int ActorGroup::findMembersNear(Actor* source, float radius_squared,
                Actor** actors, int buffer_size) {

  // Loop through all group members that are not the source and pull out the
  // ones that are within the rage of the source actor.
  int members_found = 0;
  for (int i = 0; i < number_of_members_ && members_found < buffer_size; ++i) {
    Actor* actor = members_[i];
    if (actor == source) continue;
    if (actor->distanceToSq(source) < radius_squared) {
      *(actors++) = actor;
      ++members_found;
    }
  }
  return members_found;
}

int ActorGroup::findEnemiesNear(Actor* enemies_of_actor, float x, float y,
                float radius_squared, Actor** actors,
                int buffer_size) {
  // This temporary buffer is large enough to include the full set of group
  // members, just in case the group is mixed with non-group enemies
  // and some members of the group are normally enemies of each other.
  Actor* temp[MAX_MEMBERS + GlobalActorManager::STD_ACTOR_SEARCH_BUFFER_SIZE];
  int actors_to_find = MAX_MEMBERS +
    buffer_size > GlobalActorManager::STD_ACTOR_SEARCH_BUFFER_SIZE ?
    GlobalActorManager::STD_ACTOR_SEARCH_BUFFER_SIZE : buffer_size;

  int actors_found =
    enemies_of_actor->getRegion()->findActorEnemiesNearby(enemies_of_actor,
                                                          x,
                                                          y,
                                                          radius_squared,
                                                          temp,
                                                          actors_to_find);

  // Copy non-group members into the buffer
  int enemies_found = 0;
  for (int i = 0; i < actors_found && enemies_found < buffer_size; ++i) {
    if (temp[i]->getGroup() != this) {
      *(actors++) = temp[i];
      ++enemies_found;
    }
  }

  return enemies_found;
}


bool ActorGroup::isGroupMember(Actor* actor) const {
  return actor != NULL && actor->getGroup() == this;
}

void ActorGroup::setGroupOpen(bool is_open) {
  is_open_ = is_open;
}



bool ActorGroup::addMember(Actor* actor) {
  if (!is_open_ || number_of_members_ >= MAX_MEMBERS) return false;
  members_[number_of_members_++] = actor;

  //// Create a message
  //char joined[MAX_SPOKEN_TEXT_CHARACTERS];
  //sprintf_s(joined, MAX_SPOKEN_TEXT_CHARACTERS, "%s joined", actor->getName());
  //NetworkPacket* packet = Evidyon::Network::Packet::Server::Encode::actorSpeaks(-1, "[Group]", joined);
  //sendToClients(packet);
  //GlobalNetworkManager::handleOrphanedPacket(packet);

  // Success
  return true;
}



void ActorGroup::removeMember(Actor* actor) {
  if (actor->getGroup() != this) return;

  for (int i = 0; i < number_of_members_; ++i) {
    if (members_[i] == actor) {
      --number_of_members_;
      if (number_of_members_ > i) {
        members_[i] = members_[number_of_members_];
      }
      members_[number_of_members_] = NULL;
      break;
    }
  }

  if (number_of_members_ == 0) {
    delete this;
  } else {
    //// Create a message
    //char left[MAX_SPOKEN_TEXT_CHARACTERS];
    //sprintf_s(left, MAX_SPOKEN_TEXT_CHARACTERS, "%s has left", actor->getName());
    //NetworkPacket* packet = Evidyon::Network::Packet::Server::Encode::actorSpeaks(-1, "[Group]", left);
    //sendToClients(packet);
    //GlobalNetworkManager::handleOrphanedPacket(packet);
  }
}



void ActorGroup::sendToClients(NetworkPacket* packet) {
  for (int i = 0; i < number_of_members_; ++i) {
    Evidyon::Client* client = members_[i]->getController()->getClient();
    if (client == NULL) continue;
    client->sendAsyncPacket(packet);
  }
}


void ActorGroup::shareExp(Actor* source, int amount) {
  for (int i = 0; i < number_of_members_; ++i) {
    Actor* member = members_[i];
    if (member == source || (member->distanceToSq(source) >= 16.0f * 16.0f)) continue;
    Evidyon::Client* client = members_[i]->getController()->getClient();
    if (client == NULL) continue;
    //client->changeEXP(amount/3);
  }
}


ActorGroup* ActorGroup::createGroup(Actor* founder) {
  ActorGroup* group = new ActorGroup();
  founder->setGroup(group);
  return group;
}



bool ActorGroup::joinGroup(Actor* actor_leader, Actor* actor_follower) {
  Evidyon::ActorGroup* group = actor_leader->getGroup();
  if (group == NULL) {
    ASSERT(createGroup(actor_leader)->addMember(actor_follower));
    return true;
  } else {
    return group->addMember(actor_follower);
  }
}



}