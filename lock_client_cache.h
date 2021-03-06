// lock client interface.

#ifndef lock_client_cache_h

#define lock_client_cache_h

#include <string>
#include "lock_protocol.h"
#include "rpc.h"
#include "lock_client.h"
#include "lang/verify.h"

// Classes that inherit lock_release_user can override dorelease so that 
// that they will be called when lock_client releases a lock.
// You will not need to do anything with this class until Lab 5.
class lock_release_user {
 public:
  virtual void dorelease(lock_protocol::lockid_t) = 0;
  virtual ~lock_release_user() {};
};

class lock_client_Object {
	public:
	bool waitingForRetry;
	bool freeWhenPossible;
	lock_client_Object()
	{
		waitingForRetry = false;
		freeWhenPossible = false;
	}
};

class lock_client_cache : public lock_client {
public:
enum lock_client_state
	{
		none,
		free,
		locked,
		acquiring,
		releasing
	};

typedef std::map<lock_protocol::lockid_t,lock_client_state>::iterator lockMapIter;
 private:
  class lock_release_user *lu;
  int rlock_port;
  std::string hostname;
  std::string id;
	
	std::map<lock_protocol::lockid_t,lock_client_state> lockMap;
	std::map<lock_protocol::lockid_t,lock_client_Object > lockMetaData;
	std::list<lock_protocol::lockid_t> toBeReleasedList;
 public:
	  static int last_port;
  lock_client_cache(std::string xdst, class lock_release_user *l = 0);
  virtual ~lock_client_cache() {};
  lock_protocol::status acquire(lock_protocol::lockid_t);
  lock_protocol::status release(lock_protocol::lockid_t);
  lock_protocol::status releaseNow();
  rlock_protocol::status revoke_handler(lock_protocol::lockid_t, 
                                        int &);
	void setRetry(lock_protocol::lockid_t lid);
	void resetRetry(lock_protocol::lockid_t lid);
	void setFreeNow(lock_protocol::lockid_t lid);
	void resetFreeNow(lock_protocol::lockid_t lid);
  rlock_protocol::status retry_handler(lock_protocol::lockid_t, 
                                       int &);
  lock_protocol::status callAcquire(lock_protocol::lockid_t);
};

namespace lock_client_utility
{
	void* releaseThread(void *);
	class releaseData
	{
		public:
		lock_protocol::lockid_t lid;
		std::string cltId;
		rpcc *cl;
		std::map<lock_protocol::lockid_t,lock_client_cache::lock_client_state>  *lockMap;
		releaseData(lock_protocol::lockid_t ilid, std::string icltId, rpcc* icl,std::map<lock_protocol::lockid_t,lock_client_cache::lock_client_state>  *ilockMap)
		{
			lid = ilid;
			cltId = icltId;
			cl = icl;	
			lockMap = ilockMap;
		}
	};
};

#endif
