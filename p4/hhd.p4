#include <core.p4>
#if __TARGET_TOFINO__ == 2
#include <t2na.p4>
#else
#include <tna.p4>
#endif

typedef bit<48> mac_addr_t;
typedef bit<32> ipv4_addr_t;
typedef bit<128> ipv6_addr_t;
typedef bit<12> vlan_id_t;

typedef bit<16> ether_type_t;
const ether_type_t ETHERTYPE_IPV4 = 16w0x0800;
const ether_type_t ETHERTYPE_ARP = 16w0x0806;
const ether_type_t ETHERTYPE_IPV6 = 16w0x86dd;
const ether_type_t ETHERTYPE_VLAN = 16w0x8100;

typedef bit<8> ip_protocol_t;
const ip_protocol_t IP_PROTOCOLS_ICMP = 1;
const ip_protocol_t IP_PROTOCOLS_TCP = 6;
const ip_protocol_t IP_PROTOCOLS_UDP = 17;


typedef bit<32> COUNTER;
struct Bucket {
    bit<32>     key;
    bit<32>      level;
}

header resubmit_h {
    bit<8> flag_resbumit;
}

header ethernet_h {
    mac_addr_t dst_addr;
    mac_addr_t src_addr;
    bit<16> ether_type;
}

header ipv4_h {
    bit<4> version;
    bit<4> ihl;
    bit<8> diffserv;
    bit<16> total_len;
    bit<16> identification;
    bit<3> flags;
    bit<13> frag_offset;
    bit<8> ttl;
    bit<8> protocol;
    bit<16> hdr_checksum;
    ipv4_addr_t src_addr;
    ipv4_addr_t dst_addr;
}

struct header_t {
    ethernet_h ethernet;
    ipv4_h ipv4;
}

struct empty_header_t {}

struct empty_metadata_t {}





struct metadata_t {
    resubmit_h resubmit_data; //8bit
    bit<32> key;
    bit<32> level;
    COUNTER counter;         //32bit
    bit<32> index;
    bit<1> replace;
    bit<32> src_ip_mask;
    bit<1> flag1;
    bit<1> flag2;
}

// ---------------------------------------------------------------------------
// Ingress parser
// ---------------------------------------------------------------------------
parser SwitchIngressParser(
        packet_in pkt,
        out header_t hdr,
        out metadata_t meta,
        out ingress_intrinsic_metadata_t ig_intr_md) {

    /* This is a mandatory state, required by Tofino Architecture */
    state start{
        meta.resubmit_data = {0};
        meta.key = 0;
        meta.level = 0;
        meta.counter = 0;
        meta. index = 0;
        meta.src_ip_mask = 0;
        meta.replace=0;
        meta.flag1 = 0;
        meta.flag2 = 0;
        pkt.extract(ig_intr_md);
        transition select(ig_intr_md.resubmit_flag) {
            1 : parse_resubmit;
            0 : parse_port_metadata;
        }
    }

    state parse_resubmit {
        pkt.extract(meta.resubmit_data);
        transition parse_ethernet;
    }

    state parse_port_metadata {
        pkt.advance(PORT_METADATA_SIZE);
        transition parse_ethernet;
    }


    state parse_ethernet {
        pkt.extract(hdr.ethernet);
        transition select (hdr.ethernet.ether_type) {
            ETHERTYPE_IPV4 : parse_ipv4;
            default : reject;
        }
    }

    state parse_ipv4 {
        pkt.extract(hdr.ipv4);
        meta.key = hdr.ipv4.src_addr;
        meta.src_ip_mask = hdr.ipv4.src_addr;
        transition accept;
    }
}


