// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

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

#ident "$XORP: xorp/mld6igmp/xrl_mld6igmp_node.cc,v 1.33 2005/02/18 00:40:00 pavlin Exp $"

#include "mld6igmp_module.h"

#include "libxorp/xorp.h"
#include "libxorp/xlog.h"
#include "libxorp/debug.h"
#include "libxorp/ipvx.hh"
#include "libxorp/status_codes.h"

#include "mld6igmp_node.hh"
#include "mld6igmp_node_cli.hh"
#include "mld6igmp_vif.hh"
#include "xrl_mld6igmp_node.hh"

const TimeVal XrlMld6igmpNode::RETRY_TIMEVAL = TimeVal(1, 0);

//
// XrlMld6igmpNode front-end interface
//

XrlMld6igmpNode::XrlMld6igmpNode(int		family,
				 xorp_module_id	module_id, 
				 EventLoop&	eventloop,
				 const string&	class_name,
				 const string&	finder_hostname,
				 uint16_t	finder_port,
				 const string&	finder_target,
				 const string&	mfea_target)
    : Mld6igmpNode(family, module_id, eventloop),
      XrlStdRouter(eventloop, class_name.c_str(), finder_hostname.c_str(),
		   finder_port),
      XrlMld6igmpTargetBase(&xrl_router()),
      Mld6igmpNodeCli(*static_cast<Mld6igmpNode *>(this)),
      _eventloop(eventloop),
      _class_name(xrl_router().class_name()),
      _instance_name(xrl_router().instance_name()),
      _finder_target(finder_target),
      _mfea_target(mfea_target),
      _xrl_mfea_client(&xrl_router()),
      _xrl_mld6igmp_client_client(&xrl_router()),
      _xrl_cli_manager_client(&xrl_router()),
      _xrl_finder_client(&xrl_router()),
      _is_finder_alive(false),
      _is_mfea_alive(false),
      _is_mfea_registered(false),
      _is_mfea_registering(false),
      _is_mfea_deregistering(false),
      _is_mfea_add_protocol_registered(false)
{

}

XrlMld6igmpNode::~XrlMld6igmpNode()
{
    Mld6igmpNodeCli::stop();
    Mld6igmpNode::stop();
}

bool
XrlMld6igmpNode::startup()
{
    if (start_mld6igmp() < 0)
	return false;

    return true;
}

bool
XrlMld6igmpNode::shutdown()
{
    if (stop_mld6igmp() < 0)
	return false;

    return true;
}

int
XrlMld6igmpNode::enable_cli()
{
    Mld6igmpNodeCli::enable();
    
    return (XORP_OK);
}

int
XrlMld6igmpNode::disable_cli()
{
    Mld6igmpNodeCli::disable();
    
    return (XORP_OK);
}

int
XrlMld6igmpNode::start_cli()
{
    if (Mld6igmpNodeCli::start() < 0)
	return (XORP_ERROR);
    
    return (XORP_OK);
}

int
XrlMld6igmpNode::stop_cli()
{
    if (Mld6igmpNodeCli::stop() < 0)
	return (XORP_ERROR);
    
    return (XORP_OK);
}

int
XrlMld6igmpNode::enable_mld6igmp()
{
    Mld6igmpNode::enable();
    
    return (XORP_OK);
}

int
XrlMld6igmpNode::disable_mld6igmp()
{
    Mld6igmpNode::disable();
    
    return (XORP_OK);
}

int
XrlMld6igmpNode::start_mld6igmp()
{
    if (Mld6igmpNode::start() < 0)
	return (XORP_ERROR);
    
    return (XORP_OK);
}

int
XrlMld6igmpNode::stop_mld6igmp()
{
    if (Mld6igmpNode::stop() < 0)
	return (XORP_ERROR);
    
    return (XORP_OK);
}

//
// Finder-related events
//
/**
 * Called when Finder connection is established.
 *
 * Note that this method overwrites an XrlRouter virtual method.
 */
void
XrlMld6igmpNode::finder_connect_event()
{
    _is_finder_alive = true;
}

/**
 * Called when Finder disconnect occurs.
 *
 * Note that this method overwrites an XrlRouter virtual method.
 */
void
XrlMld6igmpNode::finder_disconnect_event()
{
    XLOG_ERROR("Finder disconnect event. Exiting immediately...");

    _is_finder_alive = false;

    Mld6igmpNode::set_status(SERVICE_FAILED);
    Mld6igmpNode::update_status();
}

//
// Register with the MFEA
//
void
XrlMld6igmpNode::mfea_register_startup()
{
    bool success;

    _mfea_register_startup_timer.unschedule();
    _mfea_register_shutdown_timer.unschedule();

    if (! _is_finder_alive)
	return;		// The Finder is dead

    if (_is_mfea_registered)
	return;		// Already registered

    if (! _is_mfea_registering) {
	Mld6igmpNode::incr_startup_requests_n();  // XXX: for add_protocol
	// XXX: another incr to wait for network interface information
	// on startup.
	Mld6igmpNode::set_vif_setup_completed(false);
	Mld6igmpNode::incr_startup_requests_n();

	_is_mfea_registering = true;
    }

    //
    // Register interest in the MFEA with the Finder
    //
    success = _xrl_finder_client.send_register_class_event_interest(
	_finder_target.c_str(), _instance_name, _mfea_target,
	callback(this, &XrlMld6igmpNode::finder_register_interest_mfea_cb));

    if (! success) {
	//
	// If an error, then start a timer to try again.
	//
	_mfea_register_startup_timer = _eventloop.new_oneoff_after(
	    RETRY_TIMEVAL,
	    callback(this, &XrlMld6igmpNode::mfea_register_startup));
	return;
    }
}

void
XrlMld6igmpNode::finder_register_interest_mfea_cb(const XrlError& xrl_error)
{
    switch (xrl_error.error_code()) {
    case OKAY:
	//
	// If success, then the MFEA birth event will startup the MFEA
	// registration.
	//
	_is_mfea_registering = false;
	_is_mfea_registered = true;
	break;

    case COMMAND_FAILED:
	//
	// If a command failed because the other side rejected it, this is
	// fatal.
	//
	XLOG_FATAL("Cannot register interest in finder events: %s",
		   xrl_error.str().c_str());
	break;

    case NO_FINDER:
    case RESOLVE_FAILED:
	//
	// A communication error that should have been caught elsewhere
	// (e.g., by tracking the status of the finder and the other targets).
	// Probably we caught it here because of event reordering.
	// In some cases we print an error. In other cases our job is done.
	//
	XLOG_ERROR("XRL communication error: %s", xrl_error.str().c_str());
	break;

    case BAD_ARGS:
    case NO_SUCH_METHOD:
    case SEND_FAILED:
    case INTERNAL_ERROR:
	//
	// An error that should happen only if there is something unusual:
	// e.g., there is XRL mismatch, no enough internal resources, etc.
	// We don't try to recover from such errors, hence this is fatal.
	//
	XLOG_FATAL("Fatal XRL error: %s", xrl_error.str().c_str());
	break;

    case REPLY_TIMED_OUT:
    case SEND_FAILED_TRANSIENT:
	//
	// If a transient error, then start a timer to try again
	// (unless the timer is already running).
	//
	if (! _mfea_register_startup_timer.scheduled()) {
	    XLOG_ERROR("Failed to register interest in finder events: %s. "
		       "Will try again.",
		       xrl_error.str().c_str());
	    _mfea_register_startup_timer = _eventloop.new_oneoff_after(
		RETRY_TIMEVAL,
		callback(this, &XrlMld6igmpNode::mfea_register_startup));
	}
	break;
    }
}

//
// De-register with the MFEA
//
void
XrlMld6igmpNode::mfea_register_shutdown()
{
    bool success;

    _mfea_register_startup_timer.unschedule();
    _mfea_register_shutdown_timer.unschedule();

    if (! _is_finder_alive)
	return;		// The Finder is dead

    if (! _is_mfea_alive)
	return;		// The MFEA is not there anymore

    if (! _is_mfea_registered)
	return;		// Not registered

    if (! _is_mfea_deregistering) {
	Mld6igmpNode::incr_shutdown_requests_n();  // XXX: for delete_protocol

	_is_mfea_deregistering = true;
    }

    //
    // De-register interest in the MFEA with the Finder
    //
    success = _xrl_finder_client.send_deregister_class_event_interest(
	_finder_target.c_str(), _instance_name, _mfea_target,
	callback(this, &XrlMld6igmpNode::finder_deregister_interest_mfea_cb));

    if (! success) {
	//
	// If an error, then start a timer to try again.
	//
	_mfea_register_shutdown_timer = _eventloop.new_oneoff_after(
	    RETRY_TIMEVAL,
	    callback(this, &XrlMld6igmpNode::mfea_register_shutdown));
	return;
    }

    send_mfea_delete_protocol();
}

