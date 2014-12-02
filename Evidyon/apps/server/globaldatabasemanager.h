#ifndef __GLOBALDATABASEMANAGER_H__
#define __GLOBALDATABASEMANAGER_H__

#include <dcx/asynchronoussisothreaddatacontroller>
#include <string>

#include "../shared/evidyon_actor.h"
#include "itemidtype.h"
#include "uniqueavatarnamechecker.h"
#include "serverstorage.h"

typedef struct sqlite3 sqlite3;
typedef void *HANDLE;

namespace Evidyon {

struct AvatarAttributes;
struct AvatarInventoryItem;


namespace Bazaar {
typedef __int64 ListingID;

// Contains the information needed to display a complete item
struct ItemProperties {
  unsigned int type;
  unsigned int quantity;
};

namespace Server {

struct SellerListing {
  Bazaar::ListingID listing_id;
  ItemProperties item;
  bool open; // true -> not sold, false -> has been purchased for $$
  unsigned int price;
  int offers;
};

struct OpenListing {
  Bazaar::ListingID listing_id;
  ItemProperties item;
  unsigned int price;
  int offers;
};

struct BuyerOfferElement {
  ItemProperties item;
};

struct ListingForBuyerOffer {
  Bazaar::ListingID listing_id;
  ItemProperties item;
  unsigned int price;

  enum Status {
    OFFER_UNPROCESSED,
    OFFER_REJECTED,
    OFFER_ACCEPTED,
  } offer_status;

  // How many items the buyer offered for this listing
  int number_of_items_offered;
};

}
}


/**
 *
 */
 class GlobalDatabaseManager {
public:
  enum DBResult
  {
    DBR_SUCCESS,
    DBR_ALREADYEXISTS,
    DBR_DOESNTEXIST,
    DBR_INCORRECTPASSWORD,
    DBR_OVERLIMIT,
    DBR_INTERNALERROR,
  };

public:
  static GlobalDatabaseManager* singleton();


public:

  GlobalDatabaseManager();
  ~GlobalDatabaseManager();

  bool create(const char* rootDBName, const char* configurationFilename);
  void destroy();

  /**
   * Determines whether or not the provided account number is valid
   */
  static bool isValidAccountNumber(unsigned int id);

  /**
   * Tries to create an account with the given settings
   * @param password The password of the account
   * @param accountID The ID number of the account that was created
   * @return What happened during the process:
   *      DBR_SUCCESS - the account was created successfully
   *      DBR_INTERNALERROR
   */
  DBResult createAccount(const char* password, unsigned int* id);

  /**
   * Attempts to access the account with the provided ID number
   * @param id The ID number of the account to access
   * @param password The password with which to compare to the account's password
   * @return One of:
   *      DBR_SUCCESS - the password matches the one for the account
   *      DBR_DOESNTEXIST - no account with the given ID number exists
   *      DBR_INCORRECTPASSWORD - the password doesn't match the one for the account
   *      DBR_INTERNALERROR
   */
  DBResult accessAccount(unsigned int id, const char* password);

  /**
   * Gets a list of all of the characters that are linked to the account with the given ID.  The
   * provided list should have at least MAX_AVATARS_PER_ACCOUNT elements.
   * @param accountID The account for which to query for characters
   * @param characterIDArray The destination array into which to put character IDs
   * @param numberOfCharacters The destination variable for how many characters were returned.  If
   *      there is an error during the query (not due to invalid parameters) this will be set
   *      to zero.
   * @return One of:
   *      DBR_SUCCESS - the character list was retrieved successfully
   *      DBR_DOESNTEXIST - no account for the given ID number exists
   *      DBR_INTERNALERROR
   */
  DBResult getCharactersOnAccount(unsigned int accountID, unsigned int* characterIDArray, size_t* numberOfCharacters);

  /**
   * 
   */
  //DBResult changeAccountPassword(unsigned int id, const char* oldPassword, const char* newPassword);

