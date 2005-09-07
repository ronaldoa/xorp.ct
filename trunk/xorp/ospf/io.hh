// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-
// vim:set sts=4 ts=8:

// Copyright (c) 2001-2004 International Computer Science Institute
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software")
// to deal in the Software without restriction, subject to the conditions
// listed in the XORP LICENSE file. These conditions include: you must
// preserve this copyright notice, and you cannot mention the copyright
// holders in advertising related to the Software without their permission.
// The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
// notice is a summary of the XORP LICENSE file; the license in that file is
// legally binding.

// $XORP: xorp/ospf/io.hh,v 1.10 2005/09/01 00:05:07 atanu Exp $

#ifndef __OSPF_IO_HH__
#define __OSPF_IO_HH__

/**
 * An abstract class that defines packet reception and
 * transmission. The details of how packets are received or transmitted
 * are therefore hidden from the internals of the OSPF code.
 */
template <typename A>
class IO {
 public:
    virtual ~IO() {}

    /**
     * Get OSPF protocol number.
     */
    uint16_t get_ip_protocol_number() const {
	return OspfTypes::IP_PROTOCOL_NUMBER;
    }

    /**
     * Send Raw frames.
     */
    virtual bool send(const string& interface, const string& vif,
		      A dst, A src,
		      uint8_t* data, uint32_t len) = 0;

    typedef typename XorpCallback6<void, const string&, const string&,
				   A, A,
				   uint8_t*,
				   uint32_t>::RefPtr ReceiveCallback;
    /**
     * Register for receiving raw frames.
     */
    virtual bool register_receive(ReceiveCallback) = 0;

    /**
     * Enable the interface/vif to receive frames.
     */
    virtual bool enable_interface_vif(const string& interface,
				      const string& vif) = 0;

    /**
     * Disable this interface/vif from receiving frames.
     */
    virtual bool disable_interface_vif(const string& interface,
				       const string& vif) = 0;


    /**
     * Is this interface/vif/address enabled?
     *
     * @return true if it is.
     */
    virtual bool enabled(const string& interface, const string& vif,
			 A address) = 0;

    /**
     * @return prefix length for this address.
     */
    virtual uint32_t get_prefix_length(const string& interface,
				       const string& vif,
				       A address) = 0;

    /**
     * @return the mtu for this interface.
     */
    virtual uint32_t get_mtu(const string& interface) = 0;

    /**
     * On the interface/vif join this multicast group.
     */
    virtual bool join_multicast_group(const string& interface,
				      const string& vif, A mcast) = 0;
    

    /**
     * On the interface/vif leave this multicast group.
     */
    virtual bool leave_multicast_group(const string& interface,
				       const string& vif, A mcast) = 0;

    /**
     * Add route
     *
     * @param net network
     * @param nexthop
     * @param metric to network
     * @param equal true if this in another route to the same destination.
     * @param discard true if this is a discard route.
     */
    virtual bool add_route(IPNet<A> net,
			   A nexthop,
			   uint32_t metric,
			   bool equal,
			   bool discard) = 0;

    /**
     * Replace route
     *
     * @param net network
     * @param nexthop
     * @param metric to network
     * @param equal true if this in another route to the same destination.
     * @param discard true if this is a discard route.
     */
    virtual bool replace_route(IPNet<A> net,
			       A nexthop,
			       uint32_t metric,
			       bool equal,
			       bool discard) = 0;

    /**
     * Delete route
     */
    virtual bool delete_route(IPNet<A> net) = 0;
};
#endif // __OSPF_IO_HH__
