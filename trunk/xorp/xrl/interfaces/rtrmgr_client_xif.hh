/*
 * Copyright (c) 2001-2007 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'clnt-gen'.
 *
 * $XORP: xorp/xrl/interfaces/rtrmgr_client_xif.hh,v 1.15 2006/03/26 22:39:28 pavlin Exp $
 */

#ifndef __XRL_INTERFACES_RTRMGR_CLIENT_XIF_HH__
#define __XRL_INTERFACES_RTRMGR_CLIENT_XIF_HH__

#undef XORP_LIBRARY_NAME
#define XORP_LIBRARY_NAME "XifRtrmgrClient"

#include "libxorp/xlog.h"
#include "libxorp/callback.hh"

#include "libxipc/xrl.hh"
#include "libxipc/xrl_error.hh"
#include "libxipc/xrl_sender.hh"


class XrlRtrmgrClientV0p2Client {
public:
    XrlRtrmgrClientV0p2Client(XrlSender* s) : _sender(s) {}
    virtual ~XrlRtrmgrClientV0p2Client() {}

    typedef XorpCallback1<void, const XrlError&>::RefPtr NewConfigUserCB;

    bool send_new_config_user(
	const char*	dst_xrl_target_name,
	const uint32_t&	userid,
	const NewConfigUserCB&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr ConfigSavedDoneCB;

    bool send_config_saved_done(
	const char*	dst_xrl_target_name,
	const bool&	success,
	const string&	errmsg,
	const ConfigSavedDoneCB&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr ConfigChangeDoneCB;

    bool send_config_change_done(
	const char*	dst_xrl_target_name,
	const bool&	success,
	const string&	errmsg,
	const ConfigChangeDoneCB&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr ConfigChangedCB;

    bool send_config_changed(
	const char*	dst_xrl_target_name,
	const uint32_t&	userid,
	const string&	deltas,
	const string&	deletions,
	const ConfigChangedCB&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr ModuleStatusCB;

    bool send_module_status(
	const char*	dst_xrl_target_name,
	const string&	modname,
	const uint32_t&	status,
	const ModuleStatusCB&	cb
    );

protected:
    XrlSender* _sender;

private:
    void unmarshall_new_config_user(
	const XrlError&	e,
	XrlArgs*	a,
	NewConfigUserCB		cb
    );

    void unmarshall_config_saved_done(
	const XrlError&	e,
	XrlArgs*	a,
	ConfigSavedDoneCB		cb
    );

    void unmarshall_config_change_done(
	const XrlError&	e,
	XrlArgs*	a,
	ConfigChangeDoneCB		cb
    );

    void unmarshall_config_changed(
	const XrlError&	e,
	XrlArgs*	a,
	ConfigChangedCB		cb
    );

    void unmarshall_module_status(
	const XrlError&	e,
	XrlArgs*	a,
	ModuleStatusCB		cb
    );

};

#endif /* __XRL_INTERFACES_RTRMGR_CLIENT_XIF_HH__ */