void
XrlMld6igmpNode::finder_deregister_interest_mfea_cb(const XrlError& xrl_error)
{
    switch (xrl_error.error_code()) {
    case OKAY:
	//
	// If success, then we are done
	//
	_is_mfea_deregistering = false;
	_is_mfea_registered = false;
	break;

    case COMMAND_FAILED:
	//
	// If a command failed because the other side rejected it, this is
	// fatal.
	//
	XLOG_FATAL("Cannot deregister interest in finder events: %s",
		   xrl_error.str().c_str());
	break;

    case NO_FINDER:
    case RESOLVE_FAILED:
	//
	// A communication error that should have been caught elsewhere
	// (e.g., by tracking the status of the finder and the other targets).
	// Probably we caught it here because of event reordering.
	// In some cases we print an error. In other cases our job is done.
	//
	_is_mfea_deregistering = false;
	_is_mfea_registered = false;
	break;

    case BAD_ARGS:
    case NO_SUCH_METHOD:
    case SEND_FAILED:
    case INTERNAL_ERROR:
	//
	// An error that should happen only if there is something unusual:
	// e.g., there is XRL mismatch, no enough internal resources, etc.
	// We don't try to recover from such errors, hence this is fatal.
	//
	XLOG_FATAL("Fatal XRL error: %s", xrl_error.str().c_str());
	break;

    case REPLY_TIMED_OUT:
    case SEND_FAILED_TRANSIENT:
	//
	// If a transient error, then start a timer to try again
	// (unless the timer is already running).
	//
	if (! _mfea_register_shutdown_timer.scheduled()) {
	    XLOG_ERROR("Failed to deregister interest in finder events: %s. "
		       "Will try again.",
		       xrl_error.str().c_str());
	    _mfea_register_shutdown_timer = _eventloop.new_oneoff_after(
		RETRY_TIMEVAL,
		callback(this, &XrlMld6igmpNode::mfea_register_shutdown));
	}
	break;
    }
}

//
// Add protocol with the MFEA
//
void
XrlMld6igmpNode::send_mfea_add_protocol()
{
    bool success = true;

    if (! _is_finder_alive)
	return;		// The Finder is dead

    //
    // Register the protocol with the MFEA
    //
    if (! _is_mfea_add_protocol_registered) {
	if (Mld6igmpNode::is_ipv4()) {
	    success = _xrl_mfea_client.send_add_protocol4(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		callback(this, &XrlMld6igmpNode::mfea_client_send_add_protocol_cb));
	    if (success)
		return;
	}

	if (Mld6igmpNode::is_ipv6()) {
	    success = _xrl_mfea_client.send_add_protocol6(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		callback(this, &XrlMld6igmpNode::mfea_client_send_add_protocol_cb));
	    if (success)
		return;
	}
    }

    if (! success) {
	//
	// If an error, then start a timer to try again
	//
	XLOG_ERROR("Failed to add protocol with the MFEA. "
		   "Will try again.");
	_mfea_add_protocol_timer = _eventloop.new_oneoff_after(
	    RETRY_TIMEVAL,
	    callback(this, &XrlMld6igmpNode::send_mfea_add_protocol));
	return;
    }
}

void
XrlMld6igmpNode::mfea_client_send_add_protocol_cb(const XrlError& xrl_error)
{
    switch (xrl_error.error_code()) {
    case OKAY:
	//
	// If success, then we are done
	//
	_is_mfea_add_protocol_registered = true;
	send_mfea_add_protocol();
	Mld6igmpNode::decr_startup_requests_n();
	break;

    case COMMAND_FAILED:
	//
	// If a command failed because the other side rejected it, this is
	// fatal.
	//
	XLOG_FATAL("Cannot add protocol with the MFEA: %s",
		   xrl_error.str().c_str());
	break;

    case NO_FINDER:
    case RESOLVE_FAILED:
	//
	// A communication error that should have been caught elsewhere
	// (e.g., by tracking the status of the finder and the other targets).
	// Probably we caught it here because of event reordering.
	// In some cases we print an error. In other cases our job is done.
	//
	XLOG_ERROR("XRL communication error: %s", xrl_error.str().c_str());
	break;

    case BAD_ARGS:
    case NO_SUCH_METHOD:
    case SEND_FAILED:
    case INTERNAL_ERROR:
	//
	// An error that should happen only if there is something unusual:
	// e.g., there is XRL mismatch, no enough internal resources, etc.
	// We don't try to recover from such errors, hence this is fatal.
	//
	XLOG_FATAL("Fatal XRL error: %s", xrl_error.str().c_str());
	break;

    case REPLY_TIMED_OUT:
    case SEND_FAILED_TRANSIENT:
	//
	// If a transient error, then start a timer to try again
	// (unless the timer is already running).
	//
	if (! _mfea_add_protocol_timer.scheduled()) {
	    XLOG_ERROR("Failed to add protocol with the MFEA: %s. "
		       "Will try again.",
		       xrl_error.str().c_str());
	    _mfea_add_protocol_timer = _eventloop.new_oneoff_after(
		RETRY_TIMEVAL,
		callback(this, &XrlMld6igmpNode::send_mfea_add_protocol));
	}
	break;
    }
}

//
// Delete protocol with the MFEA
//
void
XrlMld6igmpNode::send_mfea_delete_protocol()
{
    bool success = true;

    if (! _is_finder_alive)
	return;		// The Finder is dead

    if (_is_mfea_add_protocol_registered) {
	if (Mld6igmpNode::is_ipv4()) {
	    bool success4;
	    success4 = _xrl_mfea_client.send_delete_protocol4(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		callback(this, &XrlMld6igmpNode::mfea_client_send_delete_protocol_cb));
	    if (success4 != true)
		success = false;
	}

	if (Mld6igmpNode::is_ipv6()) {
	    bool success6;
	    success6 = _xrl_mfea_client.send_delete_protocol6(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		callback(this, &XrlMld6igmpNode::mfea_client_send_delete_protocol_cb));
	    if (success6 != true)
		success = false;
	}
    }

    if (! success) {
	XLOG_ERROR("Failed to delete protocol with the MFEA. "
		   "Will give up.");
	Mld6igmpNode::set_status(SERVICE_FAILED);
	Mld6igmpNode::update_status();
    }
}

void
XrlMld6igmpNode::mfea_client_send_delete_protocol_cb(const XrlError& xrl_error)
{
    switch (xrl_error.error_code()) {
    case OKAY:
	//
	// If success, then we are done
	//
	_is_mfea_add_protocol_registered = false;
	Mld6igmpNode::decr_shutdown_requests_n();
	break;

    case COMMAND_FAILED:
	//
	// If a command failed because the other side rejected it, this is
	// fatal.
	//
	XLOG_FATAL("Cannot delete protocol with the MFEA: %s",
		   xrl_error.str().c_str());
	break;

    case NO_FINDER:
    case RESOLVE_FAILED:
	//
	// A communication error that should have been caught elsewhere
	// (e.g., by tracking the status of the finder and the other targets).
	// Probably we caught it here because of event reordering.
	// In some cases we print an error. In other cases our job is done.
	//
	_is_mfea_add_protocol_registered = false;
	Mld6igmpNode::decr_shutdown_requests_n();
	break;

    case BAD_ARGS:
    case NO_SUCH_METHOD:
    case SEND_FAILED:
    case INTERNAL_ERROR:
	//
	// An error that should happen only if there is something unusual:
	// e.g., there is XRL mismatch, no enough internal resources, etc.
	// We don't try to recover from such errors, hence this is fatal.
	//
	XLOG_FATAL("Fatal XRL error: %s", xrl_error.str().c_str());
	break;

    case REPLY_TIMED_OUT:
    case SEND_FAILED_TRANSIENT:
	//
	// If a transient error, then start a timer to try again
	// (unless the timer is already running).
	//
	if (! _mfea_register_shutdown_timer.scheduled()) {
	    XLOG_ERROR("Failed to delete protocol with the MFEA: %s. "
		       "Will try again.",
		       xrl_error.str().c_str());
	    _mfea_register_shutdown_timer = _eventloop.new_oneoff_after(
		RETRY_TIMEVAL,
	    callback(this, &XrlMld6igmpNode::mfea_register_shutdown));
	}
	break;
    }
}

