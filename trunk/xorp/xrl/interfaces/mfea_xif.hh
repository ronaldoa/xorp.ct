/*
 * Copyright (c) 2001-2003 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by '../scripts/clnt-gen'.
 *
 * $XORP: xorp/xrl/interfaces/mfea_xif.hh,v 1.6 2003/03/10 23:01:29 hodson Exp $
 */

#ifndef __XRL_INTERFACES_MFEA_XIF_HH__
#define __XRL_INTERFACES_MFEA_XIF_HH__

#undef XORP_LIBRARY_NAME
#define XORP_LIBRARY_NAME "XifMfea"

#include "libxorp/xlog.h"
#include "libxorp/callback.hh"

#include "libxipc/xrl.hh"
#include "libxipc/xrl_error.hh"
#include "libxipc/xrl_sender.hh"


class XrlMfeaV0p1Client {
public:
    XrlMfeaV0p1Client(XrlSender* s) : _sender(s) {}
    virtual ~XrlMfeaV0p1Client() {}

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB0;
    /**
     *  Send Xrl intended to:
     *  
     *  Add/delete a protocol in the Multicast FEA.
     *  
     *  @param tgt_name Xrl Target name
     *  
     *  @param xrl_sender_name the XRL name of the originator of this XRL.
     *  
     *  @param protocol_name the name of the protocol to add/delete.
     *  
     *  @param protocol_id the ID of the protocol to add/delete (both sides
     *  must agree on the particular values).
     */
    bool send_add_protocol4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const CB0&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB1;

    bool send_add_protocol6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const CB1&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB2;

    bool send_delete_protocol4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const CB2&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB3;

    bool send_delete_protocol6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const CB3&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB4;
    /**
     *  Send Xrl intended to:
     *  
     *  Start/stop a protocol on an interface in the Multicast FEA.
     *  
     *  @param tgt_name Xrl Target name
     *  
     *  @param xrl_sender_name the XRL name of the originator of this XRL.
     *  
     *  @param protocol_name the name of the protocol to start/stop on the
     *  particular vif.
     *  
     *  @param protocol_id the ID of the protocol to add/stop on the particular
     *  vif (both sides must agree on the particular values).
     *  
     *  @param vif_name the name of the vif to start/stop for the particular
     *  protocol.
     *  
     *  @param vif_index the index of the vif to start/stop for the particular
     *  protocol.
     */
    bool send_start_protocol_vif4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const string&	vif_name, 
	const uint32_t&	vif_index, 
	const CB4&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB5;

    bool send_start_protocol_vif6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const string&	vif_name, 
	const uint32_t&	vif_index, 
	const CB5&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB6;

    bool send_stop_protocol_vif4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const string&	vif_name, 
	const uint32_t&	vif_index, 
	const CB6&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB7;

