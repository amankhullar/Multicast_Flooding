#ifndef MFLOOD_PKT_H_
#define MFLOOD_PKT_H_

#include <packet.h>
#include <config.h>
#include <random.h>
#include <timer-handler.h>
#include <trace.h>
#include <classifier-port.h>

#define HDR_MFLOOD_PKT(p) hdr_mflood_pkt::access(p)

struct hdr_mflood_pkt {
    
    nsaddr_t pkt_src_;       //Node which originated this packet
    u_int16_t pkt_len_;    //Packet length (in bytes)
    u_int8_t pkt_seq_num_; //Packet Sequence Number
    u_int16_t pkt_geo_loc_; //Packet Geo Location
    
    inline nsaddr_t& pkt_src() {return pkt_src_;}
    inline u_int16_t& pkt_len() {return pkt_len_;}
    inline u_int8_t& pkt_seq_num() {return pkt_seq_num_;}
    inline u_int16_t& pkt_geo_loc() {return pkt_geo_loc_;}
    
    static int offset_;
    inline static int& offset() {return offset_;}
    
    // To access the hdr_mflood_pkt packet header in a packet pointed by p, we use the access fn
    inline static hdr_mflood_pkt* access(const Packet* p) {
        return (hdr_mflood_pkt*)p->access(offset_);
    }
};

#endif /* MFLOOD_PKT_H_ */