int
XrlMld6igmpNode::start_protocol_kernel_vif(uint16_t vif_index)
{
    Mld6igmpVif *mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(vif_index);
    
    if (mld6igmp_vif == NULL) {
	XLOG_ERROR("Cannot start in the kernel vif with vif_index %d: "
		   "no such vif", vif_index);
	return (XORP_ERROR);
    }
    
    _start_stop_protocol_kernel_vif_queue.push_back(make_pair(vif_index, true));

    // If the queue was empty before, start sending the changes
    if (_start_stop_protocol_kernel_vif_queue.size() == 1) {
	send_start_stop_protocol_kernel_vif();
    }

    return (XORP_OK);
}

int
XrlMld6igmpNode::stop_protocol_kernel_vif(uint16_t vif_index)
{
    Mld6igmpVif *mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(vif_index);
    
    if (mld6igmp_vif == NULL) {
	XLOG_ERROR("Cannot stop in the kernel vif with vif_index %d: "
		   "no such vif", vif_index);
	return (XORP_ERROR);
    }
    
    _start_stop_protocol_kernel_vif_queue.push_back(make_pair(vif_index, false));

    // If the queue was empty before, start sending the changes
    if (_start_stop_protocol_kernel_vif_queue.size() == 1) {
	send_start_stop_protocol_kernel_vif();
    }

    return (XORP_OK);
}

void
XrlMld6igmpNode::send_start_stop_protocol_kernel_vif()
{
    bool success = true;
    Mld6igmpVif *mld6igmp_vif = NULL;

    if (! _is_finder_alive)
	return;		// The Finder is dead

    if (_start_stop_protocol_kernel_vif_queue.empty())
	return;			// No more changes

    uint16_t vif_index = _start_stop_protocol_kernel_vif_queue.front().first;
    bool is_start = _start_stop_protocol_kernel_vif_queue.front().second;

    //
    // Check whether we have already registered with the MFEA
    //
    if (! _is_mfea_add_protocol_registered) {
	success = false;
	goto start_timer_label;
    }

    mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(vif_index);
    if (mld6igmp_vif == NULL) {
	XLOG_ERROR("Cannot %s protocol vif with vif_index %d with the MFEA: "
		   "no such vif",
		   (is_start)? "start" : "stop",
		   vif_index);
	_start_stop_protocol_kernel_vif_queue.pop_front();
	return;
    }

    if (is_start) {
	// Start a vif with the MFEA
	if (Mld6igmpNode::is_ipv4()) {
	    success = _xrl_mfea_client.send_start_protocol_vif4(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		mld6igmp_vif->name(),
		vif_index,
		callback(this, &XrlMld6igmpNode::mfea_client_send_start_stop_protocol_kernel_vif_cb));
	    if (success) {
		Mld6igmpNode::incr_startup_requests_n();
		return;
	    }
	}

	if (Mld6igmpNode::is_ipv6()) {
	    success = _xrl_mfea_client.send_start_protocol_vif6(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		mld6igmp_vif->name(),
		vif_index,
		callback(this, &XrlMld6igmpNode::mfea_client_send_start_stop_protocol_kernel_vif_cb));
	    if (success) {
		Mld6igmpNode::incr_startup_requests_n();
		return;
	    }
	}
    } else {
	// Stop a vif with the MFEA
	if (Mld6igmpNode::is_ipv4()) {
	    success = _xrl_mfea_client.send_stop_protocol_vif4(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		mld6igmp_vif->name(),
		vif_index,
		callback(this, &XrlMld6igmpNode::mfea_client_send_start_stop_protocol_kernel_vif_cb));
	    if (success) {
		Mld6igmpNode::incr_shutdown_requests_n();
		return;
	    }
	}

	if (Mld6igmpNode::is_ipv6()) {
	    success = _xrl_mfea_client.send_stop_protocol_vif6(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		mld6igmp_vif->name(),
		vif_index,
		callback(this, &XrlMld6igmpNode::mfea_client_send_start_stop_protocol_kernel_vif_cb));
	    if (success) {
		Mld6igmpNode::incr_shutdown_requests_n();
		return;
	    }
	}
    }

    if (! success) {
        //
        // If an error, then start a timer to try again
        //
	XLOG_ERROR("Failed to %s protocol vif %s with the MFEA. "
		   "Will try again.",
		   (is_start)? "start" : "stop",
		   mld6igmp_vif->name().c_str());
    start_timer_label:
	_start_stop_protocol_kernel_vif_queue_timer = _eventloop.new_oneoff_after(
	    RETRY_TIMEVAL,
	    callback(this, &XrlMld6igmpNode::send_start_stop_protocol_kernel_vif));
    }
}

void
XrlMld6igmpNode::mfea_client_send_start_stop_protocol_kernel_vif_cb(
    const XrlError& xrl_error)
{
    bool is_start = _start_stop_protocol_kernel_vif_queue.front().second;

    switch (xrl_error.error_code()) {
    case OKAY:
	//
	// If success, then send the next change
	//
	_start_stop_protocol_kernel_vif_queue.pop_front();
	send_start_stop_protocol_kernel_vif();
	if (is_start)
	    Mld6igmpNode::decr_startup_requests_n();
	else
	    Mld6igmpNode::decr_shutdown_requests_n();
	break;

    case COMMAND_FAILED:
	//
	// If a command failed because the other side rejected it, this is
	// fatal.
	//
	XLOG_FATAL("Cannot %s protocol vif with the MFEA: %s",
		   (is_start)? "start" : "stop",
		   xrl_error.str().c_str());
	break;

    case NO_FINDER:
    case RESOLVE_FAILED:
	//
	// A communication error that should have been caught elsewhere
	// (e.g., by tracking the status of the finder and the other targets).
	// Probably we caught it here because of event reordering.
	// In some cases we print an error. In other cases our job is done.
	//
	if (is_start) {
	    XLOG_ERROR("XRL communication error: %s", xrl_error.str().c_str());
	} else {
	    _start_stop_protocol_kernel_vif_queue.pop_front();
	    send_start_stop_protocol_kernel_vif();
	    Mld6igmpNode::decr_shutdown_requests_n();
	}
	break;

    case BAD_ARGS:
    case NO_SUCH_METHOD:
    case SEND_FAILED:
    case INTERNAL_ERROR:
	//
	// An error that should happen only if there is something unusual:
	// e.g., there is XRL mismatch, no enough internal resources, etc.
	// We don't try to recover from such errors, hence this is fatal.
	//
	XLOG_FATAL("Fatal XRL error: %s", xrl_error.str().c_str());
	break;

    case REPLY_TIMED_OUT:
    case SEND_FAILED_TRANSIENT:
	//
	// If a transient error, then start a timer to try again
	// (unless the timer is already running).
	//
	if (! _start_stop_protocol_kernel_vif_queue_timer.scheduled()) {
	    XLOG_ERROR("Failed to %s protocol vif with the MFEA: %s. "
		       "Will try again.",
		       (is_start)? "start" : "stop",
		       xrl_error.str().c_str());
	    _start_stop_protocol_kernel_vif_queue_timer = _eventloop.new_oneoff_after(
		RETRY_TIMEVAL,
		callback(this, &XrlMld6igmpNode::send_start_stop_protocol_kernel_vif));
	}
	break;
    }
}

int
XrlMld6igmpNode::join_multicast_group(uint16_t vif_index,
				 const IPvX& multicast_group)
{
    Mld6igmpVif *mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(vif_index);
    
    if (mld6igmp_vif == NULL) {
	XLOG_ERROR("Cannot join group %s on vif with vif_index %d: "
		   "no such vif", cstring(multicast_group), vif_index);
	return (XORP_ERROR);
    }
    
    _join_leave_multicast_group_queue.push_back(
	JoinLeaveMulticastGroup(vif_index, multicast_group, true));

    // If the queue was empty before, start sending the changes
    if (_join_leave_multicast_group_queue.size() == 1) {
	send_join_leave_multicast_group();
    }

    return (XORP_OK);
}

int
XrlMld6igmpNode::leave_multicast_group(uint16_t vif_index,
				       const IPvX& multicast_group)
{
    Mld6igmpVif *mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(vif_index);
    
    if (mld6igmp_vif == NULL) {
	XLOG_ERROR("Cannot leave group %s on vif with vif_index %d: "
		   "no such vif", cstring(multicast_group), vif_index);
	return (XORP_ERROR);
    }
    
    _join_leave_multicast_group_queue.push_back(
	JoinLeaveMulticastGroup(vif_index, multicast_group, false));

    // If the queue was empty before, start sending the changes
    if (_join_leave_multicast_group_queue.size() == 1) {
	send_join_leave_multicast_group();
    }

    return (XORP_OK);
}

