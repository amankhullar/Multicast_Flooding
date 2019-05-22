#include "mflood.h"
#include "mflood_pkt.h"
#include <timer-handler.h>
#include <node.h>
#include <random.h>
#include <cmu-trace.h>
#include <iostream>
#include <classifier-port.h>
#include <packet.h>
#include <address.h>

// To Bind our packet in OTcl Interface
int hdr_mflood_pkt::offset_;
static class MfloodHeaderClass : public PacketHeaderClass {
public:
    MfloodHeaderClass() : PacketHeaderClass("PacketHeader/MFLOOD", sizeof(hdr_mflood_pkt)) {
        bind_offset(&hdr_mflood_pkt::offset_);
    }
}class_Mflood_hdr;

static class MfloodClass : public TclClass
{
public:
    MfloodClass() : TclClass("Agent/MFLOOD") {}
    TclObject* create(int, const char*const* ) {
        return(new Mflood());
    }
}class_Mflood;

Mflood::Mflood() : Agent(PT_PING) {}

void
Mflood_PktTimer::expire(Event* e) {
    agent_->send_mflood_pkt();
    agent_->reset_mflood_pkt_timer();
}

Mflood::Mflood(nsaddr_t id) : Agent(PT_MFLOOD), pkt_timer_(this) {
    bind_bool("accessible_var_", &accessible_var_);
    ra_addr_ = id;
}

int
Mflood::command(int argc, const char*const* argv) {
    if(argc == 3) {
        if (strcmp(argv[1], "port-dmux") == 0) {
            dmux_ = (PortClassifier *)TclObject::lookup (argv[2]);
            if (dmux_ == 0) {
                fprintf (stderr, "%s: %s lookup of %s failed\n", __FILE__, argv[1], argv[2]);
                return TCL_ERROR;
            }
            pkt_timer_.resched(0.0);
            return TCL_OK;
        }
        else if (strcmp(argv[1], "tracetarget") == 0) {
            tracetarget_ = (Trace *)TclObject::lookup (argv[2]);
            if (tracetarget_ == 0) {
                fprintf (stderr, "%s: %s lookup of %s failed\n", __FILE__, argv[1],
                         argv[2]);
                return TCL_ERROR;
            }
            return TCL_OK;
        }
    }
    //Pass the command to the base class
    return Agent::command(argc, argv);
}

void
Mflood::recv(Packet* p, Handler* h) {
    struct hdr_cmn* ch    = HDR_CMN(p);
    struct hdr_ip* ih     = HDR_IP(p);
    
    if (ih->saddr() == ra_addr()){
        //If there exists a routing loop, drop the packet
        if(ch->num_forwards() > 0) {
            drop(p, DROP_RTR_ROUTE_LOOP);
            return;
        }
        
        //else if this is a packet I am originating, must add IP header
        else if(ch->num_forwards() == 0)
            ch->size() += IP_HDR_LEN;
    }
    
    // If it is a mflood packet, must process it
    if(ch->ptype() == PT_MFLOOD)
        recv_mflood_pkt(p);
    
    //Otherwise, must forward the packet (unless TTL reaches zero
    else {
        ih->ttl_--;
        if(ih->ttl_ == 0) {
            drop(p, DROP_RTR_TTL);
            return;
        }
        forward_data(p);
    }
}

void
Mflood::recv_mflood_pkt(Packet* p) {
    struct hdr_ip* ih                = HDR_IP(p);
    struct hdr_mflood_pkt* ph    = HDR_MFLOOD_PKT(p);
    
    // All routing messages are sent from and to port RT_PORT, so we shall check it
    assert(ih->sport() == RT_PORT);
    assert(ih->dport() == RT_PORT);
    
    /* processing of mflood packet */
    
    // Release resources
    Packet::free(p);
}

void
Mflood::send_mflood_pkt() {
    Packet* p                        = allocpkt();
    struct hdr_cmn* ch               = HDR_CMN(p);
    struct hdr_ip* ih                = HDR_IP(p);
    struct hdr_mflood_pkt* ph        = HDR_MFLOOD_PKT(p);
    
    ph->pkt_src()                    = ra_addr();
    ph->pkt_len()                    = 7;
    ph->pkt_seq_num()                = seq_num_++;
    
    ch->ptype()                      = PT_MFLOOD;
    ch->direction()                  = hdr_cmn::DOWN;
    ch->size()                       = IP_HDR_LEN + ph->pkt_len();
    ch->error()                      = 0;
    ch->next_hop()                   = IP_BROADCAST;
    ch->addr_type()                  = NS_AF_INET;
    
    ih->saddr()                      = ra_addr();
    ih->daddr()                      = IP_BROADCAST;
    ih->sport()                      = RT_PORT;
    ih->dport()                      = RT_PORT;
    ih->ttl()                        = IP_DEF_TTL;
    
    Scheduler::instance().schedule(target_, p, JITTER);
}

void
Mflood::reset_mflood_pkt_timer() {
    pkt_timer_.resched((double)5.0);
}

void
Mflood::forward_data(Packet* p) {
    struct hdr_cmn* ch = HDR_CMN(p);
    struct hdr_ip* ih = HDR_IP(p);
    
    if(ch->direction() == hdr_cmn::UP && ((u_int32_t)ih->daddr() == IP_BROADCAST || ih->daddr() == ra_addr())) {
        dmux_->recv(p, NULL);
        return;
    }
    else {
        ch->direction() = hdr_cmn::DOWN;
        ch->addr_type() = NS_AF_INET;
        if ((u_int32_t)ih->daddr() == IP_BROADCAST)
            ch->next_hop() = IP_BROADCAST;
        else {
            nsaddr_t next_hop = rtable_.lookup(ih->daddr());
            if(next_hop == IP_BROADCAST) {
                debug("%f: Agent %d can not forward a packet destined to %d \n", CURRENT_TIME,ra_addr(),ih->daddr());
                drop(p, DROP_RTR_NO_ROUTE);
                return;
            }
            else
                ch->next_hop() =  next_hop;
        }
        Scheduler::instance().schedule(target_, p, 0.0);
    }
}
