#ifndef __PACKETS_H__
#define __PACKETS_H__

#include "../../common/enet/enet.h"
#include <string>

namespace Evidyon {
static const int MAX_ITEMS_VISIBLE_PER_LOCATION = 32;


namespace Network {
namespace Packet {

// Specifies the different kinds of messages that can be passed in a
// network packet.
enum Message {

  NETMSG_INVALID,

  NETMSG_TELLCLIENTVERSIONINFO,   // client informs server of its version information

  // client->server requests
  NETMSG_ASKFORACCOUNTNUMBER, // client wants an account number
  NETMSG_LOGINTOACCOUNT,
  NETMSG_CREATEACCOUNT,     // client wants to make a new account
  NETMSG_ACCESSACCOUNT,     // request to access an account
  NETMSG_LOGOUTACCOUNT,     // logs the client out of the account on the server
  NETMSG_CREATECHARACTER,     // attempts to create a new character
  NETMSG_DELETECHARACTER,     // attempts to delete a character on the account
  NETMSG_ASKCHARACTERNAME,    // gets the name of a character
  NETMSG_ASKACCOUNTINFO,    // gets the characters on this account
  NETMSG_ASKCHARACTERMAPLOCATION,   // gets the map location in the world where the character is; used to prime the map renderer before logging in

  NETMSG_ENTERWORLD,      // logs into the world with a character (reply is NETMSG_ENTERWORLD_SUCCEEDED or NETMSG_REQUESTACTIONRESULT)
  NETMSG_LEAVEWORLD,      // leaves the world
  NETMSG_REQUEST_ABILITY,     // use an ability (generic)
  NETMSG_CHANGEEQUIPPEDSTATE,   // client wants to move items from equipped->unequipped or vice-versa
  NETMSG_DROPINVENTORYITEMS,    // client wants to drop items

  NETMSG_REQUEST_MOVE,            // move avatar; change combat target actor
  NETMSG_REQUEST_MOVE_STREAMING,  // move avatar in the direction of the given coordinates, but don't stop there.  doesn't contain target actor ID

  NETMSG_ASKACTORDESCRIPTION,   // gets information about an actor, such as their mesh type

  NETMSG_ASKMYAVATARSTATS,    // requests the avatar's displayable statistics (hp/mp values, defense, level, str/agi...etc) from the server

  NETMSG_CHANGEITEMSTOTRADE,    // client wants to change its side of the current trade. full update. kills acceptance of trade.
  NETMSG_ACCEPTTRADE,     // client has accepted the current trade.  TODO: should probably add a hash code to give client's view of partner's trade.

  NETMSG_BAZAAR_LISTITEMSFORSALE, // reply is an inventory update
  NETMSG_BAZAAR_GETITEMSFORSALE,
  NETMSG_BAZAAR_BUYITEM,  // response is NETMSG_TELLYOURMONEY, NETMSG_INVENTORYUPDATE or NETMSG_REMOVEITEMLISTINGS
  NETMSG_BAZAAR_GETSELLERSTATUS,
  NETMSG_BAZAAR_WITHDRAWSELLERITEMS,  // array of listing_id values; reply is NETMSG_TELLSELLERSTATUS

  NETMSG_USEITEMABILITY,


  // server->client messages
  NETMSG_TELLSERVERVERSIONUPDATEINFO, // server tells client whether its version is valid

  NETMSG_CREATEACCOUNT_SUCCEEDED,   // creating an account never fails, unless something is wrong with the server
  NETMSG_LOGOUTACCOUNT_CONFIRMED,   // confirms that the user is now logged out of the account (also used to force client logouts when bumped from server)
  NETMSG_TELLACCOUNTINFO,     // server is telling the client account information, like days left and which characters are on the account
  NETMSG_REQUESTACTIONRESULT,   // provides a result code to a user request
  NETMSG_TELLCHARACTERNAME,     // gives the client the name of a character
  NETMSG_CREATECHARACTER_SUCCEEDED,   // if a character was created, the server replies with this message instead of NETMSG_REQUESTACTIONRESULT
  NETMSG_TELLCHARACTERMAPLOCATION,  // provides the map location where a character is located