  /**
   * Tries to create a character with the provided attributes
   * @param accountID The ID of the account with which to associate the character
   * @param name The name of the character to create
   * @param characterID Destination field for the ID number of the character that was created
   * @return One of:
   *      DBR_SUCCEESS - the character was created and linked to the account successfully
   *      DBR_OVERLIMIT - there are too many characters on this account to make another one
   *      DBR_ALREADYEXISTS - there is already a character with that name
   *      DBR_INTERNALERROR - something went horribly wrong
   */
  DBResult createCharacter(unsigned int accountID, const char* name, unsigned int* characterID, float worldX, float worldY,
                           bool male, int chRace, int chClass, int level, int hp, int mp, int attributes[5]);

  /**
   * Unlinks a character from the account.  When the user deletes a character from their account, it is
   * simply removed from being linked into the account--it isn't deleted.
   * @param accountID The ID number of the account from which to remove the character
   * @param characterID The character to remove from the account
   * @return One of:
   *      DBR_SUCCESS - the character was removed successfully
   *      DBR_DOESNTEXIT - either the account doesn't exit or the character isn't on this account
   *      DBR_INTERNALERROR
   */
  DBResult removeCharacterFromAccount(unsigned int accountID, unsigned int characterID);

  /**
   * Gets the amount of play time that an account has free.  The daysLeft is calculated absolutely,
   * so it is negative if the account is out of time.
   * @param accountID The ID number of the account for which to retrieve the time left
   * @param daysLeft Output variable for the number of days left on this account
   * @return One of:
   *      DBR_SUCCESS - the account time was retrieved successfully
   *      DBR_DOESNTEXIST - the requested account ID doesn't exist
   *      DBR_INTERNALERROR
   */
  DBResult getAccountTimeLeft(unsigned int accountID, float* daysLeft);

  /**
   * Gets the name of the character with the given ID
   */
  DBResult getCharacterName(unsigned int accountID, unsigned int characterID, char* nameBuffer, size_t nameBufferLength);

  /**
   * Gets the full description of a character
   */
  DBResult getCharacterDesc(unsigned int account_id,
                            unsigned int character_id,
                            char* name_buffer,
                            size_t name_buffer_length,
                            ActorTypeIndex* actor_type,
                            size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT],
                            int* map_index,
                            float* world_x,
                            float* world_y);

  /**
   * Gets information about a character.  The character must exist within the account, or this
   * method fails.
   */
  DBResult getCharacterData(unsigned int accountID,
                            unsigned int characterID,
                            float* x,
                            float* y,
                            AvatarAttributes* character,
                            AvatarInventoryItem* inventory,
                            int* map_index);

  /**
   * Saves information about a character into the database.  The map coordinates of the character are
   * separated because they aren't part of the player character structure, as these variables are
   * modified by the player's actor and not the character's attributes
   */
  DBResult commitCharacterData(unsigned int accountID, unsigned int characterID, float x, float y,
                               AvatarAttributes* character, AvatarInventoryItem* inventory,
                               int map_index);

  DBResult renameCharacter(const char* old_name, const char* new_name);
  DBResult findAccountFromName(const char* name, unsigned int* number);

  // Builds the list of storage items for the given account.  The input array should
  // be big enough to hold MAX_ITEMS_IN_STORAGE (evidyon_storage.h)
  DBResult storageGet(unsigned int account_id,
                      ServerStorageItem* storage_items,
                      int* number_of_items);

  // Removes an item from storage.  It is the responsibility of the calling method to
  // make sure that it gets put into the inventory and later committed.  The output
  // pointer is filled with an allocation from the item manager.
  DBResult storageWithdraw(unsigned int account_id,
                           ItemIDType item_id,
                           AvatarInventoryItem* inventory_item);

