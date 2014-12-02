#include "globalactormanager.h"
#include "globalclientmanager.h"
#include "globalmagicmanager.h"
#include "globalnetworkmanager.h"
#include "globalworldmanager.h"
#include "globaldatabasemanager.h"
#include "globalgamefilemanager.h"
#include "globallifeformaimanager.h"
#include "globalitemmanager.h"
#include "../shared/evidyon_network.h"
#include <dc/debug>
#include <dcx/win32>
#include <dcx/inifilereader>
#include <dcx/asyncdebugout>

#include "time.h"
#include "statisticslogger.h"

#include "main.h"

namespace Evidyon {

bool running = true;

}

//------------------------------------------------------------------------------------------------
// Name: ConsoleHandler
// Desc: 
//------------------------------------------------------------------------------------------------
BOOL WINAPI ConsoleHandler(DWORD CtrlType)
{
  if (CtrlType == CTRL_SHUTDOWN_EVENT ||
    CtrlType == CTRL_CLOSE_EVENT)
    Evidyon::running = false;

  return TRUE;
}



namespace Evidyon {

struct GlobalContext {
  Evidyon::GlobalNetworkManager network;
  Evidyon::GlobalDatabaseManager database;
  Evidyon::GlobalClientManager clients;
  Evidyon::GlobalGamefileManager gamefile;
  Evidyon::GlobalActorManager actors;
  Evidyon::GlobalWorldManager world;
  Evidyon::GlobalLifeformAIManager lifeform_ai;
  Evidyon::GlobalItemManager items;
  Evidyon::GlobalMagicManager magic;
  Evidyon::StatisticsLogger statistics_logger;
};

bool startup(GlobalContext& server) {
  dcx::dcxIniFileReader ini_file;
  ini_file.read("config.ini");
  CONFIRM(server.statistics_logger.create()) else return false;
  CONFIRM(server.network.create(Evidyon::GlobalClientManager::MAX_CLIENTS, Evidyon::Network::SERVER_PORT)) else return false;
  CONFIRM(server.database.create("evidyon", ini_file.getSetting("Settings","database","database.cfg"))) else return false;
  CONFIRM(server.clients.create()) else return false;
  CONFIRM(server.gamefile.create(ini_file.getSetting("Settings","media","evidyon.gsvr"),
                                 server.database.getUniqueAvatarNameChecker(),
                                &server.items,
                                &server.actors,
                                &server.world,
                                &server.lifeform_ai)) else return false;
  CONFIRM(server.magic.create()) else return false;
  SetConsoleCtrlHandler(ConsoleHandler, TRUE);

  printf("(started)");

  return true;
}

void shutdown(GlobalContext& server) {
  DEBUG_INFO("disconnecting clients...");
  server.clients.disconnectAll();
  DEBUG_INFO("shutting down...");
  SetConsoleCtrlHandler(ConsoleHandler, FALSE);
  server.lifeform_ai.destroy();
  server.magic.destroy();
  server.world.destroy();
  server.actors.destroy();
  server.items.destroy();
  server.gamefile.destroy();
  server.clients.destroy();
  server.database.destroy();
  server.network.destroy();
  server.statistics_logger.destroy();
  DEBUG_ATEXIT(); // Display debugging messages
}

}