void
XrlMld6igmpNode::send_join_leave_multicast_group()
{
    bool success = true;
    Mld6igmpVif *mld6igmp_vif = NULL;

    if (! _is_finder_alive)
	return;		// The Finder is dead

    if (_join_leave_multicast_group_queue.empty())
	return;			// No more changes

    JoinLeaveMulticastGroup& group = _join_leave_multicast_group_queue.front();
    bool is_join = group.is_join();

    //
    // Check whether we have already registered with the MFEA
    //
    if (! _is_mfea_add_protocol_registered) {
	success = false;
	goto start_timer_label;
    }

    mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(group.vif_index());
    if (mld6igmp_vif == NULL) {
	XLOG_ERROR("Cannot %s group %s on vif with vif_index %d: "
		   "no such vif",
		   (is_join)? "join" : "leave",
		   cstring(group.multicast_group()),
		   group.vif_index());
	_join_leave_multicast_group_queue.pop_front();
	return;
    }

    if (is_join) {
	// Join a multicast group on a vif with the MFEA
	if (Mld6igmpNode::is_ipv4()) {
	    success = _xrl_mfea_client.send_join_multicast_group4(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		mld6igmp_vif->name(),
		group.vif_index(),
		group.multicast_group().get_ipv4(),
		callback(this, &XrlMld6igmpNode::mfea_client_send_join_leave_multicast_group_cb));
	    if (success) {
		Mld6igmpNode::incr_startup_requests_n();
		return;
	    }
	}

	if (Mld6igmpNode::is_ipv6()) {
	    success = _xrl_mfea_client.send_join_multicast_group6(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		mld6igmp_vif->name(),
		group.vif_index(),
		group.multicast_group().get_ipv6(),
		callback(this, &XrlMld6igmpNode::mfea_client_send_join_leave_multicast_group_cb));
	    if (success) {
		Mld6igmpNode::incr_startup_requests_n();
		return;
	    }
	}
    } else {
	// Leave a multicast group on a vif with the MFEA
	if (Mld6igmpNode::is_ipv4()) {
	    success = _xrl_mfea_client.send_leave_multicast_group4(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		mld6igmp_vif->name(),
		group.vif_index(),
		group.multicast_group().get_ipv4(),
		callback(this, &XrlMld6igmpNode::mfea_client_send_join_leave_multicast_group_cb));
	    if (success) {
		Mld6igmpNode::incr_shutdown_requests_n();
		return;
	    }
	}

	if (Mld6igmpNode::is_ipv6()) {
	    success = _xrl_mfea_client.send_leave_multicast_group6(
		_mfea_target.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		mld6igmp_vif->name(),
		group.vif_index(),
		group.multicast_group().get_ipv6(),
		callback(this, &XrlMld6igmpNode::mfea_client_send_join_leave_multicast_group_cb));
	    if (success) {
		Mld6igmpNode::incr_shutdown_requests_n();
		return;
	    }
	}
    }

    if (! success) {
        //
        // If an error, then start a timer to try again
        //
	XLOG_ERROR("Failed to %s group %s on vif %s with the MFEA. "
		   "Will try again.",
		   (is_join)? "join" : "leave",
		   cstring(group.multicast_group()),
		   mld6igmp_vif->name().c_str());
    start_timer_label:
	_join_leave_multicast_group_queue_timer = _eventloop.new_oneoff_after(
	    RETRY_TIMEVAL,
	    callback(this, &XrlMld6igmpNode::send_join_leave_multicast_group));
    }
}

void
XrlMld6igmpNode::mfea_client_send_join_leave_multicast_group_cb(
    const XrlError& xrl_error)
{
    bool is_join = _join_leave_multicast_group_queue.front().is_join();

    switch (xrl_error.error_code()) {
    case OKAY:
	//
	// If success, then send the next change
	//
	_join_leave_multicast_group_queue.pop_front();
	send_join_leave_multicast_group();
	if (is_join)
	    Mld6igmpNode::decr_startup_requests_n();
	else
	    Mld6igmpNode::decr_shutdown_requests_n();
	break;

    case COMMAND_FAILED:
	//
	// If a command failed because the other side rejected it, this is
	// fatal.
	//
	XLOG_FATAL("Cannot %s a multicast group with the MFEA: %s",
		   (is_join)? "join" : "leave",
		   xrl_error.str().c_str());
	break;

    case NO_FINDER:
    case RESOLVE_FAILED:
	//
	// A communication error that should have been caught elsewhere
	// (e.g., by tracking the status of the finder and the other targets).
	// Probably we caught it here because of event reordering.
	// In some cases we print an error. In other cases our job is done.
	//
	if (is_join) {
	    XLOG_ERROR("XRL communication error: %s", xrl_error.str().c_str());
	} else {
	    _join_leave_multicast_group_queue.pop_front();
	    send_join_leave_multicast_group();
	    Mld6igmpNode::decr_shutdown_requests_n();
	}
	break;

    case BAD_ARGS:
    case NO_SUCH_METHOD:
    case SEND_FAILED:
    case INTERNAL_ERROR:
	//
	// An error that should happen only if there is something unusual:
	// e.g., there is XRL mismatch, no enough internal resources, etc.
	// We don't try to recover from such errors, hence this is fatal.
	//
	XLOG_FATAL("Fatal XRL error: %s", xrl_error.str().c_str());
	break;

    case REPLY_TIMED_OUT:
    case SEND_FAILED_TRANSIENT:
	//
	// If a transient error, then start a timer to try again
	// (unless the timer is already running).
	//
	if (! _join_leave_multicast_group_queue_timer.scheduled()) {
	    XLOG_ERROR("Failed to %s a multicast group with the MFEA: %s. "
		       "Will try again.",
		       (is_join)? "join" : "leave",
		       xrl_error.str().c_str());
	    _join_leave_multicast_group_queue_timer = _eventloop.new_oneoff_after(
		RETRY_TIMEVAL,
		callback(this, &XrlMld6igmpNode::send_join_leave_multicast_group));
	}
	break;
    }
}

int
XrlMld6igmpNode::send_add_membership(const string& dst_module_instance_name,
				     xorp_module_id dst_module_id,
				     uint16_t vif_index,
				     const IPvX& source,
				     const IPvX& group)
{
    Mld6igmpVif *mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(vif_index);
    
    if (mld6igmp_vif == NULL) {
	XLOG_ERROR("Cannot send add_membership to %s for (%s,%s) on vif "
		   "with vif_index %d: no such vif",
		   dst_module_instance_name.c_str(),
		   cstring(source),
		   cstring(group),
		   vif_index);
	return (XORP_ERROR);
    }

    _send_add_delete_membership_queue.push_back(SendAddDeleteMembership(
						    dst_module_instance_name,
						    dst_module_id,
						    vif_index,
						    source,
						    group,
						    true));

    // If the queue was empty before, start sending the changes
    if (_send_add_delete_membership_queue.size() == 1) {
	send_add_delete_membership();
    }

    return (XORP_OK);
}

int
XrlMld6igmpNode::send_delete_membership(const string& dst_module_instance_name,
					xorp_module_id dst_module_id,
					uint16_t vif_index,
					const IPvX& source,
					const IPvX& group)
{
    Mld6igmpVif *mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(vif_index);
    
    if (mld6igmp_vif == NULL) {
	XLOG_ERROR("Cannot send delete_membership to %s for (%s,%s) on vif "
		   "with vif_index %d: no such vif",
		   dst_module_instance_name.c_str(),
		   cstring(source),
		   cstring(group),
		   vif_index);
	return (XORP_ERROR);
    }

    _send_add_delete_membership_queue.push_back(SendAddDeleteMembership(
						    dst_module_instance_name,
						    dst_module_id,
						    vif_index,
						    source,
						    group,
						    false));

    // If the queue was empty before, start sending the changes
    if (_send_add_delete_membership_queue.size() == 1) {
	send_add_delete_membership();
    }

    return (XORP_OK);
}

