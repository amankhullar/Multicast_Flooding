#ifndef MFLOOD_H_
#define MFLOOD_H_

#include "mflood_pkt.h"
#include <agent.h>
#include <packet.h>
#include <trace.h>
#include <timer-handler.h>
#include <random.h>
#include <classifier-port.h>
#include "arp.h"
#include "ll.h"
#include "mac.h"
#include "ip.h"
#include "delay.h"

#define CURRENT_TIME Scheduler::instance().clock()
#define JITTER (Random::uniform()*0.5)

class Mflood;    //Forward Declaration

/* TIMERS */

class Mflood_PktTimer : public TimerHandler {
    public :
    Mflood_PktTimer(Mflood* agent) : TimerHandler() {
        agent_ = agent;
    }
protected:
    Mflood* agent_;
    virtual void expire(Event* e);
};

/* Agent */
class Mflood : public Agent {
    /* Friends */
    friend class Mflood_PktTimer;
    
    /*Private Members*/
    nsaddr_t             ra_addr_;
    //mflood_state     state_;
    mflood_rtable     rtable_;
    int                 accessible_var_;
    u_int8_t             seq_num_;
    
    protected :
    
    PortClassifier*     dmux_;            //For Passing Packets Up To Agents
    Trace*              logtarget_;        //For Logging
    Mflood_PktTimer     pkt_timer_;        //Timer for sending packets
    
    inline nsaddr_t&         ra_addr()             {return ra_addr_; }
    //inline mflood_state& state()                {return state_;}
    inline int&             accessible_var()     {return accessible_var_;}
    
    void forward_data(Packet*);
    void recv_mflood_pkt(Packet*);
    void send_mflood_pkt();
    
    void reset_mflood_pkt_timer();
    
public:
    Mflood(nsaddr_t);
    int command(int, const char*const*);
    void recv(Packet*, Handler*);
    int node_x = 0;
    int node_y = 0;
};

#endif /* MFLOOD_H_ */
