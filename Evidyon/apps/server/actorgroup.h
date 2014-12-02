#ifndef __ACTORGROUP_H__
#define __ACTORGROUP_H__

#include "../shared/networkdecl.h"

namespace Evidyon {

class Actor;

// An actor group is a *SMALL* collection of actors: a raid party, for example
class ActorGroup {
public:
  static const int MAX_MEMBERS = 16;

public:
  ActorGroup();

  int findMembersNear(Actor* source, float radius_squared,
                      Actor** actors, int buffer_size);

  // Returns actors that are enemies of 'enemies_of_actor' that are near (x,y)
  // and are not members of this group.
  int findEnemiesNear(Actor* enemies_of_actor, float x, float y,
                      float radius_squared, Actor** actors,
                      int buffer_size);

  bool isGroupMember(Actor* actor) const;

  // If a group is open, new members can join
  void setGroupOpen(bool is_open);

  // Tries to put a new member in this group.  Returns 'true' if the member was
  // successfully added.  A member can't be added to a group that is full.
  // If removeMember erases the last entry in the group, it will deallocate that
  // group--so after calling removeMember, it is not valid to call any other
  // method on this group!
  // These methods do NOT call Actor::setGroup(), since that method invokes
  // these ones!
  bool addMember(Actor* actor);
  void removeMember(Actor* actor);

  // Sends a packet on the secondary channel to each client whose actor is a member
  // of this class.
  void sendToClients(NetworkPacket* packet);

  // Distributes exp gained by one character among all nearby clients in this group
  void shareExp(Actor* source, int amount);

public:
  // Allocates a new group and adds the given actor as its founder.  The group will
  // be erased when it has no more members.  This method sets the founder's group
  // to be the newly created group automatically.
  static ActorGroup* createGroup(Actor* founder);

  // Adds the follower to the leader's group.  If the leader doesn't have an active
  // ActorGroup, this method creates one.  If the return value is 'false', neither
  // actors' groups will have changed.  'true' indicates the follower successfully
  // joined the leader's group.
  static bool joinGroup(Actor* actor_leader, Actor* actor_follower);


private:
  Actor* members_[MAX_MEMBERS];
  int number_of_members_;

  // New members can only be added if the group is open.  Groups
  // default to being open.
  bool is_open_;
};

}

#endif