void
XrlMld6igmpNode::send_add_delete_membership()
{
    bool success = true;
    Mld6igmpVif *mld6igmp_vif = NULL;

    if (! _is_finder_alive)
	return;		// The Finder is dead

    if (_send_add_delete_membership_queue.empty())
	return;			// No more changes

    const SendAddDeleteMembership& membership = _send_add_delete_membership_queue.front();
    bool is_add = membership.is_add();

    mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(membership.vif_index());
    if (mld6igmp_vif == NULL) {
	XLOG_ERROR("Cannot send %s for (%s,%s) on vif "
		   "with vif_index %d to %s: no such vif",
		   (is_add)? "add_membership" : "delete_membership",
		   cstring(membership.source()),
		   cstring(membership.group()),
		   membership.vif_index(),
		   membership.dst_module_instance_name().c_str());
	_send_add_delete_membership_queue.pop_front();
	goto start_timer_label;
    }

    if (is_add) {
	// Send add_membership to the client protocol
	if (Mld6igmpNode::is_ipv4()) {
	    success = _xrl_mld6igmp_client_client.send_add_membership4(
		membership.dst_module_instance_name().c_str(),
		my_xrl_target_name(),
		mld6igmp_vif->name(),
		membership.vif_index(),
		membership.source().get_ipv4(),
		membership.group().get_ipv4(),
		callback(this, &XrlMld6igmpNode::mld6igmp_client_send_add_delete_membership_cb));
	    if (success)
		return;
	}

	if (Mld6igmpNode::is_ipv6()) {
	    success = _xrl_mld6igmp_client_client.send_add_membership6(
		membership.dst_module_instance_name().c_str(),
		my_xrl_target_name(),
		mld6igmp_vif->name(),
		membership.vif_index(),
		membership.source().get_ipv6(),
		membership.group().get_ipv6(),
		callback(this, &XrlMld6igmpNode::mld6igmp_client_send_add_delete_membership_cb));
	    if (success)
		return;
	}
    } else {
	// Send delete_membership to the client protocol
	if (Mld6igmpNode::is_ipv4()) {
	    success = _xrl_mld6igmp_client_client.send_delete_membership4(
		membership.dst_module_instance_name().c_str(),
		my_xrl_target_name(),
		mld6igmp_vif->name(),
		membership.vif_index(),
		membership.source().get_ipv4(),
		membership.group().get_ipv4(),
		callback(this, &XrlMld6igmpNode::mld6igmp_client_send_add_delete_membership_cb));
	    if (success)
		return;
	}

	if (Mld6igmpNode::is_ipv6()) {
	    success = _xrl_mld6igmp_client_client.send_delete_membership6(
		membership.dst_module_instance_name().c_str(),
		my_xrl_target_name(),
		mld6igmp_vif->name(),
		membership.vif_index(),
		membership.source().get_ipv6(),
		membership.group().get_ipv6(),
		callback(this, &XrlMld6igmpNode::mld6igmp_client_send_add_delete_membership_cb));
	    if (success)
		return;
	}
    }

    if (! success) {
        //
        // If an error, then start a timer to try again
        //
	XLOG_ERROR("Failed to send %s for (%s,%s) on vif %s to %s. "
		   "Will try again.",
		   (is_add)? "add_membership" : "delete_membership",
		   cstring(membership.source()),
		   cstring(membership.group()),
		   mld6igmp_vif->name().c_str(),
		   membership.dst_module_instance_name().c_str());
    start_timer_label:
	_send_add_delete_membership_queue_timer = _eventloop.new_oneoff_after(
	    RETRY_TIMEVAL,
	    callback(this, &XrlMld6igmpNode::send_add_delete_membership));
    }
}

void
XrlMld6igmpNode::mld6igmp_client_send_add_delete_membership_cb(
    const XrlError& xrl_error)
{
    bool is_add = _send_add_delete_membership_queue.front().is_add();

    switch (xrl_error.error_code()) {
    case OKAY:
	//
	// If success, then send the next change
	//
	_send_add_delete_membership_queue.pop_front();
	send_add_delete_membership();
	break;

    case COMMAND_FAILED:
	//
	// If a command failed because the other side rejected it, this is
	// fatal.
	//
	XLOG_FATAL("Cannot %s a multicast group with a client: %s",
		   (is_add)? "add" : "delete",
		   xrl_error.str().c_str());
	break;

    case NO_FINDER:
    case RESOLVE_FAILED:
	//
	// A communication error that should have been caught elsewhere
	// (e.g., by tracking the status of the finder and the other targets).
	// Probably we caught it here because of event reordering.
	// In some cases we print an error. In other cases our job is done.
	//
	XLOG_ERROR("XRL communication error: %s", xrl_error.str().c_str());
	break;

    case BAD_ARGS:
    case NO_SUCH_METHOD:
    case SEND_FAILED:
    case INTERNAL_ERROR:
	//
	// An error that should happen only if there is something unusual:
	// e.g., there is XRL mismatch, no enough internal resources, etc.
	// We don't try to recover from such errors, hence this is fatal.
	//
	XLOG_FATAL("Fatal XRL error: %s", xrl_error.str().c_str());
	break;

    case REPLY_TIMED_OUT:
    case SEND_FAILED_TRANSIENT:
	//
	// If a transient error, then start a timer to try again
	// (unless the timer is already running).
	//
	if (! _send_add_delete_membership_queue_timer.scheduled()) {
	    XLOG_ERROR("Failed to %s a multicast group with a client: %s. "
		       "Will try again.",
		       (is_add)? "add" : "delete",
		       xrl_error.str().c_str());
	    _send_add_delete_membership_queue_timer = _eventloop.new_oneoff_after(
		RETRY_TIMEVAL,
		callback(this, &XrlMld6igmpNode::send_add_delete_membership));
	}
	break;
    }
}

//
// Protocol node methods
//

/**
 * XrlMld6igmpNode::proto_send:
 * @dst_module_instance name: The name of the protocol instance-destination
 * of the message.
 * @dst_module_id: The #xorp_module_id of the destination of the message.
 * @vif_index: The vif index of the interface to send this message.
 * @src: The source address of the message.
 * @dst: The destination address of the message.
 * @ip_ttl: The IP TTL of the message. If it has a negative value,
 * the TTL will be set by the lower layers.
 * @ip_tos: The IP TOS of the message. If it has a negative value,
 * the TOS will be set by the lower layers.
 * @router_alert_bool: If true, the Router Alert IP option for the IP
 * packet of the incoming message should be set.
 * @sndbuf: The data buffer with the message to send.
 * @sndlen: The data length in @sndbuf.
 * 
 * Send a protocol message through the FEA/MFEA.
 * 
 * Return value: %XORP_OK on success, otherwise %XORP_ERROR.
 **/
int
XrlMld6igmpNode::proto_send(const string& dst_module_instance_name,
			    xorp_module_id		, // dst_module_id,
			    uint16_t vif_index,
			    const IPvX& src,
			    const IPvX& dst,
			    int ip_ttl,
			    int ip_tos,
			    bool router_alert_bool,
			    const uint8_t *sndbuf,
			    size_t sndlen)
{
    Mld6igmpVif *mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(vif_index);

    if (! _is_finder_alive)
	return (XORP_ERROR);	// The Finder is dead

    if (mld6igmp_vif == NULL) {
	XLOG_ERROR("Cannot send a protocol message on vif with vif_index %d: "
		   "no such vif",
		   vif_index);
	return (XORP_ERROR);
    }
    
    // Copy 'sndbuf' to a vector
    vector<uint8_t> snd_vector;
    snd_vector.resize(sndlen);
    for (size_t i = 0; i < sndlen; i++)
	snd_vector[i] = sndbuf[i];
    
    do {
	if (dst.is_ipv4()) {
	    _xrl_mfea_client.send_send_protocol_message4(
		dst_module_instance_name.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		mld6igmp_vif->name(),
		vif_index,
		src.get_ipv4(),
		dst.get_ipv4(),
		ip_ttl,
		ip_tos,
		router_alert_bool,
		snd_vector,
		callback(this, &XrlMld6igmpNode::mfea_client_send_protocol_message_cb));
	    break;
	}
	
	if (dst.is_ipv6()) {
	    _xrl_mfea_client.send_send_protocol_message6(
		dst_module_instance_name.c_str(),
		my_xrl_target_name(),
		string(Mld6igmpNode::module_name()),
		Mld6igmpNode::module_id(),
		mld6igmp_vif->name(),
		vif_index,
		src.get_ipv6(),
		dst.get_ipv6(),
		ip_ttl,
		ip_tos,
		router_alert_bool,
		snd_vector,
		callback(this, &XrlMld6igmpNode::mfea_client_send_protocol_message_cb));
	    break;
	}
	
	XLOG_UNREACHABLE();
	break;
    } while (false);
    
    return (XORP_OK);
}

void
XrlMld6igmpNode::mfea_client_send_protocol_message_cb(
    const XrlError& xrl_error)

