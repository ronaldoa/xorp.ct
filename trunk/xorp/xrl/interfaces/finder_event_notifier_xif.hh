/*
 * Copyright (c) 2001-2006 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'clnt-gen'.
 *
 * $XORP: xorp/xrl/interfaces/finder_event_notifier_xif.hh,v 1.10 2006/03/16 00:06:12 pavlin Exp $
 */

#ifndef __XRL_INTERFACES_FINDER_EVENT_NOTIFIER_XIF_HH__
#define __XRL_INTERFACES_FINDER_EVENT_NOTIFIER_XIF_HH__

#undef XORP_LIBRARY_NAME
#define XORP_LIBRARY_NAME "XifFinderEventNotifier"

#include "libxorp/xlog.h"
#include "libxorp/callback.hh"

#include "libxipc/xrl.hh"
#include "libxipc/xrl_error.hh"
#include "libxipc/xrl_sender.hh"


class XrlFinderEventNotifierV0p1Client {
public:
    XrlFinderEventNotifierV0p1Client(XrlSender* s) : _sender(s) {}
    virtual ~XrlFinderEventNotifierV0p1Client() {}

    typedef XorpCallback1<void, const XrlError&>::RefPtr RegisterClassEventInterestCB;
    /**
     *  Send Xrl intended to:
     *
     *  Register interest in events relating to a particular class. Following
     *  this call the requester will receive birth event notifications for
     *  existing instances within the named class and will receive birth and
     *  deatch event notifications as instances are created and destroyed
     *  subsequently.
     *
     *  @param dst_xrl_target_name Destination Xrl Target name
     *
     *  @param requester_instance instance name of Xrl Target interested in
     *  receiving event notifications.
     *
     *  @param class_name class of Xrl Targets to generate event notifications
     *  for.
     */
    bool send_register_class_event_interest(
	const char*	dst_xrl_target_name,
	const string&	requester_instance,
	const string&	class_name,
	const RegisterClassEventInterestCB&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr DeregisterClassEventInterestCB;
    /**
     *  Send Xrl intended to:
     *
     *  Deregister interest in events relating to a particular class.
     *
     *  @param dst_xrl_target_name Destination Xrl Target name
     *
     *  @param requester_instance instance name of Xrl Target interested in
     *  receiving event notifications.
     *
     *  @param class_name class of Xrl Targets to stop event notifications for.
     */
    bool send_deregister_class_event_interest(
	const char*	dst_xrl_target_name,
	const string&	requester_instance,
	const string&	class_name,
	const DeregisterClassEventInterestCB&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr RegisterInstanceEventInterestCB;
    /**
     *  Send Xrl intended to:
     *
     *  Register interest in events relating to a particular instance.
     *  Following this call the requester will receive a birth event
     *  notification for the named instance if it exists. When the named
     *  instance dies, the requester will receive a death event notification.
     *  This method will fail if the named instance does not exist.
     *
     *  @param dst_xrl_target_name Destination Xrl Target name
     *
     *  @param requester_instance instance name of Xrl Target interested in
     *  receiving event notifications.
     *
     *  @param instance_name name of Xrl Target instance to receive event
     *  notifications for.
     */
    bool send_register_instance_event_interest(
	const char*	dst_xrl_target_name,
	const string&	requester_instance,
	const string&	instance_name,
	const RegisterInstanceEventInterestCB&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr DeregisterInstanceEventInterestCB;
    /**
     *  Send Xrl intended to:
     *
     *  Register interest in events relating to a particular instance.
     *
     *  @param dst_xrl_target_name Destination Xrl Target name
     *
     *  @param requester_instance instance name of Xrl Target interested in
     *  receiving event notifications.
     *
     *  @param instance_name name of Xrl Target instance to stop event
     *  notifications for.
     */
    bool send_deregister_instance_event_interest(
	const char*	dst_xrl_target_name,
	const string&	requester_instance,
	const string&	instance_name,
	const DeregisterInstanceEventInterestCB&	cb
    );

protected:
    XrlSender* _sender;

private:
    void unmarshall_register_class_event_interest(
	const XrlError&	e,
	XrlArgs*	a,
	RegisterClassEventInterestCB		cb
    );

    void unmarshall_deregister_class_event_interest(
	const XrlError&	e,
	XrlArgs*	a,
	DeregisterClassEventInterestCB		cb
    );

    void unmarshall_register_instance_event_interest(
	const XrlError&	e,
	XrlArgs*	a,
	RegisterInstanceEventInterestCB		cb
    );

    void unmarshall_deregister_instance_event_interest(
	const XrlError&	e,
	XrlArgs*	a,
	DeregisterInstanceEventInterestCB		cb
    );

};

#endif /* __XRL_INTERFACES_FINDER_EVENT_NOTIFIER_XIF_HH__ */