int main(int argc, char* argv[]) {

  // Activate a special debugger that writes to the disk using an
  // intermediate buffer.
  //dcx::dcxAsyncDebugOut async_debug_output;
  //async_debug_output.acquireDebugStream();

  Evidyon::GlobalContext server;
  if (!Evidyon::startup(server)) {
    Evidyon::shutdown(server);
    return -1;
  }

  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

  typedef dcx::dcxWin32Clock Clock;
  double last_update_time = Clock::getAccurateSystemTime();

#if 0
  { // test the database
    static const unsigned int SELLING_ACCOUNT_ID = 123456;
    static const unsigned int ITEM_BEING_SOLD = 73;

    Evidyon::AvatarInventoryItem inventory_item;
    inventory_item.item = server.items.acquireItem(1, ITEM_BEING_SOLD);
    inventory_item.equipped = false;
    inventory_item.stackable = item.item->isStackable();
    server.database.bazaarCreateListing(&inventory_item, 150, SELLING_ACCOUNT_ID);

    // get seller listings
    Evidyon::Bazaar::Server::SellerListing seller_listings[2];
    size_t listings_returned = 0;
    server.database.bazaarGetSellerListings(seller_listings, 2, SELLING_ACCOUNT_ID, &listings_returned);
    ASSERT(listings_returned == 1);
    ASSERT(seller_listings[0].open == true);
    ASSERT(seller_listings[0].item.quantity == 1);
    ASSERT(seller_listings[0].price == 150);
    ASSERT(seller_listings[0].item.type == ITEM_BEING_SOLD);
    ASSERT(seller_listings[0].offers == 0);

    // get current listings
    Evidyon::Bazaar::Server::OpenListing open_listings[2];
    server.database.bazaarGetOpenListings(open_listings, 2, &listings_returned);
    ASSERT(listings_returned == 1);
    ASSERT(open_listings[0].offers == 0);
    ASSERT(open_listings[0].listing_id == seller_listings[0].listing_id);
    ASSERT(open_listings[0].item.quantity == 1);
    ASSERT(open_listings[0].price == 150);
    ASSERT(open_listings[0].item.type == ITEM_BEING_SOLD);

    // buy the item
    unsigned int selling_account;
    static const unsigned int BUYER_INITIAL_MONEY = 1000;
    unsigned int money = BUYER_INITIAL_MONEY;
    server.database.bazaarPurchaseListing(open_listings[0].listing_id,
                                          &money,
                                          1892795,
                                          &selling_account);
    ASSERT(money == BUYER_INITIAL_MONEY - 150);
    ASSERT(selling_account == SELLING_ACCOUNT_ID);

    // should be no listings now
    server.database.bazaarGetOpenListings(open_listings, 2, &listings_returned);
    ASSERT(listings_returned == 0);

    // get seller listings; should indicate a purchase was made
    Evidyon::Bazaar::Server::SellerListing seller_listings[2];
    size_t listings_returned = 0;
    server.database.bazaarGetSellerListings(seller_listings, 2, SELLING_ACCOUNT_ID, &listings_returned);
    ASSERT(listings_returned == 1);
    ASSERT(seller_listings[0].open == false);
    ASSERT(seller_listings[0].item.quantity == 1);
    ASSERT(seller_listings[0].price == 150);
    ASSERT(seller_listings[0].item.type == ITEM_BEING_SOLD);
    ASSERT(seller_listings[0].offers == 0);

    // withdraw
    unsigned int income;
    server.database.bazaarRedeemListing(seller_listings[0].listing_id, SELLING_ACCOUNT_ID, &income);
    ASSERT(income == 150);

  }
#endif


  enum ManagersToUpdate {
    MANAGER_CLIENTS,
    MANAGER_LIFEFORM_AI,
    MANAGER_MAGIC,
    MANAGER_WORLD,
    NUMBER_OF_MANAGERS,
  };

  double last_update_times[NUMBER_OF_MANAGERS];
  { // initialize the update times
    double current_time = Clock::getAccurateSystemTime();
    for (int i = 0; i < NUMBER_OF_MANAGERS; ++i) {
      last_update_times[i] = current_time;
    }
  }

  int next_manager_to_update = 0;

  while (Evidyon::running) {
    double current_time = Clock::getAccurateSystemTime();
    Evidyon::Time::current_frame = current_time;

    // This loop tries to keep the server from being idle by spreading out the work
    double time_since_last_update = current_time - last_update_times[next_manager_to_update];
    if (time_since_last_update > Evidyon::Time::MAIN_LOOP_PERIOD) {
      switch (next_manager_to_update) {
        case MANAGER_CLIENTS:     server.clients.update(current_time, time_since_last_update); break;
        case MANAGER_LIFEFORM_AI: server.lifeform_ai.update(current_time, time_since_last_update); break;
        case MANAGER_MAGIC:       server.magic.update(&server.world, &server.actors, current_time, time_since_last_update); break;
        case MANAGER_WORLD:       server.world.update(current_time); break;
      }
      last_update_times[next_manager_to_update] = current_time;//Evidyon::Time::MAIN_LOOP_PERIOD; // = current_time
      next_manager_to_update = (next_manager_to_update + 1) % NUMBER_OF_MANAGERS;
    }

    server.network.update(&server.clients);
  }

  //async_debug_output.releaseDebugStream();

  Evidyon::shutdown(server);
  return 0;
}