  NETMSG_ENTERWORLD_SUCCEEDED,  // character logs in; tells the client its player's actor ID and other character-specific information
  NETMSG_LEAVEWORLD_CONFIRMED,  // player left the world successfully
  NETMSG_TELLACTORACTIVITY,     // tells the client what a given actor is doing and where it is (this controls the client's actor also!)
  NETMSG_DESTROYACTOR,    // erase an actor from the client
  NETMSG_ACTORSPEAKS,     // an actor says some text
  NETMSG_UPDATEYOUREFFECTAVAILABILITYMASK,  // tell the client that the set of abilities it can use has been modified

  NETMSG_TELLACTORDESCRIPTION,  // tells the client a given actor's description
  NETMSG_TELLYOURAVATARSTATS,   // updates the client's avatar's statistics


  // These messages are sent frequently from server->client to update the
  // client with the current world state.
  // The slow sync tells the client where all visible actors are in
  //   the world.  This message is sent 2-5 times per second.
  // The fast sync is much smaller and sent more frequently.  It informs the
  //   client of its own location in the world.
  NETMSG_SLOWSYNC,
  NETMSG_FASTSYNC,

  // Switches the map that the client is on
  NETMSG_CHANGEMAP,

  // An actor has been completely removed from the world
  NETMSG_ERASEACTORFROMWORLD,

  // Client wants to know what actors in the world that it has never seen
  // before look like.  Sent in response to NETMSG_SLOWSYNC when a client
  // has never seen an actor before.
  NETMSG_ASKACTORFULLDESCRIPTIONS,
  NETMSG_TELLACTORFULLDESCRIPTIONS,

  // Finds out what an actor's name is.  Most of the time, an actor's name
  // is specified by its template type and this command is avoided.
  NETMSG_ASKACTORNAME,
  NETMSG_TELLACTORNAME,

  NETMSG_CAST_SPELL,
  NETMSG_USE_ITEM,
  NETMSG_REQUEST_SPEAK,     // say something

  // Generates a magical special effect
  NETMSG_CREATEMAGICFX,
  NETMSG_CREATEPROJECTILE,    // creates a new projectile
  NETMSG_DESTROYPROJECTILE,     // destroys a projectile

  // Tells the client to create some items on the map
  NETMSG_TELLITEMSONMAPLOCATION,
  NETMSG_UPDATELOCALWORLDREIGON,

  // Updates specific indices in the client's inventory.  These messages
  // have the same structure, but the client should clear all existing
  // items from the inventory before setting the results of the second.
  NETMSG_UPDATEINVENTORY,
  NETMSG_TELLINVENTORY,

  // Manage the contents of this account's storage
  NETMSG_STORAGE_OPEN,      // gets the storage status; needs storage NPC
  NETMSG_STORAGE_TELL,      // response to "open"
  NETMSG_STORAGE_DEPOSIT,   // item from inventory -> storage; needs storage NPC
  NETMSG_STORAGE_WITHDRAW,  // item from storage -> inventory; needs storage NPC
  NETMSG_STORAGE_UPDATE,    // response to deposit/withdraw

  // Interaction with a geosid
  NETMSG_GEOSID_TELLGEONITE,  // tell the client how much geonite they have
  NETMSG_GEOSID_SACRIFICE,    // request to extract genite from items (from inventory).  can only be done at a crystal.
  //NETMSG_GEOSID_BIND,       // set this geosid as the one to which the client is bound

  // bazaar HAS NO PAGINATION!
  NETMSG_BAZAAR_GETLISTINGS,  // ask for the items that are listed in a category categories include "own listings", "all", "swords", ...
  NETMSG_BAZAAR_TELLLISTINGS, // updates the items on a given page.  echoes the params from GETLISTINGS.
  NETMSG_BAZAAR_SELLER_ADDLISTING,    // client puts item into bazaar
  NETMSG_BAZAAR_SELLER_REMOVELISTING, // client recalls previously listed item from bazaar
  NETMSG_BAZAAR_SELLER_REJECTOFFER,
  NETMSG_BAZAAR_SELLER_ACCEPTOFFER,

  NETMSG_BAZAAR_GETOFFERSFORLISTING,  // gets a list of offers that were made for an item (any item!)
  NETMSG_BAZAAR_TELLOFFERSFORLISTING, //
  NETMSG_BAZAAR_GETOFFERDETAILS,    // gets the details of an offer that was made for the client's items
  NETMSG_BAZAAR_TELLOFFERDETAILS,   //

  NETMSG_TELLGOLD,  // server updates client with amount of gold currently held