    bool send_stop_protocol_vif6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const string&	vif_name, 
	const uint32_t&	vif_index, 
	const CB7&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB8;
    /**
     *  Send Xrl intended to:
     *  
     *  Enable/disable the receiving of kernel-originated signal messages.
     *  
     *  @param tgt_name Xrl Target name
     *  
     *  @param xrl_sender_name the XRL name of the originator of this XRL.
     *  
     *  @param protocol_name the name of the protocol to add.
     *  
     *  @param protocol_id the ID of the protocol to add (both sides must agree
     *  on the particular values).
     *  
     *  @param is_allow if true, enable the receiving of kernel-originated
     *  signal messages by protocol 'protocol_name'.
     */
    bool send_allow_signal_messages(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const bool&	is_allow, 
	const CB8&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB9;
    /**
     *  Send Xrl intended to:
     *  
     *  Enable/disable the receiving of Multicast Routing Information Base
     *  information.
     *  
     *  @param tgt_name Xrl Target name
     *  
     *  @param xrl_sender_name the XRL name of the originator of this XRL.
     *  
     *  @param protocol_name the name of the protocol to add.
     *  
     *  @param protocol_id the ID of the protocol to add (both sides must agree
     *  on the particular values).
     *  
     *  @param is_allow if true, enable the receiving of MRIB information
     *  messages by protocol 'protocol_name'.
     */
    bool send_allow_mrib_messages(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const bool&	is_allow, 
	const CB9&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB10;
    /**
     *  Send Xrl intended to:
     *  
     *  Join/leave a multicast group.
     *  
     *  @param tgt_name Xrl Target name
     *  
     *  @param xrl_sender_name the XRL name of the originator of this XRL.
     *  
     *  @param protocol_name the name of the protocol that joins/leave the
     *  group.
     *  
     *  @param protocol_id the ID of the protocol that joins/leave the group
     *  (both sides must agree on the particular values).
     *  
     *  @param vif_name the name of the vif to join/leave the multicast group.
     *  
     *  @param vif_index the index of the vif to join/leave the multicast
     *  group.
     *  
     *  @param group_address the multicast group to join/leave.
     */
    bool send_join_multicast_group4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const string&	vif_name, 
	const uint32_t&	vif_index, 
	const IPv4&	group_address, 
	const CB10&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB11;

    bool send_join_multicast_group6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const string&	vif_name, 
	const uint32_t&	vif_index, 
	const IPv6&	group_address, 
	const CB11&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB12;

    bool send_leave_multicast_group4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const string&	vif_name, 
	const uint32_t&	vif_index, 
	const IPv4&	group_address, 
	const CB12&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB13;

    bool send_leave_multicast_group6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const string&	vif_name, 
	const uint32_t&	vif_index, 
	const IPv6&	group_address, 
	const CB13&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB14;
    /**
     *  Send Xrl intended to:
     *  
     *  Add/delete a Multicast Forwarding Cache with the kernel.
     *  
     *  @param tgt_name Xrl Target name
     *  
     *  @param xrl_sender_name the XRL name of the originator of this XRL.
     *  
     *  @param source_address the source address of the MFC to add/delete.
     *  
     *  @param group_address the group address of the MFC to add/delete.
     *  
     *  @param iif_vif_index the index of the vif that is the incoming
     *  interface.
     *  
     *  @param oiflist the bit-vector with the set of outgoing interfaces.
     *  
     *  @param oiflist_disable_wrongvif the bit-vector with the set of outgoing
     *  interfaces to disable WRONGVIF kernel signal.
     *  
     *  @param max_vifs_oiflist the number of vifs covered by oiflist or
     *  oiflist_disable_wrongvif .
     *  
     *  @param rp_address the RP address of the MFC to add.
     */
    bool send_add_mfc4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const IPv4&	source_address, 
	const IPv4&	group_address, 
	const uint32_t&	iif_vif_index, 
	const vector<uint8_t>&	oiflist, 
	const vector<uint8_t>&	oiflist_disable_wrongvif, 
	const uint32_t&	max_vifs_oiflist, 
	const IPv4&	rp_address, 
	const CB14&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB15;

    bool send_add_mfc6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const IPv6&	source_address, 
	const IPv6&	group_address, 
	const uint32_t&	iif_vif_index, 
	const vector<uint8_t>&	oiflist, 
	const vector<uint8_t>&	oiflist_disable_wrongvif, 
	const uint32_t&	max_vifs_oiflist, 
	const IPv6&	rp_address, 
	const CB15&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB16;

    bool send_delete_mfc4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const IPv4&	source_address, 
	const IPv4&	group_address, 
	const CB16&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB17;

    bool send_delete_mfc6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const IPv6&	source_address, 
	const IPv6&	group_address, 
	const CB17&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB18;
    /**
     *  Send Xrl intended to:
     *  
     *  Send a protocol message to the MFEA.
     *  
     *  @param tgt_name Xrl Target name
     *  
     *  @param xrl_sender_name the XRL name of the originator of this XRL.
     *  
     *  @param protocol_name the name of the protocol that sends a message.
     *  
     *  @param protocol_id the ID of the protocol that sends a message (both
     *  sides must agree on the particular values).
     *  
     *  @param vif_name the name of the vif to send the message.
     *  
     *  @param vif_index the vif index of the vif to send the message.
     *  
     *  @param source_address the address of the sender.
     *  
     *  @param dest_address the destination address.
     *  
     *  @param ip_ttl the TTL of the IP packet to send. If it has a negative
     *  value, the TTL will be set by the lower layers.
     *  
     *  @param ip_tos the TOS of the IP packet to send. If it has a negative
     *  value, the TOS will be set by the lower layers.
     *  
     *  @param is_router_alert set/reset the IP Router Alert option in the IP
     *  packet to send (when applicable).
     */
    bool send_send_protocol_message4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const string&	vif_name, 
	const uint32_t&	vif_index, 
	const IPv4&	source_address, 
	const IPv4&	dest_address, 
	const int32_t&	ip_ttl, 
	const int32_t&	ip_tos, 
	const bool&	is_router_alert, 
	const vector<uint8_t>&	protocol_message, 
	const CB18&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB19;

    bool send_send_protocol_message6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const string&	protocol_name, 
	const uint32_t&	protocol_id, 
	const string&	vif_name, 
	const uint32_t&	vif_index, 
	const IPv6&	source_address, 
	const IPv6&	dest_address, 
	const int32_t&	ip_ttl, 
	const int32_t&	ip_tos, 
	const bool&	is_router_alert, 
	const vector<uint8_t>&	protocol_message, 
	const CB19&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB20;
    /**
     *  Send Xrl intended to:
     *  
     *  Add/delete a dataflow monitor with the MFEA.
     *  
     *  @param tgt_name Xrl Target name
     *  
     *  @param xrl_sender_name the XRL name of the originator of this XRL.
     *  
     *  @param source_address the source address of the dataflow to start/stop
     *  monitoring.
     *  
     *  @param group_address the group address of the dataflow to start/stop
     *  monitoring.
     *  
     *  @param threshold_interval_sec the number of seconds in the interval to
     *  measure.
     *  
     *  @param threshold_interval_usec the number of microseconds in the
     *  interval to measure.
     *  
     *  @param threshold_packets the threshold (in number of packets) to
     *  compare against.
     *  
     *  @param threshold_bytes the threshold (in number of bytes) to compare
     *  against.
     *  
     *  @param is_threshold_in_packets if true, threshold_packets is valid.
     *  
     *  @param is_threshold_in_bytes if true, threshold_bytes is valid.
     *  
     *  @param is_geq_upcall if true, the operation for comparison is ">=".
     *  
     *  @param is_leq_upcall if true, the operation for comparison is "<=".
     */
    bool send_add_dataflow_monitor4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const IPv4&	source_address, 
	const IPv4&	group_address, 
	const uint32_t&	threshold_interval_sec, 
	const uint32_t&	threshold_interval_usec, 
	const uint32_t&	threshold_packets, 
	const uint32_t&	threshold_bytes, 
	const bool&	is_threshold_in_packets, 
	const bool&	is_threshold_in_bytes, 
	const bool&	is_geq_upcall, 
	const bool&	is_leq_upcall, 
	const CB20&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB21;

    bool send_add_dataflow_monitor6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const IPv6&	source_address, 
	const IPv6&	group_address, 
	const uint32_t&	threshold_interval_sec, 
	const uint32_t&	threshold_interval_usec, 
	const uint32_t&	threshold_packets, 
	const uint32_t&	threshold_bytes, 
	const bool&	is_threshold_in_packets, 
	const bool&	is_threshold_in_bytes, 
	const bool&	is_geq_upcall, 
	const bool&	is_leq_upcall, 
	const CB21&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB22;

    bool send_delete_dataflow_monitor4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const IPv4&	source_address, 
	const IPv4&	group_address, 
	const uint32_t&	threshold_interval_sec, 
	const uint32_t&	threshold_interval_usec, 
	const uint32_t&	threshold_packets, 
	const uint32_t&	threshold_bytes, 
	const bool&	is_threshold_in_packets, 
	const bool&	is_threshold_in_bytes, 
	const bool&	is_geq_upcall, 
	const bool&	is_leq_upcall, 
	const CB22&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB23;

    bool send_delete_dataflow_monitor6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const IPv6&	source_address, 
	const IPv6&	group_address, 
	const uint32_t&	threshold_interval_sec, 
	const uint32_t&	threshold_interval_usec, 
	const uint32_t&	threshold_packets, 
	const uint32_t&	threshold_bytes, 
	const bool&	is_threshold_in_packets, 
	const bool&	is_threshold_in_bytes, 
	const bool&	is_geq_upcall, 
	const bool&	is_leq_upcall, 
	const CB23&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB24;

    bool send_delete_all_dataflow_monitor4(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const IPv4&	source_address, 
	const IPv4&	group_address, 
	const CB24&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB25;

    bool send_delete_all_dataflow_monitor6(
	const char*	target_name, 
	const string&	xrl_sender_name, 
	const IPv6&	source_address, 
	const IPv6&	group_address, 
	const CB25&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB26;
    /**
     *  Send Xrl intended to:
     *  
     *  Enable/disable/start/stop a MFEA vif interface.
     *  
     *  @param tgt_name Xrl Target name
     *  
     *  @param vif_name the name of the vif to enable/disable/start/stop.
     */
    bool send_enable_vif(
	const char*	target_name, 
	const string&	vif_name, 
	const CB26&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB27;

    bool send_disable_vif(
	const char*	target_name, 
	const string&	vif_name, 
	const CB27&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB28;

    bool send_start_vif(
	const char*	target_name, 
	const string&	vif_name, 
	const CB28&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB29;

    bool send_stop_vif(
	const char*	target_name, 
	const string&	vif_name, 
	const CB29&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB30;
    /**
     *  Send Xrl intended to:
     *  
     *  Enable/disable/start/stop all MFEA vif interfaces.
     *  
     *  @param tgt_name Xrl Target name
     */
    bool send_enable_all_vifs(
	const char*	target_name, 
	const CB30&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB31;

    bool send_disable_all_vifs(
	const char*	target_name, 
	const CB31&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB32;

    bool send_start_all_vifs(
	const char*	target_name, 
	const CB32&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB33;

    bool send_stop_all_vifs(
	const char*	target_name, 
	const CB33&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB34;
    /**
     *  Send Xrl intended to:
     *  
     *  Enable/disable/start/stop the MFEA and the MFEA CLI access.
     *  
     *  @param tgt_name Xrl Target name
     */
    bool send_enable_mfea(
	const char*	target_name, 
	const CB34&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB35;

    bool send_disable_mfea(
	const char*	target_name, 
	const CB35&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB36;

    bool send_enable_cli(
	const char*	target_name, 
	const CB36&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB37;

    bool send_disable_cli(
	const char*	target_name, 
	const CB37&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB38;

    bool send_start_mfea(
	const char*	target_name, 
	const CB38&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB39;

    bool send_stop_mfea(
	const char*	target_name, 
	const CB39&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB40;

    bool send_start_cli(
	const char*	target_name, 
	const CB40&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB41;

    bool send_stop_cli(
	const char*	target_name, 
	const CB41&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB42;
    /**
     *  Send Xrl intended to:
     *  
     *  Enable/disable the MFEA trace log.
     *  
     *  @param tgt_name Xrl Target name
     */
    bool send_enable_log_trace(
	const char*	target_name, 
	const CB42&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB43;

    bool send_disable_log_trace(
	const char*	target_name, 
	const CB43&	cb
    );

    typedef XorpCallback2<void, const XrlError&, const uint32_t*>::RefPtr CB44;
    /**
     *  Send Xrl intended to:
     *  
     *  Configure MFEA MRIB-related parameters. The 'set_foo' XRLs set the
     *  particular values. The 'reset_foo' XRLs reset the metrics to their
     *  default values.
     *  
     *  @param tgt_name Xrl Target name
     */
    bool send_get_mrib_table_default_metric_preference(
	const char*	target_name, 
	const CB44&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB45;

    bool send_set_mrib_table_default_metric_preference(
	const char*	target_name, 
	const uint32_t&	metric_preference, 
	const CB45&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB46;

    bool send_reset_mrib_table_default_metric_preference(
	const char*	target_name, 
	const CB46&	cb
    );

    typedef XorpCallback2<void, const XrlError&, const uint32_t*>::RefPtr CB47;

    bool send_get_mrib_table_default_metric(
	const char*	target_name, 
	const CB47&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB48;

    bool send_set_mrib_table_default_metric(
	const char*	target_name, 
	const uint32_t&	metric, 
	const CB48&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CB49;

    bool send_reset_mrib_table_default_metric(
	const char*	target_name, 
	const CB49&	cb
    );

protected:
    XrlSender* _sender;

private:
    void unmarshall0(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB0		cb
    );

    void unmarshall1(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB1		cb
    );

    void unmarshall2(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB2		cb
    );

    void unmarshall3(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB3		cb
    );

    void unmarshall4(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB4		cb
    );

    void unmarshall5(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB5		cb
    );

    void unmarshall6(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB6		cb
    );

    void unmarshall7(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB7		cb
    );

    void unmarshall8(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB8		cb
    );

    void unmarshall9(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB9		cb
    );

    void unmarshall10(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB10		cb
    );

    void unmarshall11(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB11		cb
    );

    void unmarshall12(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB12		cb
    );

    void unmarshall13(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB13		cb
    );

    void unmarshall14(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB14		cb
    );

    void unmarshall15(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB15		cb
    );

    void unmarshall16(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB16		cb
    );

    void unmarshall17(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB17		cb
    );

    void unmarshall18(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB18		cb
    );

    void unmarshall19(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB19		cb
    );

    void unmarshall20(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB20		cb
    );

    void unmarshall21(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB21		cb
    );

    void unmarshall22(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB22		cb
    );

    void unmarshall23(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB23		cb
    );

    void unmarshall24(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB24		cb
    );

    void unmarshall25(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB25		cb
    );

    void unmarshall26(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB26		cb
    );

    void unmarshall27(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB27		cb
    );

    void unmarshall28(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB28		cb
    );

    void unmarshall29(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB29		cb
    );

    void unmarshall30(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB30		cb
    );

    void unmarshall31(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB31		cb
    );

    void unmarshall32(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB32		cb
    );

    void unmarshall33(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB33		cb
    );

    void unmarshall34(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB34		cb
    );

    void unmarshall35(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB35		cb
    );

    void unmarshall36(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB36		cb
    );

    void unmarshall37(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB37		cb
    );

    void unmarshall38(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB38		cb
    );

    void unmarshall39(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB39		cb
    );

    void unmarshall40(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB40		cb
    );

    void unmarshall41(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB41		cb
    );

    void unmarshall42(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB42		cb
    );

    void unmarshall43(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB43		cb
    );

    void unmarshall44(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB44		cb
    );

    void unmarshall45(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB45		cb
    );

    void unmarshall46(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB46		cb
    );

    void unmarshall47(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB47		cb
    );

    void unmarshall48(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB48		cb
    );

    void unmarshall49(
	const XrlError&	e, 
	XrlArgs*	a, 
	CB49		cb
    );

};

#endif /* __XRL_INTERFACES_MFEA_XIF_HH__ */
