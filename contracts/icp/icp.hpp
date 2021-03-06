/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/singleton.hpp>

#include "types.hpp"
#include "fork.hpp"

namespace eosio {

struct icp : public contract {
    explicit icp(account_name self);

    [[eosio::action]]
    void setpeer(account_name peer);
    [[eosio::action]]
    void setmaxpackes(uint32_t maxpackets); // limit the maximum stored packets, to support icp rate limiting
    [[eosio::action]]
    void setmaxblocks(uint32_t maxblocks);

    [[eosio::action]]
    void openchannel(const bytes& data); // initialize with a block_header_state as trust seed
    [[eosio::action]]
    void closechannel();

    [[eosio::action]]
    void addblocks(const bytes& data);
    [[eosio::action]]
    void addblock(const bytes& data);
    [[eosio::action]]
    void onpacket(const icp_action& ia);
    [[eosio::action]]
    void onreceipt(const icp_action& ia);
    [[eosio::action]]
    void oncleanup(const icp_action& ia);
    [[eosio::action]]
    void cleanup(uint64_t start_seq, uint64_t end_seq);
    [[eosio::action]]
    void sendaction(uint64_t seq, const bytes& send_action, uint32_t expiration, const bytes& receipt_action);
    [[eosio::action]]
    void genproof(uint64_t packet_seq, uint64_t receipt_seq); // regenerate a proof of old packet/receipt
    [[eosio::action]]
    void prune(uint64_t receipt_start_seq, uint64_t receipt_end_seq); // prune oldest receipts that will not be used any more

    uint64_t next_packet_seq() const;

private:
    bytes extract_action(const icp_action& ia);
    void update_peer();

    void meter_add_packets(uint32_t num);
    void meter_remove_packets(uint32_t num = std::numeric_limits<uint32_t>::max());

    struct [[eosio::table]] peer_contract {
        account_name peer = 0;
        uint64_t last_outgoing_packet_seq = 0;
        uint64_t last_incoming_packet_seq = 0; // to validate
        uint64_t last_outgoing_receipt_seq = 0;
        uint64_t last_incoming_receipt_seq = 0; // to validate
    };

    struct [[eosio::table]] icp_meter {
        uint32_t max_packets;
        uint32_t current_packets;
    };

    typedef eosio::singleton<N(peer), peer_contract> peer_singleton;
    typedef eosio::singleton<N(icpmeter), icp_meter> meter_singleton;

    peer_contract _peer;
    std::unique_ptr<fork_store> store;
};

}