  NETMSG_MERCHANT_GETSALES,   // player wants to see merchant wares; param = NPC id
  NETMSG_MERCHANT_TELLSALES,  // server telling client what the merchant has; params = array of TradeItemDescription
  NETMSG_MERCHANT_SELLTO,     // sells inventory item to merchant; params = NPC id, inventory index
  NETMSG_MERCHANT_BUYFROM,    // purchases something from merchant;  params = NPC id, merchant index 

  // add messages for p2p trade

  // add messages for p2p gift
};




/**
* Various requests clients make of the server end in the server simply acknowledging that the
* request has been executed, or giving a status code of that request.  For example, logging into
* or out of an account, logging off of a character, creating an account, deleting a character,
* changing account password and more are all mutually exclusive and have similar replies.  Thus,
* it is most efficient to combine all of the server replies into a single message type with a
* result code.  This is the result code enumeration.
*/
enum RequestActionResult
{
  RAR_INVALID,
  RAR_SUCCEEDED,      // The requested action completed successfully
  RAR_FAILED_ACCOUNTDOESNTEXIST,  // The account doesn't exist
  RAR_FAILED_INCORRECTPASSWORD,   // The password for the account was incorrect
  RAR_FAILED_TOOMANYCHARACTERS,   // There are too many characters on the account to create a new one
  RAR_FAILED_NAMEALREADYEXISTS,   // The character's name has already been taken
  RAR_FAILED_SUSPICIOUS,    // The action wasn't completed, and has been flagged as suspicious activity (ex. delete character message for a character not on the account)
  RAR_FAILED_OUTOFACCOUNTTIME,  // No more time left on the account
  RAR_FAILED_ALREADYACTIVE,   // The account trying to be accessed is already active on the server
  RAR_FAILED_VERSIONOUTOFDATE,  // The client's version is outdated
};



/// These constants are used when encoding/decoding string values.  Any string in a message
/// sent to a peer is always prefixed by a 16-bit magic number and a 16-bit length.  These
/// values are concatenated and XORed with the constant STRING_MAGICLENGTHMASK.  When the
/// string is received, the value is XORed by the constant again to restore the original
/// values.  The magic number is checked; if it doesn't match, the message is discarded.  The
/// null-terminator is sent in all strings, as well.  The string routines check to make sure
/// that, at the specified number of characters into the buffer, the null-terminator is found.
static const unsigned long NETWORK_STRING_MAGICLENGTHMASK  = ((unsigned long)0xD156C79);
static const unsigned short NETWORK_STRING_MAGICNUMBER   = ((unsigned short)0x5346);

/// The number of bytes added to a transmitted string (in addition to the characters themselves)
static const size_t NETWORK_STRING_PADDING_BYTES  = (sizeof(unsigned long) + 1);


/// The number of bytes added to an encoded array
static const size_t NETWORK_ARRAY_PADDING_BYTES   = (sizeof(size_t));

/// The size of the network message structure that starts messages
//static const size_t NETWORK_MESSAGE_SIZE  = sizeof(Message);
static const size_t NETWORK_MESSAGE_SIZE  = 1;


/**
 * Writes the network message into the buffer
 */
char* encodeMessageType(Message message, char* buffer, const char* endOfBuffer);

/**
 * Reads the network message type from the packet.  This doesn't do anything to the packet.
 * If the packet is invalid or doesn't contain enough information to read the message, this
 * method returns false.
 */
bool decodeMessageType(ENetPacket* packet, Message* message);

/**
 * Reads the network message type from the provided buffer.  If anything is wrong with the source
 * buffer (it is null, the end is null, or is too short) then this method returns null.
 */
const char* decodeMessageType(const char* sourceBuffer, const char* endOfSourceBuffer, Message* message);

/**
 * Takes the indicated string and writes it into the given buffer, and returns a pointer to
 * the next unwritten byte in the buffer.  If the return value is null (0) then the buffer
 * is too small to contain the string.
 */
char* encodeString(const std::string& in, char* buffer, const char* endOfBuffer);

/**
 * Takes the indicated string and writes it into the given buffer, and returns a pointer to
 * the next unwritten byte in the buffer.  If the return value is null (0) then the buffer
 * is too small to contain the string.
 */
char* encodeString(const char* in, char* buffer, const char* endOfBuffer);

/**
 * Reads a string that was encoded with encodeString from the given buffer, and returns a
 * pointer to the next unread byte in the buffer.  If the source buffer was too small to read a
 * string or there was an error reading the string, the return value is null (0)
 */
const char* decodeString(const char* sourceBuffer, const char* endOfSourceBuffer, std::string* outString);

/**
 * Reads a string that was encoded with encodeString from the given buffer, and returns a
 * pointer to the next unread byte in the buffer.  If the source buffer was too small to read a
 * string or there was an error reading the string, the return value is null (0).  If the
 * destination buffer is too small to print the string that was read, as many characters as can
 * fit are read into the destination buffer.
 */
const char* decodeString(const char* sourceBuffer, const char* endOfSourceBuffer, char* outBuffer, size_t outBufferSize);


/**
 * Writes a structure's raw byte data to the given buffer
 */
template <typename T> char* encodeStructure(const T& structure, char* buffer, const char* endOfBuffer)
{
  if (!buffer || (buffer + sizeof(structure) > endOfBuffer)) return 0;
  memcpy_s(buffer, endOfBuffer - buffer, (const void*)&structure, sizeof(T));
  return buffer + sizeof(T);
}

// Casts a structure into a packed type, then writes it
template <typename T, typename E> char* encodePackedStructure(const T& structure, char*buffer, const char* endOfBuffer) {
  E packed_structure = (E)structure;
  if (!buffer || (buffer + sizeof(packed_structure) > endOfBuffer)) return 0;
  memcpy_s(buffer, endOfBuffer - buffer, (const void*)&structure, sizeof(packed_structure));
  return buffer + sizeof(packed_structure);
}

/**
 * Reads a structure's raw byte data from the given buffer
 */
template <typename T> const char* decodeStructure(const char* buffer, const char* endOfBuffer, T* out)
{
  if (!buffer || (buffer + sizeof(T) > endOfBuffer) || !out) return 0;
  memcpy_s(out, sizeof(T), buffer, sizeof(T));
  return buffer + sizeof(T);
}


template <typename T, typename E> const char* decodePackedStructure(const char* buffer, const char* endOfBuffer, T* out) {
  E packed_structure;
  if (!buffer || (buffer + sizeof(packed_structure) > endOfBuffer) || !out) return 0;
  memcpy_s(&packed_structure, sizeof(packed_structure), buffer, sizeof(packed_structure));
  *out = (T)packed_structure;
  return buffer + sizeof(packed_structure);
}


/**
 * Writes an array into the buffer
 */
template <typename E, typename T> char* encodeArray(E* arrayPointer, T arrayEntries, char* buffer, const char* endOfBuffer) {
  size_t array_bytes = arrayEntries * sizeof(E);
  if (!buffer || (buffer + sizeof(T) + array_bytes > endOfBuffer)) return 0;
  buffer = encodeStructure<T>(arrayEntries, buffer, endOfBuffer);
  if (arrayEntries) {
    memcpy_s(buffer, endOfBuffer - buffer, arrayPointer, array_bytes);
  }
  return buffer + array_bytes;
}

/**
 * Reads an array from the buffer
 */
template <typename E, typename T> const char* decodeArray(
    const char* buffer, const char* endOfBuffer,
    E* arrayBuffer, size_t arrayBufferEntries, T* arrayElements) {
  T entries;
  const char* internalBuffer = decodeStructure<T>(buffer, endOfBuffer, &entries);
  size_t array_bytes = entries * sizeof(E);
  if (!internalBuffer || (entries > arrayBufferEntries) ||
      (internalBuffer + array_bytes > endOfBuffer)) return 0;
  if (entries) {
    memcpy_s(arrayBuffer, arrayBufferEntries*sizeof(E), internalBuffer, array_bytes);
    internalBuffer += array_bytes;
  }
  *arrayElements = entries;
  return internalBuffer;
}

/**
 * Creates a packet with no extra data that holds only a message
 */
ENetPacket* encodeMessageOnlyPacket(Message message);

/**
 * Ensures that the packet in the parameter represents a message of the provided type
 * that was encoded by encodeMessageOnlyPacket
 */
bool decodeMessageOnlyPacket(ENetPacket* packet, Message messageToConfirm);


bool createPacket(bool reliable, Message message, size_t payload_size,
      ENetPacket** packet_out, char** start, const char** end);
bool validateAndTrimPacket(ENetPacket* packet, char* next_unwritten_byte);

bool openPacket(ENetPacket* packet, Message message, const char** payload, const char** end);

}
}
}

#endif