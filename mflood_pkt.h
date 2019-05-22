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
    u_int16_t pkt_geo_loc_max_x_; //Packet Geo Location
    u_int16_t pkt_geo_loc_min_x_; //Packet Geo Location
    u_int16_t pkt_geo_loc_max_y_; //Packet Geo Location
    u_int16_t pkt_geo_loc_min_y_; //Packet Geo Location
    
    u_int16_t pkt_fwd_loc_min_x_; //Forward Location
    u_int16_t pkt_fwd_loc_max_x_; //Forward Location
    u_int16_t pkt_fwd_loc_min_y_; //Forward Location
    u_int16_t pkt_fwd_loc_max_y_; //Forward Location
    
    inline nsaddr_t& pkt_src() {return pkt_src_;}
    inline u_int16_t& pkt_len() {return pkt_len_;}
    inline u_int8_t& pkt_seq_num() {return pkt_seq_num_;}
    inline u_int16_t& pkt_max_x_geo_loc() {return pkt_geo_loc_max_x_;}
    inline u_int16_t& pkt_min_x_geo_loc() {return pkt_geo_loc_min_x_;}
    inline u_int16_t& pkt_max_y_geo_loc() {return pkt_geo_loc_max_y_;}
    inline u_int16_t& pkt_min_y_geo_loc() {return pkt_geo_loc_min_y_;}
    
    inline u_int16_t& pkt_min_x_fwd_loc() {return pkt_fwd_loc_min_x_;}
    inline u_int16_t& pkt_max_x_fwd_loc() {return pkt_fwd_loc_max_x_;}
    inline u_int16_t& pkt_min_y_fwd_loc() {return pkt_fwd_loc_min_y_;}
    inline u_int16_t& pkt_max_y_fwd_loc() {return pkt_fwd_loc_max_y_;}
    
    static int offset_;
    inline static int& offset() {return offset_;}
    
    // To access the hdr_mflood_pkt packet header in a packet pointed by p, we use the access fn
    inline static hdr_mflood_pkt* access(const Packet* p) {
        return (hdr_mflood_pkt*)p->access(offset_);
    }
};

#endif /* MFLOOD_PKT_H_ */