{
    switch (xrl_error.error_code()) {
    case OKAY:
	//
	// If success, then we are done
	//
	break;

    case COMMAND_FAILED:
	//
	// If a command failed because the other side rejected it, this is
	// fatal.
	//
	XLOG_FATAL("Cannot send a protocol message: %s",
		   xrl_error.str().c_str());
	break;

    case NO_FINDER:
    case RESOLVE_FAILED:
	//
	// A communication error that should have been caught elsewhere
	// (e.g., by tracking the status of the finder and the other targets).
	// Probably we caught it here because of event reordering.
	// In some cases we print an error. In other cases our job is done.
	//
	XLOG_ERROR("Cannot send a protocol message: %s",
		   xrl_error.str().c_str());
	break;

    case BAD_ARGS:
    case NO_SUCH_METHOD:
    case SEND_FAILED:
    case INTERNAL_ERROR:
	//
	// An error that should happen only if there is something unusual:
	// e.g., there is XRL mismatch, no enough internal resources, etc.
	// We don't try to recover from such errors, hence this is fatal.
	//
	XLOG_FATAL("Fatal XRL error: %s", xrl_error.str().c_str());
	break;

    case REPLY_TIMED_OUT:
    case SEND_FAILED_TRANSIENT:
	//
	// XXX: if a transient error, then don't try again.
	// All protocol messages are soft-state (i.e., they are
	// retransmitted periodically by the protocol),
	// hence we don't retransmit them here if there was an error.
	//
	XLOG_ERROR("Failed to send a protocol message: %s",
		   xrl_error.str().c_str());
	break;
    }
}

//
// Protocol node CLI methods
//
int
XrlMld6igmpNode::add_cli_command_to_cli_manager(const char *command_name,
						const char *command_help,
						bool is_command_cd,
						const char *command_cd_prompt,
						bool is_command_processor
    )
{
    if (! _is_finder_alive)
	return (XORP_ERROR);	// The Finder is dead

    _xrl_cli_manager_client.send_add_cli_command(
	xorp_module_name(family(), XORP_MODULE_CLI),
	my_xrl_target_name(),
	string(command_name),
	string(command_help),
	is_command_cd,
	string(command_cd_prompt),
	is_command_processor,
	callback(this, &XrlMld6igmpNode::cli_manager_client_send_add_cli_command_cb));
    
    return (XORP_OK);
}

void
XrlMld6igmpNode::cli_manager_client_send_add_cli_command_cb(
    const XrlError& xrl_error)
{
    switch (xrl_error.error_code()) {
    case OKAY:
	//
	// If success, then we are done
	//
	break;

    case COMMAND_FAILED:
	//
	// If a command failed because the other side rejected it, this is
	// fatal.
	//
	XLOG_FATAL("Cannot add a command to CLI manager: %s",
		   xrl_error.str().c_str());
	break;

    case NO_FINDER:
    case RESOLVE_FAILED:
	//
	// A communication error that should have been caught elsewhere
	// (e.g., by tracking the status of the finder and the other targets).
	// Probably we caught it here because of event reordering.
	// In some cases we print an error. In other cases our job is done.
	//
	XLOG_ERROR("Cannot add a command to CLI manager: %s",
		   xrl_error.str().c_str());
	break;

    case BAD_ARGS:
    case NO_SUCH_METHOD:
    case SEND_FAILED:
    case INTERNAL_ERROR:
	//
	// An error that should happen only if there is something unusual:
	// e.g., there is XRL mismatch, no enough internal resources, etc.
	// We don't try to recover from such errors, hence this is fatal.
	//
	XLOG_FATAL("Fatal XRL error: %s", xrl_error.str().c_str());
	break;

    case REPLY_TIMED_OUT:
    case SEND_FAILED_TRANSIENT:
	//
	// If a transient error, then start a timer to try again
	// (unless the timer is already running).
	//
	//
	// TODO: if the command failed, then we should retransmit it
	//
	XLOG_ERROR("Failed to add a command to CLI manager: %s",
		   xrl_error.str().c_str());
	break;
    }
}

int
XrlMld6igmpNode::delete_cli_command_from_cli_manager(const char *command_name)
{
    if (! _is_finder_alive)
	return (XORP_ERROR);	// The Finder is dead

    _xrl_cli_manager_client.send_delete_cli_command(
	xorp_module_name(family(), XORP_MODULE_CLI),
	my_xrl_target_name(),
	string(command_name),
	callback(this, &XrlMld6igmpNode::cli_manager_client_send_delete_cli_command_cb));
    
    return (XORP_OK);
}

void
XrlMld6igmpNode::cli_manager_client_send_delete_cli_command_cb(
    const XrlError& xrl_error)
{
    switch (xrl_error.error_code()) {
    case OKAY:
	//
	// If success, then we are done
	//
	break;

    case COMMAND_FAILED:
	//
	// If a command failed because the other side rejected it, this is
	// fatal.
	//
	XLOG_FATAL("Cannot delete a command from CLI manager: %s",
		   xrl_error.str().c_str());
	break;

    case NO_FINDER:
    case RESOLVE_FAILED:
	//
	// A communication error that should have been caught elsewhere
	// (e.g., by tracking the status of the finder and the other targets).
	// Probably we caught it here because of event reordering.
	// In some cases we print an error. In other cases our job is done.
	//
	XLOG_ERROR("Cannot delete a command from CLI manager: %s",
		   xrl_error.str().c_str());
	break;

    case BAD_ARGS:
    case NO_SUCH_METHOD:
    case SEND_FAILED:
    case INTERNAL_ERROR:
	//
	// An error that should happen only if there is something unusual:
	// e.g., there is XRL mismatch, no enough internal resources, etc.
	// We don't try to recover from such errors, hence this is fatal.
	//
	XLOG_FATAL("Fatal XRL error: %s", xrl_error.str().c_str());
	break;

    case REPLY_TIMED_OUT:
    case SEND_FAILED_TRANSIENT:
	//
	// If a transient error, then start a timer to try again
	// (unless the timer is already running).
	//
	//
	// TODO: if the command failed, then we should retransmit it
	//
	XLOG_ERROR("Failed to delete a command from CLI manager: %s",
		   xrl_error.str().c_str());
	break;
    }
}


//
// XRL target methods
//

