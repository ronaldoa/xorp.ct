/*
 * Copyright (c) 2001-2007 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'clnt-gen'.
 */

#ident "$XORP: xorp/xrl/interfaces/policy_backend_xif.cc,v 1.6 2006/03/26 22:39:25 pavlin Exp $"

#include "policy_backend_xif.hh"

bool
XrlPolicyBackendV0p1Client::send_configure(
	const char*	dst_xrl_target_name,
	const uint32_t&	filter,
	const string&	conf,
	const ConfigureCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy_backend/0.1/configure");
    x.args().add("filter", filter);
    x.args().add("conf", conf);
    return _sender->send(x, callback(this, &XrlPolicyBackendV0p1Client::unmarshall_configure, cb));
}


/* Unmarshall configure */
void
XrlPolicyBackendV0p1Client::unmarshall_configure(
	const XrlError&	e,
	XrlArgs*	a,
	ConfigureCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(0));
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}

bool
XrlPolicyBackendV0p1Client::send_reset(
	const char*	dst_xrl_target_name,
	const uint32_t&	filter,
	const ResetCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy_backend/0.1/reset");
    x.args().add("filter", filter);
    return _sender->send(x, callback(this, &XrlPolicyBackendV0p1Client::unmarshall_reset, cb));
}


/* Unmarshall reset */
void
XrlPolicyBackendV0p1Client::unmarshall_reset(
	const XrlError&	e,
	XrlArgs*	a,
	ResetCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(0));
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}

bool
XrlPolicyBackendV0p1Client::send_push_routes(
	const char*	dst_xrl_target_name,
	const PushRoutesCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy_backend/0.1/push_routes");
    return _sender->send(x, callback(this, &XrlPolicyBackendV0p1Client::unmarshall_push_routes, cb));
}


/* Unmarshall push_routes */
void
XrlPolicyBackendV0p1Client::unmarshall_push_routes(
	const XrlError&	e,
	XrlArgs*	a,
	PushRoutesCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(0));
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}
