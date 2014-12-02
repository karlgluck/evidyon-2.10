#ifndef __CLIENT_INLINE_H__
#define __CLIENT_INLINE_H__


NetworkPeer* Client::getPeer() {
  return peer_;
}

bool Client::isValid() const {
  return peer_ && (state_ != INVALID);
}

unsigned int Client::getLoggedIntoAccountID() const {
  return (state_ == LOGGED_INTO_ACCOUNT ||
          state_ == ENTERING_WORLD ||
          state_ == LOGGED_INTO_CHARACTER) ? account_id_ : 0;
}

unsigned int Client::getLoggedIntoCharacterID() const {
  return state_ == LOGGED_INTO_CHARACTER ? character_id_ : 0;
}

void Client::addToSlowSyncPacket(
  Evidyon::Network::Packet::ActorSyncData* actor_data) {
  slow_sync_encoder_.add(actor_data, peer_);
}
void Client::dispatchSlowSyncPacket() {
  slow_sync_encoder_.dispatch(peer_);
}

void Client::setRequest(ClientRequest type) {
  client_request_mask_ |= 1<<type;
}

const ActorPointer* Client::getActor() const {
  return &actor_;
}



#endif