XrlCmdError
XrlMld6igmpNode::common_0_1_get_target_name(
    // Output values, 
    string&		name)
{
    name = my_xrl_target_name();
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::common_0_1_get_version(
    // Output values, 
    string&		version)
{
    version = XORP_MODULE_VERSION;
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::common_0_1_get_status(// Output values, 
				       uint32_t& status,
				       string& reason)
{
    status = Mld6igmpNode::node_status(reason);
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::common_0_1_shutdown()
{
    bool is_error = false;
    string error_msg;

    if (stop_mld6igmp() != XORP_OK) {
	if (! is_error)
	    error_msg = c_format("Failed to stop %s",
				 Mld6igmpNode::proto_is_igmp() ?
				 "IGMP" : "MLD");
	is_error = true;
    }

    if (is_error)
	return XrlCmdError::COMMAND_FAILED(error_msg);

    return XrlCmdError::OKAY();
}

/**
 *  Announce target birth to observer.
 *
 *  @param target_class the target class name.
 *
 *  @param target_instance the target instance name.
 */
XrlCmdError
XrlMld6igmpNode::finder_event_observer_0_1_xrl_target_birth(
    // Input values,
    const string&	target_class,
    const string&	target_instance)
{
    if (target_class == _mfea_target) {
	_is_mfea_alive = true;
	send_mfea_add_protocol();
    }

    return XrlCmdError::OKAY();
    UNUSED(target_instance);
}

/**
 *  Announce target death to observer.
 *
 *  @param target_class the target class name.
 *
 *  @param target_instance the target instance name.
 */
XrlCmdError
XrlMld6igmpNode::finder_event_observer_0_1_xrl_target_death(
    // Input values,
    const string&	target_class,
    const string&	target_instance)
{
    bool do_shutdown = false;

    if (target_class == _mfea_target) {
	XLOG_ERROR("MFEA (instance %s) has died, shutting down.",
		   target_instance.c_str());
	_is_mfea_alive = false;
	do_shutdown = true;
    }

    if (do_shutdown)
	stop_mld6igmp();

    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::cli_processor_0_1_process_command(
    // Input values, 
    const string&	processor_name, 
    const string&	cli_term_name, 
    const uint32_t&	cli_session_id,
    const string&	command_name, 
    const string&	command_args, 
    // Output values, 
    string&		ret_processor_name, 
    string&		ret_cli_term_name, 
    uint32_t&		ret_cli_session_id,
    string&		ret_command_output)
{
    Mld6igmpNodeCli::cli_process_command(processor_name,
					 cli_term_name,
					 cli_session_id,
					 command_name,
					 command_args,
					 ret_processor_name,
					 ret_cli_term_name,
					 ret_cli_session_id,
					 ret_command_output);
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_new_vif(
    // Input values, 
    const string&	vif_name, 
    const uint32_t&	vif_index)
{
    string error_msg;
    
    if (Mld6igmpNode::add_vif(vif_name, vif_index, error_msg) != XORP_OK)
	return XrlCmdError::COMMAND_FAILED(error_msg);
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_delete_vif(
    // Input values, 
    const string&	vif_name)
{
    string error_msg;
    
    if (Mld6igmpNode::delete_vif(vif_name, error_msg) != XORP_OK)
	return XrlCmdError::COMMAND_FAILED(error_msg);
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_add_vif_addr4(
    // Input values, 
    const string&	vif_name, 
    const IPv4&		addr, 
    const IPv4Net&	subnet, 
    const IPv4&		broadcast, 
    const IPv4&		peer)
{
    string error_msg;
    
    if (Mld6igmpNode::add_vif_addr(vif_name,
				   IPvX(addr),
				   IPvXNet(subnet),
				   IPvX(broadcast),
				   IPvX(peer),
				   error_msg)
	!= XORP_OK) {
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_add_vif_addr6(
    // Input values, 
    const string&	vif_name, 
    const IPv6&		addr, 
    const IPv6Net&	subnet, 
    const IPv6&		broadcast, 
    const IPv6&		peer)
{
    string error_msg;
    
    if (Mld6igmpNode::add_vif_addr(vif_name,
				   IPvX(addr),
				   IPvXNet(subnet),
				   IPvX(broadcast),
				   IPvX(peer),
				   error_msg)
	!= XORP_OK) {
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_delete_vif_addr4(
    // Input values, 
    const string&	vif_name, 
    const IPv4&		addr)
{
    string error_msg;
    
    if (Mld6igmpNode::delete_vif_addr(vif_name,
				      IPvX(addr),
				      error_msg)
	!= XORP_OK) {
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_delete_vif_addr6(
    // Input values, 
    const string&	vif_name, 
    const IPv6&		addr)
{
    string error_msg;
    
    if (Mld6igmpNode::delete_vif_addr(vif_name,
				      IPvX(addr),
				      error_msg)
	!= XORP_OK) {
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_set_vif_flags(
    // Input values, 
    const string&	vif_name, 
    const bool&		is_pim_register, 
    const bool&		is_p2p, 
    const bool&		is_loopback, 
    const bool&		is_multicast, 
    const bool&		is_broadcast, 
    const bool&		is_up)
{
    string error_msg;
    
    if (Mld6igmpNode::set_vif_flags(vif_name,
				    is_pim_register,
				    is_p2p,
				    is_loopback,
				    is_multicast,
				    is_broadcast,
				    is_up,
				    error_msg)
	!= XORP_OK) {
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_set_all_vifs_done()
{
    bool old_is_vif_setup_completed = Mld6igmpNode::is_vif_setup_completed();

    Mld6igmpNode::set_vif_setup_completed(true);

    if (Mld6igmpNode::is_vif_setup_completed()
	&& ! old_is_vif_setup_completed) {
	// XXX: a decr to compensate the wait for network interface
	// information on startup.
	Mld6igmpNode::decr_startup_requests_n();
    }

    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_is_vif_setup_completed(
    // Output values, 
    bool&	is_completed)
{
    is_completed = Mld6igmpNode::is_vif_setup_completed();
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_recv_protocol_message4(
    // Input values, 
    const string&	xrl_sender_name, 
    const string&	, // protocol_name, 
    const uint32_t&	protocol_id, 
    const string&	, // vif_name, 
    const uint32_t&	vif_index, 
    const IPv4&		source_address, 
    const IPv4&		dest_address, 
    const int32_t&	ip_ttl, 
    const int32_t&	ip_tos, 
    const bool&		is_router_alert, 
    const vector<uint8_t>& protocol_message)
{
    //
    // Verify the address family
    //
    if (! Mld6igmpNode::is_ipv4()) {
	string error_msg = c_format("Received protocol message with "
				    "invalid address family: IPv4");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }

    //
    // Verify the module ID
    //
    xorp_module_id src_module_id = static_cast<xorp_module_id>(protocol_id);
    if (! is_valid_module_id(src_module_id)) {
	string error_msg = c_format("Invalid module ID = %d", protocol_id);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    //
    // Receive the message
    //
    Mld6igmpNode::proto_recv(xrl_sender_name,
			     src_module_id,
			     vif_index,
			     IPvX(source_address),
			     IPvX(dest_address),
			     ip_ttl,
			     ip_tos,
			     is_router_alert,
			     &protocol_message[0],
			     protocol_message.size());
    // XXX: no error returned, because if there is any, it is at the
    // protocol level, and the MFEA shoudn't care about it.
    
    //
    // Success
    //
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mfea_client_0_1_recv_protocol_message6(
    // Input values, 
    const string&	xrl_sender_name, 
    const string&	, // protocol_name, 
    const uint32_t&	protocol_id, 
    const string&	, // vif_name, 
    const uint32_t&	vif_index, 
    const IPv6&		source_address, 
    const IPv6&		dest_address, 
    const int32_t&	ip_ttl, 
    const int32_t&	ip_tos, 
    const bool&		is_router_alert, 
    const vector<uint8_t>& protocol_message)
{
    //
    // Verify the address family
    //
    if (! Mld6igmpNode::is_ipv6()) {
	string error_msg = c_format("Received protocol message with "
				    "invalid address family: IPv6");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }

    //
    // Verify the module ID
    //
    xorp_module_id src_module_id = static_cast<xorp_module_id>(protocol_id);
    if (! is_valid_module_id(src_module_id)) {
	string error_msg = c_format("Invalid module ID = %d", protocol_id);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    //
    // Receive the message
    //
    Mld6igmpNode::proto_recv(xrl_sender_name,
			     src_module_id,
			     vif_index,
			     IPvX(source_address),
			     IPvX(dest_address),
			     ip_ttl,
			     ip_tos,
			     is_router_alert,
			     &protocol_message[0],
			     protocol_message.size());
    // XXX: no error returned, because if there is any, it is at the
    // protocol level, and the MFEA shoudn't care about it.
    
    //
    // Success
    //
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_enable_vif(
    // Input values,
    const string&	vif_name,
    const bool&	enable)
{
    string error_msg;
    int ret_value;

    if (enable)
	ret_value = Mld6igmpNode::enable_vif(vif_name, error_msg);
    else
	ret_value = Mld6igmpNode::disable_vif(vif_name, error_msg);

    if (ret_value != XORP_OK)
	return XrlCmdError::COMMAND_FAILED(error_msg);

    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_start_vif(
    // Input values, 
    const string&	vif_name)
{
    string error_msg;
    
    if (Mld6igmpNode::start_vif(vif_name, error_msg) != XORP_OK)
	return XrlCmdError::COMMAND_FAILED(error_msg);
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_stop_vif(
    // Input values, 
    const string&	vif_name)
{
    string error_msg;
    
    if (Mld6igmpNode::stop_vif(vif_name, error_msg) != XORP_OK)
	return XrlCmdError::COMMAND_FAILED(error_msg);
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_enable_all_vifs(
    // Input values,
    const bool&	enable)
{
    string error_msg;
    int ret_value;

    if (enable)
	ret_value = Mld6igmpNode::enable_all_vifs();
    else
	ret_value = Mld6igmpNode::enable_all_vifs();

    if (ret_value != XORP_OK) {
	if (enable)
	    error_msg = c_format("Failed to enable all vifs");
	else
	    error_msg = c_format("Failed to disable all vifs");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }

    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_start_all_vifs()
{
    if (Mld6igmpNode::start_all_vifs() < 0) {
	string error_msg = c_format("Failed to start all vifs");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_stop_all_vifs()
{
    if (Mld6igmpNode::stop_all_vifs() < 0) {
	string error_msg = c_format("Failed to stop all vifs");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_enable_mld6igmp(
    // Input values,
    const bool&	enable)
{
    string error_msg;
    int ret_value;

    if (enable)
	ret_value = enable_mld6igmp();
    else
	ret_value = disable_mld6igmp();

    if (ret_value != XORP_OK) {
	if (enable)
	    error_msg = c_format("Failed to enable MLD6IGMP");
	else
	    error_msg = c_format("Failed to disable MLD6IGMP");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }

    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_start_mld6igmp()
{
    if (start_mld6igmp() != XORP_OK) {
	string error_msg = c_format("Failed to start MLD6IMGP");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_stop_mld6igmp()
{
    if (stop_mld6igmp() != XORP_OK) {
	string error_msg = c_format("Failed to stop MLD6IMGP");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_enable_cli(
    // Input values,
    const bool&	enable)
{
    string error_msg;
    int ret_value;

    if (enable)
	ret_value = enable_cli();
    else
	ret_value = disable_cli();

    if (ret_value != XORP_OK) {
	if (enable)
	    error_msg = c_format("Failed to enable MLD6IGMP CLI");
	else
	    error_msg = c_format("Failed to disable MLD6IGMP CLI");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }

    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_start_cli()
{
    if (start_cli() != XORP_OK) {
	string error_msg = c_format("Failed to start MLD6IMGP CLI");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_stop_cli()
{
    if (stop_cli() != XORP_OK) {
	string error_msg = c_format("Failed to stop MLD6IMGP CLI");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_get_vif_proto_version(
    // Input values, 
    const string&	vif_name, 
    // Output values, 
    uint32_t&		proto_version)
{
    string error_msg;
    
    int v;
    if (Mld6igmpNode::get_vif_proto_version(vif_name, v, error_msg) != XORP_OK)
	return XrlCmdError::COMMAND_FAILED(error_msg);
    
    proto_version = v;
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_set_vif_proto_version(
    // Input values, 
    const string&	vif_name, 
    const uint32_t&	proto_version)
{
    string error_msg;
    
    if (Mld6igmpNode::set_vif_proto_version(vif_name, proto_version, error_msg)
	!= XORP_OK)
	return XrlCmdError::COMMAND_FAILED(error_msg);
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_reset_vif_proto_version(
    // Input values, 
    const string&	vif_name)
{
    string error_msg;
    
    if (Mld6igmpNode::reset_vif_proto_version(vif_name, error_msg) != XORP_OK)
	return XrlCmdError::COMMAND_FAILED(error_msg);
    
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_log_trace_all(
    // Input values,
    const bool&	enable)
{
    Mld6igmpNode::set_log_trace(enable);

    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_add_protocol4(
    // Input values, 
    const string&	xrl_sender_name, 
    const string&	, // protocol_name, 
    const uint32_t&	protocol_id, 
    const string&	vif_name, 
    const uint32_t&	vif_index)
{
    //
    // Verify the address family
    //
    if (! Mld6igmpNode::is_ipv4()) {
	string error_msg = c_format("Received protocol message with "
				    "invalid address family: IPv4");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }

    //
    // Verify the module ID
    //
    xorp_module_id src_module_id = static_cast<xorp_module_id>(protocol_id);
    if (! is_valid_module_id(src_module_id)) {
	string error_msg = c_format("Invalid module ID = %d", protocol_id);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    if (Mld6igmpNode::add_protocol(xrl_sender_name, src_module_id, vif_index)
	< 0) {
	// TODO: must find-out and return the reason for failure
	string error_msg = c_format("Cannot add protocol instance '%s' "
				    "on vif %s with vif_index %d",
				    xrl_sender_name.c_str(),
				    vif_name.c_str(),
				    vif_index);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    //
    // Send info about all existing membership on the particular vif.
    //
    Mld6igmpVif *mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(vif_index);
    if (mld6igmp_vif == NULL) {
	Mld6igmpNode::delete_protocol(xrl_sender_name, src_module_id, vif_index);
	string error_msg = c_format("Cannot add protocol instance '%s' "
				    "on vif %s with vif_index %d: "
				    "no such vif",
				    xrl_sender_name.c_str(),
				    vif_name.c_str(),
				    vif_index);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    list<MemberQuery *>::const_iterator iter;
    for (iter = mld6igmp_vif->members().begin();
	 iter != mld6igmp_vif->members().end();
	 ++iter) {
	const MemberQuery *member_query = *iter;
	send_add_membership(xrl_sender_name.c_str(),
			    src_module_id,
			    mld6igmp_vif->vif_index(),
			    member_query->source(),
			    member_query->group());
    }
    
    //
    // Success
    //
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_add_protocol6(
    // Input values, 
    const string&	xrl_sender_name, 
    const string&	, // protocol_name, 
    const uint32_t&	protocol_id, 
    const string&	vif_name, 
    const uint32_t&	vif_index)
{
    //
    // Verify the address family
    //
    if (! Mld6igmpNode::is_ipv6()) {
	string error_msg = c_format("Received protocol message with "
				    "invalid address family: IPv6");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }

    //
    // Verify the module ID
    //
    xorp_module_id src_module_id = static_cast<xorp_module_id>(protocol_id);
    if (! is_valid_module_id(src_module_id)) {
	string error_msg = c_format("Invalid module ID = %d", protocol_id);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    if (Mld6igmpNode::add_protocol(xrl_sender_name, src_module_id, vif_index)
	< 0) {
	// TODO: must find-out and return the reason for failure
	string error_msg = c_format("Cannot add protocol instance '%s' "
				    "on vif %s with vif_index %d",
				    xrl_sender_name.c_str(),
				    vif_name.c_str(),
				    vif_index);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    //
    // Send info about all existing membership on the particular vif.
    //
    Mld6igmpVif *mld6igmp_vif = Mld6igmpNode::vif_find_by_vif_index(vif_index);
    if (mld6igmp_vif == NULL) {
	Mld6igmpNode::delete_protocol(xrl_sender_name, src_module_id, vif_index);
	string error_msg = c_format("Cannot add protocol instance '%s' "
				    "on vif %s with vif_index %d: "
				    "no such vif",
				    xrl_sender_name.c_str(),
				    vif_name.c_str(),
				    vif_index);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    list<MemberQuery *>::const_iterator iter;
    for (iter = mld6igmp_vif->members().begin();
	 iter != mld6igmp_vif->members().end();
	 ++iter) {
	const MemberQuery *member_query = *iter;
	send_add_membership(xrl_sender_name.c_str(),
			    src_module_id,
			    mld6igmp_vif->vif_index(),
			    member_query->source(),
			    member_query->group());
    }
    
    //
    // Success
    //
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_delete_protocol4(
    // Input values, 
    const string&	xrl_sender_name, 
    const string&	, // protocol_name, 
    const uint32_t&	protocol_id, 
    const string&	vif_name, 
    const uint32_t&	vif_index)
{
    //
    // Verify the address family
    //
    if (! Mld6igmpNode::is_ipv4()) {
	string error_msg = c_format("Received protocol message with "
				    "invalid address family: IPv4");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }

    //
    // Verify the module ID
    //
    xorp_module_id src_module_id = static_cast<xorp_module_id>(protocol_id);
    if (! is_valid_module_id(src_module_id)) {
	string error_msg = c_format("Invalid module ID = %d", protocol_id);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    if (Mld6igmpNode::delete_protocol(xrl_sender_name, src_module_id, vif_index)
	< 0) {
	// TODO: must find-out and return the reason for failure
	string error_msg = c_format("Cannot delete protocol instance '%s' "
				    "on vif %s with vif_index %d",
				    xrl_sender_name.c_str(),
				    vif_name.c_str(),
				    vif_index);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    //
    // Success
    //
    return XrlCmdError::OKAY();
}

XrlCmdError
XrlMld6igmpNode::mld6igmp_0_1_delete_protocol6(
    // Input values, 
    const string&	xrl_sender_name, 
    const string&	, // protocol_name, 
    const uint32_t&	protocol_id, 
    const string&	vif_name, 
    const uint32_t&	vif_index)
{
    //
    // Verify the address family
    //
    if (! Mld6igmpNode::is_ipv6()) {
	string error_msg = c_format("Received protocol message with "
				    "invalid address family: IPv6");
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }

    //
    // Verify the module ID
    //
    xorp_module_id src_module_id = static_cast<xorp_module_id>(protocol_id);
    if (! is_valid_module_id(src_module_id)) {
	string error_msg = c_format("Invalid module ID = %d", protocol_id);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    if (Mld6igmpNode::delete_protocol(xrl_sender_name, src_module_id, vif_index)
	< 0) {
	// TODO: must find-out and return the reason for failure
	string error_msg = c_format("Cannot delete protocol instance '%s' "
				    "on vif %s with vif_index %d",
				    xrl_sender_name.c_str(),
				    vif_name.c_str(),
				    vif_index);
	return XrlCmdError::COMMAND_FAILED(error_msg);
    }
    
    //
    // Success
    //
    return XrlCmdError::OKAY();
}