  // Adds an item from inventory to storage.  This item is removed from the main items
  // database, if it exists there, to prevent duping.  The structure passed into the
  // method will be released back to the item manager.
  DBResult storageDeposit(unsigned int account_id,
                          AvatarInventoryItem* inventory_item);

  //----------------------------------------------------------------------------------------------------
  //  The Bazaar
  //----------------------------------------------------------------------------------------------------
  // The bazaar allows asynchronous item trades between players either by barter or purchase.
  //  bazaar_open_listings - items that are available for purchase or barter
  //  bazaar_purchased_listings - items that were bought at their purchase price, cash destined for seller
  //  bazaar_unprocessed_offers - barter offers that have been made for open listings
  //  bazaar_rejected_offers - barter offers that have been rejected or for which the item was purchased by another
  //  bazaar_accepted_offers - barter trades accepted by seller, item destined for winning buyer

  // Creates a new listing in the bazaar.  A price of 0 indicates that
  // this item cannot be bought for currency.  Unless DBR_SUCCESS is returned,
  // inventory_item is not affected.  If the item was listed in the bazaar,
  // the item is erased from the inventory structure.
  DBResult bazaarCreateListing(AvatarInventoryItem* inventory_item,
                               unsigned int price,                // todo: unsigned int -> money_t
                               unsigned int selling_account_id);  // todo: unsigned int -> accountid_t

  // Purchases the given listing if its price is less than *money and nonzero.
  // This will move the item to the purchaser's storage and change *money
  // to reflect the cost of the item.  The listing is move from the open-
  // listings table and an entry is created in purchased listings.
  // The selling account ID is listed so that, if it is logged in, the account
  // can be notified of the change.
  DBResult bazaarPurchaseListing(Bazaar::ListingID listing_id,
                                 unsigned int* money,
                                 unsigned int purchaser_account_id,
                                 unsigned int* selling_account_id);

  // Takes a listing out of the bazaar.  If successful, the item will appear
  // in the storage for listing account.  The 'selling_account_id' is used
  // to confirm the authenticity of the origin of this transaction (use the
  // acting client's account ID).
  // If this method fails, it's likely the item was purchased.  The client's
  // seller listings should be updated.
  DBResult bazaarWithdrawListing(Bazaar::ListingID listing_id,
                                 unsigned int selling_account_id);

  // Takes a listing that was purchased out of the bazaar.  If successful,
  // *income will be set to the price for which the item was sold and the
  // listing will be removed from the database.  If this method fails, be
  // sure the client's seller listing are synchronized since
  // this method only works once.
  DBResult bazaarRedeemListing(Bazaar::ListingID listing_id,
                               unsigned int selling_account_id,
                               unsigned int* income);

  // Gets a list of the items put up for sale by an account.  This
  // method returns both open and purchased listings, setting values
  // in the returned array of BazaarListing structures to denote
  // which group each item belongs to.
  DBResult bazaarGetSellerListings(Bazaar::Server::SellerListing* listings,
                                   size_t buffer_size,
                                   unsigned int selling_account_id,
                                   size_t* listings_returned);

  // Returns all of the open listings, up to the buffer size, sorted most
  // to least recent.
  DBResult bazaarGetOpenListings(Bazaar::Server::OpenListing* listings,
                                 size_t buffer_size,
                                 size_t* listings_returned);

  // Opens up an unprocessed offer for the given listing that consists of
  // items from the avatar's inventory.  If the listing doesn't exist or
  // the offering account already has an unprocessed or rejected offer
  // for the given listing, this method will fail.  If bazaarCreateOffer
  // succeeds, the inventory indices will be erased.
  // todo: should we erase items with matching IDs from 'items'?
  DBResult bazaarCreateOffer(Bazaar::ListingID listing_id,        // desired item listing
                             unsigned int offering_character_id,  // character trying to make the purchase
                             AvatarInventoryItem* inventory,
                             unsigned int* inventory_indices,
                             size_t number_of_inventory_indices);

