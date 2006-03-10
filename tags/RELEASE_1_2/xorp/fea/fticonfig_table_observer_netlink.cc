// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2005 International Computer Science Institute
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

#ident "$XORP: xorp/fea/fticonfig_table_observer_netlink.cc,v 1.10 2005/03/05 01:41:23 pavlin Exp $"

#include "fea_module.h"

#include "libxorp/xorp.h"
#include "libxorp/xlog.h"
#include "libxorp/debug.h"

// TODO: XXX: PAVPAVPAV: move this include somewhere else!!
#ifdef HOST_OS_LINUX
#include <linux/types.h>
#endif
#ifdef HAVE_LINUX_RTNETLINK_H
#include <linux/rtnetlink.h>
#endif

#include "fticonfig.hh"
#include "fticonfig_table_observer.hh"


//
// Observe whole-table information change about the unicast forwarding table.
//
// E.g., if the forwarding table has changed, then the information
// received by the observer would NOT specify the particular entry that
// has changed.
//
// The mechanism to observe the information is netlink(7) sockets.
//


FtiConfigTableObserverNetlink::FtiConfigTableObserverNetlink(FtiConfig& ftic)
    : FtiConfigTableObserver(ftic),
      NetlinkSocket4(ftic.eventloop()),
      NetlinkSocket6(ftic.eventloop()),
      NetlinkSocketObserver(*(NetlinkSocket4 *)this, *(NetlinkSocket6 *)this)
{
#ifdef HAVE_NETLINK_SOCKETS
    register_ftic_primary();
#endif
}

FtiConfigTableObserverNetlink::~FtiConfigTableObserverNetlink()
{
    string error_msg;

    if (stop(error_msg) != XORP_OK) {
	XLOG_ERROR("Cannot stop the netlink(7) sockets mechanism to observe "
		   "whole forwarding table from the underlying "
		   "system: %s",
		   error_msg.c_str());
    }
}

int
FtiConfigTableObserverNetlink::start(string& error_msg)
{
#ifndef HAVE_NETLINK_SOCKETS
    error_msg = c_format("The netlink(7) mechanism to observe "
			 "whole forwarding table from the "
			 "underlying system is not supported");
    XLOG_UNREACHABLE();
    return (XORP_ERROR);

#else // HAVE_NETLINK_SOCKETS

    if (_is_running)
	return (XORP_OK);

    //
    // Listen to the netlink multicast group for IPv4 routing entries.
    //
    if (ftic().have_ipv4()) {
	NetlinkSocket4::set_nl_groups(RTMGRP_IPV4_ROUTE);

	if (NetlinkSocket4::start(error_msg) < 0)
	    return (XORP_ERROR);
    }

#ifdef HAVE_IPV6
    //
    // Listen to the netlink multicast group for IPv6 routing entries.
    //
    if (ftic().have_ipv6()) {
	NetlinkSocket6::set_nl_groups(RTMGRP_IPV6_ROUTE);

	if (NetlinkSocket6::start(error_msg) < 0) {
	    string error_msg2;
	    if (ftic().have_ipv4())
		NetlinkSocket4::stop(error_msg2);
	    return (XORP_ERROR);
	}
    }
#endif // HAVE_IPV6

    _is_running = true;

    return (XORP_OK);
#endif // HAVE_NETLINK_SOCKETS
}
    
int
FtiConfigTableObserverNetlink::stop(string& error_msg)
{
    int ret_value4 = XORP_OK;
    int ret_value6 = XORP_OK;

    if (! _is_running)
	return (XORP_OK);

    if (ftic().have_ipv4())
	ret_value4 = NetlinkSocket4::stop(error_msg);
    
#ifdef HAVE_IPV6
    if (ftic().have_ipv6()) {
	string error_msg2;
	ret_value6 = NetlinkSocket6::stop(error_msg);
	if ((ret_value6 < 0) && (ret_value4 >= 0))
	    error_msg = error_msg2;	// XXX: update the error message
    }
#endif
    
    if ((ret_value4 < 0) || (ret_value6 < 0))
	return (XORP_ERROR);

    _is_running = false;

    return (XORP_OK);
}

void
FtiConfigTableObserverNetlink::receive_data(const uint8_t* data, size_t nbytes)
{
    list<FteX> fte_list;

    //
    // Get the IPv4 routes
    //
    if (ftic().have_ipv4()) {
	ftic().ftic_table_get_primary().parse_buffer_nlm(AF_INET, fte_list,
							 data, nbytes, false);
	if (! fte_list.empty()) {
	    propagate_fib_changes(fte_list);
	    fte_list.clear();
	}
    }

#ifdef HAVE_IPV6
    //
    // Get the IPv6 routes
    //
    if (ftic().have_ipv6()) {
	ftic().ftic_table_get_primary().parse_buffer_nlm(AF_INET6, fte_list,
							 data, nbytes, false);
	if (! fte_list.empty()) {
	    propagate_fib_changes(fte_list);
	    fte_list.clear();
	}
    }
#endif // HAVE_IPV6
}

void
FtiConfigTableObserverNetlink::nlsock_data(const uint8_t* data, size_t nbytes)
{
    receive_data(data, nbytes);
}