control SwitchIngress(
        inout header_t hdr,
        inout metadata_t meta,
        in ingress_intrinsic_metadata_t ig_intr_md,
        in ingress_intrinsic_metadata_from_parser_t ig_intr_prsr_md,
        inout ingress_intrinsic_metadata_for_deparser_t ig_intr_dprsr_md,
        inout ingress_intrinsic_metadata_for_tm_t ig_intr_tm_md) {

    action noaction(){}    
    Random<bit<32>>() random;
    action get_r() {
        meta.level = random.get();
    }
    @pragma stage 0
    table Get_r {
        actions = {
            get_r;
        }
        const default_action = get_r;
        size=1;
    }
    action get_level() {
        meta.level = (meta.level & 0x03);
    }
    
    @pragma stage 1
    table Get_4_level {
        actions = {
            get_level;
        }
        const default_action = get_level;
        size=1;
    }
    action shift(bit<32> mask) {
        meta.src_ip_mask = meta.src_ip_mask & mask;
    }
    @pragma stage 2
    table mask_src {
        key = {
            meta.level :exact;
        }
        actions = {
            shift;
            // shift_4;
        }
        const entries = {
            (0x00) : shift(0xFFFFFFFF);
            (0x01) : shift(0xFFFFFF00);
            (0x02) : shift(0xFFFF0000);
            (0x03) : shift(0xFF000000);
        }
        size = 4;
    }


    Hash<bit<16>> (HashAlgorithm_t.CUSTOM, CRCPolynomial<bit<16>> (16w0x3f0b, true, false, false, 16w0xFFFF, 16w0xFFFF)) hash_unit;
    action hash_fun() {
        meta.index = (bit<32>)hash_unit.get({meta.src_ip_mask, meta.level});
    }
    //进行hash的Table
    @pragma stage 3
    table hash_index{
        actions = {
            hash_fun;
        }
        const default_action = hash_fun();
        size = 1;
    }

    action index_shift() {
        meta.index = meta.index >> 2;
    }

    Register<Bucket, bit<32>>(21845) Bucket_Array;
    RegisterAction<Bucket, bit<32>, bit<1>>(Bucket_Array) replace_action = {
        void apply(inout Bucket value, out bit<1> flag) {
            value.key = meta.src_ip_mask;
            value.level = meta.level;
            flag = 0;
        }
    };
    RegisterAction<Bucket, bit<32>, bit<1>>(Bucket_Array) check_action = {
        void apply(inout Bucket value, out bit<1> flag) {
            if(value.key == meta.src_ip_mask && value.level == meta.level) {
                flag = 0;
            }
            else {
                flag = 1;
            }
        }
    };
    //向桶数组插入元素
    action replace_Bucket() {
        meta.flag2 = replace_action.execute(meta.index);
    }
    action check_Bucket() {
        meta.flag2 = check_action.execute(meta.index);
    }
    
    @pragma stage 4
    table Update_Bucket{
        key = {
            meta.resubmit_data.flag_resbumit : exact;
        }
        actions = {
            replace_Bucket;
            check_Bucket;
        }
        const entries = {
            (0x0) : check_Bucket();
            (0x1) : replace_Bucket();
        }
        size = 2;
    }
//计数器数组
    Register<COUNTER, bit<32>>(21845) Counter_Array;
    RegisterAction<COUNTER, bit<32>, bit<1>>(Counter_Array) Increment = {
        void apply(inout COUNTER value, out bit<1> flag) {
            value = value |+| 1;
            flag = 0;
        }
    };
    RegisterAction<COUNTER, bit<32>, bit<1>>(Counter_Array) Decrement = {
        void apply(inout COUNTER value, out bit<1> flag) {
            if(value==1)
            {
               value = value |-| 1;
               flag = 1; 
            }
            else
            {
                value = value |-| 1;
                flag = 0;
            }
        }
    };
    
    action Increment_Counter() {
        meta.flag1 = Increment.execute(meta.index);
    }
    action Decrement_Counter() {
        meta.flag1 = Decrement.execute(meta.index);
    }
    
    @pragma stage 4
    table Update_Counter{
        key = {
            meta.flag2 : exact;
        }
        actions = {
            Increment_Counter;
            Decrement_Counter;
        }
        // const default_action = Update_Counter_Array;
        const entries = {
            (0x0) : Increment_Counter();
            (0x1) : Decrement_Counter();
        }
        size = 2;
    }

    apply {
        Get_r.apply();
        Get_4_level.apply();
        mask_src.apply();
        hash_index.apply();
        Update_Bucket.apply();
        Update_Counter.apply();
        if(meta.flag1 == 1) {
            meta.resubmit_data.flag_resbumit = 1;
            ig_intr_dprsr_md.resubmit_type = 2;
        }
    }
}

// ---------------------------------------------------------------------------
// Ingress Deparser
// ---------------------------------------------------------------------------
control SwitchIngressDeparser(
        packet_out pkt,
        inout header_t hdr,
        in metadata_t meta,
        in ingress_intrinsic_metadata_for_deparser_t ig_intr_dprsr_md) {

    Resubmit() resubmit;

    apply {
        if (ig_intr_dprsr_md.resubmit_type == 2) {
            resubmit.emit(meta.resubmit_data);
        }

        pkt.emit(hdr);
    }
}


// egress parser/control blocks
parser SwitchEgressParser(
        packet_in pkt,
        out empty_header_t hdr,
        out empty_metadata_t eg_md,
        out egress_intrinsic_metadata_t eg_intr_md) {
    state start {
        transition accept;
    }
}

control SwitchEgressDeparser(
        packet_out pkt,
        inout empty_header_t hdr,
        in empty_metadata_t eg_md,
        in egress_intrinsic_metadata_for_deparser_t ig_intr_dprs_md) {
    apply {}
}

control SwitchEgress(
        inout empty_header_t hdr,
        inout empty_metadata_t eg_md,
        in egress_intrinsic_metadata_t eg_intr_md,
        in egress_intrinsic_metadata_from_parser_t eg_intr_md_from_prsr,
        inout egress_intrinsic_metadata_for_deparser_t ig_intr_dprs_md,
        inout egress_intrinsic_metadata_for_output_port_t eg_intr_oport_md) {
    apply {}
}



Pipeline(SwitchIngressParser(),
         SwitchIngress(),
         SwitchIngressDeparser(),
         SwitchEgressParser(),
         SwitchEgress(),
         SwitchEgressDeparser()) pipe;

Switch(pipe) main;