  // The seller has rejected a specific offer for his or her listing.  This
  // will move the offer from the unprocessed table to the rejected one.
  // Be sure to notify the offering account of the change of it is logged in!
  DBResult bazaarRejectOffer(Bazaar::ListingID listing_id,
                             unsigned int offering_character_id,
                             unsigned int* returned_offering_account_id);

  // Called to automatically reject all offers in the 'unprocessed' category
  // for a given listing.  This method can be invoked at a client's request,
  // but is also used to clean up dangling offers when an item has been
  // traded or purchased.
  DBResult bazaarRejectAllOffers(Bazaar::ListingID listing_id);

  // Returns items bid on a given listing to the storage of the account
  // that offered them from *either* the unprocssed or the rejected
  // table.  If this method fails, synchronize the client's offers
  // since it's possible the offer was accepted.
  DBResult bazaarWithdrawOffer(Bazaar::ListingID listing_id,
                               unsigned int offering_character_id);

  // Gets the offers that this buyer has made on listings.
  DBResult bazaarGetBuyerOffers(Bazaar::Server::ListingForBuyerOffer* listings_desired,
                                size_t listings_buffer_size,
                                Bazaar::Server::BuyerOfferElement* offerings,
                                size_t offerings_buffer_size,
                                unsigned int offering_character_id,
                                size_t* listings_returned);

  // Moves an item into the offering account's storage, if it was accepted.
  DBResult bazaarClaimAcceptedOffer(Bazaar::ListingID listing_id,
                                    unsigned int offering_character_id);


  DBResult commitCharacterMoney(unsigned int accountID, unsigned int characterID, unsigned int money);


  void executeDiskDBThread();
  int sqlite3_executeDiskQuery(const char* query);
  int sqlite3_executeQuery(const char* query, char** errorMessage);

  DBResult itemGenerated(size_t type, int quantity, ItemIDType* unique_id); // creates an item with ITEMLOCATION_NONE and owner_character_id == 0
  DBResult itemOnGroundErased(ItemIDType unique_id); // confirms that owner_character_id == 0 and location == ITEMLOCATION_WORLD
  DBResult itemPickedUp(unsigned int owner_character_id, ItemIDType unique_id);
  DBResult itemDroppedByOwner(ItemIDType unique_id); // ensures that owner_character_id = 0; also sets location to ITEMLOCATION_WORLD
  DBResult itemListedInBazaar(ItemIDType unique_id, unsigned int seller_character_id, unsigned int price);
  DBResult itemPurchasedFromBazaar(ItemIDType unique_id, unsigned int new_owner_character_id);

  // if 'profit' returns a nonzero value, the item was purchased and the seller no longer has the item
  DBResult itemWithdrawnFromBazaar(ItemIDType unique_id, unsigned int seller_character_id);
  DBResult itemProfitsWithdrawn(unsigned int seller_character_id, unsigned int* profit);

  // Used to add names of monsters, races/classes, etc. to the reserved name list.  This method
  // is used to provide the structure to the gamefile manager when the server starts.
  UniqueAvatarNameChecker* getUniqueAvatarNameChecker();



private:

  /// The root name for this database, from which the source file names are generated
  std::string myRootDBName;

  /// Database connection to the in-memory database.
  sqlite3* myDB;

  /// The last time at which the database was backed up
  unsigned long myLastBackupTime;

  /// The next available character ID
  unsigned int myNextCharacterID;

  sqlite3* myDiskDB;
  dcx::dcxAsynchronousSISOThreadDataController< const char*, 500 > myDiskDBQueriesToExecute;
  HANDLE myDiskDBThreadHandle;
  HANDLE myDiskDBThreadWaitEvent;

  /// Makes sure that characters don't have the same name
  UniqueAvatarNameChecker unique_avatar_name_checker_;

private:
  static GlobalDatabaseManager* singleton_;
};

}


#endif